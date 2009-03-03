/*
$Id: KVIDGChIoSi.cpp,v 1.32 2009/03/03 13:36:00 franklan Exp $
$Revision: 1.32 $
$Date: 2009/03/03 13:36:00 $
$Author: franklan $
*/

#include "KVIDGChIoSi.h"
#include "KVIDZALine.h"
#include "KVIDCutLine.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRA.h"
#include "KVChIo.h"
#include "KVSilicon.h"
#include "KVNewGridRRMDialog.h"
#include "TObjString.h"
#include "KVParameter.h"
#include "TObjArray.h"
#include "TROOT.h"

ClassImp(KVIDGChIoSi)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KVIDGChIoSi
//
// Z-only identification grids for INDRA Ionisation chamber-Silicon telescopes.
// After attempting identification with method Identify, GetStatus returns
// one of the following status codes:
//
// 0 KVIDGChIoSi::kOK               "ok",
// 1 KVIDGChIoSi::k_BelowPunchThrough   "point to identify below punch-through line (bruit)",
// 2 KVIDGChIoSi::k_BelowSeuilSi      "point to identify left of Si threshold line (bruit/arret ChIo?)",
// 3 KVIDGChIoSi::k_LeftOfBragg       "point to identify below Bragg curve. Z given is a Zmin",
// 4 KVIDGChIoSi::k_RightOfEmaxSi         "point to identify has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable",
// 5 KVIDGChIoSi::k_ZgtZmax           "point to identify above largest identifiable Z line. Z given is a Zmin.",
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KVIDGChIoSi::KVIDGChIoSi()
{
   //Default constructor
   init();
}

    KVIDGChIoSi::KVIDGChIoSi(TString pattern)
{
   //constructor with a pattern
   init();
	SetPattern(pattern);
}

void KVIDGChIoSi::init()
{
   //default initialisations
   //SetOnlyZId(kTRUE) is called
	fStatus = kOK;
   fBragg = fPunch = fSeuil = fEmaxSi = 0;
   SetOnlyZId(kTRUE);
}


KVIDGChIoSi::~KVIDGChIoSi()
{
   //Dtor.
}

//______________________________________________________________________________________________________//

Bool_t KVIDGChIoSi::IsIdentifiable(Double_t x, Double_t y) const
{
   // Sets fStatus for particle depending on its position in the grid.
   //
   // Returns kTRUE if the particle corresponding to point (x=esi,y=echio) in the ChIo-Si map
   // is identifiable i.e. if it is contained within the region delimited by
   //      - the line indicating the limit of particles stopping in the ChIo (seuil_si)
   //      - the line indicating the limit of particles punching through to the CsI (punch_through)
   //      - the line of the largest Z identified in the map (zmaxline)
   //

   Bool_t can_id = kTRUE;
   const_cast < KVIDGChIoSi * >(this)->fStatus = kOK;
   //must be above punch through line
   if (fPunch) {
      can_id = fPunch->WhereAmI(x, y, "above");
      if (!can_id) {
         const_cast < KVIDGChIoSi * >(this)->fStatus = k_BelowPunchThrough;
         return kFALSE;
      };
   }
   //must be right of arret chio/seuil si line
   if (fSeuil) {
      can_id = fSeuil->WhereAmI(x, y, "right");
      if (!can_id) {
         const_cast < KVIDGChIoSi * >(this)->fStatus = k_BelowSeuilSi;
         return kFALSE;
      };
   }
   //must be left of Emax_Si line
   if (fEmaxSi) {
      can_id =  fEmaxSi->WhereAmI(x, y, "left");
      if (!can_id) {
         const_cast < KVIDGChIoSi * >(this)->fStatus = k_RightOfEmaxSi;
         return kFALSE;
      };
   }
   //must be below max Z line
   can_id =fZMaxLine->WhereAmI(x, y, "below");
   if (!can_id) {
      const_cast < KVIDGChIoSi * >(this)->fStatus = k_ZgtZmax;
      return kFALSE;
   };
   //particles below (left of) Bragg line are identified, but their Z is a Zmin
   if (fBragg) {
      if (!fBragg->WhereAmI(x, y, "right")) {
         const_cast < KVIDGChIoSi * >(this)->fStatus = k_LeftOfBragg;
			return kTRUE;
      };
   }
   return can_id;
}

//_______________________________________________________________________________________________//

void KVIDGChIoSi::MakeELossGrid(UInt_t Zmax, Bool_t withPHD,
                                Double_t ChIo_press, Int_t npoints)
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

   Double_t x_scale = GetXScaleFactor();
   Double_t y_scale = GetYScaleFactor();
   //clear old lines from grid (and scaling parameters)
   Clear();
   //set conditions of ChIo and Si in telescope
	KVIDTelescope* tel = ((KVIDTelescope*)fTelescopes.At(0));
	if(!tel) {
      Error("MakeELossGrid",
            "No identification telescope associated to this grid");
      return;
   }
   KVSilicon *si = (KVSilicon *) tel->GetDetector(2);
   if (!si) {
      Error("MakeELossGrid",
            "No silicon detector !");
      return;
   }
   //increase Si thickness by 50%
   Double_t Si_thick = si->GetThickness();
   si->SetThickness(1.5 * Si_thick);

   if (withPHD)
      GetParameters()->SetValue("With PHD", 1);
   else
      GetParameters()->RemoveParameter("With PHD");

   KVChIo *chio = si->GetChIo();
   if (!chio) {
      Error("MakeELossGrid", "No ChIo in front of %s", si->GetName());
      return;
   }
   //nominal pressure of ChIo
   Double_t press_nom = chio->GetPressure();
   //change pressure if required
   if (ChIo_press > 0.)
      chio->SetPressure(ChIo_press);
   //keep actual ChIo pressure used for calculation to write in header of ascii file
   GetParameters()->SetValue("ChIo pressure", chio->GetPressure());

   //loop over Z
   KVNucleus part;
   //set mass formula used for calculating A from Z
   part.SetMassFormula(GetMassFormula());

   Info("MakeELossGrid",
        "Calculating dE-E grid: dE detector = %s, E detector = %s",
        chio->GetName(), si->GetName());

   KVIDLine *B_line = (KVIDLine *) Add("OK", "KVIDCutLine");
   Int_t npoi_bragg = 0;
   B_line->SetName("Bragg_line");

   for (int z = 1; z <= (int) Zmax; z++) {
      part.SetZ(z);

      //loop over energy
      //first find :
      //      ****E1 = energy at which particle passes ChIo and starts to enter Si****
      //      E2 = energy at which particle passes Si
      //then perform npoints calculations between these two energies and use these
      //to construct a KVIDZLine

      Double_t E1, E2;
      //find E1
      //go from 0.1 MeV to chio->GetBraggE(part.GetZ(),part.GetA()))
      Double_t E1min = 0.1, E1max = chio->GetBraggE(part.GetZ(),part.GetA());
      E1 = (E1min + E1max) / 2.;

      while ((E1max - E1min) > 0.1) {

         part.SetEnergy(E1);
         si->Clear();
         chio->DetectParticle(&part);
         si->DetectParticle(&part);
         if (si->GetEnergy() > .1) {
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
      Double_t dE_B = chio->GetBraggDE(z, part.GetA());
      Double_t E_B = chio->GetBraggE(z, part.GetA());
      Double_t Esi_B = chio->GetERes(z, part.GetA(), E_B);
      B_line->SetPoint(npoi_bragg++, Esi_B, dE_B);

      //find E2
      //go from E1 MeV to 6000 MeV
      Double_t E2min = E1, E2max = 6000.;
      E2 = (E2min + E2max) / 2.;

      while ((E2max - E2min > 0.1)) {

         part.SetEnergy(E2);
         chio->DetectParticle(&part);
         si->DetectParticle(&part);
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
      
      cout << "Z=" << z << " E1 = " << E1 << " E2 = " << E2 << endl;
      KVIDZALine *line = (KVIDZALine *) Add("ID", "KVIDZALine");
      line->SetZ(z);

      Double_t logE1 = TMath::Log(E1);
      Double_t logE2 = TMath::Log(E2);
      Double_t dLog = (logE2 - logE1) / (npoints - 1.);

      for (int i = 0; i < npoints; i++) {

//                      Double_t E = E1 + i*(E2-E1)/(npoints-1.);
         Double_t E = TMath::Exp(logE1 + i * dLog);

         Double_t Esi = 0.;
         while (Esi < 0.1) {
            chio->Clear();
            si->Clear();
            part.SetEnergy(E);
            chio->DetectParticle(&part);
            si->DetectParticle(&part);
            Esi = si->GetEnergy();
            E += 0.1;
         }
         Double_t Echio = chio->GetEnergy();
         //PHD correction
         if (withPHD)
            Esi -= si->GetPHD(Esi, z);
         line->SetPoint(i, Esi, Echio);
      }
   }
   //reset Si thickness
   si->SetThickness(Si_thick);
   //put real pressure back if changed
   if (ChIo_press > 0.)
      chio->SetPressure(press_nom);
   //if this grid has scaling factors, we need to apply them to the result
   if (x_scale != 1)
      SetXScaleFactor(x_scale);
   if (y_scale != 1)
      SetYScaleFactor(y_scale);
}

//___________________________________________________________________________________//
void KVIDGChIoSi::NewGridDialog(const TGWindow * p, const TGWindow * main,
                                UInt_t w, UInt_t h, Option_t * type)
{
   //create and return pointer to dialog box used by KVIDGridManagerGUI to create, modify, and
   //copy grids. for ChIo-Si grids it is a KVNewGridRRMDialog object
   new KVNewGridRRMDialog(p, main, w, h, type, this);
}

//___________________________________________________________________________________

void KVIDGChIoSi::Initialize()
{
   // General initialisation method for identification grid.
   // This method MUST be called once before using the grid for identifications.
   // The ID lines are sorted.
   // The natural line widths of all ID lines are calculated.
   // The line with the largest Z (Zmax line) is found.
   // Pointers to different 'OK lines' are initialised
   
   KVIDZAGrid::Initialize();
   fSeuil = (KVIDLine*)GetCut("Seuil_Si");
   fBragg = (KVIDLine*)GetCut("Bragg_line");
   fPunch = (KVIDLine*)GetCut("Punch_through");
   fEmaxSi = (KVIDLine*)GetCut("Emax_Si");
}

//___________________________________________________________________________________

void KVIDGChIoSi::BackwardsCompatibilityFix()
{
	// Called after reading a grid from an ascii file.
	// Tries to convert information written by an old version of the class:
	//
	//<PARAMETER> Ring min=...  ----> <PARAMETER> IDTelescopes=...
	//<PARAMETER> Ring max=...
	//<PARAMETER> Mod min=...
	//<PARAMETER> Mod max=...
	
	KVIDZAGrid::BackwardsCompatibilityFix();
	if( fPar->HasParameter("IDTelescopes") ) return;
	if( fPar->HasParameter("Rings") && gIndra ){
		KVNumberList Rings(fPar->GetStringValue("Rings"));
		KVNumberList Modules(fPar->GetStringValue("Modules"));
		int r,m;
		Rings.Begin();
		while( !Rings.End() ){
			Modules.Begin();r=Rings.Next();
			while( !Modules.End() ){
				m=Modules.Next();
				KVIDTelescope* id = gIndra->GetIDTelescope( Form("CI_SI_%02d%02d", r,m) );
				if(id) AddIDTelescope(id);
			}
		}
		WriteParameterListOfIDTelescopes();
		fPar->RemoveParameter("Rings");
		fPar->RemoveParameter("Modules");
	}
   fSeuil = (KVIDLine*)GetCut("Seuil_Si");
   fBragg = (KVIDLine*)GetCut("Bragg_line");
   fPunch = (KVIDLine*)GetCut("Punch_through");
   fEmaxSi = (KVIDLine*)GetCut("Emax_Si");
	if(fSeuil) ((KVIDCutLine*)fSeuil)->SetAcceptedDirection("right");
	if(fBragg) ((KVIDCutLine*)fBragg)->SetAcceptedDirection("right");
	if(fPunch) ((KVIDCutLine*)fPunch)->SetAcceptedDirection("above");
	if(fEmaxSi) ((KVIDCutLine*)fEmaxSi)->SetAcceptedDirection("left");
	SetVarY("CI-E/PG/GG");
	SetVarX("SI-E/PG/GG");
	SetOnlyZId();
}
