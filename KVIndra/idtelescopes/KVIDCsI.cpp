/***************************************************************************
                          KVIDCsI.cpp  -  description
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

#include "KVIDCsI.h"
#include "KVCsI.h"
#include "KVIDGCsI.h"
#include "KVReconstructedNucleus.h"
#include "KVINDRACodes.h"
#include "KVINDRAReconNuc.h"
#include "KVIDCsIRLLine.h"
#include "KVINDRA.h"
#include "TMath.h"

ClassImp(KVIDCsI)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KVIDCsI
//
//Identification in CsI R-L matrices of INDRA
//
//Identification subcodes are written in bits 0-3 of KVIDSubCodeManager
//(see KVINDRACodes). They correspond to the values of KVIDGCsI::GetQualityCode()
//(see KVIDGCsI class description).
    KVIDCsI::KVIDCsI()
{
   fIDCode = kIDCode_CsI;
   fZminCode = kIDCode_ZminCsI;
   fECode = kECode1;
   SetSubCodeManager(4, 3);
   CsIGrid = 0;
	fCsI = 0;
}

KVIDCsI::~KVIDCsI()
{
}

//________________________________________________________________________________________//

const Char_t *KVIDCsI::GetArrayName()
{
   // Name of telescope given in the form CSI_R_L_Ring-numberTelescope-number
   //where ring and telescope numbers are those of the smallest (in angular terms)
   //detector of the telescope (if both are the same size, either one will do).
   // The detectors are signified by their TYPE names i.e. KVDetector::GetType

   //in order to access angular dimensions of detectors, we need their KVTelescopes
   KVTelescope *de_det = GetDetector(1)->GetTelescope();
   UInt_t ring, mod;
   ring = de_det->GetRingNumber();
   mod = de_det->GetNumber();
   TString dummy;
   dummy.Form("%s_R_L", GetDetector(1)->GetType());
   SetType(dummy.Data());
   dummy.Form("%s_%02d%02d", GetType(), ring, mod);
   SetName(dummy.Data());

   return fName.Data();
}

//________________________________________________________________________________________//

Bool_t KVIDCsI::Identify(KVReconstructedNucleus * nuc)
{
   //Particle identification and code setting using identification grid KVIDGCsI* fGrid.

      //perform identification
      KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;
      Double_t csir = GetIDMapY();
      Double_t csil = GetIDMapX();
      CsIGrid->Identify(csil, csir, irnuc);

      //set subcode in particle
      SetIDSubCode(irnuc->GetCodes().GetSubCodes(), CsIGrid->GetQualityCode());

      //ID totally unsuccessful if ICode=8
      if (CsIGrid->GetQualityCode() == KVIDGCsI::kICODE8)
         return kFALSE;

      //ID should be attempted in preceding telescope if ICode=6 or 7
      if (CsIGrid->GetQualityCode() == KVIDGCsI::kICODE6
          || CsIGrid->GetQualityCode() == KVIDGCsI::kICODE7)
         return kFALSE;

      // set general ID code
      irnuc->SetIDCode( kIDCode2 );

      // general ID code for gammas
      if (CsIGrid->GetQualityCode() == KVIDGCsI::kICODE10)
         irnuc->SetIDCode(kIDCode0);

      return kTRUE;

}

//____________________________________________________________________________________

Double_t KVIDCsI::GetIDMapX(Option_t * opt)
{
   //X-coordinate for CsI identification map is raw "L" coder value
   return (Double_t) fCsI->GetACQData("L");
}

//____________________________________________________________________________________

Double_t KVIDCsI::GetIDMapY(Option_t * opt)
{
   //Y-coordinate for CsI identification map is raw "R" coder value
   return (Double_t) fCsI->GetACQData("R");
}

//____________________________________________________________________________________

const Char_t *KVIDCsI::GetIDSubCodeString(KVIDSubCode & concat) const
{
   //Returns string explaining subcodes for this telescope

   static TString messages[] = {
      "ok",
      "Z ok, mais les masses superieures a A sont possibles",
      "Z ok, mais les masses inferieures a A sont possibles",
      "Z ok, mais les masses superieures ou inferieures a A sont possibles",
      "Z ok, masse hors limite superieure ou egale a A",
      "Z ok, masse hors limite inferieure ou egale a A",
      "au-dessus de la ligne fragment, Z est alors un Zmin",
      "a gauche de la ligne fragment, Z est alors un Zmin et le plus probable",
      "Z indetermine ou (x,y) hors limites",
      "pas de lignes pour ce module",
      "gamma"
   };
   Int_t code = GetIDSubCode(concat);
   if (code == -1)
      return SubCodeNotSet();
   return messages[code];
}

//____________________________________________________________________________________

void KVIDCsI::Initialize()
{
   // Initialisation of telescope before identification.
   // This method MUST be called once before any identification is attempted.
   // Initialisation of grid is performed here.
   // IsReadyForID() will return kTRUE if a grid is associated to this telescope for the current run.

   CsIGrid = (KVIDGCsI *) GetIDGrid();
	fCsI = GetDetector(1);
   if( CsIGrid ) {
      CsIGrid->Initialize();
      SetBit(kReadyForID);
   }
   else
      ResetBit(kReadyForID);
}
