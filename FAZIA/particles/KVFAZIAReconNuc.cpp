/***************************************************************************
$Id: KVFAZIAReconNuc.cpp,v 1.61 2009/04/03 14:28:37 franklan Exp $
                          kvdetectedparticle.cpp  -  description
                             -------------------
    begin                : Thu Oct 10 2002
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
#include "TBuffer.h"
#include "TEnv.h"
#include "KVDataSet.h"
#include "KVFAZIAReconNuc.h"
#include "KVList.h"
#include "TCollection.h"
#include "KVDetector.h"
#include "KVMultiDetArray.h"
#include "KVFAZIADetector.h"
#include "KVIDZAGrid.h"
#include "KVLightEnergyCsIFull.h"
#include "KVLightEnergyCsI.h"
#include "KVIDGCsI.h"

using namespace std;

ClassImp(KVFAZIAReconNuc);

////////////////////////////////////////////////////////////////////////////
//KVFAZIAReconNuc
//
//Nuclei reconstructed from data measured in the FAZIA array.
//Most useful methods are already defined in parent classes KVReconstructedNucleus,
//KVNucleus and KVParticle.
////////////////////////////////////////////////////////////////////////////


void KVFAZIAReconNuc::init()
{
   //default initialisations
   if (gDataSet)
      SetMassFormula(UChar_t(gDataSet->GetDataSetEnv("KVFAZIAReconNuc.MassFormula", Double_t(kEALMass))));
   fECSI = fESI1 = fESI2 = 0.;
}

KVFAZIAReconNuc::KVFAZIAReconNuc()
{
   //default ctor
   init();
}

KVFAZIAReconNuc::KVFAZIAReconNuc(const KVFAZIAReconNuc& obj):
   KVReconstructedNucleus()
{
   //copy ctor
   init();
   obj.Copy(*this);

}

KVFAZIAReconNuc::~KVFAZIAReconNuc()
{
   //dtor
   init();
}

void KVFAZIAReconNuc::Copy(TObject& obj) const
{
   //
   //Copy this to obj
   //
   KVReconstructedNucleus::Copy(obj);
}


void KVFAZIAReconNuc::Print(Option_t*) const
{
   // Print information on particle

   if (IsIdentified()) {

      cout << " =======> ";
      cout << " Z=" << GetZ() << " A=" << ((KVFAZIAReconNuc*) this)->
           GetA();
      if (((KVFAZIAReconNuc*) this)->IsAMeasured()) cout << " Areal=" << ((KVFAZIAReconNuc*) this)->GetRealA();
      else cout << " Zreal=" << GetRealZ();

   } else {
      cout << "(unidentified)" << endl;
   }
   if (IsCalibrated()) {
      cout << " Total Energy = " << GetEnergy() << " MeV,  Theta=" << GetTheta() << " Phi=" << GetPhi() << endl;
      cout << "    Target energy loss correction :  " << GetTargetEnergyLoss() << " MeV" << endl;

   } else {
      cout << "(uncalibrated)" << endl;
   }

   cout << "Analysis : ";
   switch (GetStatus()) {
      case 0:
         cout <<
              "Particle alone in group, or identification independently of other"
              << endl;
         cout << "particles in group is directly possible." << endl;
         break;

      default:
         cout << "Particle status code" << GetStatus() << endl;
         break;
   }

   cout <<
        "-------------------------------------------------------------------------------"
        << endl;
}

//________________________________________________________________________________________

void KVFAZIAReconNuc::Clear(Option_t* t)
{
   //reset nucleus' properties
   KVReconstructedNucleus::Clear(t);
   init();

}

KVFAZIADetector* KVFAZIAReconNuc::Get(const Char_t* label) const
{
   // Access one of the detectors hit by this particle: "SI1", "SI2", or "CSI"
   // If the requested detector type was not hit (i.e. requesting "CSI" for a particle
   // which stopped in "SI1" or "SI2") then a null pointer will be returned.

   KVFAZIADetector* det = (KVFAZIADetector*) GetDetectorList()->FindObjectByLabel(label);
   return det;
}

KVFAZIADetector* KVFAZIAReconNuc::GetCSI() const
{
   // Access CSI detector hit by particle. If particle stopped before CSI, returns null pointer.
   return Get("CSI");
}
KVFAZIADetector* KVFAZIAReconNuc::GetSI1() const
{
   // Access SI1 detector hit by particle.
   return Get("SI1");
}
KVFAZIADetector* KVFAZIAReconNuc::GetSI2() const
{
   // Access SI2 detector hit by particle. If particle stopped in SI1, returns null pointer.
   return Get("SI2");
}

Int_t KVFAZIAReconNuc::GetIndex() const
{
   // Return index of dectector in which particle stopped
   // (see KVFaziaDetector::GetIndex for meaning of index).
   // If no stopping detector defined (weird?), returns -1.

   KVFAZIADetector* det(nullptr);
   if (!(det = (KVFAZIADetector*)GetStoppingDetector())) return -1;
   return det->GetIndex();
}


Bool_t KVFAZIAReconNuc::StoppedIn(const Char_t* label) const
{
   //Returns kTRUE if particle stopped in the given detector: "SI1", "SI2" or "CSI"
   return (GetStoppingDetector() == Get(label));
}

Int_t KVFAZIAReconNuc::GetIdentifierOfStoppingDetector() const
{
   // Return the identifier of the detector in which particle stopped.
   // For the definition of possible returned values, see KVFAZIADetector::GetIdentifier

   return ((KVFAZIADetector*)GetStoppingDetector())->GetIdentifier();
}

Bool_t KVFAZIAReconNuc::StoppedInSI2() const
{
   return StoppedIn("SI2");
}
Bool_t KVFAZIAReconNuc::StoppedInSI1() const
{
   return StoppedIn("SI1");
}
Bool_t KVFAZIAReconNuc::StoppedInCSI() const
{
   return StoppedIn("CSI");
}



void KVFAZIAReconNuc::Identify()
{
   // Try to identify this nucleus by calling the Identify() function of each
   // ID telescope crossed by it, starting with the telescope where the particle stopped, in order
   //      -  attempt identification only in ID telescopes containing the stopping detector
   //      -  only telescopes which have been correctly initialised for the current run are used,
   //         i.e. those for which KVIDTelescope::IsReadyForID() returns kTRUE.
   // This continues until a successful identification is achieved or there are no more ID telescopes to try.
   // The identification code corresponding to the identifying telescope is set as the identification code of the particle.


   KVList* idt_list = GetStoppingDetector()->GetIDTelescopes();
   KVIdentificationResult* IDR = 0;
   Int_t idnumber = 1;
   if (idt_list && idt_list->GetSize() > 0) {

      KVIDTelescope* idt;
      TIter next(idt_list);
      while ((idt = (KVIDTelescope*) next())) { // && !IsIdentified()) {
         if (StoppedInSI1() && !strcmp(idt->GetType(), "Si-Si")) continue; // why ?
         if (StoppedInSI2() && !strcmp(idt->GetType(), "Si-CsI")) continue; // why ?
         IDR = GetIdentificationResult(idnumber);
         IDR->SetName(idt->GetName());
         IDR->SetType(idt->GetType());
         if (idt->IsReadyForID()) { // is telescope able to identify for this run ?

            IDR->IDattempted = kTRUE;
            IDR->IDOK = kFALSE;
            idt->Identify(IDR);
            if (IDR->IDOK) {  //Correspond to Quality code <=3
               SetIdentification(IDR);
               SetIdentifyingTelescope(idt);
               SetIsIdentified();
               //return;
            } else {
               SetIdentification(IDR);
               SetIdentifyingTelescope(idt);
               //SetIDCode(0);
               //SetZandA(0, 0);
               //SetIsIdentified();
            }
         } else {
            IDR->IDattempted = kFALSE;
         }
         idnumber += 1;
      }

      KVIdentificationResult partID;
      Bool_t ok = kFALSE;
      if (StoppedInSI1()) {
         ok = CoherencySi(partID);
      } else if (StoppedInSI2()) {
         ok = CoherencySiSi(partID);
      } else if (StoppedInCSI()) {
         ok = CoherencySiCsI(partID);
      }

      if (ok) {
         SetIsIdentified();
         KVIDTelescope* idt = (KVIDTelescope*)GetIDTelescopes()->FindObjectByType(partID.GetType());
         if (!idt) {
            Warning("Identify", "cannot find ID telescope with type %s", partID.GetType());
            GetIDTelescopes()->ls();
            partID.Print();
         }
         SetIdentifyingTelescope(idt);
         SetIdentification(&partID);
      }

   }

}

Bool_t KVFAZIAReconNuc::CoherencySi(KVIdentificationResult& theID)
{
   KVIdentificationResult* IDsi = GetIdentificationResult("SiPSA");
   if (IDsi && IDsi->IDOK) {
      theID = *IDsi;
      return kTRUE;
   } else return kFALSE;
}

Bool_t KVFAZIAReconNuc::CoherencySiSi(KVIdentificationResult& theID)
{
   KVIdentificationResult* IDsisi = GetIdentificationResult("Si-Si");
   KVIdentificationResult* IDsi = GetIdentificationResult("SiPSA");
   if (IDsisi && IDsisi->IDOK)    {
      theID = *IDsisi;
      return kTRUE;
   } else if (IDsi && IDsi->IDOK) {
      theID = *IDsi;
      return kTRUE;
   } else return kFALSE;
}

Bool_t KVFAZIAReconNuc::CoherencySiCsI(KVIdentificationResult& theID)
{
   KVIdentificationResult* IDcsi = GetIdentificationResult("CsI");
   KVIdentificationResult* IDsicsi = GetIdentificationResult("Si-CsI");
   fCoherent = kTRUE;
   fPileup = kFALSE;

   // the following procedure has been copied/adapted from KVINDRAReconNuc::CoherencySiCsI()

   // Unsuccessful/no CsI id attempt with successful Si-CsI id
   // Then use Si-CsI identification result
   if (IDsicsi->IDOK && !IDcsi->IDOK) {
      theID = *IDsicsi;
      return kTRUE;
   }

   // check coherency of CsI-R/L and Si-CsI identifications
   if (IDcsi->IDOK) {
      // gammas
      if (IDcsi->IDcode == 0) {
         theID = *IDcsi;
         return kTRUE;
      }

      // I don't know how to treat neutrons for now... two options:
      // - set a pedestal cut on Si-CsI matrices
      // - identify them due to the incoherency between calibration and identification

      /*
      // Neutrons have no energy loss in Si detector (pedestal), have a reaction in the CsI and create a Z=1
      // or Z=2 which is identified in CsI R-L,  while they show up in Si-CsI maps as a horizontal
      // band around the Si pedestal for low energies (energies where proton dE is significantly larger than
      // the pedestal).
      if ((IDcsi->Z == 1 || IDcsi->Z == 2) && GetSi()) {
         if (IDsicsi->deltaEpedestal == KVIdentificationResult::deltaEpedestal_UNKNOWN) {
            // no explicit treatment of 'neutron-like' particles with a cut in Si-CsI id grid
            // First we check that we are in the domain where proton dE can be distinguished from pedestal.
            // If so, if the measured dE is below [ped + factor*(dE_exp - ped)], then we label the particle as a neutron.
            // 'factor' depends on the Si-CsI telescope: if it has mass identification, factor=0.3; if not, factor=0.1
            // (these empirical values correspond to 5th campaign data)
            KVIDTelescope* idt = (KVIDTelescope*)GetIDTelescopes()->FindObjectByType(IDsicsi->GetType());
            if (idt) {
               Double_t ped = idt->GetPedestalY();
               Int_t status;
               Double_t dE_exp = idt->GetMeanDEFromID(status, 1, 1);
               if (status == KVIDTelescope::kMeanDE_OK) { // proton/Z=1 line exists, and we are in its energy range
                  if (dE_exp > ped + 5.) { // arbitrary choice, must have expected dE at least 5 channels above pedestal

                     // if Si-CsI has no isotopic identification, reduce factor
                     Double_t factor = (idt->HasMassID() ? 0.3 : 0.1);
                     if (idt->GetIDMapY() < (ped + factor * (dE_exp - ped))) {
                        theID = *IDsicsi;
                        theID.IDOK = kTRUE;
                        theID.Zident = kTRUE;
                        theID.Aident = kTRUE;
                        theID.Z = 0;
                        theID.A = 1;
                        theID.IDcode = kIDCode1; // general code for neutrons
                        return kTRUE;
                     }
                  }
               }
            }
         } else if (IDsicsi->deltaEpedestal == KVIdentificationResult::deltaEpedestal_YES) {
            // explicit treatment of 'neutron-like' particles with a cut in Si-CsI id grid
            theID = *IDsicsi;
            theID.IDOK = kTRUE;
            theID.Zident = kTRUE;
            theID.Aident = kTRUE;
            theID.Z = 0;
            theID.A = 1;
            theID.IDcode = kIDCode1; // general code for neutrons
            return kTRUE;
         }
      }
      */

      // We check the coherency of the mass and charge identifications
      // If a successful Si-CsI identification is available we check:
      //   if Si-CsI gives A & Z - must have same Z, A within +/-1 unit
      //                                      if Z or A smaller => incoherency : pile-up of two particles in CsI ?
      //                                               or bad signal from Si detector (dead zones) ?
      //                                      if Z or A larger, CsI identification is good,
      //                                      assume another particle stopped in Si (identify in ChIo-Si)
      //   if Si-CsI gives just Z - must have same Z
      //                                      if Z smaller => incoherency : pile-up of two particles in CsI ?
      //                                               or bad signal from Si detector (dead zones) ?
      //                                      if Z larger => CsI identification is good,
      //                                      assume another particle stopped in Si (identify in ChIo-Si)
      //
      // If CsI identification gives code KVIDGCsI::kICode1 ou KVIDGCsI::kICode3 and the
      // Si-CsI gives the same Z but A = Acsi + 1, we use the Si-CsI identification.
      //
      // If CsI identification gives code KVIDGCsI::kICode2 ou KVIDGCsI::kICode3 and the
      // Si-CsI gives the same Z but A = Acsi - 1, we use the Si-CsI identification.
      //
      // N.B. if CsI-R/L identification gives "8Be" (2 alpha particles) then there are two correct possibilities:
      //     1) Si-CsI identification gives 7Li => CsI identification is correct ("8Be")
      //     2) Si-CsI identification gives 8He => the particle is 8He (falls on same R/L line as 2*alpha)
      // Seeing the very low statistics for 8He compared to 8Be/2alpha, we assume that if Si-CsI id
      // gives something above 8He it is either incoherent (below 7Li) or 8Be + something else in ChIo-Si
      // (above 7Li).
      if (IDsicsi->IDOK) {

         // priority to Si-CsI identification (if any) for Z>=5
         if (IDsicsi->Z >= 5) {
            theID = *IDsicsi;
            return kTRUE;
         }

         theID = *IDcsi;
         Int_t Zref = IDcsi->Z;
         Int_t Aref = IDcsi->A;
         if (IDsicsi->Aident) { // Si-CsI provides mass identification

            if (IDcsi->Z == 4 && IDcsi->A == 8) {
               // traitement special 8Be
               // if sicsi => 7Li, it is 8Be (2alpha)
               // if sicsi => 8He, it is 8He
               if (IDsicsi->Z < 2 || (IDsicsi->Z == 2 && IDsicsi->A < 7)) {
                  fCoherent = kFALSE;
                  IDsicsi->SetComment("CsI-R/L & Si-CsI identifications not coherent");
                  return kTRUE;
               } else if (IDsicsi->Z == 2 && IDsicsi->A > 6 && IDsicsi->A < 10) {
                  // accept helium-7,8,9 as 8He
                  theID = *IDsicsi;
                  return kTRUE;
               } else if ((IDsicsi->Z == 2 && IDsicsi->A > 9) || (IDsicsi->Z == 3 && IDsicsi->A < 6)) {
                  fCoherent = kFALSE;
                  IDsicsi->SetComment("CsI-R/L & Si-CsI identifications not coherent");
                  return kTRUE;
               } else if (IDsicsi->Z == 3 && IDsicsi->A > 5 && IDsicsi->A < 9) {
                  // accept lithium-6,7,8 as 7Li
                  return kTRUE;
               } else if ((IDsicsi->Z == 3 && IDsicsi->A > 8) || IDsicsi->Z > 3) {
                  fPileup = kTRUE;
                  IDsicsi->SetComment("Second particle stopping in Si, identification ChIo-Si required");
                  return kTRUE;
               }
            }
            // if CsI says A could be bigger and Si-CsI gives same Z and A+1, use Si-CsI
            if ((IDsicsi->Z == Zref) && (IDsicsi->A == (Aref + 1))
                  && (IDcsi->IDquality == KVIDGCsI::kICODE1 || IDcsi->IDquality == KVIDGCsI::kICODE3)) {
               theID = *IDsicsi;
               return kTRUE;
            }
            // if CsI says A could be smaller and Si-CsI gives same Z and A-1, use Si-CsI
            if ((IDsicsi->Z == Zref) && (IDsicsi->A == (Aref - 1))
                  && (IDcsi->IDquality == KVIDGCsI::kICODE2 || IDcsi->IDquality == KVIDGCsI::kICODE3)) {
               theID = *IDsicsi;
               return kTRUE;
            }
            // everything else - Z must be same, A +/- 1 unit
            if (IDsicsi->Z == Zref && TMath::Abs(IDsicsi->A - Aref) < 2) {
               return kTRUE;
            } else if (IDsicsi->Z < Zref || (IDsicsi->Z == Zref && IDsicsi->A < (Aref - 1))) {
               fCoherent = kFALSE;
               IDsicsi->SetComment("CsI-R/L & Si-CsI identifications not coherent");
               return kTRUE;
            } else  if (IDsicsi->Z > Zref || (IDsicsi->Z == Zref && IDsicsi->A > (Aref + 1))) {
               fPileup = kTRUE;
               IDsicsi->SetComment("Second particle stopping in Si, identification ChIo-Si required");
               return kTRUE;
            }
         } else { // only Z identification from Si-CsI
            if (IDcsi->Z == 4 && IDcsi->A == 8) {
               // traitement special 8Be
               // we ask for Z to be equal 3 in SiCsI, but with no mass identification
               // we do not try for 8He identification
               if (IDsicsi->Z < 3) {
                  fCoherent = kFALSE;
                  IDsicsi->SetComment("CsI-R/L & Si-CsI identifications not coherent");
                  return kTRUE;
               } else if (IDsicsi->Z == 3) {
                  return kTRUE;
               } else {
                  fPileup = kTRUE;
                  IDsicsi->SetComment("Second particle stopping in Si, identification ChIo-Si required");
                  return kTRUE;
               }
            }
            // everything else
            if (IDsicsi->Z == Zref) {
               return kTRUE;
            } else if (IDsicsi->Z < Zref) {
               fCoherent = kFALSE;
               IDsicsi->SetComment("CsI-R/L & Si-CsI identifications not coherent");
               return kTRUE;
            } else {
               fPileup = kTRUE;

               IDsicsi->SetComment("Second particle stopping in Si, identification ChIo-Si required");
               return kTRUE;
            }
         }
      }
      // in all other cases accept CsI identification
      theID = *IDcsi;
      return kTRUE;
   }
   return kFALSE;

   /*
      if (IDsicsi && IDsicsi->IDOK)    {
         theID = *IDsicsi;
         return kTRUE;
      } else if (IDcsi && IDcsi->IDOK) {
         theID = *IDcsi;
         return kTRUE;
      } else return kFALSE;
   */
}

//_________________________________________________________________________________

void KVFAZIAReconNuc::Calibrate()
{
   // Perform energy calibration of (previously identified) particle

   KVNucleus avatar;
   //printf("start Calibrate\n");
   Int_t ntot = GetDetectorList()->GetEntries();
   if (ntot < 1)
      return;
   Bool_t punch_through = kFALSE;
   Bool_t incoherency = kFALSE;
   Bool_t pileup = kFALSE;
   Bool_t check_error = kFALSE;

   double error_si1 = 0, error_si2 = 0; // error_csi=0;
   Double_t* eloss = new Double_t[ntot];
   for (Int_t ii = 0; ii < ntot; ii += 1) eloss[ii] = 0;
   TIter next(GetDetectorList());
   KVFAZIADetector* det = 0;
   Int_t ndet = 0;
   Int_t ndet_calib = 0;
   Double_t etot = 0;
   while ((det = (KVFAZIADetector*)next())) {

      if (det->IsCalibrated()) {
         if (det->GetIdentifier() == KVFAZIADetector::kCSI) {
            if (det->GetCalibrator("Channel-Energy")->InheritsFrom("KVLightEnergyCsIFull")) {
               KVLightEnergyCsIFull* calib = (KVLightEnergyCsIFull*)det->GetCalibrator("Channel-Energy");
               calib->SetZ(GetZ());
               calib->SetA(GetA());
               eloss[ntot - ndet - 1] = calib->Compute(det->GetQ3Amplitude());
            } else if (det->GetCalibrator("Channel-Energy")->InheritsFrom("KVLightEnergyCsI")) {
               KVLightEnergyCsI* calib = (KVLightEnergyCsI*)det->GetCalibrator("Channel-Energy");
               calib->SetZ(GetZ());
               calib->SetA(GetA());
               eloss[ntot - ndet - 1] = calib->Compute(det->GetQ3Amplitude());
            }
         } else eloss[ntot - ndet - 1] = det->GetEnergy();

         if (det->GetIdentifier() == KVFAZIADetector::kSI1)   fESI1 = eloss[ntot - ndet - 1];
         else if (det->GetIdentifier() == KVFAZIADetector::kSI2) fESI2 = eloss[ntot - ndet - 1];
         else if (det->GetIdentifier() == KVFAZIADetector::kCSI) fECSI = eloss[ntot - ndet - 1];
         etot += eloss[ntot - ndet - 1];
         ndet_calib += 1;
      }
      ndet += 1;
   }
   if (ndet == ndet_calib) {
      Double_t E_targ = 0;
      SetEnergy(etot);

      if (IsAMeasured()) {
         Double_t etot_avatar = 0;
         Double_t chi2 = 0;
         avatar.SetZAandE(GetZ(), GetA(), GetKE());
         for (Int_t nn = ntot - 1; nn >= 0; nn -= 1) {
            det = (KVFAZIADetector*)GetDetector(nn);
            Double_t temp = det->GetELostByParticle(&avatar);
            etot_avatar += temp;
            chi2 += TMath::Power((eloss[ntot - 1 - nn] - temp) / eloss[ntot - 1 - nn], 2.);
            avatar.SetKE(avatar.GetKE() - temp);
            if (det->GetIdentifier() == KVFAZIADetector::kSI1)      error_si1 = (fESI1 - temp) / fESI1;
            else if (det->GetIdentifier() == KVFAZIADetector::kSI2) error_si2 = (fESI2 - temp) / fESI2;
//                else if (det->GetIdentifier() == KVFAZIADetector::kCSI) error_csi = (fECSI-temp)/fECSI;
         }

         chi2 /= ndet;
         if (avatar.GetKE() > 0) {
            //Warning("Calibrate", "Incoherence energie residuelle %lf (PUNCH THROUGH) %s", avatar.GetKE(),GetStoppingDetector()->GetName());
            punch_through = kTRUE;
            //         } else if (TMath::Abs(etot - etot_avatar) > 1e-3) {
         } else if (chi2 > 10.) {
            //Warning("Calibrate", "Incoherence %lf != %lf", etot, etot_avatar);
            incoherency = kTRUE;
         } else if (TMath::Abs(error_si1) > 0.15 || TMath::Abs(error_si1) + TMath::Abs(error_si2) > 0.15) {
            if (StoppedInCSI() && (fECSI / etot) < 0.03) pileup = kTRUE;
            else check_error = kTRUE;
         } else {
            //            chi2 /= ndet;
         }
      }

      if (GetZ() && GetEnergy() > 0) {
         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         SetTargetEnergyLoss(E_targ);
      }

      Double_t E_tot = GetEnergy() + E_targ;
      SetEnergy(E_tot);
      // set particle momentum from telescope dimensions (random)
      GetAnglesFromStoppingDetector();
      SetECode(0);
      if (punch_through)   SetECode(2);
      if (incoherency)     SetECode(3);
      if (check_error)     SetECode(5); //
      if (pileup)          SetECode(4); //

      SetIsCalibrated();
   } else {
      if (StoppedInCSI() && !(GetCSI()->IsCalibrated()) && ndet_calib == 2) {
         if (GetZ() > 0) {
            if (!IsAMeasured()) {

               if (GetZ() == 1)       SetA(1);
               else if (GetZ() == 2)  SetA(4);
               else if (GetZ() == 20) SetA(48);
               else {
                  SetA(1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
               }
            }

            Double_t E_targ = 0;
            fECSI = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);
            SetEnergy(fECSI + fESI1 + fESI2);

            E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
            Double_t E_tot = GetEnergy() + E_targ;
            SetECode(1);
            SetIsCalibrated();
            SetEnergy(E_tot);
            GetAnglesFromStoppingDetector();
         }
      }
   }

   delete [] eloss;
}

void KVFAZIAReconNuc::ComputePSA()
{
   // Perform Pulse Shape Analysis for all detectors hit by this particle

   KVFAZIADetector* det = 0;

   TIter nextd(GetDetectorList());
   while ((det = (KVFAZIADetector*)nextd())) {
      det->ComputePSA();
   }

}

