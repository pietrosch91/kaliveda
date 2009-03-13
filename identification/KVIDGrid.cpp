/***************************************************************************
                          KVIDGrid.cpp  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDGrid.cpp,v 1.56 2009/03/13 16:51:08 ebonnet Exp $
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
#include "TContextMenu.h"
#include "TSystem.h"
#include "TF1.h"
#include "KVIDZALine.h"
#include "KVIDCutLine.h"
#include "KVTGIDFitter.h"

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


ClassImp(KVIDGrid)

//________________________________________________________________________________

KVIDGrid::KVIDGrid()
{
   //Default constructor
   init();
}

//________________________________________________________________________________

void KVIDGrid::init()
{
   //Initialisations, used by constructors
}

//________________________________________________________________________________

KVIDGrid::~KVIDGrid()
{
}

//_______________________________________________________________________________________________//

KVIDLine *KVIDGrid::NewLine(const Char_t * idline_class)
{
   // Create a new line compatible with this grid.
	//
	// If idline_class = "id" or "ID":
	//�      create default identification line object for this grid
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

void KVIDGrid::ReadIdentifierFromAsciiFile(TString &name, TString &type, TString &cl, ifstream& gridfile)
{
	// Read in new identifier object from file
	// Backwards-compatibility fixes
	
   KVIDentifier* line = 0;
   /************ BACKWARDS COMPATIBILITY FIX *************
   	 transform all 'OK' KVIDLines into KVIDCutLines
   */
	Bool_t oldcutline=kFALSE;
   if(type=="OK"&&cl=="KVIDLine"){
   	oldcutline=kTRUE;
   }
   /************ BACKWARDS COMPATIBILITY FIX *************
   	 transform all 'ID' KVIDZLines into KVIDZALines
   */
   Bool_t zline=kFALSE;
   if(type=="ID"&&cl=="KVIDZLine"){
   	cl="KVIDZALine";
   	zline=kTRUE;
   }
   line = New( cl.Data() );
   //now use ReadAscii method of class to read coordinates and other informations
   /************ BACKWARDS COMPATIBILITY FIX *************
   	 special read method for old KVIDZLines
   */
   if(zline) ((KVIDZALine*)line)->ReadAsciiFile_KVIDZLine(gridfile);
   else line->ReadAsciiFile(gridfile);
	if(oldcutline){
		KVIDentifier *oldcut = line;
		line = new KVIDCutLine;
		line->CopyGraph(oldcut);
		delete oldcut;
	}
   if(type=="OK") line->SetName( name.Data() );
   Add(type, line);
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
   //      Suppose we have lines of Z identification (KVIDZALine) in a map with x=E and y=dE.
   //The KVIDZALine::Compare() function sorts lines in order of increasing Z, i.e. in terms of increasing
   //dE, or from bottom to top when looking at the (E,dE) map. Thus the "position" string to use is
   //"above" : if the point (E,dE) is "above" the line currently tested, the algorithm will choose a line
   //which is "higher up" i.e. has a larger Z, or in fact, has a larger index in the list of ID lines. The indices
   //idx_min and idx_max would correspond to the line immediately below and above the point, respectively.
   //       This is for a case of more-or-less horizontal lines sorted from bottom to top.
   //In a map of more-or-less vertical ID lines, with the sorting going from left to right,
   //we would use position="right". (see KVIDLine::WhereAmI for an explanation of "above", "below", etc.).

   Int_t nlines = GetNumberOfIdentifiers();
   idx_min = 0;                 //minimum index
   idx_max = nlines - 1;        // maximum index
   Int_t idx = idx_max / 2;     //current index i.e. we begin in the middle

   while (idx_max > idx_min + 1) {

      KVIDLine *line = (KVIDLine*)GetIdentifierAt(idx);
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
   KVIDLine *upper = (KVIDLine*)GetIdentifierAt(idx_max);
   KVIDLine *lower = (KVIDLine*)GetIdentifierAt(idx_min);
   Int_t dummy = 0;
   return (TMath::Abs(upper->DistanceToLine(x, y, dummy)) >
           TMath::Abs(lower->DistanceToLine(x, y, dummy)) ? lower : upper);
}

//________________________________________________________________________________________//

TList *KVIDGrid::GetIDLinesEmbracingPoint(const Char_t * direction,
                                          Double_t x, Double_t y,
                                          Int_t & nlines) const
{
   //Creates and fills list of subset of ID lines for which IsBetweenEndPoints(x,y,direction) == kTRUE.
   //nlines = number of lines in list
   //User must delete the TList after use.

   TIter next(GetIdentifiers());
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

void KVIDGrid::Initialize()
{
   // General initialisation method for identification grid.
   // This method MUST be called once before using the grid for identifications.
   // The ID lines are sorted.
   // The natural line widths of all ID lines are calculated.
   
   SortIdentifiers();
   CalculateLineWidths();
}

//___________________________________________________________________________________

void KVIDGrid::FitPanel()
{
	// GUI method, use default ID fitter object in order to fit this ID grid.
	
	KVVirtualIDFitter * fitter = KVVirtualIDFitter::GetDefaultFitter();
	fitter->SetGrid(this);
	fitter->SetPad(fPad);
	TMethod * m = fitter->IsA()->GetMethodAny("FitPanel");
	TContextMenu * cm = new TContextMenu("KVIDGrid::FitPanel", "Context menu for KVIDGrid::FitPanel");
	cm->Action(fitter,m);
	delete cm;
}

//___________________________________________________________________________________

void KVIDGrid::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVIDGrid.
	// 
   // For backwards compatibility, we transform the fParameters KVList of parameters
   // that was used in versions < 4 into the new KVGenParList format

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
		if (R__v <5){
			KVList *fParameters = 0;
			KVBase base_obj;
			base_obj.Streamer(R__b);
			SetName(base_obj.GetName());
			SetTitle(base_obj.GetName());
      	TAttLine::Streamer(R__b);
			TQObjSender monQObject;
      	monQObject.Streamer(R__b);
      	fIdentifiers->Streamer(R__b);
      	fCuts->Streamer(R__b);
      	R__b >> fXmin;
      	R__b >> fYmin;
      	R__b >> fParameters;
      	fPar->Streamer(R__b);			
      	if(fParameters){
         	//translate old parameter list
         	TIter next_param(fParameters);
         	KVParameter < Double_t > *par = 0;
         	while ((par = (KVParameter < Double_t > *)next_param())) {
            	fPar->SetValue( par->GetName(), par->GetVal() );
         	}
         	delete fParameters; fParameters = 0;
      	}
		}
		else
		{
			KVIDGrid::Class()->ReadBuffer(R__b,this);
		}
   } else {
      KVIDGrid::Class()->WriteBuffer(R__b,this);
   }
}

//___________________________________________________________________________________

void KVIDGrid::SimulateGrid(const Char_t* strz, Bool_t thickness_factor, Double_t deltaMasse, Bool_t PHD_param, Int_t npoints) 
{

   //For a given ChIo-Si ID telescope, construct a new ID grid with Z-lines
   //calculated from energy loss calculations, but with
   //      - different chio pressure if required (give -1 to use current Chio pressure)
   //      - simulation of PHD in silicon detector (withPHD=kTRUE)
   //         *** PHD of detector must be set via its KVPulseHeightDefect calibration object ***
   //         *** i.e. call KVSilicon::SetMoultonPHDParameters() method for detector in question ***
   //
   //The characteristics of the detectors are taken from the ionisation chamber and silicon
   //pad corresponding to the lowest ring and module numbers for which this grid has been
   //declared to be valid. Do not call this method before setting this. Also make sure that you perform
   //e.g. a call to gIndra->SetParameters(some_run_number) beforehand in order to ensure that
   //the gas pressure in the ionisation chamber is correct for the runs for which the grid is valid
   //(this is not done automatically).
   //
   //Lines are calculated from energy at which particle first passes ChIo and stops in silicon
   //to punch-through
   //but Silicon thickness is increased by 50% so that lines continue further than data
   //
   //To change the formula used to calculate fragment A from Z, use method SetMassFormula
   //before calculating grid.
   //
   //More points are used at the beginning than at the end of the line (logarithmic)
   //Bragg "OK"-line is also calculated and added (each point is GetBraggE and GetBraggDE for each Z)
   //
   //If this grid has X/Y scaling factors, we apply them to the results of the calculation.
	
	KVNumberList nlz(strz);
	
	Double_t save_thick=-1;
	
	if (OnlyZId()) {
		Info("SimulateGrid","only z identification grid");
		deltaMasse=0;
	}
	Double_t x_scale = GetXScaleFactor();
   Double_t y_scale = GetYScaleFactor();
   //clear old lines from grid (and scaling parameters)
   Clear();
   //set conditions of ChIo and Si in telescope
	KVIDTelescope* tel = ((KVIDTelescope*)fTelescopes.At(0));
	if(!tel) {
      Error("SimulateGrid",
            "No identification telescope associated to this grid");
      return;
   }
	if (tel->GetSize()<=1) return;
   
	KVDetector* det_eres = tel->GetDetector(2);	if (!det_eres) 	return;
	KVDetector* det_de = tel->GetDetector(1);		if (!det_de)		return;
	
	if ( det_eres->InheritsFrom("KVSilicon") || det_eres->InheritsFrom("KVCsI")  ){
		if (thickness_factor!=-1){
			save_thick = det_eres->GetThickness();
			det_eres->SetThickness(thickness_factor * save_thick);
		}
	}
	else {
		if (thickness_factor!=-1) 
		Info("SimulateGrid",
            "Thickness Factor can only be applied on silicon or cesium iodide detectors");
	}
	
	if (PHD_param)
      GetParameters()->SetValue("With PHD", 1);
   else
      GetParameters()->RemoveParameter("With PHD");

	KVNucleus part;
	
	Info("SimulateGrid",
        "Calculating dE-E grid: dE detector = %s, E detector = %s",
        det_de->GetName(), det_eres->GetName());

   KVIDLine *B_line = (KVIDLine *) Add("OK", "KVIDCutLine");
   Int_t npoi_bragg = 0;
   B_line->SetName("Bragg_line");

   nlz.Begin(); while (!nlz.End()){
		Int_t zzz = nlz.Next();
		part.SetZ(zzz);
		Int_t aref = part.GetA();
		printf("%d %d\n",zzz,aref);
		for (Int_t aaa=aref-deltaMasse; aaa<=aref+deltaMasse; aaa+=1){
      	part.SetA(aaa);
			printf("%d %d\n",aaa,aref);
			if (part.IsKnown()){
				
				printf("%d %d\n",zzz,aaa);
				
				//loop over energy
      		//first find :
      		//      ****E1 = energy at which particle passes ChIo and starts to enter Si****
      		//      E2 = energy at which particle passes Si
      		//then perform npoints calculations between these two energies and use these
      		//to construct a KVIDZLine

      		Double_t E1, E2;
      		//find E1
      		//go from 0.1 MeV to chio->GetBraggE(part.GetZ(),part.GetA()))
      		Double_t E1min = 0.1, E1max = det_de->GetBraggE(zzz,aaa);
      		E1 = (E1min + E1max) / 2.;

				while ((E1max - E1min) > 0.1) {
					printf("1ere iteration %lf>0.1\n",E1max - E1min);
         		part.SetEnergy(E1);
         		det_de->Clear();
					det_eres->Clear();

         		det_de->DetectParticle(&part);
         		det_eres->DetectParticle(&part);
         		if (det_eres->GetEnergy() > .1) {
            		//particle got through - decrease energy
            		E1max = E1;
            		E1 = (E1max + E1min) / 2.;
         		} else {
            		//particle stopped - increase energy
            		E1min = E1;
            		E1 = (E1max + E1min) / 2.;
         		}
      		}

      		//add point to Bragg line
      		Double_t dE_B = det_de->GetBraggDE(zzz, aaa);
      		Double_t E_B = det_de->GetBraggE(zzz, aaa);
      		Double_t Eres_B = det_de->GetERes(zzz, aaa, E_B);
      		B_line->SetPoint(npoi_bragg++, Eres_B, dE_B);

      		//find E2
      		//go from E1 MeV to 6000 MeV
      		Double_t E2min = E1, E2max = 6000.;
      		E2 = (E2min + E2max) / 2.;

      		while ((E2max - E2min > 0.1)) {
					printf("2ere iteration %lf>0.1\n",E2max - E2min);
         		part.SetEnergy(E2);
         		det_de->Clear();
					det_eres->Clear();
         		
					det_de->DetectParticle(&part);
         		det_eres->DetectParticle(&part);
         		if (part.GetEnergy() > .1) {
            		//particle got through - decrease energy
            		E2max = E2;
            		E2 = (E2max + E2min) / 2.;
         		} else {
            		//particle stopped - increase energy
            		E2min = E2;
            		E2 = (E2max + E2min) / 2.;
         		}
      		}
      
      		cout << "Z=" << zzz << " E1 = " << E1 << " E2 = " << E2 << endl;
      		KVIDZALine *line = (KVIDZALine *) Add("ID", "KVIDZALine");
      		line->SetZ(zzz);
				line->SetA(aaa);

      		Double_t logE1 = TMath::Log(E1);
      		Double_t logE2 = TMath::Log(E2);
      		Double_t dLog = (logE2 - logE1) / (npoints - 1.);

      		for (Int_t i = 0; i < npoints; i++) {
//                      Double_t E = E1 + i*(E2-E1)/(npoints-1.);
         		Double_t E = TMath::Exp(logE1 + i * dLog);
					printf("points %d sur %d - %lf\n",i,npoints,E);

         		Double_t Eres = 0.;
         		Int_t niter=0;
					while (Eres < 0.1 && niter<=20) {
            		det_de->Clear();
            		det_eres->Clear();
            
						part.SetEnergy(E);
            		
						det_de->DetectParticle(&part);
            		det_eres->DetectParticle(&part);
            
						Eres = det_eres->GetEnergy();
            		E += 0.1;
         		
						printf("3eme iteration %lf<0.1 - %lf\n",Eres,E);
						niter+=1;
					}
         		if (!(niter>20)){
         			Double_t dE = det_de->GetEnergy();
         			//PHD correction
         			if (PHD_param && det_de->InheritsFrom("KVSilicon")) 		
							dE -= ((KVSilicon* )det_de)->GetPHD(dE, zzz);
         			if (PHD_param && det_eres->InheritsFrom("KVSilicon")) 	
							Eres -= ((KVSilicon* )det_eres)->GetPHD(Eres, zzz);
         			
						line->SetPoint(i, Eres, dE);
      			}
				}
				printf("sort de boucle points");
			}
		}		
   }
   //reset Si thickness
	if ( det_eres->InheritsFrom("KVSilicon") || det_eres->InheritsFrom("KVCsI")  ){
		if (thickness_factor!=-1) det_eres->SetThickness(save_thick);
	}
	
	//put real pressure back if changed
   /*
	if (ChIo_press > 0.)
      chio->SetPressure(press_nom);
   */
	//if this grid has scaling factors, we need to apply them to the result
   if (x_scale != 1)
      SetXScaleFactor(x_scale);
   if (y_scale != 1)
      SetYScaleFactor(y_scale);
}
