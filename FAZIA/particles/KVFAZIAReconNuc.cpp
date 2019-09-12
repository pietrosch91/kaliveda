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
   fECSIPIETRO = -1;
   eECSI = eESI1 = eESI2 = 0;
   aECSI = aESI1 = aESI2 = 0;
   fEpattern = 0;
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

   }
   else {
      cout << "(unidentified)" << endl;
   }
   if (IsCalibrated()) {
      cout << " Total Energy = " << GetEnergy() << " MeV,  Theta=" << GetTheta() << " Phi=" << GetPhi() << endl;
      cout << "    Target energy loss correction :  " << GetTargetEnergyLoss() << " MeV" << endl;

   }
   else {
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

   // cout << "Dentro il mio Identify" << endl;
   // cout << "Stopped In " << GetStoppingDetector()->GetLabel() << "\n";
   KVList* idt_list = GetStoppingDetector()->GetIDTelescopes();
   KVIdentificationResult* IDR = 0;
   Int_t idnumber = 1;

   if (idt_list && idt_list->GetSize() > 0) {

      KVIDTelescope* idt;
      TIter next(idt_list);

      while ((idt = (KVIDTelescope*) next())) { // && !IsIdentified()) {
         //  cout << "Attempting identification with telescope " << idt->GetName() << " ( " << idt->GetType() << " ) : ";
         if (StoppedInSI1() && !strcmp(idt->GetType(), "Si-Si")) continue; // why ?
         if (StoppedInSI2() && !strcmp(idt->GetType(), "Si-CsI")) continue; // why ?

         IDR = GetIdentificationResult(idnumber);
         IDR->SetName(idt->GetName());
         IDR->SetType(idt->GetType());
         if (idt->IsReadyForID()) { // is telescope able to identify for this run ?
            // cout << "IDOK\n";
            IDR->IDattempted = kTRUE;
            IDR->IDOK = kFALSE;
            idt->Identify(IDR);
            if (IDR->IDOK && !(IDR->Z >= 3 && IDR->IDcode == 33)) { //Correspond to Quality code <=3 !!!!!Condizione rigetto Z>3 da CSI
               SetIdentification(IDR);
               SetIdentifyingTelescope(idt);
               SetIsIdentified();
               //return;
            }
            else {
               SetIdentification(IDR);
               SetIdentifyingTelescope(idt);
               //SetIDCode(0);
               //SetZandA(0, 0);
               //SetIsIdentified();
            }
         }
         else {
            // cout << "NOID!!!!!\n";
            IDR->IDattempted = kFALSE;
         }
         idnumber += 1;

      }

      KVIdentificationResult partID;
      Bool_t ok = kFALSE;
      if (StoppedInSI1()) {
         ok = CoherencySi(partID);
      }
      else if (StoppedInSI2()) {
         ok = CoherencySiSi(partID);
      }
      else if (StoppedInCSI()) {
         ok = CoherencySiCsI(partID);
      }
      if (ok) {
         SetIsIdentified();
         KVIDTelescope* idt = (KVIDTelescope*)GetIDTelescopes()->FindObjectByType(partID.GetType());
         // if (!idt) {
         //    Warning("Identify", "cannot find ID telescope with type %s", partID.GetType());
         //    GetIDTelescopes()->ls();
         //    partID.Print();
         // }
         SetIdentifyingTelescope(idt);
         SetIdentification(&partID);
         //if(GetZ()==3 && IDCode==33) cout << "fine " << GetZ() << endl;
      }
   }
}

Bool_t KVFAZIAReconNuc::CoherencySi(KVIdentificationResult& theID)
{
   KVIdentificationResult* IDsi = GetIdentificationResult("SiPSA");
   if (IDsi && IDsi->IDOK) {
      theID = *IDsi;
      //cout << "PSA IDCode" << theID.IDcode << endl;
      return kTRUE;
   }
   else return kFALSE;
}

Bool_t KVFAZIAReconNuc::CoherencySiSi(KVIdentificationResult& theID)
{
   KVIdentificationResult* IDsisi = GetIdentificationResult("Si-Si");
   KVIdentificationResult* IDsi = GetIdentificationResult("SiPSA");
   if (IDsisi && IDsisi->IDOK)    {
      theID = *IDsisi;
      //cout << "SiSi IDCode" << theID.IDcode << endl;
      return kTRUE;
   }
   else if (IDsi && IDsi->IDOK) {
      theID = *IDsi;
      // cout << "Si (daSiSi) IDCode" << theID.IDcode << endl;
      return kTRUE;
   }
   else return kFALSE;
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

   //Standard

   if (IDsicsi && IDsicsi->IDOK)    {
      theID = *IDsicsi;
      return kTRUE;
   }
   else if (IDcsi && IDcsi->IDOK && IDcsi->Z <= 2) {
      theID = *IDcsi;
      return kTRUE;
   }



   /*    if (IDsicsi->IDOK && !IDcsi->IDOK) {
         theID = *IDsicsi;
         return kTRUE;
      }

      // check coherency of CsI-R/L and Si-CsI identifications
      if (IDcsi->IDOK) {
         // gammas: if sicsi ok->sicsi; else keep csi id
         if (IDcsi->IDcode == 0) {
            theID = *IDcsi;
            return kTRUE;
         }


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
    */
   return kFALSE;
}

//_________________________________________________________________________________

int KVFAZIAReconNuc::ECodeRefinementZ1_Sandro(int idtype, double error_si1, double error_si2)
{

   //--
   //Condizioni di Sandro per Z=1 per CsI
   //--
   if (GetZ() == 1) {
      if (idtype == 11) SetECode(1); //per p, d, t
      if (idtype == 12) {
         if (GetA() == 1) {
            if (TMath::Abs(error_si2) > 0.5) SetECode(5);
            else SetECode(1);
         }
         if (GetA() == 2) {
            if (TMath::Abs(error_si2) > 0.4) SetECode(5);
            else SetECode(1);
         }
         if (GetA() == 3) SetECode(1);
      }
      if (idtype == 23) {
         if (GetA() == 1) {
            if (TMath::Abs(error_si2) > 4) SetECode(5);
            else SetECode(1);
         }
         if (GetA() == 2) {
            if (TMath::Abs(error_si2) > 1.2) SetECode(5);
            else SetECode(1);
         }
         if (GetA() == 3) {
            if (TMath::Abs(error_si2) > 1.) SetECode(5);
            else SetECode(1);
         }
      }
      if (idtype == 33) {
         SetECode(1);
      }
   }
   return GetECode();
}


int KVFAZIAReconNuc::ECodeRefinementZ1_Pietro(int idtype, double error_si1, double error_si2)
{
   //--
   //Condizioni di Pietro per Z=1 per CsI
   //--
   if (GetZ() == 1) {
      if (idtype == 11) SetECode(1); //per p, d, t
      if (idtype == 12) {
         if (GetSI1()->GetQH1Amplitude() < 1) SetECode(10);
         if (GetA() == 1) {
            if (TMath::Abs(error_si2) > 0.5) SetECode(5);
            else SetECode(1);
         }
         if (GetA() == 2) {
            if (TMath::Abs(error_si2) > 0.4) SetECode(5);
            else SetECode(1);
         }
         if (GetA() == 3) SetECode(1);
      }
      if (idtype == 23) {
         if (GetSI2()->GetQ2Amplitude() < 1) SetECode(10);
         else if (TMath::Abs(eESI2 / fESI2) < 1 && TMath::Abs(eESI2 / (fESI2 - eESI2)) < 1 && TMath::Abs(eESI1 / (fESI1 - eESI1)) < 1) SetECode(1);
         else SetECode(5);
      }
      if (idtype == 33) {
         if (GetSI2()->GetQ2Amplitude() < 1) SetECode(5);
         else if (TMath::Abs(eESI2 / (fESI2 - eESI2)) < 9 && TMath::Abs(eESI1 / (fESI1 - eESI1)) < 9) SetECode(1);
         else SetECode(5);
      }
   }
   return GetECode();
}




void KVFAZIAReconNuc::Calibrate()
{
   // Perform energy calibration of (previously identified) particle
   KVNucleus avatar;
   //printf("start Calibrate\n");
   Int_t ntot = GetDetectorList()->GetEntries();
   if (ntot < 1)
      return;

   double error_si1 = 0, error_si2 = 0; // error_csi=0;
   Double_t* eloss = new Double_t[ntot];
   for (Int_t ii = 0; ii < ntot; ii += 1) eloss[ii] = 0;
   TIter next(GetDetectorList());
   KVFAZIADetector* det = 0;
   KVIDTelescope* idt = GetIdentifyingTelescope();
   Int_t ndet = 0;
   Int_t ndet_calib = 0;
   Double_t etot = 0;
   int detname = 0;


   while ((det = (KVFAZIADetector*)next())) {
      detname = det->GetBlockNumber() * 100 + det->GetQuartetNumber() * 10 + det->GetTelescopeNumber();
      if (det->IsCalibrated() && GetZ() <= 2) {
         if (det->GetIdentifier() == KVFAZIADetector::kCSI) {

            if (det->GetCalibrator("Channel-Energy")->InheritsFrom("KVLightEnergyCsIFull")) {
               KVLightEnergyCsIFull* calib = (KVLightEnergyCsIFull*)det->GetCalibrator("Channel-Energy");
               calib->SetZ(GetZ());
               calib->SetA(GetA());
               eloss[ntot - ndet - 1] = calib->Compute(det->GetQ3Amplitude());
               //cout << detname << " " << calib->GetParameter(0) << endl;

            }
            else if (det->GetCalibrator("Channel-Energy")->InheritsFrom("KVLightEnergyCsI") && GetZ()) {
               KVLightEnergyCsI* calib = (KVLightEnergyCsI*)det->GetCalibrator("Channel-Energy");
               calib->SetZ(GetZ());
               calib->SetA(GetA());
               //cout << detname << " " << calib->GetParameter(0) << endl;
               eloss[ntot - ndet - 1] = calib->Compute(det->GetQ3Amplitude());
            }
         }
         else eloss[ntot - ndet - 1] = det->GetEnergy();

         if (det->GetIdentifier() == KVFAZIADetector::kSI1)   fESI1 = eloss[ntot - ndet - 1];
         else if (det->GetIdentifier() == KVFAZIADetector::kSI2) fESI2 = eloss[ntot - ndet - 1];
         else if (det->GetIdentifier() == KVFAZIADetector::kCSI) fECSI = eloss[ntot - ndet - 1];
         etot += eloss[ntot - ndet - 1];
         ndet_calib += 1;
      }
      if (det->IsCalibrated() && GetZ() > 2 && det->GetIdentifier() != KVFAZIADetector::kCSI) {
         eloss[ntot - ndet - 1] = det->GetEnergy();
         if (det->GetIdentifier() == KVFAZIADetector::kSI1)   fESI1 = eloss[ntot - ndet - 1];
         else if (det->GetIdentifier() == KVFAZIADetector::kSI2) fESI2 = eloss[ntot - ndet - 1];
         else if (det->GetIdentifier() == KVFAZIADetector::kCSI) fECSI = eloss[ntot - ndet - 1];
         etot += eloss[ntot - ndet - 1];
         ndet_calib += 1;
      }
      ndet += 1;
      /*if(GetZ()==8&& (detname==234||detname==221)){
      cout << det->GetName() << " " <<GetZ() << " " << GetA() << " " << fESI1 << " " << fESI2 << " " << fECSI << endl;
      cout << ndet << " = " << ndet_calib << endl;
      }*/
   }


   if (ndet == ndet_calib) {
      Double_t E_targ = 0;
      SetEnergy(etot);
      SetECode(0);


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
            if (det->GetIdentifier() == KVFAZIADetector::kSI1)      eESI1 = (fESI1 - temp);// / fESI1;
            else if (det->GetIdentifier() == KVFAZIADetector::kSI2) eESI2 = (fESI2 - temp);// / fESI2;
            else if (det->GetIdentifier() == KVFAZIADetector::kCSI) eECSI = (fECSI - temp);// / fECSI;
         }

         chi2 /= ndet;

         int idtype = GetIdentificationResult(idt->GetTitle())->IDcode;

         error_si1 = eESI1 / fESI1;
         error_si2 = eESI2 / fESI2;



         if (GetZ() > 0) {
            //if (punch_through)   SetECode(2);
            //if (incoherency)     SetECode(3);
            //if (check_error)     SetECode(5);
            //if (pileup)          SetECode(4);
            if ((avatar.GetKE() / GetKE()) > 0.0) SetECode(2);
            else if (chi2 > 10.) SetECode(3);
            else if (TMath::Abs(error_si1) > 0.15 || TMath::Abs(error_si1) + TMath::Abs(error_si2) > 0.15) {
               if (StoppedInCSI() && (fECSI / etot) < 0.03) SetECode(4);
               else SetECode(5);
            }

            //--
            //Condizioni di Sandro per Z=1 per CsI
            //--
            if (GetZ() == 1) {
               if (idtype == 11) SetECode(1); //per p, d, t

               if (idtype == 12) {
                  if (GetA() == 1) {
                     if (TMath::Abs(error_si2) > 0.5) SetECode(5);
                     else SetECode(1);
                  }
                  if (GetA() == 2) {
                     if (TMath::Abs(error_si2) > 0.4) SetECode(5);
                     else SetECode(1);
                  }
                  if (GetA() == 3) SetECode(1);
               }

               if (idtype == 23) {
                  if (GetA() == 1) {
                     if (TMath::Abs(error_si2) > 4) SetECode(5);
                     else SetECode(1);
                  }
                  if (GetA() == 2) {
                     if (TMath::Abs(error_si2) > 1.2) SetECode(5);
                     else SetECode(1);
                  }
                  if (GetA() == 3) {
                     if (TMath::Abs(error_si2) > 1.) SetECode(5);
                     else SetECode(1);
                  }
               }
               if (idtype == 33) {
                  SetECode(1);
               }
            }
            //if(GetZ()==2) if(idtype==23) SetECode(1); //alfa trattate come Z>2; Z>2 vanno in ndet!=ncalib, perche le csi calib sono solo per Z=1, Z=2

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
      SetIsCalibrated();
   }
   else {
      if (StoppedInCSI() && /*!(GetCSI()->IsCalibrated()) &&*/ ndet_calib == 2) {
         if (GetZ() > 2) { //per Z=1 e Z=2 questa cosa porta a valori assurdi! Se il CSI non è calibrato per loro non prendo
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
         else if (GetCSI()->IsCalibrated() && !(GetSI1()->IsCalibrated())) {
            Double_t E_targ = 0;
            fESI1 = GetSI1()->GetDeltaEFromERes(GetZ(), GetA(), fESI2 + fECSI);
            SetEnergy(fECSI + fESI2 + fESI1);

            E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
            Double_t E_tot = GetEnergy() + E_targ;
            SetECode(1);
            SetIsCalibrated();
            SetEnergy(E_tot);
            GetAnglesFromStoppingDetector();

         }
      }
      if (StoppedInCSI() && !(GetSI1()->IsCalibrated()) && ndet_calib == 1) { //se ho solo SI2-CSI e il Si2 Calibrato, quando il Si1 è morto
         if (GetZ() > 0) { //per Z=1 e Z=2 questa cosa porta a valori assurdi! Se il CSI non è calibrato per loro non prendo
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
            fESI1 = GetSI1()->GetDeltaEFromERes(GetZ(), GetA(), fESI2 + fECSI);
            SetEnergy(fECSI + fESI2 + fESI1);

            E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
            Double_t E_tot = GetEnergy() + E_targ;
            SetECode(1);
            SetIsCalibrated();
            SetEnergy(E_tot);
            GetAnglesFromStoppingDetector();
         }
      }
   }

   /* if(GetZ()==8&& (detname==234||detname==221)){
    cout << fESI1 << " " << fESI2 << " " << fECSI << " " << GetEnergy() << endl;
    }*/

   delete [] eloss;
}

void KVFAZIAReconNuc::CheckEnergyConsistencySi1()
{
   aESI1 = aESI2 = aECSI = 0;
   double error_si1 = 0, error_si2 = 0;
   int Avalue;
   KVIDTelescope* idt = GetIdentifyingTelescope();
   KVFAZIADetector* det_si1;
   //KVFAZIADetector *det_si2;
   //KVFAZIADetector *det_csi;
   KVNucleus avatar;
   //assign mass if not present
   if (IsAMeasured()) Avalue = GetA();
   else {
      if (GetZ() == 1)       Avalue = 1;
      else if (GetZ() == 2)  Avalue = 4;
      else if (GetZ() == 20) Avalue = 44;
      else {
         Avalue = (1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
      }
   }
   Double_t etot_avatar = 0;
   Double_t chi2 = 0;
   avatar.SetZAandE(GetZ(), Avalue, GetKE());
   det_si1 = GetSI1();
   Double_t temp = det_si1->GetELostByParticle(&avatar);
   aESI1 = temp;
   etot_avatar += temp;
   chi2 = TMath::Power((fESI1 - temp) / fESI1, 2.);
   avatar.SetKE(avatar.GetKE() - temp);
   eESI1 = (fESI1 - temp);// / fESI1;

   int idtype = GetIdentificationResult(idt->GetTitle())->IDcode;
   error_si1 = eESI1 / fESI1;

   if (GetZ() > 0) {
      //if (punch_through)   SetECode(2);
      //if (incoherency)     SetECode(3);
      //if (check_error)     SetECode(5);
      //if (pileup)          SetECode(4);
      if ((avatar.GetKE() / GetKE()) > 0.0) SetECode(2); //still residual energy after stopping layer, punch through error
      //else if (chi2 > 10.) SetECode(3);
      else if (TMath::Abs(error_si1) > 0.15) SetECode(5);

      //--
      //Condizioni di Sandro per Z=1 per CsI
      //--
      if (GetZ() == 1) {
         ECodeRefinementZ1_Pietro(idtype, error_si1, 0);
      }
   }
}



void KVFAZIAReconNuc::CheckEnergyConsistencySi2()
{
   Double_t etot = fESI1 + fESI2;
   double error_si1 = 0, error_si2 = 0;
   int Avalue;
   KVIDTelescope* idt = GetIdentifyingTelescope();
   KVFAZIADetector* det_si1 = GetSI1();
   KVFAZIADetector* det_si2 = GetSI2();
   //KVFAZIADetector *det_csi;
   KVNucleus avatar;
   //assign mass if not present
   if (IsAMeasured()) Avalue = GetA();
   else {
      if (GetZ() == 1)       Avalue = 1;
      else if (GetZ() == 2)  Avalue = 4;
      else if (GetZ() == 20) Avalue = 44;
      else {
         Avalue = (1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
      }
   }
   Double_t etot_avatar = 0;
   Double_t chi2 = 0;
   avatar.SetZAandE(GetZ(), Avalue, GetKE());
   //Energy loss in Si1
   Double_t temp = det_si1->GetELostByParticle(&avatar);
   etot_avatar += temp;
   aESI1 = temp;
   chi2 += TMath::Power((fESI1 - temp) / fESI1, 2.);
   avatar.SetKE(avatar.GetKE() - temp);
   eESI1 = (fESI1 - temp);
   //energy loss in Si2
   temp = det_si2->GetELostByParticle(&avatar);
   etot_avatar += temp;
   aESI2 = temp;
   chi2 += TMath::Power((fESI2 - temp) / fESI2, 2.);
   avatar.SetKE(avatar.GetKE() - temp);
   eESI2 = (fESI2 - temp);
   chi2 /= 2;
   int idtype = GetIdentificationResult(idt->GetTitle())->IDcode;
   error_si1 = eESI1 / fESI1;
   error_si2 = eESI2 / fESI2;
   if (GetZ() > 0) {
      //if (punch_through)   SetECode(2);
      //if (incoherency)     SetECode(3);
      //if (check_error)     SetECode(5);
      //if (pileup)          SetECode(4);
      if ((avatar.GetKE() / GetKE()) > 0.0) SetECode(2);
      // else if (chi2 > 10.) SetECode(3);
      else if (TMath::Abs(error_si1) > 0.5 || TMath::Abs(error_si1) + TMath::Abs(error_si2) > 8) {
         SetECode(5);
      }
      if (GetZ() == 1) {
         ECodeRefinementZ1_Pietro(idtype, error_si1, error_si2);
      }
   }
}

void KVFAZIAReconNuc::CheckEnergyConsistencyCsI()
{
   aESI1 = aESI2 = aECSI = 0;
   Double_t etot = fESI1 + fESI2 + fECSI;
   double error_si1 = 0, error_si2 = 0, error_csi = 0;
   int Avalue;
   KVIDTelescope* idt = GetIdentifyingTelescope();
   KVFAZIADetector* det_si1 = GetSI1();
   KVFAZIADetector* det_si2 = GetSI2();
   KVFAZIADetector* det_csi = GetCSI();
   KVNucleus avatar;
   //assign mass if not present
   if (IsAMeasured()) Avalue = GetA();
   else {
      if (GetZ() == 1)       Avalue = 1;
      else if (GetZ() == 2)  Avalue = 4;
      else if (GetZ() == 20) Avalue = 44;
      else {
         Avalue = (1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
      }
   }
   Double_t etot_avatar = 0;
   Double_t chi2 = 0;
   avatar.SetZAandE(GetZ(), Avalue, GetKE());
   //Energy loss in Si1
   Double_t temp = det_si1->GetELostByParticle(&avatar);
   etot_avatar += temp;
   aESI1 = temp;
   chi2 += TMath::Power((fESI1 - temp) / fESI1, 2.);
   avatar.SetKE(avatar.GetKE() - temp);
   eESI1 = (fESI1 - temp);
   //energy loss in Si2
   temp = det_si2->GetELostByParticle(&avatar);
   etot_avatar += temp;
   aESI2 = temp;
   chi2 += TMath::Power((fESI2 - temp) / fESI2, 2.);
   avatar.SetKE(avatar.GetKE() - temp);
   eESI2 = (fESI2 - temp);
   //energyloss in CsI
   temp = etot - etot_avatar;
   etot_avatar += temp;
   aECSI = temp;
   chi2 += TMath::Power((fECSI - temp) / fECSI, 2.);
   avatar.SetKE(avatar.GetKE() - temp);
   eECSI = (fECSI - temp);
   chi2 /= 3;

   int idtype = GetIdentificationResult(idt->GetTitle())->IDcode;
   error_si1 = eESI1 / fESI1;
   error_si2 = eESI2 / fESI2;
   error_csi = eECSI / fECSI;
   if (GetZ() > 0) {
      //if (punch_through)   SetECode(2);
      //if (incoherency)     SetECode(3);
      //if (check_error)     SetECode(5);
      //if (pileup)          SetECode(4);
      //if ((avatar.GetKE() / GetKE()) > 0.0) SetECode(2);
      if (chi2 > 10.) SetECode(3);
      else if (TMath::Abs(error_si1) > 0.15 || TMath::Abs(error_si1) + TMath::Abs(error_si2) > 0.15) {
         if (StoppedInCSI() && (fECSI / etot) < 0.03) SetECode(4);
         else SetECode(5);
      }
      //--
      //Condizioni di Sandro per Z=1 per CsI
      //--
      if (GetZ() == 1) {
         ECodeRefinementZ1_Pietro(idtype, error_si1, error_si2);
      }
   }
}






void KVFAZIAReconNuc::Calibrate_Modular(Bool_t override_light)
{
   fEpattern = 0;
   if (StoppedInSI1()) CalibrateSi1();
   else if (StoppedInSI2()) CalibrateSi2();
   else if (StoppedInCSI()) CalibrateCsI(override_light);
}

void KVFAZIAReconNuc::CalibrateSi1()
{
   fECSIPIETRO = -1;
   // Perform energy calibration of (previously identified) particle
   KVNucleus avatar;
   //printf("start Calibrate\n");
   Int_t ntot = GetDetectorList()->GetEntries();// -->ntot=1
   if (ntot != 1) return;
//   Bool_t punch_through = kFALSE;
   // Bool_t incoherency = kFALSE;
//  Bool_t pileup = kFALSE;
//  Bool_t check_error = kFALSE;

//  double error_si1 = 0, error_si2 = 0; // error_csi=0;
   Double_t* eloss = new Double_t[ntot];
   for (Int_t ii = 0; ii < ntot; ii += 1) eloss[ii] = 0;
   TIter next(GetDetectorList());
   KVFAZIADetector* det = GetSI1();
//   KVIDTelescope* idt = GetIdentifyingTelescope();
   Int_t ndet = 0;
   Int_t ndet_calib = 0;
   Double_t etot = 0;
   int detname = 0;
   detname = det->GetBlockNumber() * 100 + det->GetQuartetNumber() * 10 + det->GetTelescopeNumber();

   if (det->IsCalibrated()) {
      eloss[0] = det->GetEnergy();
      fESI1 = eloss[0];
      etot += eloss[0];
      ndet_calib += 1;
      fEpattern = fEpattern | 1;
   }
   ndet += 1;

   if (!IsAMeasured()) {
      if (GetZ() == 1)       SetA(1);
      else if (GetZ() == 2)  SetA(4);
      else if (GetZ() == 20) SetA(48);
      else {
         SetA(1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
      }
   }

   //If Silicon is calibrated
   if (ndet == ndet_calib) {
      Double_t E_targ = 0;
      SetEnergy(etot);
      SetECode(0);

      CheckEnergyConsistencySi1();

      if (GetZ() && GetEnergy() > 0) {
         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         SetTargetEnergyLoss(E_targ);
      }
      Double_t E_tot = GetEnergy() + E_targ;
      SetEnergy(E_tot);
      // set particle momentum from telescope dimensions (random)
      GetAnglesFromStoppingDetector();
      SetIsCalibrated();
   }
   delete [] eloss;
}

void KVFAZIAReconNuc::CalibrateSi2()
{
   // Perform energy calibration of (previously identified) particle
   fECSIPIETRO = -1;
   KVNucleus avatar;
   //printf("start Calibrate\n");
   Int_t ntot = GetDetectorList()->GetEntries(); //->2
   if (ntot != 2) return;

//  Bool_t punch_through = kFALSE;
//  Bool_t incoherency = kFALSE;
//  Bool_t pileup = kFALSE;
//  Bool_t check_error = kFALSE;

//  double error_si1 = 0, error_si2 = 0; // error_csi=0;
   Double_t* eloss = new Double_t[ntot];
   for (Int_t ii = 0; ii < ntot; ii += 1) eloss[ii] = 0;
   TIter next(GetDetectorList());
   KVFAZIADetector* det = 0;
//  KVIDTelescope* idt = GetIdentifyingTelescope();
   Int_t ndet = 0;
   Int_t ndet_calib = 0;
   Double_t etot = 0;
   int detname = 0;

   Bool_t Si1Calib = false;
   Bool_t Si2Calib = false;

   while ((det = (KVFAZIADetector*)next())) {
      detname = det->GetBlockNumber() * 100 + det->GetQuartetNumber() * 10 + det->GetTelescopeNumber();
      if (det->IsCalibrated()) {
         eloss[ntot - ndet - 1] = det->GetEnergy();
         if (det->GetIdentifier() == KVFAZIADetector::kSI1) {
            fESI1 = eloss[ntot - ndet - 1];
            Si1Calib = true;
            fEpattern = fEpattern | 1;
         }
         else if (det->GetIdentifier() == KVFAZIADetector::kSI2) {
            fESI2 = eloss[ntot - ndet - 1];
            Si2Calib = true;
            fEpattern = fEpattern | 2;
         }
         etot += eloss[ntot - ndet - 1];
         ndet_calib += 1;
      }
      ndet += 1;
   }

   if (!IsAMeasured()) {
      if (GetZ() == 1)       SetA(1);
      else if (GetZ() == 2)  SetA(4);
      else if (GetZ() == 20) SetA(48);
      else {
         SetA(1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
      }
   }


   if (ndet == ndet_calib) { //Everything OK
      Double_t E_targ = 0;
      SetEnergy(etot);
      SetECode(0);

      CheckEnergyConsistencySi2();

      if (GetZ() && GetEnergy() > 0) {
         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         SetTargetEnergyLoss(E_targ);
      }

      Double_t E_tot = GetEnergy() + E_targ;
      SetEnergy(E_tot);
      // set particle momentum from telescope dimensions (random)
      GetAnglesFromStoppingDetector();
      SetIsCalibrated();
   }
   else if (ndet_calib == 1) {
      if (Si1Calib) {
         //try to recover Si2 Energy loss from Si1
         //Assign mass if not present
         Double_t E_targ = 0;
         fESI2 = GetSI1()->GetEResFromDeltaE(GetZ(), GetA(), fESI1);
         SetEnergy(fESI1 + fESI2);
         SetECode(1);

         CheckEnergyConsistencySi2();

         if (GetZ() && GetEnergy() > 0) {
            E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
            SetTargetEnergyLoss(E_targ);
         }

         Double_t E_tot = GetEnergy() + E_targ;
         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
      }
      else {
         Double_t E_targ = 0;
         fESI1 = GetSI2()->GetDeltaEFromERes(GetZ(), GetA(), fESI2);
         SetEnergy(fESI1 + fESI2);
         SetECode(1);

         CheckEnergyConsistencySi2();

         if (GetZ() && GetEnergy() > 0) {
            E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
            SetTargetEnergyLoss(E_targ);
         }

         Double_t E_tot = GetEnergy() + E_targ;
         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
      }
   }

   /* if(GetZ()==8&& (detname==234||detname==221)){
    cout << fESI1 << " " << fESI2 << " " << fECSI << " " << GetEnergy() << endl;
    }*/

   delete [] eloss;
}

void KVFAZIAReconNuc::CalibrateCsI(Bool_t override_light)
{
   if (GetZ() <= 2 && !override_light) CalibrateCsI_Light();
   else CalibrateCsI_Heavy();
}

void KVFAZIAReconNuc::CalibrateCsI_Heavy()
{
   // Perform energy calibration of (previously identified) particle

   KVNucleus avatar;
   //printf("start Calibrate\n");
   Int_t ntot = GetDetectorList()->GetEntries(); //Must be 3
   if (ntot != 3) return;
//  Bool_t punch_through = kFALSE;
//  Bool_t incoherency = kFALSE;
//  Bool_t pileup = kFALSE;
//  Bool_t check_error = kFALSE;
   Bool_t Si1Calib = false;
   Bool_t Si2Calib = false;


//  double error_si1 = 0, error_si2 = 0; // error_csi=0;
   Double_t* eloss = new Double_t[ntot];
   for (Int_t ii = 0; ii < ntot; ii += 1) eloss[ii] = 0;
   TIter next(GetDetectorList());
   KVFAZIADetector* det = 0;
//   KVIDTelescope* idt = GetIdentifyingTelescope();
   Int_t ndet = 0;
   Int_t ndet_calib = 0;
   Double_t etot = 0;
   int detname = 0;


   while ((det = (KVFAZIADetector*)next())) {
      detname = det->GetBlockNumber() * 100 + det->GetQuartetNumber() * 10 + det->GetTelescopeNumber();
      if (det->IsCalibrated()) {
         if (det->GetIdentifier() != KVFAZIADetector::kCSI) { //Always ignore CsI calibration
            eloss[ntot - ndet - 1] = det->GetEnergy();
            if (det->GetIdentifier() == KVFAZIADetector::kSI1) {
               fESI1 = eloss[ntot - ndet - 1];
               Si1Calib = true;
               fEpattern = fEpattern | 1;
            }
            else if (det->GetIdentifier() == KVFAZIADetector::kSI2) {
               fESI2 = eloss[ntot - ndet - 1];
               Si2Calib = true;
               fEpattern = fEpattern | 2;
            }
            etot += eloss[ntot - ndet - 1];
            ndet_calib += 1;
         }
      }
      ndet += 1;
   }

   if (!IsAMeasured()) {
      if (GetZ() == 1)       SetA(1);
      else if (GetZ() == 2)  SetA(4);
      else if (GetZ() == 20) SetA(48);
      else {
         SetA(1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
      }
   }

   if (ndet_calib == 2) { //this means both Si1 and Si2 are calibrated
      Double_t E_targ = 0;
      fECSI = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);
      fECSIPIETRO = fECSI;
      SetEnergy(fECSI + fESI1 + fESI2);
      SetECode(1);

      CheckEnergyConsistencyCsI();

      E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
      Double_t E_tot = GetEnergy() + E_targ;
      SetIsCalibrated();
      SetEnergy(E_tot);
      GetAnglesFromStoppingDetector();
   }
   else if (ndet_calib == 1) {   //ndet
      if (Si2Calib) { //recover Esi1 and EcsI from Esi2
         Double_t E_targ = 0;
         fECSI = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);
         fECSIPIETRO = fECSI;
         fESI1 = GetSI1()->GetDeltaEFromERes(GetZ(), GetA(), fESI2 + fECSI);
         SetEnergy(fECSI + fESI2 + fESI1);
         SetECode(1);

         CheckEnergyConsistencyCsI();

         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         Double_t E_tot = GetEnergy() + E_targ;
         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
      }
      else {   //recover Esi2 and ECSI from ESi1
         Double_t E_targ = 0;
         Double_t Eres = GetSI1()->GetEResFromDeltaE(GetZ(), GetA(), fESI1);
         avatar.SetZAandE(GetZ(), GetA(), Eres);
         fESI2 = GetSI2()->GetELostByParticle(&avatar);
         fECSI = Eres - fESI2;
         fECSIPIETRO = fECSI;
         SetEnergy(fECSI + fESI2 + fESI1);
         SetECode(6);

         CheckEnergyConsistencyCsI();

         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         Double_t E_tot = GetEnergy() + E_targ;
         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
      }

   }
   delete [] eloss;
}

void KVFAZIAReconNuc::CalibrateCsI_Light()
{
   // Perform energy calibration of (previously identified) particle

   KVNucleus avatar;
   //printf("start Calibrate\n");
   Int_t ntot = GetDetectorList()->GetEntries();//-->3
   if (ntot != 3) return;
//   Bool_t punch_through = kFALSE;
//   Bool_t incoherency = kFALSE;
//   Bool_t pileup = kFALSE;
//   Bool_t check_error = kFALSE;

   Bool_t Si1Calib = false;
   Bool_t Si2Calib = false;
   Bool_t CsICalib = false;

//   double error_si1 = 0, error_si2 = 0; // error_csi=0;
   Double_t* eloss = new Double_t[ntot];
   for (Int_t ii = 0; ii < ntot; ii += 1) eloss[ii] = 0;
   TIter next(GetDetectorList());
   KVFAZIADetector* det = 0;
//   KVIDTelescope* idt = GetIdentifyingTelescope();
   Int_t ndet = 0;
   Int_t ndet_calib = 0;
   Double_t etot = 0;
   int detname = 0;


   while ((det = (KVFAZIADetector*)next())) {
      detname = det->GetBlockNumber() * 100 + det->GetQuartetNumber() * 10 + det->GetTelescopeNumber();
      if (det->IsCalibrated()) {
         if (det->GetIdentifier() == KVFAZIADetector::kCSI) {
            if (det->GetCalibrator("Channel-Energy")->InheritsFrom("KVLightEnergyCsIFull")) {
               KVLightEnergyCsIFull* calib = (KVLightEnergyCsIFull*)det->GetCalibrator("Channel-Energy");
               calib->SetZ(GetZ());
               calib->SetA(GetA());
               eloss[ntot - ndet - 1] = calib->Compute(det->GetQ3Amplitude());

               //cout << detname << " " << calib->GetParameter(0) << endl;

            }
            else if (det->GetCalibrator("Channel-Energy")->InheritsFrom("KVLightEnergyCsI") && GetZ()) {
               KVLightEnergyCsI* calib = (KVLightEnergyCsI*)det->GetCalibrator("Channel-Energy");
               calib->SetZ(GetZ());
               calib->SetA(GetA());
               //cout << detname << " " << calib->GetParameter(0) << endl;
               eloss[ntot - ndet - 1] = calib->Compute(det->GetQ3Amplitude());
            }

         }
         else eloss[ntot - ndet - 1] = det->GetEnergy();

         if (det->GetIdentifier() == KVFAZIADetector::kSI1) {
            fESI1 = eloss[ntot - ndet - 1];
            Si1Calib = true;
            fEpattern = fEpattern | 1;
         }
         else if (det->GetIdentifier() == KVFAZIADetector::kSI2) {
            fESI2 = eloss[ntot - ndet - 1];
            Si2Calib = true;
            fEpattern = fEpattern | 2;
         }
         else if (det->GetIdentifier() == KVFAZIADetector::kCSI) {
            fECSI = eloss[ntot - ndet - 1];
            CsICalib = true;
            fEpattern = fEpattern | 4;
         }
         etot += eloss[ntot - ndet - 1];
         ndet_calib += 1;
      }
      ndet += 1;
   }

   if (!IsAMeasured()) {
      if (GetZ() == 1)       SetA(1);
      else if (GetZ() == 2)  SetA(4);
      else if (GetZ() == 20) SetA(48);
      else {
         SetA(1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
      }
   }

   if (ndet == ndet_calib) {
      Double_t E_targ = 0;
      SetEnergy(etot);
      SetECode(0);

      CheckEnergyConsistencyCsI();

      fECSIPIETRO = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);
      if (GetZ() && GetEnergy() > 0) {
         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         SetTargetEnergyLoss(E_targ);
      }

      Double_t E_tot = GetEnergy() + E_targ;
      SetEnergy(E_tot);
      // set particle momentum from telescope dimensions (random)
      GetAnglesFromStoppingDetector();
      SetIsCalibrated();
   }
   else if (ndet_calib == 2) {
      if (!CsICalib) {
         Double_t E_targ = 0;
         fECSI = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);
         SetEnergy(fECSI + fESI1 + fESI2);


         CheckEnergyConsistencyCsI();
         fECSIPIETRO = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);

         //Force ECode to 5 because it is not reliable
         SetECode(5);
         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         Double_t E_tot = GetEnergy() + E_targ;
         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
      }
      else if (!Si2Calib) {
         Double_t E_targ = 0;
         Double_t Eres = GetSI1()->GetEResFromDeltaE(GetZ(), GetA(), fESI1);
         avatar.SetZAandE(GetZ(), GetA(), Eres);
         fESI2 = GetSI2()->GetELostByParticle(&avatar);
         fECSIPIETRO = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);
         //  fECSI=Eres-fESI2;
         SetEnergy(fECSI + fESI2 + fESI1);

         CheckEnergyConsistencyCsI();

         //Force ecode because calib is not reliable
         SetECode(8);

         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         Double_t E_tot = GetEnergy() + E_targ;
         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
      }
      else {
         Double_t E_targ = 0;
         fESI1 = GetSI1()->GetDeltaEFromERes(GetZ(), GetA(), fESI2 + fECSI);
         fECSIPIETRO = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);
         SetEnergy(fECSI + fESI2 + fESI1);
         SetECode(1);

         CheckEnergyConsistencyCsI();

         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         Double_t E_tot = GetEnergy() + E_targ;
         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
      }
   }
   else if (ndet_calib == 1) {
      if (Si2Calib) { //recover Esi1 and EcsI from Esi2
         Double_t E_targ = 0;
         fECSI = GetSI2()->GetEResFromDeltaE(GetZ(), GetA(), fESI2);
         fECSIPIETRO = fECSI;
         fESI1 = GetSI1()->GetDeltaEFromERes(GetZ(), GetA(), fESI2 + fECSI);
         SetEnergy(fECSI + fESI2 + fESI1);
         SetECode(1);
         CheckEnergyConsistencyCsI();
         SetECode(5);

         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         Double_t E_tot = GetEnergy() + E_targ;
         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
      }
      else if (Si1Calib) {   //recover Esi2 and ECSI from ESi1
         Double_t E_targ = 0;
         Double_t Eres = GetSI1()->GetEResFromDeltaE(GetZ(), GetA(), fESI1);
         avatar.SetZAandE(GetZ(), GetA(), Eres);
         fESI2 = GetSI2()->GetELostByParticle(&avatar);
         fECSI = Eres - fESI2;
         fECSIPIETRO = fECSI;
         SetEnergy(fECSI + fESI2 + fESI1);
         SetECode(6);

         CheckEnergyConsistencyCsI();
         SetECode(6);

         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
         Double_t E_tot = GetEnergy() + E_targ;

         SetIsCalibrated();
         SetEnergy(E_tot);
         GetAnglesFromStoppingDetector();
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

