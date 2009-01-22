/***************************************************************************
                          KVIDGrid.cpp  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDGrid.cpp,v 1.52 2008/10/13 13:52:29 franklan Exp $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVIDGrid.h"
#include "TCanvas.h"
#include "TObjString.h"
#include "Riostream.h"
#include "TObjArray.h"
#include "TROOT.h"
#include "KVString.h"
#include "KVTGID.h"
#include "TVirtualFitter.h"
#include "KVTGIDFunctions.h"
#include "KVParameter.h"
#include "KVNewGridDialog.h"
#include "TClass.h"
#include "TSystem.h"
#include "TF1.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KVIDGrid
//
//Abstract base class for 2D identification grids in e.g. (dE,E) maps.
//Such a grid is supposed to be made up of two parts:
//      - a set of lines for the identification i.e. indicating the position of the
//        loci of points corresponding to a given nucleus
//      - a set of lines used to indicate the area of the map in which identification
//        is possible (or conversely, the limits beyond which identification is
//        not possible).
//
//Specific implementations of such identification grids must define the following
//methods :
//
//      Bool_t IsIdentifiable(Double_t x, Double_t y) const
//
//      - returns either kTRUE or kFALSE for each point (x,y) of the identification
//        map depending on whether such a point can lead to a successful identification
//        or not
//
//      void    Identify        (Double_t x, Double_t y, KVNucleus *nuc) const
//
//      - attribute an identification to the KVNucleus for a given point (x,y) in the map.
//        This is most often based on the result of FindNearestIDLine(), which is a general
//      algorithm for finding the ID line which is the closest to a given point.
//
// Make sure to call the Initialise() method of the grid once before using it.
//
//Get/SetParameter methods
//You can associate any number of parameters with the grid (KVParameter<Double_t> objects)
//A parameter associates a name and a value. These parameters are used to name the grid.
//
//Grid names
//The name of the ID grid is dynamically generated from the ID grid type (KVBase::GetType)
//and the list of all parameters associated to it:
//   
//  grid name = grid type Parameter1=value Parameter2=value Parameter3= ...
//
//Grid scaling factors
//Get/SetX/YScaleFactor methods can be used to apply global linear scaling factor to grids
//in one or both dimensions. i.e. if we have a grid pointer
//
//      KVIDGrid* g;//pointer to a grid
//      g->SetXScaleFactor(1.2);//we apply a 20% scaling factor to the X-coordinates
//      g->Draw(); // X-coordinates of all points in lines are 1.2*original value
//
//To remove the scaling factor, call SetX/YScaleFactor() or SetX/YScaleFactor(0).
//Note that this scaling is taken into account also in IsIdentifiable and Identify methods
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**************************USED FOR FITTING WITH A KVTGID********************************/
void KVIDGridfcnk0(Int_t & npar, Double_t * gin, Double_t & f,
                   Double_t * par, Int_t iflag);
KVIDGrid *fCurrentIDGrid = 0;
KVTGID *fCurrentTGID = 0;
/*****************************************************************************************/

ClassImp(KVIDGrid)

//________________________________________________________________________________

KVIDGrid::KVIDGrid()
{
   //Default constructor
   init();
}

//________________________________________________________________________________

KVIDGrid::KVIDGrid(const KVIDGrid & grid)
{
   //Copy constructor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   grid.Copy(*this);
#else
   ((KVIDGrid &) grid).Copy(*this);
#endif
}

//________________________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVIDGrid::Copy(TObject & obj) const
#else
void KVIDGrid::Copy(TObject & obj)
#endif
{
   //Copy this to 'obj'
   KVBase::Copy(obj);
   TAttLine::Copy((TAttLine &) obj);
   KVIDGrid & grid = (KVIDGrid &) obj;
   //as in WriteAsciiFile, we need to remove any scaling factors from the coordinates
   //before copying the coordinates, then we set the scaling factors at the end

   //remove scaling if there is one
   if (fLastScaleX != 1 || fLastScaleY != 1)
      const_cast < KVIDGrid * >(this)->Scale(1. / fLastScaleX,
                                             1. / fLastScaleY);

   if (fIDLines)
     fIDLines->Copy((TObject &) (*grid.GetIDLines()));
   if (fOKLines)
     fOKLines->Copy((TObject &) (*grid.GetOKLines()));
     //copy all parameters EXCEPT scaling parameters
     KVParameter<KVString> *par = 0;
     for( int i=0; i<fPar->GetNPar(); i++) { //loop over all parameters
        
        par = fPar->GetParameter(i);

       TString parname(par->GetName());
       if (!parname.Contains("ScaleFactor"))  grid.fPar->SetParameter(par->GetName(), par->GetVal());
     }
   //restore scaling to this grid if there was one, and apply it to the copied grid
   if (fLastScaleX != 1 || fLastScaleY != 1) {
      const_cast < KVIDGrid * >(this)->Scale(fLastScaleX, fLastScaleY);
      grid.SetXScaleFactor(fLastScaleX);
      grid.SetYScaleFactor(fLastScaleY);
   }
}

//________________________________________________________________________________

void KVIDGrid::init()
{
   //Initialisations, used by constructors
   fIDLines = new KVList;
   fOKLines = new KVList;
   fXmin = fYmin = fXmax = fYmax = 0;
   fParameters = 0;
   fPar = new KVGenParList;
   fLastScaleX = 1.0;
   fLastScaleY = 1.0;
   SetType("KVIDGrid");
}

//________________________________________________________________________________

void KVIDGrid::Clear(Option_t * opt)
{
   //reset the grid (destroying any previously defined lines) ready to start anew.
   //resets axis limits
   //scaling factors (if any) are removed
   fIDLines->Delete();
   fOKLines->Delete();
   fXmin = fYmin = fXmax = fYmax = 0;
   SetXScaleFactor();
   SetYScaleFactor();
}

//________________________________________________________________________________

KVIDGrid::~KVIDGrid()
{
   //Dtor. Deletes lists of grid lines (which in turn delete the lines themselves).
   delete fIDLines;
   fIDLines = 0;
   delete fOKLines;
   fOKLines = 0;
   SafeDelete(fParameters);
   delete fPar;
   fPar = 0;
}

//________________________________________________________________________________

Bool_t KVIDGrid::IsSorted() const
{
   //Returns kTRUE if the ID lines have been sorted
   return GetIDLines()->IsSorted();
}

//________________________________________________________________________________

KVIDLine *KVIDGrid::GetIDLine(const Char_t * name) const
{
   //Returns ID line "name" in the list fIDLine
   return (KVIDLine *) GetIDLines()->FindObjectByName(name);
}

//________________________________________________________________________________

KVIDLine *KVIDGrid::GetOKLine(const Char_t * name) const
{
   //Returns line "name" used to check if ID is possible from the list fOKLine
   return (KVIDLine *) GetOKLines()->FindObjectByName(name);
}

//________________________________________________________________________________

KVIDLine *KVIDGrid::GetIDLine(UInt_t index) const
{
   //Returns ID line by index in the list fIDLine
   return (KVIDLine *) GetIDLines()->At(index);
}

//________________________________________________________________________________

KVIDLine *KVIDGrid::GetOKLine(UInt_t index) const
{
   //Returns line used to check if ID is possible by index in the list fOKLine
   return (KVIDLine *) GetOKLines()->At(index);
}

//_______________________________________________________________________________________________//

void KVIDGrid::AddIDLine(Double_t x1, Double_t y1, Double_t x2,
                         Double_t y2, const Char_t * name,
                         const Char_t * idline_class)
{
   //Add an ID line (KVIDLine) to the grid and draw it in current display (if one is open).
   //Initial coordinates of first segment are given.
   //If you want the ID line to be an object of a class derived from KVIDLine,
   //give the class name as argument "idline_class".

   KVIDLine *line = NewLine(idline_class);
   if (name)
      line->SetName(name);
   line->SetPoint(0, x1, y1);
   line->SetPoint(1, x2, y2);
   AddIDLine(line);
   //draw in current display pad if there is one
   if (gPad)
      line->Draw();
}

//_______________________________________________________________________________________________//

void KVIDGrid::AddOKLine(Double_t x1, Double_t y1, Double_t x2,
                         Double_t y2, const Char_t * name,
                         const Char_t * idline_class)
{
   //Add a line used to check if ID is possible (KVIDLine) to the grid and draw it in current display (if one is open).
   //Initial coordinates of first segment are given.
   //If you want the line to be an object of a class derived from KVIDLine, give the class name
   //as argument "idline_class".

   KVIDLine *line = NewLine(idline_class);
   if (name)
      line->SetName(name);
   line->SetPoint(0, x1, y1);
   line->SetPoint(1, x2, y2);
   AddOKLine(line);
   //draw in current display pad if there is one
   if (gPad)
      line->Draw();
}

//_______________________________________________________________________________________________//

void KVIDGrid::WriteAsciiFile(const Char_t * filename)
{
   //Open, write and close ascii file containing each line of this grid.

   ofstream gridfile(filename);
   WriteAsciiFile(gridfile);
   gridfile.close();
}

//_______________________________________________________________________________________________//

void KVIDGrid::WriteAsciiFile(ofstream & gridfile)
{
   //Write info on this grid in already-open ascii file stream 'gridfile'.
   //This begins with the line
   //++ClassName
   //where '"ClassName'" is the name of the class actual instance of this object
   //followed by informations on the grid and its lines.
   //Any associated parameters are written before the lines, starting with
   //<PARAMETER>
   //then the name of the parameter and its value
   //e.e.
   //<PARAMETER> ChIo pressure=45.5
   //A single '!' on a line by itself signals the end of the informations for this grid.
   //
   //Scaling factors
   //Any scaling factors are written in the file as 
   //<PARAMETER> XScaleFactor=...
   //<PARAMETER> YScaleFactor=...
   //However the coordinates written in the file are the unscaled factors
   //When the file is read back again, the unscaled coordinates are read in and
   //then the scaling is applied.

   gridfile << "# ASCII file generated by " << ClassName() <<
       "::WriteAsciiFile" << endl;
   gridfile << "# Grid Name : " << GetName() << endl;
   gridfile << "# This file can be read using " << ClassName() <<
       "::ReadAsciiFile" << endl;
   gridfile << "# " << endl;
   gridfile << "++" << ClassName() << endl;

   //write parameters
   KVParameter<KVString> *par = 0;
   for( int i=0; i<fPar->GetNPar(); i++) { //loop over all parameters
      par = fPar->GetParameter(i);
      gridfile << "<PARAMETER> " << par->GetName() << "=" << par->GetVal().Data() << endl;
   }
   
   //remove scaling if there is one
   if (fLastScaleX != 1 || fLastScaleY != 1)
      Scale(1. / fLastScaleX, 1. / fLastScaleY);

   //Write ID lines
   TIter next_IDline(GetIDLines());
   KVIDLine *line;
   while ((line = (KVIDLine *) next_IDline())) {
      line->WriteAsciiFile(gridfile, "ID");
   }
   //Write OK lines
   TIter next_OKline(GetOKLines());
   while ((line = (KVIDLine *) next_OKline())) {
      line->WriteAsciiFile(gridfile, "OK");
   }
   gridfile << "!" << endl;

   //restore scaling if there is one
   if (fLastScaleX != 1 || fLastScaleY != 1)
      Scale(fLastScaleX, fLastScaleY);
}

//_______________________________________________________________________________________________//

void KVIDGrid::ReadAsciiFile(ifstream & gridfile)
{
   //Read info from already open ascii file stream containing each line of this grid.
   //Any previous lines in the grid are destroyed.
   //Comment lines begin with #
   //Grid info begins with
   //++ClassName
   //where ClassName is actual class name of instance of this object
   //Parameter lines begin with <PARAMETER>
   //New lines begin with +name_of_class
   //'!' signals end of grid info
   //Any scaling factors are applied once the coordinates have been read

   //reset grid - destroy old lines, axis limits...
   Clear();
   //clear parameters
   fPar->Clear();

   KVString s;
   int line_no=0;// counter for lines read

   while (gridfile.good()) {
      //read a line
      s.ReadLine(gridfile);
      if (s.BeginsWith('!')) {
         //end of grid signalled
         break;
      } else if (s.BeginsWith("++")) {  //will only happen if we are reading a file using ReadAsciiFile(const char*)
         //check classname
         s.Remove(0, 2);
         if (s != ClassName())
            Warning("ReadAsciiFile(ofstream&)",
                    "Class name in file %s does not correspond to this class (%s)",
                    s.Data(), ClassName());
      } else if (s.BeginsWith("<PARAMETER>")) {
         //parameter definition
         s.Remove(0, 11);
         //split into tokens separated by '='
         TObjArray *toks = s.Tokenize('=');
         TString name =
             ((TObjString *) toks->At(0))->GetString().
             Strip(TString::kBoth);
         KVString value(((TObjString *) toks->At(1))->GetString());
         fPar->SetValue(name.Data(), value);
         delete toks;           //must clean up             
      } else if (s.BeginsWith('+')) {
         //New line
         line_no++;
         //Get name of class by stripping off the '+' at the start of the line
         s.Remove(0, 1);
         //Make new line using this class
         KVIDLine *line = NewLine(s.Data());
         //next line is type ("ID" or "OK") followed by ':', followed by name of line
         s.ReadLine(gridfile);
         //split into tokens separated by ':'
         TObjArray *toks = s.Tokenize(':');
         TString type = ((TObjString *) toks->At(0))->GetString();
         TString name = "";
         // make sure the line actually has a name !
         if( toks->GetEntries()>1 ) name = ((TObjString *) toks->At(1))->GetString();
         else
         {
            // print warning allowing to find nameless line in file
            Warning("ReadAsciiFile",
                  "In grid : %s\nLine with no name. Line type = %s. Line number in grid = %d",
                  GetName(), type.Data(), line_no);
         }
         delete toks;           //must clean up
         AddLine(type, line);
         line->SetName(name.Data());
         //now use ReadAscii method of class to read coordinates and other informations
         line->ReadAsciiFile(gridfile);
      }
   }
   fLastScaleX = GetXScaleFactor();
   fLastScaleY = GetYScaleFactor();
   if (fLastScaleX != 1 || fLastScaleY != 1) {
      Scale(fLastScaleX, fLastScaleY);
   }
}

//_______________________________________________________________________________________________//

void KVIDGrid::ReadAsciiFile(const Char_t * filename)
{
   //Open, read and close ascii file containing each line of this grid.
   //Any previous lines in the grid are destroyed.
   //Comment lines begin with #
   //Grid info begins with
   //++ClassName
   //where ClassName is actual class name of instance of this object
   //New lines begin with +name_of_class
   //'!' signals end of grid

   ifstream gridfile(filename);

   if (gridfile.good())
      ReadAsciiFile(gridfile);

   gridfile.close();
}

//_______________________________________________________________________________________________//

KVIDLine *KVIDGrid::NewLine(const Char_t * idline_class)
{
   // Create a new line compatible with this grid.
	//
	// If idline_class = "id" or "ID":
	//§      create default identification line object for this grid
	//
	// If idline_class = "ok" or "OK":
	//       create default 'OK' line object for this grid
	//
	// If idline_class = class name:
	//       create line object of given class
	//
	// If idline_class = "":
	//       create KVIDLine object

	TString _cl, _type(idline_class); _type.ToUpper();
	TClass *clas = 0;
	
	if(_type=="ID") clas = DefaultIDLineClass();
	else if(_type=="OK") clas = DefaultOKLineClass();
	else _cl = idline_class;
	
	if(_cl=="") _cl="KVIDLine";
   if(!clas) clas = gROOT->GetClass( _cl.Data() );
	
   KVIDLine *line = 0;
	
   if (!clas) {
      Error("AddIDLine",
            "%s is not a valid classname. No known class.", _cl.Data() );
   } else {
      if (!clas->InheritsFrom("KVIDLine")) {
         Error("AddIDLine",
               "%s is not a valid class deriving from KVIDLine.",
               _cl.Data() );
      } else {
         line = (KVIDLine *) clas->New();
      }
   }
   return line;
}

//_______________________________________________________________________________________________//

void KVIDGrid::Draw(Option_t * opt)
{
   //Draw all lines in grid on the current display, if one exists: i.e. in order to superimpose the grid
   //on a 2D display of data, use KVIGGrid::Draw() (no option "same" needed).
   //
   //If no TCanvas exists, or if one exists but it is empty,  a frame large enough to display all
   //the lines in the grid will be generated.
   //
   //The title of the canvas is set to the name of the grid

   if (!gPad) {
      new TCanvas("c1", GetName());
   } else {
      gPad->SetTitle(GetName());
   }
   if (!gPad->GetListOfPrimitives()->GetSize()) {
      //calculate size of pad necessary to show grid
      if (GetXmin() == GetXmax())
         const_cast < KVIDGrid * >(this)->FindAxisLimits();
      gPad->DrawFrame(GetXmin(), GetYmin(), GetXmax(), GetYmax());
   }
   {
      GetIDLines()->R__FOR_EACH(KVIDLine, Draw) ("PL");
   }
   {
      GetOKLines()->R__FOR_EACH(KVIDLine, Draw) ("PL");
   }
}

//_______________________________________________________________________________________________//

void KVIDGrid::UnDraw()
{
   //Make the grid disappear from the current canvas/pad
   //In case any lines have (accidentally) been drawn more than once, we keep calling
   //gPad->GetListOfPrimitives()->Remove() as long as gPad->GetListOfPrimitives()->FindObject()
   //returns kTRUE for each line.

   TIter next_id(GetIDLines());
   KVIDLine *line;
   //check grid is drawn in pad - look for first ID line in list of primitives
   line = (KVIDLine *) next_id();
   if (!gPad->GetListOfPrimitives()->FindObject(line)) {
      Error("UnDraw", "Select pad in which grid is displayed");
      return;
   } else {
      while (gPad->GetListOfPrimitives()->FindObject(line))
         gPad->GetListOfPrimitives()->Remove(line);
   }
   //remove the rest of the lines
   while ((line = (KVIDLine *) next_id())) {
      while (gPad->GetListOfPrimitives()->FindObject(line))
         gPad->GetListOfPrimitives()->Remove(line);
   }
   TIter next_ok(GetOKLines());
   while ((line = (KVIDLine *) next_ok())) {
      while (gPad->GetListOfPrimitives()->FindObject(line))
         gPad->GetListOfPrimitives()->Remove(line);
   }
   gPad->Modified();
   gPad->Update();
}

//_______________________________________________________________________________________________//

void KVIDGrid::Print(Option_t * opt) const
{
   //Print out all lines in grid
   cout << ClassName() << " : " << GetName() << endl;
   cout << "Title : " << GetTitle() << endl;
   //print list of parameters
   fPar->Print();
   KVIDLine *line = 0;
   TIter nextOK(GetOKLines());
   while ((line = (KVIDLine *) nextOK()))
      line->Print("OK");
   TIter nextID(GetIDLines());
   while ((line = (KVIDLine *) nextID()))
      line->Print("ID");
}

//_______________________________________________________________________________________________//

void KVIDGrid::AddLine(const Char_t * type, Double_t x1, Double_t y1,
                       Double_t x2, Double_t y2, const Char_t * name,
                       const Char_t * classname)
{
   //Add a line to the grid. The type is "ID" (for an identification line) or "OK" (for a line delimiting
   //regions where identification is/isn't possible).
   //Initial coordinates of first segment are given.
   //If you want the line to be an object of a class derived from KVIDLine, give the class name
   //as argument "classname".
   //The new line is drawn in the current display, if there is one.

   TString Type(type);
   Type.ToUpper();
   if (Type == "OK")
      AddOKLine(x1, y1, x2, y2, name, classname);
   else if (Type == "ID")
      AddIDLine(x1, y1, x2, y2, name, classname);
   else
      Error("AddLine", "Unknown type : %s", type);
}

//_______________________________________________________________________________________________//

void KVIDGrid::AddLine(const Char_t * type, KVIDLine * line)
{
   //Add a line to the grid. The type is "ID" (for an identification line) or "OK" (for a line delimiting
   //regions where identification is/isn't possible).

   TString Type(type);
   Type.ToUpper();
   if (Type == "OK")
      AddOKLine(line);
   else if (Type == "ID")
      AddIDLine(line);
   else
      Error("AddLine", "Unknown type : %s", type);
}

//________________________________________________________________________________

KVIDLine *KVIDGrid::AddLine(const Char_t * type,
                            const Char_t * idline_class)
{
   //Create and add a line to the grid, returning a pointer to the new line.
   //By default the line is a KVIDLine, unless you specify a different class.
   //The type is "ID" (for an identification line) or "OK" (for a line delimiting
   //regions where identification is/isn't possible).

   KVIDLine *line = NewLine(idline_class);
   AddLine(type, line);
   return line;
}

//________________________________________________________________________________

KVIDLine *KVIDGrid::GetLine(const Char_t * type, const Char_t * name) const
{
   //Returns line of type "type" ("ID" or "OK") called "name"
   TString Type(type);
   Type.ToUpper();
   if (Type == "OK")
      return GetOKLine(name);
   else if (Type == "ID")
      return GetIDLine(name);
   else
      Error("GetLine", "Unknown type : %s", type);
   return 0;
}

//________________________________________________________________________________

KVIDLine *KVIDGrid::GetLine(const Char_t * type, UInt_t index) const
{
   //Returns line of type "type" ("ID" or "OK") by index
   TString Type(type);
   Type.ToUpper();
   if (Type == "OK")
      return GetOKLine(index);
   else if (Type == "ID")
      return GetIDLine(index);
   else
      Error("GetLine", "Unknown type : %s", type);
   return 0;
}

//________________________________________________________________________________

KVList *KVIDGrid::GetLines(const Char_t * type) const
{
   //Returns list of lines of type "type" ("ID" or "OK")
   TString Type(type);
   Type.ToUpper();
   if (Type == "OK")
      return GetOKLines();
   else if (Type == "ID")
      return GetIDLines();
   else
      Error("GetLines", "Unknown type : %s", type);
   return 0;
}

//_______________________________________________________________________________________________//

KVIDLine *KVIDGrid::FindNearestIDLine(Double_t x, Double_t y,
                                      const Char_t * position,
                                      Int_t & idx_min,
                                      Int_t & idx_max) const
{
   //General algorithm for finding the ID line the closest to a given point (x,y) by dichotomy.
   //The returned pointer is the closest line, whereas idx_min and idx_max are the indices of
   //the two closest lines between (x,y) lies. The order of the indices follows the order of the
   //sorting of the ID line list which is in turn determined by the Compare() function of the class
   //of which the ID lines are members.
   //
   //For the algorithm to work, the ID lines must be sorted according to the Compare() function defined for the
   //KVIDLine-based class of which they are members. The "position" string then corresponds to the
   //way lines are sorted in the map (vertically or horizontally, and in which direction). For this reason
   //it is essential to call the Initialise() method of the grid before using it (this will sort the lines).
   //
   //For example:
   //      Suppose we have lines of Z identification (KVIDZLine) in a map with x=E and y=dE.
   //The KVIDZline::Compare() function sorts lines in order of increasing Z, i.e. in terms of increasing
   //dE, or from bottom to top when looking at the (E,dE) map. Thus the "position" string to use is
   //"above" : if the point (E,dE) is "above" the line currently tested, the algorithm will choose a line
   //which is "higher up" i.e. has a larger Z, or in fact, has a larger index in the list of ID lines. The indices
   //idx_min and idx_max would correspond to the line immediately below and above the point, respectively.
   //       This is for a case of more-or-less horizontal lines sorted from bottom to top.
   //In a map of more-or-less vertical ID lines, with the sorting going from left to right,
   //we would use position="right". (see KVIDLine::WhereAmI for an explanation of "above", "below", etc.).

   Int_t nlines = NumberOfIDLines();
   idx_min = 0;                 //minimum index
   idx_max = nlines - 1;        // maximum index
   Int_t idx = idx_max / 2;     //current index i.e. we begin in the middle

   while (idx_max > idx_min + 1) {

      KVIDLine *line = GetIDLine(idx);
      Bool_t point_above_line = line->WhereAmI(x, y, position);

      if (point_above_line) {
         //increase index
         idx_min = idx;
         idx += (Int_t) ((idx_max - idx) / 2 + 0.5);
      } else {
         //decrease index
         idx_max = idx;
         idx -= (Int_t) ((idx - idx_min) / 2 + 0.5);
      }
   }
   //calculate distance of point to the two lines above and below
   //and return pointer of the closest one
   KVIDLine *upper = GetIDLine(idx_max);
   KVIDLine *lower = GetIDLine(idx_min);
   Int_t dummy = 0;
   return (TMath::Abs(upper->DistanceToLine(x, y, dummy)) >
           TMath::Abs(lower->DistanceToLine(x, y, dummy)) ? lower : upper);
}

//_______________________________________________________________________________________________//

void KVIDGrid::FindAxisLimits()
{
   //Calculate X/Y min/max of all lines in grid.
   KVIDLine *line;
   Double_t x, y, xmin, ymin, xmax, ymax;
   xmax = ymax = -999999;
   xmin = ymin = 999999;
   TIter nextOK(GetOKLines());
   while ((line = (KVIDLine *) nextOK())) {
      for (int i = 0; i < line->GetN(); i++) {
         line->GetPoint(i, x, y);
         xmin = (x < xmin ? x : xmin);
         xmax = (x > xmax ? x : xmax);
         ymin = (y < ymin ? y : ymin);
         ymax = (y > ymax ? y : ymax);
      }
   }
   TIter nextID(GetIDLines());
   while ((line = (KVIDLine *) nextID())) {
      for (int i = 0; i < line->GetN(); i++) {
         line->GetPoint(i, x, y);
         xmin = (x < xmin ? x : xmin);
         xmax = (x > xmax ? x : xmax);
         ymin = (y < ymin ? y : ymin);
         ymax = (y > ymax ? y : ymax);
      }
   }
   fXmin = xmin;
   fYmin = ymin;
   fXmax = xmax;
   fYmax = ymax;
   Info("FindAxisLimits()", "Xmin=%f Ymin=%f Xmax=%f Ymax=%f", GetXmin(),
        GetYmin(), GetXmax(), GetYmax());
}

//________________________________________________________________________________________//

void KVIDGrid::SetLineColor(Color_t lcolor)
{
   //Set colour of all lines in grid
   GetIDLines()->Execute("SetLineColor", Form("%d", (Int_t) lcolor));
   GetOKLines()->Execute("SetLineColor", Form("%d", (Int_t) lcolor));
}

//________________________________________________________________________________________//

void KVIDGrid::SetLineStyle(Style_t lstyle)
{
   //Set style of all lines in grid
   GetIDLines()->Execute("SetLineStyle", Form("%d", (Int_t) lstyle));
   GetOKLines()->Execute("SetLineStyle", Form("%d", (Int_t) lstyle));
}

//________________________________________________________________________________________//

void KVIDGrid::SetLineWidth(Width_t lwidth)
{
   //Set width of all lines in grid
   GetIDLines()->Execute("SetLineWidth", Form("%d", (Int_t) lwidth));
   GetOKLines()->Execute("SetLineWidth", Form("%d", (Int_t) lwidth));
}

//________________________________________________________________________________________//

TList *KVIDGrid::GetIDLinesEmbracingPoint(const Char_t * direction,
                                          Double_t x, Double_t y,
                                          Int_t & nlines) const
{
   //Creates and fills list of subset of ID lines for which IsBetweenEndPoints(x,y,direction) == kTRUE.
   //nlines = number of lines in list
   //User must delete the TList after use.

   TIter next(fIDLines);
   nlines = 0;
   KVIDLine *line;
   TList *tmp = new TList;
   while ((line = (KVIDLine *) next())) {
      if (line->IsBetweenEndPoints(x, y, direction)) {
         tmp->Add(line);
         nlines++;
      }
   }
   return tmp;
}

//________________________________________________________________________________________//

void KVIDGrid::Fit(KVTGID * _tgid)
{
   //Fit this grid using a KVTGID (Tassan-Got functional)
   //We use 1% as initial step size
   //We take the number of parameters from the KVTGID, minus 2 (the last two are always X and Y)
   //Starting points of all lines are fitted to get values of lambda and alpha

   // Initialize TMinuit via generic fitter interface
   TVirtualFitter *minuit =
       TVirtualFitter::Fitter(this, _tgid->GetNpar() - 2);

   fCurrentIDGrid = this;
   fCurrentTGID = _tgid;

   //fit starting points
   Double_t lambda, alpha, beta, g;
   FitStartingPoints(lambda, alpha, beta, g);

   minuit->SetFCN(KVIDGridfcnk0);

   //parameter limits recommended by LTG
   //    lambda  alpha beta mu nu zeta  g  pedestal-X  pedestal-Y
   Double_t vlow[] = { .25, .5, .2, .2, .1, .1, .25, 0., 0., 0., 0. };
   Double_t vhigh[] =
       { 4., 1.5, 1., 1.5, 4., 10000., 4., 1000., 1000., 1000., 1000. };
   Double_t vinit[] =
       { 0., 1., .5, 1., 1., 5000., 0., 100., 100., 100., 100. };
   vinit[0] = lambda;
   vlow[0] = .25 * lambda;
   vhigh[0] = 4. * lambda;
   vinit[1] = alpha;
   vinit[2] = beta;
   vinit[6] = g;
   vlow[6] = .25 * g;
   vhigh[6] = 4. * g;

   Bool_t A_ident = kFALSE;

   for (int ipar = 0; ipar < _tgid->GetNpar() - 2; ipar++) {

      if (strcmp(_tgid->GetParName(ipar), "Z")) {       //KVTGIDZA objects have 'parameter' for nuclear charge

         minuit->SetParameter(ipar, _tgid->GetParName(ipar), vinit[ipar],
                              vinit[ipar] * 0.01, vlow[ipar], vhigh[ipar]);

      } else {
         A_ident = kTRUE;
         break;
      }
   }
   //for Z identification, par[2]=0.5
   if (!A_ident) {
      minuit->SetParameter(2, _tgid->GetParName(2), 0.5, 0., vlow[2],
                           vhigh[2]);
      minuit->FixParameter(2);
   }

        /******FIRST FIT : keep lambda, alpha, beta and g fixed ***********************/
   minuit->FixParameter(0);
   minuit->FixParameter(1);
   minuit->FixParameter(2);
   minuit->FixParameter(6);

   //*-*-       Request FCN to read in (or generate random) data (IFLAG=1)
   Double_t arglist[100];
   arglist[0] = 1;
   minuit->ExecuteCommand("CALL FCN", arglist, 1);

   arglist[0] = 0;
   minuit->ExecuteCommand("SET PRINT", arglist, 1);
   minuit->ExecuteCommand("MIGRAD", arglist, 0);
   minuit->ExecuteCommand("MINOS", arglist, 0);

   arglist[0] = 3;
   minuit->ExecuteCommand("CALL FCN", arglist, 1);
        /******2ND FIT : all free ***********************/
   minuit->ReleaseParameter(0);
   minuit->ReleaseParameter(1);
   if (A_ident)
      minuit->ReleaseParameter(2);
   minuit->ReleaseParameter(6);

   for (int ipar = 0; ipar < _tgid->GetNpar() - 2; ipar++) {

      if (strcmp(_tgid->GetParName(ipar), "Z")) {       //KVTGIDZA objects have 'parameter' for nuclear charge

         minuit->SetParameter(ipar, _tgid->GetParName(ipar),
                              _tgid->GetParameter(ipar),
                              _tgid->GetParameter(ipar) * 0.01, vlow[ipar],
                              vhigh[ipar]);

      } else {
         A_ident = kTRUE;
         break;
      }
   }
   //*-*-       Request FCN to read in (or generate random) data (IFLAG=1)
   arglist[0] = 1;
   minuit->ExecuteCommand("CALL FCN", arglist, 1);

   arglist[0] = 0;
   minuit->ExecuteCommand("SET PRINT", arglist, 1);
   minuit->ExecuteCommand("MIGRAD", arglist, 0);
   minuit->ExecuteCommand("MINOS", arglist, 0);

   arglist[0] = 3;
   minuit->ExecuteCommand("CALL FCN", arglist, 1);
}

//________________________________________________________________________________________//

Double_t KVIDGrid::TGIDChiSquare(KVTGID * _tgid, Double_t * params)
{
   //Calculate "generalised chi-square" for this grid and the KVTGID functional (using parameters
   //in array 'params' if given, otherwise we use parameters of KVTGID).
   //We calculate the sum of the squared distances between each point of each line of the grid and
   //the corresponding line represented by the functional.

   TIter next(fIDLines);
   KVIDLine *line;
   Double_t x, y, chisqr = 0.0;
   Int_t id;
   while ((line = (KVIDLine *) next())) {

      for (int point = 0; point < line->GetN(); point++) {

         line->GetPoint(point, x, y);
         id = line->GetID();
         chisqr +=
             TMath::Power(_tgid->GetDistanceToLine(x, y, id, params), 2);

      }
   }
   return chisqr;
}

//________________________________________________________________________________________//

void KVIDGrid::fcnk0(Int_t & npar, Double_t * gin, Double_t & f,
                     Double_t * par, Int_t iflag)
{
   //Function used by TFitter to fit the grid with a KVTGID

   //calculate chi-square
   f = TGIDChiSquare(fCurrentTGID, par);

   if (iflag == 3) {
      cout << "Fit complete: chisquare=" << f << endl;
      fCurrentTGID->SetParameters(par);
      fCurrentTGID->Print();
   }
}

//________________________________________________________________________________________//

void KVIDGridfcnk0(Int_t & npar, Double_t * gin, Double_t & f,
                   Double_t * par, Int_t iflag)
{
   //wrapper for class-member function, for TVirtualFitter::SetFCN()

   fCurrentIDGrid->fcnk0(npar, gin, f, par, iflag);
}

//________________________________________________________________________________________//

void KVIDGrid::RemoveLine(const Char_t * type, const Char_t * name)
{
   //Remove and delete line from grid
   KVIDLine *line = GetLine(type, name);
   if (line)
      RemoveLine(type, line);
}

//________________________________________________________________________________________//

void KVIDGrid::RemoveLine(const Char_t * type, KVIDLine * line)
{
   //Remove and delete line from grid
   TString _typ(type);
   if (_typ == "ID" || _typ == "id")
      fIDLines->Remove(line);
   else if (_typ == "OK" || _typ == "ok")
      fOKLines->Remove(line);
}

//________________________________________________________________________________________//

void KVIDGrid::FitStartingPoints(Double_t & lambda, Double_t & alpha,
                                 Double_t & beta, Double_t & g)
{
   //Used by Fit(KVTGID*) - we need to get starting values for lambda, alpha and g by fitting delta_E at E=0
   //i.e. fitting how the y-coordinate of the first point of each line varies with Z
   //g is taken as average slopes of first segment of every line

   //fitting function
   TF1 *_fst =
       new TF1("_fst", KVTGIDFunctions::starting_points_Z, 0., 100., 2);

   //fill graph with points - calculate average slope
   g = 0.;
   TGraph *start_points = new TGraph;
   for (int i = 0; i < GetIDLines()->GetSize(); i++) {
      Double_t x, y, x1, y1;
      GetIDLine(i)->GetStartPoint(x, y);
      GetIDLine(i)->GetPoint(1, x1, y1);
      g += TMath::Abs((y1 - y) / (x1 - x));
      start_points->SetPoint(i, GetIDLine(i)->GetID(), y);
   }

   g /= GetIDLines()->GetSize();

   //initial parameters
   //lambda=1 alpha=1
   _fst->SetParameters(1., 1.);
   _fst->SetParLimits(0, 0., 5000.);    //lambda
   _fst->SetParLimits(1, 0.5, 1.5);     //alpha

   start_points->Fit(_fst, "QN");

   lambda = _fst->GetParameter(0);
   alpha = _fst->GetParameter(1);
   beta = 0.5;
   cout << "Fitted alpha = " << alpha << endl;
   cout << "Fixed beta = " << beta << endl;
   cout << "Fitted lambda = " << lambda << endl;
   cout << "Calculated slope parameter g = " << g << endl;
   delete _fst;
   delete start_points;
}

//___________________________________________________________________________________

const Char_t *KVIDGrid::GetName() const
{
   //Dynamically-generated name of grid:
   //   grid type Parameter1=value Parameter2=value ...

   Char_t par_value[512], dummy[128];
   sprintf(par_value, "%s", GetType());
   KVParameter<KVString> *par = 0;
   for( int i=0; i<fPar->GetNPar(); i++) { //loop over all parameters
      par = fPar->GetParameter(i);
      sprintf(dummy, " %s=%s", par->GetName(), par->GetVal().Data());
      strcat(par_value, dummy);
   }
   const_cast < KVIDGrid * >(this)->SetName(par_value);
   return TNamed::GetName();
}

//___________________________________________________________________________________

void KVIDGrid::Scale(TF1 *sx, TF1 *sy)
{
   //Same as KVIDGrid::Scale(Double_t sx, Double_t sy) but not implemented
	// in Write and Read AsciiFile and graphic interface for the moment.

   if ( !sx && !sy )
      return;
   if (NumberOfIDLines() > 0) {
      GetIDLines()->R__FOR_EACH(KVIDLine, Scale) (sx, sy);
   }
   if (NumberOfOKLines() > 0) {
      GetOKLines()->R__FOR_EACH(KVIDLine, Scale) (sx, sy);
   }
}

//___________________________________________________________________________________

void KVIDGrid::Scale(Double_t sx, Double_t sy)
{
   //Called by SetX/YScaleFactor methods to rescale every point of every line in the grid

   if (TMath::Abs(sx) == 1 && TMath::Abs(sy) == 1)
      return;
   if (NumberOfIDLines() > 0) {
      GetIDLines()->R__FOR_EACH(KVIDLine, Scale) (sx, sy);
   }
   if (NumberOfOKLines() > 0) {
      GetOKLines()->R__FOR_EACH(KVIDLine, Scale) (sx, sy);
   }
}

//___________________________________________________________________________________

void KVIDGrid::TestIdentification(TH2F * data, TH1F * id_real,
                                  TH2F * id_real_vs_e_res)
{
   //This method allows to test the identification capabilities of the grid using data in a TH2F.
   //We assume that 'data' contains an identification map, whose 'x' and 'y' coordinates correspond
   //to this grid. Then we loop over every bin of the histogram, perform the identification (if
   //IsIdentifiable() returns kTRUE) and fill the two histograms with the resulting identification
   //and its dependence on the 'residual energy' i.e. the 'x'-coordinate of the 'data' histogram,
   //each identification weighted by the contents of the original data bin.
   //
   //The 'identification" we represent is the result of the KVReconstructedNucleus::GetPID() method.
   //For particles identified in Z only, this is the "real Z".
   //For particles with A & Z identification, this is Z + 0.2*(A - 2*Z)

   KVReconstructedNucleus *nuc = new KVReconstructedNucleus;

   id_real->Reset();
   id_real_vs_e_res->Reset();
   Int_t tot_events = (Int_t) data->GetSum();
   Int_t events_read = 0;
   Float_t percent = 0., cumul = 10.;

   //loop over data in histo
   for (int i = 1; i <= data->GetNbinsX(); i++) {
      for (int j = 1; j <= data->GetNbinsY(); j++) {

         Stat_t poids = data->GetBinContent(i, j);
         if (poids == 0)
            continue;

         Axis_t x0 = data->GetXaxis()->GetBinCenter(i);
         Axis_t y0 = data->GetYaxis()->GetBinCenter(j);
         Axis_t wx = data->GetXaxis()->GetBinWidth(i);
         Axis_t wy = data->GetYaxis()->GetBinWidth(j);
         //If bin content ('poids') is <=1000, we perform the identification 'poids' times, each time with
         //randomly-drawn x and y coordinates inside this bin
         //If 'poids'>1000, we perform the identification 1000 times and we fill the histograms with
         //a weight poids/1000
         Double_t x, y;
         Int_t kmax = (Int_t) TMath::Min(20., poids);
         Double_t weight = (kmax == 20 ? poids / 20. : 1.);
         for (int k = 0; k < kmax; k++) {
            nuc->Clear();
            x = gRandom->Uniform(x0 - .5 * wx, x0 + .5 * wx);
            y = gRandom->Uniform(y0 - .5 * wy, y0 + .5 * wy);
            if (IsIdentifiable(x, y)) {
               Identify(x, y, nuc);
               if(AcceptIDForTest()){
                  id_real->Fill(nuc->GetPID(), weight);
                  id_real_vs_e_res->Fill(x, nuc->GetPID(), weight);
               }
            }
         }
         events_read += (Int_t) poids;
         percent = (1. * events_read / tot_events) * 100.;
         Increment((Float_t) events_read);      //sends signal to GUI progress bar
         if (percent >= cumul) {
            cout << (Int_t) percent << "\% processed" << endl;
            cumul += 10;
         }
         gSystem->ProcessEvents();
      }
   }

   delete nuc;
}

//___________________________________________________________________________________

void KVIDGrid::Increment(Float_t x)
{
   //Used by TestIdentification and KVTestIDGridDialog to send
   //signals to TGHProgressBar about the progress of the identification test
   Emit("Increment(Float_t)", x);
}

//___________________________________________________________________________________

void KVIDGrid::DrawAndAddLine(const Char_t * type,
                              const Char_t * classname)
{
   //Use this method to add lines to a grid which is drawn in the current pad gPad.
   //We wait for the user to draw the line with the graphical editor (we put the pad automatically into this mode,
   //there is no need to use the Editor toolbar).
   //Then we retrieve this TGraph, create a new KVIDLine (or 'classname' if given) and add it to
   //the 'type'' list ('type' = "OK" or "ID", "ID" is default value).
   //DrawAndAddLine() (no arguments) will add a KVIDLine to the ID list

   if (!gPad)
      return;
   //wait for user to draw line
   TGraph *gr = (TGraph *) gPad->WaitPrimitive("Graph", "PolyLine");
   //create new ID line
   KVIDLine *line = NewLine(classname);
   //copy coordinates of user's line
   line->CopyGraph(gr);
   //add line to list
   AddLine(type, line);
   //remove TGraph and draw the KVIDLine in its place
   gPad->GetListOfPrimitives()->Remove(gr);
   delete gr;
   line->Draw("PL");
}

//___________________________________________________________________________________

KVIDGrid *KVIDGrid::AddGrids(KVIDGrid * g1, Int_t id1_min, Int_t id1_max,
                             KVIDGrid * g2, Int_t id2_min, Int_t id2_max)
{
   //Static method taking lines id1_min to id2_min from grid g1 and lines id2_min to id2_max from
   //grid g2 and making a new grid containing them. Returns the pointer to the new grid.
   //
   //It is assumed that g1 and g2 are grids of the same class. The new grid will be an object of the
   //same class as g1.
   //
   //Any scaling factors are applied to the line coordinates as they are coped from their parent grids.
   //As scaling may be different in g1 and g2, the resulting grid has no scaling factor, but the coordinates
   //of all its lines result from application of the different scaling factors of the two grids.

   //make new grid
   KVIDGrid *merger = (KVIDGrid *) g1->IsA()->New();

   //loop over lines in first grid
   TIter next_id1(g1->GetLines("ID"));
   KVIDLine *line;
   while ((line = (KVIDLine *) next_id1())) {
      if (line->GetID() >= id1_min && line->GetID() <= id1_max)
         merger->AddLine("ID", (KVIDLine *) line->Clone());     //add clone of lines with right ID
   }

   //loop over lines in second grid
   TIter next_id2(g2->GetLines("ID"));
   while ((line = (KVIDLine *) next_id2())) {
      if (line->GetID() >= id2_min && line->GetID() <= id2_max)
         merger->AddLine("ID", (KVIDLine *) line->Clone());
   }

   //just in case any of the lines were highlighted...
   merger->SetLineColor(kBlack);

   return merger;
}

//___________________________________________________________________________________

void KVIDGrid::NewGridDialog(const TGWindow * p, const TGWindow * main,
                             UInt_t w, UInt_t h, Option_t * type)
{
   //create and return pointer to dialog box used by KVIDGridManagerGUI to create, modify, and
   //copy grids. by default this is a KVNewGridDialog object, but derived classes of KVIDGrid may
   //require a derived dialog box.
   new KVNewGridDialog(p, main, w, h, type, this);
}

//___________________________________________________________________________________

Bool_t KVIDGrid::AcceptIDForTest()
{
   //Used by TestIdentification.
   //The result of the identification may be excluded from the histograms of PID
   //and PID vs. Eres, depending on e.g. some status code of the identification
   //algorithm.
   //By default, this returns kTRUE (accept all), but may be overridden in child classes.
   return kTRUE;
}

//___________________________________________________________________________________

void KVIDGrid::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVIDGrid.
   // For backwards compatibility, we transform the fParameters KVList of parameters
   // that was used in versions < 4 into the new KVGenParList format

   if (R__b.IsReading()) {
      // make sure fParameters is zero before reading
      if( fParameters ) { delete fParameters; fParameters=0; }
      KVIDGrid::Class()->ReadBuffer(R__b,this);
      if(fParameters){
         //translate old parameter list
         TIter next_param(fParameters);
         KVParameter < Double_t > *par = 0;
         while ((par = (KVParameter < Double_t > *)next_param())) {
            fPar->SetValue( par->GetName(), par->GetVal() );
         }
         delete fParameters; fParameters = 0;
      }
   } else {
      KVIDGrid::Class()->WriteBuffer(R__b,this);
   }
}

//___________________________________________________________________________________

void KVIDGrid::Initialize()
{
   // General initialisation method for identification grid.
   // This method MUST be called once before using the grid for identifications.
   // The ID lines are sorted.
   // The natural line widths of all ID lines are calculated.
   
   SortIDLines();
   CalculateLineWidths();
}
