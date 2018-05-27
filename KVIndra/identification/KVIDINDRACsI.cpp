/***************************************************************************
                          KVIDINDRACsI.cpp  -  description
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

#include "KVIDINDRACsI.h"
#include "KVCsI.h"
#include "KVReconstructedNucleus.h"
#include "KVINDRACodeMask.h"
#include "TMath.h"
#include "KVIdentificationResult.h"
#include "KVDataSet.h"

ClassImp(KVIDINDRACsI)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KVIDINDRACsI
//
//Identification in CsI R-L matrices of INDRA
//
//Identification subcodes are written in bits 0-3 of KVIDSubCodeManager
//(see KVINDRACodes). They correspond to the values of KVIDGCsI::GetQualityCode()
//(see KVIDGCsI class description).
KVIDINDRACsI::KVIDINDRACsI()
{
   fIDCode = kIDCode_CsI;
   fZminCode = kIDCode_ZminCsI;
   fECode = kECode1;
   SetSubCodeManager(4, 3);
   CsIGrid = 0;
   fCsI = 0;

   fThresMin[0][0] = 1;
   fThresMax[0][0] = 2;    // protons
   fThresMin[0][1] = 2;
   fThresMax[0][1] = 6;    // deutons
   fThresMin[0][2] = 5;
   fThresMax[0][2] = 11;   // tritons
   fThresMin[1][1] = 20;
   fThresMax[1][1] = 40;   // 3He
   fThresMin[1][2] = 1;
   fThresMax[1][2] = 3;    // alphas

   /* in principle all CsI R-L telescopes can identify mass & charge */
   SetHasMassID(kTRUE);
}

KVIDINDRACsI::~KVIDINDRACsI()
{
}

//________________________________________________________________________________________//

const Char_t* KVIDINDRACsI::GetArrayName()
{
   // Name of telescope given in the form CSI_R_L_Ring-numberTelescope-number
   //where ring and telescope numbers are those of the smallest (in angular terms)
   //detector of the telescope (if both are the same size, either one will do).
   // The detectors are signified by their TYPE names i.e. KVDetector::GetType

   //in order to access angular dimensions of detectors, we need their KVTelescopes
   KVINDRATelescope* de_det = dynamic_cast<KVINDRADetector*>(GetDetector(1))->GetTelescope();
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

Bool_t KVIDINDRACsI::Identify(KVIdentificationResult* IDR, Double_t x, Double_t y)
{
   //Particle identification and code setting using identification grid KVIDGCsI* fGrid.

   IDR->SetIDType(GetType());
   IDR->IDattempted = kTRUE;

   //perform identification
   Double_t csir = (y < 0. ? GetIDMapY() : y);
   Double_t csil = (x < 0. ? GetIDMapX() : x);
   CsIGrid->Identify(csil, csir, IDR);

   // set general ID code
   IDR->IDcode = kIDCode2;

   // general ID code for gammas
   if (IDR->IDquality == KVIDGCsI::kICODE10)
      IDR->IDcode = kIDCode0;

   return kTRUE;

}

//____________________________________________________________________________________

Double_t KVIDINDRACsI::GetIDMapX(Option_t*)
{
   //X-coordinate for CsI identification map is raw "L" coder value
   return (Double_t) fCsI->GetACQData("L");
}

//____________________________________________________________________________________

Double_t KVIDINDRACsI::GetIDMapY(Option_t*)
{
   //Y-coordinate for CsI identification map is raw "R" coder value
   return (Double_t) fCsI->GetACQData("R");
}

//____________________________________________________________________________________


void KVIDINDRACsI::Initialize()
{
   // Initialisation of telescope before identification.
   // This method MUST be called once before any identification is attempted.
   // Initialisation of grid is performed here.
   // IsReadyForID() will return kTRUE if a grid is associated to this telescope for the current run,
   // or if no calibr/ident parameters are defined for the dataset

   CsIGrid = (KVIDGCsI*) GetIDGrid();
   fCsI = GetDetector(1);
   if (CsIGrid) {
      CsIGrid->Initialize();
      SetBit(kReadyForID);
   } else
      ResetBit(kReadyForID);
   if (!gDataSet->HasCalibIdentInfos()) SetBit(kReadyForID);
}


void KVIDINDRACsI::SetIdentificationStatus(KVReconstructedNucleus* n)
{
   // For filtering simulations
   // If n->GetEnergy() is above threshold for mass identification, we set
   // n->IsAMeasured(kTRUE) (and n->IsZMeasured(kTRUE)).
   // Otherwise, we just set n->IsZMeasured(kTRUE) and use the A given by
   // the mass formula for the particle
   //
   // Z-dependence of A identification:
   //    all ok above threshold if Z<=16, decreasing probability for 17<=Z<=21
   //    no A identification for Z>21
   n->SetZMeasured();

   if (n->GetA() > 5) {
      if (fCsI->GetEnergy() < 40) n->SetAMeasured();
      else n->SetZ(n->GetZ());
      return;
   }

   Bool_t okmass = gRandom->Uniform() < smootherstep(fThresMin[n->GetZ() - 1][n->GetA() - 1], fThresMax[n->GetZ() - 1][n->GetA() - 1], fCsI->GetEnergy());
   if (okmass) {
      n->SetAMeasured();
   } else
      n->SetZ(n->GetZ());
}


float KVIDINDRACsI::clamp(float x, float lowerlimit, float upperlimit)
{
   if (x < lowerlimit)
      x = lowerlimit;
   if (x > upperlimit)
      x = upperlimit;
   return x;
}

float KVIDINDRACsI::smootherstep(float edge0, float edge1, float x)
{
   // Scale, and clamp x to 0..1 range
   x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
   // Evaluate polynomial
   return x * x * x * (x * (x * 6 - 15) + 10);
}
