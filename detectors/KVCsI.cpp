/***************************************************************************
$Id: KVCsI.cpp,v 1.37 2009/01/21 10:04:59 franklan Exp $
                          kvcsi.cpp  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
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
#include "Riostream.h"
#include "KVCsI.h"
#include "KVGroup.h"
#include "KVChIo.h"
#include "KVTelescope.h"
#include "KVACQParam.h"
#include "KVMaterial.h"
#include "KVINDRA.h"
#include "KVLightEnergyCsI.h"

ClassImp(KVCsI);
//_______________________________________________________________________________________
//
//Child class of KVDetector, specifically describing the
//CsI(Tl) scintillation detectors of the INDRA multidetector array.
//
//Type of detector: "CSI"
//
//Calibration:
//  Two KVLightEnergyCsI calibrators are used, one for Z=1, the other for Z>1

//_______________________________________________________________________________________

KVCsI::KVCsI()
{
   //Default ctor
   //This ctor (which in turn calls the KVDetector default ctor) must exist in
   //order for Cloning of detectors to work (as used in KVTelescope::AddDetector).
   //Do not replace this ctor by giving a default value for the argument of KVCsI(Float_t).
   //
   fSegment = 2;
   fLumiereTotale = 0.0;
   fLumTotStatus = NOT_CALCULATED;
   fCal = fCalZ1 = 0;
	fPinLaser = 0;
}

//______________________________________________________________________________
KVCsI::KVCsI(Float_t thick):KVDetector("CsI", thick)
{
   //Make a CsI detector "thick" cm long
   //Set type of detector to "CSI"
   //By default 'thick'=0

   fSegment = 2;
   fLumTotStatus = NOT_CALCULATED;
   fLumiereTotale = 0.0;
   SetType("CSI");
   fCal = fCalZ1 = 0;
	fPinLaser = 0;
}

//____________________________________________________________________________________________

KVCsI::~KVCsI()
{
}

//____________________________________________________________________________________________
KVChIo *KVCsI::GetChIo() const
{
// return pointer to ChIo corresponding to the detector
   if (GetTelescope()) {
      KVGroup *kvgr = GetTelescope()->GetGroup();
      if (kvgr) {
         KVList *dets = kvgr->GetDetectors();
         TIter next_det(dets);
         KVDetector *dd;
         while ((dd = (KVDetector *) next_det())) {
            if (dd->InheritsFrom("KVChIo"))
               return (KVChIo *) dd;
         }
      }
   }
   return 0;
}

//______________________________________________________________________________________________

Double_t KVCsI::GetLumiereTotale(Double_t rapide, Double_t lente)
{
   //Returns calculated total light output for a CsI detector from its pedestal-
   //corrected measured fast and slow components.
   //If no arguments are given, the detector's own ACQData parameters ("R" and "L")
   //are used, with pedestal subtraction.

   return Calculate(kLumiere, rapide, lente);
}

//______________________________________________________________________________________________

Double_t KVCsI::GetTauZero(Double_t rapide, Double_t lente)
{
   //Returns "tau_0" for a CsI detector. This is the time constant for the exponential
   //fall-off of the light output, i.e. i(t) ~ exp( - t / tau_0 ). See Parlog et al.
   //for details.
   //If no arguments are given, the detector's own ACQData parameters ("R" and "L")
   //are used, with pedestal subtraction.

   return Calculate(kTau, rapide, lente);
}

//______________________________________________________________________________________________

Double_t KVCsI::Calculate(UShort_t mode, Double_t rapide, Double_t lente)
{
   //Private method
   //This function is called by GetLumiereTotale and GetTauZero, the return value depends
   //on the value of 'mode' (kLumiere or kTau).
   //The 'TauZero' in the code is the variable 'x'.
   //In either case, if problems are encountered in the calculation, the value returned
   //is zero and fLumTotStatus should give information on the problem (GetStatusLumiere).

   UInt_t ring = GetRingNumber();

   //pedestal-corrected fast and slow components from raw data
   Double_t rap_corr;
   Double_t len_corr;
   rap_corr = (rapide < 0 ? GetR() : rapide);
   len_corr = (lente < 0 ? GetL() : lente);

   if (rap_corr <= 0. && len_corr <= 0.) {
      //no R/L coder values set
      fLumiereTotale = 0.0;
      return fLumiereTotale;
   }
   rap_corr -= GetPedestal("R");
   len_corr -= GetPedestal("L");

   if (rap_corr < 0 || len_corr < 0) {
      fLumTotStatus = NEGATIVE_PEDESTAL_CORRECTED_VALUE;
      return (fLumiereTotale = 0.0);
   }
   Double_t p0 = 400;
   Double_t p1 = 900;
   Double_t eps = 1.e-4;
/* Cette variable n'est pas utilisee, et ne figure pas dans e.g. le fortran de la 4eme campagne 
 Float_t pre=0.4318;
*/
   Double_t c1 = 1.5;
   Double_t c2;
   Double_t x;
   Double_t x2;
   Double_t x3;
   Double_t bx;
/****************************************************************************	
* Definition of PM rise-time constant tau
* Comment based on a note from Bernard Borderie:
*   rings 11-16 use PM bases with a different component, thus tau is 60 nsec
*   ring 16, module 5 is an exception, it has a tau of 20 nsec !!
*   rung 5, module 11 is also an exception, it has a tau of 60 nsec !!
*/
   Double_t tau = 20.;
   if (ring >= 11 && ring <= 16)
      tau = 60.;
   if (ring == 16 && GetModuleNumber() == 5)
      tau = 20.;
   if (ring == 5 && GetModuleNumber() == 11)
      tau = 60.;
/****************************************************************************/
   Double_t tau0 = 390.;
   Double_t tau1 = 1590.;
   Double_t tau2 = 3090.;

   if (ring > 3) {
      c2 = 2.2;
   } else {
      c2 = 3.3;
   }
   Double_t dx;
   Double_t rp = (len_corr / rap_corr) * (c1 / c2);
   if (rp >= 1 && rap_corr <= 10) {
      //calculation without correction for PM gain
      x = 600;
      x2 = TMath::Exp(-tau1 / x);
      x3 = TMath::Exp(-tau2 / x);
      bx = x2 - x3;
      fLumTotStatus = NO_GAIN_CORRECTION;
   } else {
      //iterative calculation of total light output
      fLumTotStatus = CALCULATED_WITH_GAIN_CORRECTION;
      x = p0 + p1 * rp;
      x = x - 100;
      Double_t at = 1. - TMath::Exp(-tau0 / tau);
      at = tau * at;
      Int_t niter = 0;
      do {
         niter++;
         Double_t xx = x * x;
         Double_t x1 = TMath::Exp(-tau0 / x);
         x2 = TMath::Exp(-tau1 / x);
         x3 = TMath::Exp(-tau2 / x);
         Double_t ax = 1. - x1;
         ax = ax - at / x;
         bx = x2 - x3;
         Double_t fx = bx / ax - rp;
         Double_t apx = -tau0 * x1;
         apx = (apx + at) / xx;
         Double_t bpx = tau1 * x2 - tau2 * x3;
         bpx = bpx / xx;
         Double_t fpx = (bpx * ax - apx * bx) / (ax * ax);
         dx = -fx / fpx;
         x = x + dx;
      } while (TMath::Abs(dx / x) >= eps && niter <= 50);

      if (niter > 50 || x <= tau) {
         fLumTotStatus = CALCULATION_NOT_CONVERGED;
         //Warning("GetLumiereTotale","Iterative calculation did not converge for %s",
         //       GetName());
         return (fLumiereTotale = 0.0);
      }
   }

   //GetLumiereTotale calling function
   if (mode == kLumiere)
      return (fLumiereTotale = len_corr * (x - tau) / (x * bx));

   //GetTauZero calling function
   return x;
}

//_______________________________________________________________________________________________

UInt_t KVCsI::GetStatusLumiere()
{
   //Status concerning the calculation of the total light output for
   //this CsI detector. The possible values have been defined in the KVCsI.h
   //header file (you can use the named constants for comparisons):
   // NOT_CALCULATED                    = 0
   // CALCULATED_WITH_GAIN_CORRECTION   = 1
   // NO_GAIN_CORRECTION                = 4
   // NEGATIVE_PEDESTAL_CORRECTED_VALUE = 15
   // CALCULATION_NOT_CONVERGED         = 12

   return fLumTotStatus;
}

//_______________________________________________________________________________________________

Bool_t KVCsI::LightIsGood()
{
   //Returns kTRUE if total light output has been successfully calculated
   //for the CsI, either with (KVCsI::GetStatusLumiere()=CALCULATED_WITH_GAIN_CORRECTION)
   //or without (KVCsI::GetStatusLumiere()=NO_GAIN_CORRECTION) iterative gain correction.

   if (GetStatusLumiere() == CALCULATED_WITH_GAIN_CORRECTION
       || GetStatusLumiere() == NO_GAIN_CORRECTION)
      return kTRUE;

   return kFALSE;
}

//_______________________________________________________________________________________________

void KVCsI::Clear(Option_t * opt)
{
//Redefinition of KVDetector::Clear - set fLumiereTotale and status to zero
   KVDetector::Clear(opt);
   fLumiereTotale = 0.0;
   fLumTotStatus = NOT_CALCULATED;
}


void KVCsI::Print(Option_t * option) const
{
   //Print info on this detector - redefinition of KVDetector::Print for option="data" to include total light
   //if option="data" the energy loss, total light and coder channel data are displayed

   if (!strcmp(option, "data")) {
      cout << ((KVCsI *) this)->GetName() << " -- H=" << ((KVCsI *) this)->
          GetLumiereTotale() << " -- E=" << ((KVCsI *) this)->
          GetEnergy() << "  ";
      TIter next(fACQParams);
      KVACQParam *acq;
      while ((acq = (KVACQParam *) next())) {
         cout << acq->GetName() << "=" << (Short_t) acq->
             GetCoderData() << "  ";
      }
      cout << endl;
      if (Reanalyse())
         cout << " ---> UP FOR REANALYSIS : ECalc = " << GetECalc() <<
             endl;
      if (BelongsToIdentifiedParticle())
         cout << "(Belongs to an identified particle)" << endl;
      if (BelongsToUnidentifiedParticle())
         cout << "(Belongs to an unidentified particle)" << endl;
   } else {
      KVDetector::Print(option);
   }
}

void KVCsI::SetACQParams()
{
   //Set acquisition parameters for this CsI.
   //Do not call before detector's name has been set.

   AddACQParam("R");
   AddACQParam("L");
   AddACQParam("T");
}

void KVCsI::SetCalibrators()
{
   //Set up calibrators for this detector. Call once name has been set.
//  Two KVLightEnergyCsI calibrators are used, one for Z=1, the other for Z>1
   fCalZ1 = new KVLightEnergyCsI(this);
   fCalZ1->SetType("Light-Energy CsI Z=1");
   AddCalibrator(fCalZ1);
   fCal = new KVLightEnergyCsI(this);
   fCal->SetType("Light-Energy CsI Z>1");
   AddCalibrator(fCal);
}

//______________________________________________________________________________

void KVCsI::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVCsI.
   // We set the pointers to the calibrator objects

   if (R__b.IsReading()) {
      KVCsI::Class()->ReadBuffer(R__b, this);
      fCalZ1 = (KVLightEnergyCsI *) GetCalibrator("Light-Energy CsI Z=1");
      fCal = (KVLightEnergyCsI *) GetCalibrator("Light-Energy CsI Z>1");
      // backwards compatibility for CsI with only one calibrator
      if( !fCal )  fCal = (KVLightEnergyCsI *) GetCalibrator("Light-Energy CsI");
   } else {
      KVCsI::Class()->WriteBuffer(R__b, this);
   }
}

//______________________________________________________________________________

Double_t KVCsI::GetCorrectedEnergy(UInt_t Z, UInt_t A, Double_t lum, Bool_t trans)
{
   //Calculate calibrated energy loss for a nucleus (Z,A) giving total light
   //output "lum". If "lum" is not given, the total light of the detector
   //calculated from the current values of the "R" and "L" acquisition
   //parameters will be used.
//  Two KVLightEnergyCsI calibrators are used, one for Z=1, the other for Z>1
   //Returns -1 in case of problems (no calibration available or light calculation
   //not valid).

   KVLightEnergyCsI* calib = 0;
   
   if( Z==1 && fCalZ1 ) calib = fCalZ1;
   else calib = fCal;
   
   if( calib && calib->GetStatus() ){
      if (lum < 0.) {
         //light not given - calculate from R and L components
         lum = GetLumiereTotale();
      } else {
         //light given as argument - force "OK" status for light
         fLumTotStatus = NO_GAIN_CORRECTION;
      }
         
      //check light calculation status
      if (LightIsGood()) {
         calib->SetZ(Z);
         calib->SetA(A);
         Double_t eloss = calib->Compute(lum);
         SetEnergy(eloss);
         return eloss;
      }
   }
   return -1.;
}

//__________________________________________________________________________________________//

Double_t KVCsI::GetLightFromEnergy(UInt_t Z, UInt_t A, Double_t E)
{
   //Calculate calibrated light output given by a nucleus (Z,A) deposing E (MeV)
   //in the detector. If E is not given, the current value of GetEnergy() is used.
   //Returns -1 in case of problems (no calibration available)

   KVLightEnergyCsI* calib = 0;
   
   if( Z==1 && fCalZ1 ) calib = fCalZ1;
   else calib = fCal;
   
   if( calib && calib->GetStatus() ){
      E = (E < 0. ? GetEnergy() : E);
      calib->SetZ(Z);
      calib->SetA(A);
      Double_t lum = calib->Invert(E);

      return lum;
   }
   return -1.;
}
