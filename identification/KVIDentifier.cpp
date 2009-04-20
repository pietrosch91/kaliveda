/*
$Id: KVIDentifier.cpp,v 1.7 2009/04/20 10:01:58 ebonnet Exp $
$Revision: 1.7 $
$Date: 2009/04/20 10:01:58 $
*/

//Created by KVClassFactory on Mon Apr 14 14:25:38 2008
//Author: franklan

#include "KVIDentifier.h"
#include "KVIDGraph.h"
#include "TF1.h"
#include "TPad.h"
#include "TClass.h"

ClassImp(KVIDentifier)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDentifier</h2>
<h4>Base class for graphical cuts used in particle identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVIDentifier::init()
{
	// Default initialisations
   // We set marker size and style for the underlying TGraph,
	// this way a small circle is drawn at each point.
   // Also SetEditable(kFALSE) is used to stop it being modified using the mouse.
	
   SetMarkerStyle(kCircle);
   SetMarkerSize(0.8);
   SetLineWidth(2);
   SetEditable(kFALSE);
	fParent = 0;
}

KVIDentifier::KVIDentifier()
	: TCutG(), fIon(1,1)
{
   // Default constructor
	init();
}

KVIDentifier::~KVIDentifier()
{
   // Destructor
}

KVIDentifier::KVIDentifier(const TCutG & gr)
		: TCutG(gr), fIon(1,1)
{
   //copy a TCutG
	init();
}

KVIDentifier::KVIDentifier(const TGraph & gr)
		: TCutG(), fIon(1,1)
{
   //copy a TGraph
	init();
	CopyGraph(gr);
}

KVIDentifier::KVIDentifier(const KVIDentifier & gr)
		: TCutG(gr), fIon(1,1)
{
   // copy constructor
	// we do not copy the fParent pointer to parent grid,
	// it is left NULL.
	init();
	SetZ(gr.GetZ());
	SetMassFormula(gr.GetMassFormula());
	SetA(gr.GetA());
}

KVIDGraph* KVIDentifier::GetParent() const
{
	// Get pointer to parent ID graph
	return fParent;
}

void KVIDentifier::SetParent(KVIDGraph* p)
{
	// Set pointer to parent ID graph
	fParent = p;
}

//_____________________________________________________________________________________________

void KVIDentifier::CopyGraph(TGraph * graph)
{
   // Copy coordinates of points from the TGraph
	
   Double_t x, y;
   //change number of points
   Set(graph->GetN());
   for (int i = 0; i < GetN(); i++) {
      graph->GetPoint(i, x, y);
      SetPoint(i, x, y);
   }
}

//_____________________________________________________________________________________________

void KVIDentifier::CopyGraph(const TGraph & graph)
{
   // Copy coordinates of points from the TGraph
	
   Double_t x, y;
   //change number of points
   Set(graph.GetN());
   for (int i = 0; i < GetN(); i++) {
      graph.GetPoint(i, x, y);
      SetPoint(i, x, y);
   }
}

//_____________________________________________________________________________________________

void KVIDentifier::WriteAsciiFile(ofstream & file, const Char_t * name_prefix)
{
   //Write name and coordinates of identifier in file buffer stream
   //Optional string name_prefix will be written just in front of the name of the object.
   //Format is :
   //
   //+classname
   //name_prefix:name_of_object
   //number_of_points
   //x1    y1
   //x2    y2
   //...
   //etc. etc.
	
  file << '+' << ClassName() << endl;
  if (name_prefix)
     file << name_prefix << ":";
  file << GetName() << endl;
  //any extra stuff defined in child class ?
  WriteAsciiFile_extras(file, name_prefix);
  file << GetN() << endl;
  for (Int_t i = 0; i < GetN(); i++) {
     Double_t x, y;
     GetPoint(i, x, y);
     file << x << "   " << y << endl;
  }
}

//_____________________________________________________________________________________________

void KVIDentifier::ReadAsciiFile(ifstream & file)
{
   //Read coordinates of identifier in file buffer stream
   //Format is :
   //
   //number_of_points
   //x1    y1
   //x2    y2
   //...
   //etc. etc.
	
   ReadAsciiFile_extras(file);//any extra stuff defined in child class ?
   Int_t N;
   file >> N;
   for (Int_t i = 0; i < N; i++) {
      Double_t x, y;
      file >> x >> y;
		SetPoint(i, x, y);
   }
}

//_____________________________________________________________________________________________

Int_t KVIDentifier::Compare(const TObject * obj) const
{
   //Dummy method used for sorting, can be redefined in more specific implementations.
   //A priori we don't know how to "order" the identification cuts.

   return 0;
}

//_____________________________________________________________________________________________

void KVIDentifier::Print(Option_t * opt) const
{
   // Print out for identification cut
   // The optional "opt" string, if given, is printed in parentheses after the line's name
   // This is used by KVIDGraph in order to show which are "ID" cuts and which are
   // "OK" cuts (i.e. used to define an identifiable area in a data map).
	
   cout << ClassName() << " : " << GetName() << "(" << opt << ")" << endl;
   TCutG::Print();
}

//_____________________________________________________________________________________________

void KVIDentifier::Scale(Double_t sx, Double_t sy)
{
   //Scale the coordinates of every point of the cut according to
   //      X_0 ---> sx*X_0
   //      Y_0 ---> sy*Y_0
   //Default argument value '-1' means leave coordinate unchanged
   if (TMath::Abs(sx) == 1 && TMath::Abs(sy) == 1)
      return;
   for (int i = 0; i < fNpoints; i++) {
      if (sx > 0.)
         fX[i] *= sx;
      if (sy > 0.)
         fY[i] *= sy;
   }
}

//_____________________________________________________________________________________________

void KVIDentifier::Scale(TF1 *sx, TF1 *sy)
{
   // Apply to the coordinates of every point the transformation
   //      X_0 ---> sx(X_0,Y_0)
   //      Y_0 ---> sy(X_0,Y_0)
	// if sx & sy are two-dimensional functions (inherits from TF2),
	// otherwise we apply the transformation
   //      X_0 ---> sx(X_0)
   //      Y_0 ---> sy(Y_0)
	// for sx & sy one-dimensionsal (TF1) functions.
	
   if (!sx && !sy) return;	// if the two functions are NULL we leave coordinate unchanged
	Bool_t sx_2d, sy_2d;
	sx_2d = (sx && sx->InheritsFrom("TF2"));
	sy_2d = (sy && sy->InheritsFrom("TF2"));
	if(sx && sy){
   	for (Int_t  ii = 0; ii < fNpoints; ii++) {
      	if (sx_2d) fX[ii] = sx->Eval(fX[ii],fY[ii]);
			else fX[ii] = sx->Eval(fX[ii]);
      	if (sy_2d) fY[ii] = sy->Eval(fX[ii],fY[ii]);
			else fY[ii] = sy->Eval(fY[ii]);
   	}
	}
	else if(sx){
   	for (Int_t  ii = 0; ii < fNpoints; ii++) {
      	if (sx_2d) fX[ii] = sx->Eval(fX[ii],fY[ii]);
			else fX[ii] = sx->Eval(fX[ii]);
   	}
	}
	else{
   	for (Int_t  ii = 0; ii < fNpoints; ii++) {
      	if (sy_2d) fY[ii] = sy->Eval(fX[ii],fY[ii]);
			else fY[ii] = sy->Eval(fY[ii]);
   	}
	}
}

//_____________________________________________________________________________________________

void KVIDentifier::WaitForPrimitive()
{
	// Method used to draw a new identifier in the active pad
	// Override in child classes so that gPad->WaitPrimitive has correct arguments
	
	if(!gPad) return;
   TGraph *gr = (TGraph *) gPad->WaitPrimitive("CUTG", "CutG");
   //copy coordinates of user's line
   CopyGraph(gr);
   //remove TGraph and draw the KVIDentifier in its place
   gPad->GetListOfPrimitives()->Remove(gr);
   delete gr;
   Draw("PL");
}
	
//_____________________________________________________________________________________________

void KVIDentifier::ExtendLine(Double_t Limit, Option_t* Direction)
{
	// Right-clicking an identifier and selecting this method from the context
	// menu allows to add a straight-line segment at the end or the beginning
	// of the line (whichever is closest to the mouse).
	//
	//  Direction = "H", "h", "hori", "HORI" etc. - add horizontal segment
	//  Direction = "v", "V", "vert", "VERT" etc. - add vertical segment
      
	if(!GetEditable()) return;
	
	// find closest point
   Int_t px = gPad->GetEventX(); // mouse position
   Int_t py = gPad->GetEventY(); // mouse position
   Int_t dpx = px - gPad->XtoAbsPixel(gPad->XtoPad(fX[0])); // first point
   Int_t dpy = py - gPad->YtoAbsPixel(gPad->XtoPad(fY[0])); // first point
	Int_t dist_1 = dpx*dpx+dpy*dpy;
   dpx = px - gPad->XtoAbsPixel(gPad->XtoPad(fX[fNpoints-1])); // last point
   dpy = py - gPad->YtoAbsPixel(gPad->XtoPad(fY[fNpoints-1])); // last point
	Int_t dist_N = dpx*dpx+dpy*dpy;
   Int_t ipoint = (dist_N < dist_1 ? fNpoints-1 : 0);
		
	// coordinates of new point
	TString opt(Direction); opt.ToUpper();
	Double_t newX = fX[ipoint];
   Double_t newY = fY[ipoint];
	if(opt.BeginsWith("H")) newX = Limit;
	else if(opt.BeginsWith("V")) newY = Limit;
	
	// add point
	Int_t iend = (ipoint>0 ? fNpoints : 0);	
   Double_t **ps = ExpandAndCopy(fNpoints + 1, iend);
   CopyAndRelease(ps, ipoint, fNpoints++, ipoint + 1);

   // To avoid redefenitions in descendant classes
   FillZero(iend, iend + 1);

	if(ipoint>0) ipoint=fNpoints-1;
   fX[ipoint] = newX;
   fY[ipoint] = newY;
   gPad->Modified();
}

void KVIDentifier::CloneScaleStore(Int_t newzt,Double_t sy,Int_t newar,Double_t sx){
	// Create a new line from the selected one 
	// with a new Z and A (optional)
	// this new line is scale from the selected one with a vertical sy 
	// and horizontal sx (optional) factor
	// you need to undraw and draw the grid to see its implementation

	TClass* cl = new TClass(this->IsA()->GetName());
	KVIDentifier* idbis = (KVIDentifier* )cl->New();
	Double_t xx,yy;
	for (Int_t nn=0;nn<this->GetN();nn+=1){
		this->GetPoint(nn,xx,yy);
		idbis->SetPoint(nn,xx,yy);
	}
	
	idbis->SetZ(newzt); if (newar!=-1){
		idbis->SetA(newar);
	}
	idbis->Scale(sx,sy);
	this->GetParent()->AddIdentifier(idbis);
	
	delete cl;
}
