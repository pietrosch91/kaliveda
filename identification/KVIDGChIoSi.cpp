/*
$Id: KVIDGChIoSi.cpp,v 1.35 2009/05/05 15:57:52 franklan Exp $
$Revision: 1.35 $
$Date: 2009/05/05 15:57:52 $
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

//______________________________________________________________________________

void KVIDGChIoSi::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVIDGChIoSi.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
		if (R__v < 6) {
      	R__v = R__b.ReadVersion(&R__s, &R__c);// read version of KVIDZGrid
			if (R__v != 1){
				Warning("Streamer", "Reading KVIDZGrid with version=%d", R__v);
			}
      	KVIDGrid::Streamer(R__b);
      	R__b >> fZMax;
      	KVINDRARRMValidator rrm;
			rrm.Streamer(R__b);
      	fPattern.Streamer(R__b);
      	R__b >> fBragg;
      	R__b >> fPunch;
      	R__b >> fSeuil;
      	R__b >> fEmaxSi;
			Bool_t fIgnorePunchThrough;
      	R__b >> fIgnorePunchThrough;
		}
		else
		{
      	KVIDGChIoSi::Class()->ReadBuffer(R__b,this,R__v,R__s,R__c);
		}
   } else {
      KVIDGChIoSi::Class()->WriteBuffer(R__b,this);
   }
}

