/***************************************************************************
                          KVIDChIoSi.cpp  -  description
                             -------------------
    begin                : Fri Feb 20 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVIDChIoSi.h"
#include "KVIDGChIoSi.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRACodes.h"
#include "KVINDRA.h"

ClassImp(KVIDChIoSi)
/////////////////////////////////////////////////////////////////////////////
//KVIDChIoSi
//
//Identification in ChIo-Si matrices of INDRA
//
//Identification subcodes are written in bits 12-15 of KVIDSubCodeManager
//(see KVINDRACodes):
//
// 0 KVIDChIoSi::kID_OK               "ok",
// 1 KVIDChIoSi::kID_BelowPunchThrough "point to identify below punch-through line (bruit)",
// 2 KVIDChIoSi::kID_BelowSeuilSi      "point to identify left of Si threshold line (bruit/arret ChIo?)",
// 3 KVIDChIoSi::kID_LeftOfBragg       "point to identify below Bragg curve. Z given is a Zmin",
// 4 KVIDChIoSi::kID_RightOfEmaxSi         "point to identify has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable",
// 5 KVIDChIoSi::kID_ZgtZmax           "point to identify above largest identifiable Z line. Z given is a Zmin",
// 6 KVIDChIoSi::kID_nogrid           "no grid"
//
//subcodes 1-6 are explanations of why the particle could not be identified.

KVIDChIoSi::KVIDChIoSi()
{
   fIDCode = kIDCode_ChIoSi;
   fZminCode = kIDCode_ArretChIo;
   fECode = kECode1;
   SetSubCodeManager(4, 15);
   ChIoSiGrid = 0;
	fchio=fsi=0;
}

KVIDChIoSi::~KVIDChIoSi()
{
}

//________________________________________________________________________________________//

Bool_t KVIDChIoSi::SetIDGrid(KVIDGraph * grid)
{
   // Accept this IDgrid if it is valid for this telescope
	// In case there are several grids for each telescope (GG, PG, etc.),
	// we do not return kTRUE as soon as 1 grid is found for this telescope.
   
	if(!grid->HandlesIDTelescope(this)) return kFALSE;
	
   //get run number from INDRA, if it exists (should do!), otherwise accept
   if (gIndra) {
      Int_t run = (Int_t) gIndra->GetCurrentRunNumber();
      if (!grid->GetRuns().Contains(run))
         return kFALSE;
   }
   //the grid is accepted -> Add it to the list
   fIDGrids->Add(grid);
   
   return kFALSE;
}

//________________________________________________________________________________________//

Bool_t KVIDChIoSi::Identify(KVReconstructedNucleus * nuc)
{
   //Particle identification and code setting using identification grid KVIDGChIoSi

      //identification
		Double_t chio = GetIDMapY();
		Double_t si = GetIDMapX();
		
      if (ChIoSiGrid->IsIdentifiable(si,chio)) 
         ChIoSiGrid->Identify(si,chio,nuc);
		Int_t quality = ChIoSiGrid->GetQualityCode();
		
      //set subcode from grid status
      KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;
      SetIDSubCode(irnuc->GetCodes().GetSubCodes(),quality);
      
      if(quality==kID_BelowPunchThrough){
         //point to ID was below punch-through line (bruit)
         return kFALSE;
      }
      
      // set general ID code
      irnuc->SetIDCode( kIDCode4 );
      //if point lies above Zmax line, we give Zmax as Z of particle (real Z is >= Zmax)
      //general ID code = kIDCode5 (Zmin)
      if(quality==kID_ZgtZmax){
         irnuc->SetZ( ChIoSiGrid->GetZmax() );
         irnuc->SetIDCode( kIDCode5 );
      }
      //Identified particles with subcode kID_LeftOfBragg are given
      //general ID code kIDCode5 (Zmin).
      if(quality==kID_LeftOfBragg){
         irnuc->SetIDCode( kIDCode5 );
      }      
      //unidentifiable particles with subcode kID_BelowSeuilSi are given
      //general ID code kIDCode5 (Zmin) and we estimate Zmin from energy
      //loss in ChIo
      if(quality==kID_BelowSeuilSi){
         irnuc->SetIDCode( kIDCode5 );
         irnuc->SetZ( fchio->FindZmin() );
      }      
      
   return kTRUE;
}

//________________________________________________________________________________________//

Double_t KVIDChIoSi::GetIDMapX(Option_t *)
{
   //Default X coordinate for ChIo-Si identification is KVSilicon::GetEnergy
   //unless silicon is not calibrated; then we use silicon pedestal-corrected 'PG' channel
   return (fsi->IsCalibrated()?
           fsi->GetEnergy() : (fsi->GetACQData("PG") - fsipgped));
}

Double_t KVIDChIoSi::GetIDMapY(Option_t *)
{
   //Default Y coordinate for ChIo-Si identification is KVChIo::GetEnergy
   //unless chio is not calibrated; then we use chio pedestal-corrected 'PG' channel
   return (fchio->IsCalibrated()?
           fchio->GetEnergy() : (fchio->GetACQData("PG") - fchiopgped));
}

//____________________________________________________________________________________

const Char_t *KVIDChIoSi::GetIDSubCodeString(KVIDSubCode & concat) const
{
   //Returns string explaining subcodes for this telescope

   static TString messages[] = {
      "ok",
      "point to identify below punch-through line (bruit)",
      "point to identify left of Si threshold line (bruit/arret ChIo?). Z is a Zmin.",
      "point to identify below Bragg curve. Z is a Zmin.",
      "point to identify has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable.",
      "point to identify above largest identifiable Z line. Z (Zmax) is a Zmin.",
      "no grid for identification",
   };
   Int_t code = GetIDSubCode(concat);
   if (code == -1)
      return SubCodeNotSet();
   return messages[code];
}

//____________________________________________________________________________________

void KVIDChIoSi::Initialize()
{   
   // Initialisation of telescope before identification.
   // This method MUST be called once before any identification is attempted.
   // Initialisation of grid is performed here.
   // IsReadyForID() will return kTRUE if a grid is associated to this telescope for the current run.
   
	fchio = GetDetector(1);
	fsi = GetDetector(2);
	fsipgped = fsi->GetPedestal("PG");
	fchiopgped = fchio->GetPedestal("PG");
   ChIoSiGrid = (KVIDGChIoSi *) GetIDGrid();
   if( ChIoSiGrid ){
      ChIoSiGrid->Initialize();
      SetBit(kReadyForID);
   }
   else
      ResetBit(kReadyForID);
}
