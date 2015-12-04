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

using namespace std;

ClassImp(KVFAZIAReconNuc);

////////////////////////////////////////////////////////////////////////////
//KVFAZIAReconNuc
//
//Nuclei reconstructed from data measured in the FAZIA array.
//Most useful methods are already defined in parent classes KVReconstructedNucleus,
//KVNucleus and KVParticle.


void KVFAZIAReconNuc::init()
{
   //default initialisations
   if (gDataSet)
      SetMassFormula(UChar_t(gDataSet->GetDataSetEnv("KVFAZIAReconNuc.MassFormula", Double_t(kEALMass))));
   fECsI = fESi1 = fESi2 = 0.;
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


void KVFAZIAReconNuc::Print(Option_t* option) const
{

   //KVReconstructedNucleus::Print(option);

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
   KVFAZIADetector* det = (KVFAZIADetector*) GetDetectorList()->FindObjectByLabel(label);
   return det;
}

KVFAZIADetector* KVFAZIAReconNuc::GetCSI() const
{
   return Get("CSI");
}
KVFAZIADetector* KVFAZIAReconNuc::GetSI1() const
{
   return Get("SI1");
}
KVFAZIADetector* KVFAZIAReconNuc::GetSI2() const
{
   return Get("SI2");
}


Bool_t KVFAZIAReconNuc::StoppedIn(const Char_t* label) const
{
   //Returns kTRUE if particle stopped in the labelled detector
   return (GetStoppingDetector() == Get(label));
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

// Int_t KVFAZIAReconNuc::GetIDSubCode(const Char_t * id_tel_type) const
// {
//    //Returns subcode/status code from identification performed in ID telescope of given type.
//    //i.e. to obtain CsI R-L subcode use GetIDSubCode("CSI_R_L").
//    //
//    //The meaning of the subcodes is defined in the corresponding KVIDTelescope class description
//    //i.e. for CsI R-L, look at KVIDCsI.
//    //
//    //The subcode is set for any attempted identification, not necessarily that which eventually
//    //leads to the definitive identification of the particle.
//    //i.e. in the example of the CsI R-L subcode, the particle in question may in fact be identified
//    //by a Si-CsI telescope, because the CsI identification routine returned e.g. KVIDGCsI::kICODE7
//    //(a gauche de la ligne fragment, Z est alors un Zmin et le plus probable).
//    //
//    //calling GetIDSubCode() with no type returns the identification subcode corresponding
//    //to the identifying telescope (whose pointer is given by GetIdentifyingTelescope()).
//    //
//    //If no subcode exists (identification in given telescope type was not attempted, etc.) value returned is -1
//
//    KVIdentificationResult* ir = 0;
//    if(strcmp(id_tel_type,"")) ir = GetIdentificationResult(id_tel_type);
//    else ir = GetIdentificationResult(GetIdentifyingTelescope());
//    if(!ir) return -1;
//    return ir->IDquality;
//    //return GetIDSubCode(id_tel_type,const_cast <KVFAZIAReconNuc *>(this)->GetCodes().GetSubCodes());
// }

// const Char_t *KVFAZIAReconNuc::GetIDSubCodeString(const Char_t *
//                                                   id_tel_type) const
// {
//    //Returns explanatory message concerning identification performed in ID telescope of given type.
//    //(see GetIDSubCode())
//    //
//    //The subcode is set for any attempted identification, not necessarily that which eventually
//    //leads to the definitive identification of the particle.
//    //
//    //calling GetIDSubCodeString() with no type returns the identification subcode message corresponding
//    //to the identifying telescope (whose pointer is given by GetIdentifyingTelescope()).
//    //
//    //In case of problems:
//    //       no ID telescope of type 'id_tel_type' :  "No identification attempted in id_tel_type"
//    //       particle not identified               :  "Particle unidentified. Identifying telescope not set."
//
//    KVIdentificationResult* ir = 0;
//    if(strcmp(id_tel_type,"")) ir = GetIdentificationResult(id_tel_type);
//    else ir = GetIdentificationResult(GetIdentifyingTelescope());
//    if (!ir) {
//        if (strcmp(id_tel_type, ""))
//            return Form("No identification attempted in %s", id_tel_type);
//        else
//            return
//                Form("Particle unidentified. Identifying telescope not set.");
//    }
//    return ir->GetComment();
// }

//____________________________________________________________________________________________


/*
void KVFAZIAReconNuc::Identify()
{
   KVIdentificationResult partID;
   KVList* idt_list = GetStoppingDetector()->GetAlignedIDTelescopes();
   //idt_list->ls();
   if (idt_list && idt_list->GetSize() > 0) {

      KVIDTelescope* idt;
      TIter next(idt_list);
      Int_t idnumber = 1;

      Bool_t goout = kFALSE;
      while ((idt = (KVIDTelescope*) next()) && !goout) {
         KVIdentificationResult* IDR = GetIdentificationResult(idnumber++);
         //printf("idt getname : %s %d\n",idt->GetName(),idnumber);

         if (idt->IsReadyForID()) { // is telescope able to identify for this run ?

            printf("idt getname : %s %d\n",idt->GetName(),idnumber);
            IDR->IDattempted = kTRUE;
            idt->Identify(IDR);

            if (IDR->IDOK) {
               partID = *IDR;
               goout = kTRUE;
            }
         } else {
            //printf("not ready\n");
            IDR->IDattempted = kFALSE;
         }

//          if(n_success_id<1 &&
//                  ((!IDR->IDattempted) || (IDR->IDattempted && !IDR->IDOK))){
//              // the particle is less identifiable than initially thought
//              // we may have to wait for secondary identification
//              Int_t nseg = GetNSegDet();
//              SetNSegDet(TMath::Max(nseg - 1, 0));
//              //if there are other unidentified particles in the group and NSegDet is < 2
//              //then exact status depends on segmentation of the other particles : reanalyse
//              if (GetNSegDet() < 2 && GetNUnidentifiedInGroup(GetGroup()) > 1){
//                  AnalyseParticlesInGroup(GetGroup());
//                  return;
//              }
//              //if NSegDet = 0 it's hopeless
//              if (!GetNSegDet()){
//                  AnalyseParticlesInGroup(GetGroup());
//                  return;
//              }
//          }
//


      }

      //if (!strcmp(partID.GetType(), "Si-Si"))
      if (partID.IDOK) {
         SetIsIdentified();
         idt = (KVIDTelescope*)GetIDTelescopes()->FindObjectByType(partID.GetType());
         if (!idt) {
            Warning("Identify", "cannot find ID telescope with type %s", partID.GetType());
            GetIDTelescopes()->ls();
            partID.Print();
         }
         SetIdentifyingTelescope(idt);
         SetIdentification(&partID);
      } else {
         //partID.Print();
      }
   }


}
*/
//---------------------------------
void KVFAZIAReconNuc::Identify()
{
   // Try to identify this nucleus by calling the Identify() function of each
   // ID telescope crossed by it, starting with the telescope where the particle stopped, in order
   //      -  only attempt identification in ID telescopes containing the stopping detector.
   //      -  only telescopes which have been correctly initialised for the current run are used,
   //         i.e. those for which KVIDTelescope::IsReadyForID() returns kTRUE.
   // This continues until a successful identification is achieved or there are no more ID telescopes to try.
   // The identification code corresponding to the identifying telescope is set as the identification code of the particle.


   //printf("New particle %d\n",GetNSegDet());

   KVList* idt_list = GetStoppingDetector()->GetAlignedIDTelescopes();
   KVIdentificationResult* IDR = 0;
   Int_t idnumber = 1;
   KVNumberList idOK;
   KVNumberList idnotOK;
   if (idt_list && idt_list->GetSize() > 0) {

      KVIDTelescope* idt;
      TIter next(idt_list);
      while ((idt = (KVIDTelescope*) next())) {
         IDR = GetIdentificationResult(idnumber);
         IDR->SetName(idt->GetName());
         if (idt->IsReadyForID()) { // is telescope able to identify for this run ?

            IDR->IDattempted = kTRUE;
            idt->Identify(IDR);
            if (IDR->IDOK) {  //Correspond to Quality code <=3
               idOK.Add(idnumber);
            } else {
               idnotOK.Add(idnumber);
            }
         } else {
            IDR->IDattempted = kFALSE;
         }
         idnumber += 1;
      }
   }


   Int_t last_quality = 2000;
   if (idOK.GetNValues() > 0) {
      idOK.Begin();
      while (!idOK.End()) {
         Int_t nn = idOK.Next();
         IDR = GetIdentificationResult(nn);
         if (IDR->IDquality < last_quality) {
            last_quality = IDR->IDquality;
            SetIDCode(IDR->IDcode);

            if (IDR->Aident) {
               SetAMeasured(kTRUE);
               SetRealA(IDR->PID);
               if (IDR->Zident)
                  SetZMeasured(kTRUE);
               else
                  SetZMeasured(kFALSE);
               SetZandA(IDR->Z, IDR->A);
            } else if (IDR->Zident) {
               SetAMeasured(kFALSE);
               SetZMeasured(kTRUE);
               SetRealZ(IDR->PID);
               SetZandA(IDR->Z, IDR->A);
            } else {
               Warning("Identify", "Nothing measured (Z&A) but identification quality code=%d ...", IDR->IDquality);
            }
         }

         SetIdentification(IDR);
         SetIdentifyingTelescope((KVIDTelescope*)GetIDTelescopes()->FindObject(IDR->GetName()));
         SetIsIdentified();
      }
   }

   if (!IsIdentified() && idnotOK.GetNValues() > 0) {
      last_quality = 2000;
      idnotOK.Begin();
      while (!idnotOK.End()) {
         Int_t nn = idnotOK.Next();
         IDR = GetIdentificationResult(nn);
         if (IDR->IDquality < last_quality) {
            last_quality = IDR->IDquality;
            SetIDCode(IDR->IDquality);

            if (IDR->Aident) {
               SetAMeasured(kTRUE);
               SetRealA(IDR->PID);
               if (IDR->Zident)
                  SetZMeasured(kTRUE);
               else
                  SetZMeasured(kFALSE);
               SetZandA(IDR->Z, IDR->A);
            } else if (IDR->Zident) {
               SetAMeasured(kFALSE);
               SetZMeasured(kTRUE);
               SetRealZ(IDR->PID);
               SetZandA(IDR->Z, IDR->A);
            }
         }

         SetIdentification(IDR);
         SetIdentifyingTelescope((KVIDTelescope*)GetIDTelescopes()->FindObject(IDR->GetName()));
         SetIsIdentified();
      }
   }

}

//_________________________________________________________________________________

void KVFAZIAReconNuc::Calibrate()
{
   // Calculate and set the energy of a (previously identified) reconstructed particle.

   /*
    if(GetRingNumber()<=9)
          CalibrateRings1To9();
      else
          CalibrateRings10To17();
   */

   SetIsCalibrated();
   //add correction for target energy loss - moving charged particles only!
   Double_t E_targ = 0.;
   if (GetZ() && GetEnergy() > 0) {
      E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
      SetTargetEnergyLoss(E_targ);
   }
   Double_t E_tot = GetEnergy() + E_targ;
   SetEnergy(E_tot);
   // set particle momentum from telescope dimensions (random)
   GetAnglesFromStoppingDetector();
   //CheckCsIEnergy();

}

// void KVFAZIAReconNuc::DoGammaCalibration()
// {
//    // no calibration is performed for gammas
//    SetNoCalibrationStatus();
//    return;
// }
//
// void KVFAZIAReconNuc::DoNeutronCalibration()
// {
//    // use energy of CsI calculated using the Z & A of the CsI identification
//    // to calculate the energy deposited by the neutron
//    KVIdentificationResult *IDcsi = GetIdentificationResult(1);
//    KVNucleus tmp(IDcsi->Z, IDcsi->A);
//    if(GetCsI() && GetCsI()->IsCalibrated()) {
//        fECsI = GetCsI()->GetCorrectedEnergy(&tmp, -1., kFALSE );
//        SetEnergy( fECsI );
//        SetECode(kECode2); // not a real energy measure
//    }
//    else
//    {
//        SetNoCalibrationStatus();
//    }
// }
//
// void KVFAZIAReconNuc::DoBeryllium8Calibration()
// {
//    // Beryllium-8 = 2 alpha particles of same energy
//    // We halve the total light output of the CsI to calculate the energy of 1 alpha
//    // Then multiply resulting energy by 2
//    // Note: fECsI is -ve, because energy is calculated not measured
//    Double_t half_light = GetCsI()->GetLumiereTotale()*0.5;
//    KVNucleus tmp(2,4);
//    fECsI = -2.*GetCsI()->GetCorrectedEnergy(&tmp,half_light,kFALSE);
//    SetECode(kECode2);
// }
//
// Bool_t KVFAZIAReconNuc::CalculateSiliconDEFromResidualEnergy()
// {
//    // calculate fESi from fECsI
//    // returns kTRUE if OK
//    Double_t e0 = GetSi()->GetDeltaEFromERes(GetZ(),GetA(),TMath::Abs(fECsI));
//    GetSi()->SetEResAfterDetector(TMath::Abs(fECsI));
//    fESi = GetSi()->GetCorrectedEnergy(this,e0);
//    if(fESi<=0){
//        // can't calculate fESi from CsI energy - bad
//        SetBadCalibrationStatus();
//        return kFALSE;
//    }
//    else
//    {
//        // calculated energy: negative
//        fESi = -TMath::Abs(fESi);
//        SetECode(kECode2);
//    }
//    return kTRUE;
// }
//
// void KVFAZIAReconNuc::CalibrateRings1To9()
// {
//     // Special calibration for particles in rings 1 to 9
//     // We set the energy calibration code for the particle here
//     //    kECode0 = no calibration (e.g. gammas)
//     //    kECode1 = everything OK
//     //    kECode2 = small warning, for example if energy loss in a detector is calculated
//     //    kECode15 = bad, calibration is no good
//    // The contributions from ChIo, Si, and CsI are stored in member variables fEChIo, fESi, fECsI
//     // If the contribution is calculated rather than measured (see below), it is stored as a negative value.
//     // NOTE: in no case do we ever calculate an energy for uncalibrated detector using measured energy
//     //       loss in the detector placed in front (i.e. calculate ECsI from deSi, or calculate ESi
//     //       from deChIo) as this gives wildly varying (mostly false) results.
//     //
//     // For gammas (IDCODE=0): no calibration performed, energy will be zero, ECODE=kECode0
//     // For neutrons (IDCODE=1): if CsI is calibrated, we use the CsI light response to calculate
//     //                          the equivalent energy for a proton.
//     // For particles stopping in the CsI detector (IDCODE=2 or 3):
//     //    - for "8Be" we use half the CsI light output to calculate 4He kinetic energy which is then doubled
//     //      (we assume two 4He of identical energy), and ECODE=kECode2 (calculated)
//     //    - if no calibration exists for the CsI, we cannot calibrate the particle (ECODE=kECode0)
//     //    - if calibrated CsI energy is bizarre (<=0), we cannot calibrate the particle (ECODE=kECode15)
//     //    - if the Silicon detector is not calibrated, or if coherency analysis indicates a pile-up
//     //      in the silicon or other inconsistency, the Silicon energy is calculated from the calibrated
//     //      CsI energy (ECODE=kECode2)
//     //    - if the Ionisation Chamber is not calibrated, or if coherency analysis indicates a pile-up
//     //      in the ChIo or other inconsistency, this contribution is calculated using the total
//     //      cumulated energy measured in the CsI and/or Silicon detectors (ECODE=kECode2)
//     //
//     // For particles stopping in the Silicon detector (IDCODE=3, 4 or 5):
//     //    - if no calibration exists for the Silicon, we cannot calibrate the particle (ECODE=kECode0)
//     //    - if the Ionisation Chamber is not calibrated, or if coherency analysis indicates a pile-up
//     //      in the ChIo or other inconsistency, this contribution is calculated using the total
//     //      cumulated energy measured in the CsI and/or Silicon detectors (ECODE=kECode2)
//
//     fECsI=fESi=fEChIo=0;
//
//     if(GetCodes().TestIDCode(kIDCode_Gamma)){
//         DoGammaCalibration();
//         return;
//     }
//     if(GetCodes().TestIDCode(kIDCode_Neutron)){
//         DoNeutronCalibration();
//         return;
//     }
//
//     SetECode(kECode1);
//
//     Bool_t stopped_in_silicon=kTRUE;
//
//     if(GetCsI()){
//         stopped_in_silicon=kFALSE;
//         if(GetCsI()->IsCalibrated()){
//             /* CSI ENERGY CALIBRATION */
//             if( GetCodes().TestIDCode(kIDCode_CsI) && GetZ()==4 && GetA()==8 ){
//                 DoBeryllium8Calibration();
//             }
//             else
//                 fECsI = GetCsI()->GetCorrectedEnergy(this, -1., kFALSE);
//         }
//         else
//         {
//             SetNoCalibrationStatus();// no CsI calibration - no calibration performed
//             return;
//         }
//         if(GetCodes().TestECode(kECode1) && fECsI<=0){ // for kECode2, fECsI is always <0
//             //Info("Calib", "ECsI = %f",fECsI);
//             SetBadCalibrationStatus();// problem with CsI energy - no calibration
//             return;
//         }
//     }
//     if(GetSi()){
//     /* SILICIUM ENERGY CONTRIBUTION */
//     // if fPileup = kTRUE, the Silicon energy appears to include a contribution from another particle
//     //     therefore we have to estimate the silicon energy for this particle using the CsI energy
//     // if fCoherent = kFALSE, the Silicon energy is too small to be consistent with the CsI identification,
//     //     therefore we have to estimate the silicon energy for this particle using the CsI energy
//
//         if(!fPileup && fCoherent && GetSi()->IsCalibrated()){
//             // all is apparently well. use full energy deposited in silicon.
//             Bool_t si_transmission=kTRUE;
//             if(stopped_in_silicon){
//                 si_transmission=kFALSE;
//             }
//             else
//             {
//                 GetSi()->SetEResAfterDetector(TMath::Abs(fECsI));
//             }
//             fESi = GetSi()->GetCorrectedEnergy(this,-1.,si_transmission);
//             if(fESi<=0) {
//                 // problem with silicon calibration
//                if(!stopped_in_silicon && (TMath::Abs(fECsI)>0.0)){
//                   if(!CalculateSiliconDEFromResidualEnergy()) return;
//                }
//                else
//                {
//                   // stopped in Si with bad Si calibration - no good
//                   SetBadCalibrationStatus();
//                   return;
//                }
//             }
//         }
//         else
//         {
//             if(!stopped_in_silicon){
//                if(!CalculateSiliconDEFromResidualEnergy()) return;
//             }
//             else
//             {
//                 // cannot calibrate particle stopping in silicon in this case
//                 SetNoCalibrationStatus();
//                 return;
//             }
//         }
//
//     }
//     if(GetChIo()){
//     /* CHIO ENERGY CONTRIBUTION */
//     // if fUseFullChIoEnergyForCalib = kFALSE, we have to estimate the ChIo energy for this particle
//        Double_t ERES = TMath::Abs(fESi)+TMath::Abs(fECsI);
//         if(fUseFullChIoEnergyForCalib && GetChIo()->IsCalibrated()){
//             // all is apparently well
//             if(!StoppedInChIo()){
//                 GetChIo()->SetEResAfterDetector(ERES);
//                 fEChIo = GetChIo()->GetCorrectedEnergy(this);
//             }
//             else
//             {
//                 // particle stopped in chio, not in transmission
//                 fEChIo = GetChIo()->GetCorrectedEnergy(this, -1., kFALSE);
//             }
//             if(fEChIo<0.){
//                // bad chio calibration
//                if(!StoppedInChIo() && (ERES>0.0)){
//                   CalculateChIoDEFromResidualEnergy(ERES);
//                }
//             }
//         }
//         else
//         {
//             if(!StoppedInChIo()){
//                 CalculateChIoDEFromResidualEnergy(ERES);
//             }
//             else
//             {
//                 // particle stopped in ChIo, no calibration available
//                 SetNoCalibrationStatus();
//                 return;
//             }
//         }
//     }
//      SetEnergy( TMath::Abs(fECsI) + TMath::Abs(fESi) + TMath::Abs(fEChIo) );
// }
//
// void KVFAZIAReconNuc::CalculateSiLiDEFromResidualEnergy(Double_t ERES)
// {
//    // Etalon modules
//    // calculate fESiLi from residual CsI energy
//     Double_t e0 = GetSiLi()->GetDeltaEFromERes(GetZ(),GetA(),ERES);
//     GetSiLi()->SetEResAfterDetector(ERES);
//     fESiLi = GetSiLi()->GetCorrectedEnergy(this,e0);
//     fESiLi = -TMath::Abs(fESiLi);
//     SetECode(kECode2);
// }
//
// void KVFAZIAReconNuc::CalculateSi75DEFromResidualEnergy(Double_t ERES)
// {
//    // Etalon modules
//    // calculate fESi75 from residual CsI+SiLi energy
//    Double_t e0 = GetSi75()->GetDeltaEFromERes(GetZ(),GetA(),ERES);
//    GetSi75()->SetEResAfterDetector(ERES);
//    fESi75 = GetSi75()->GetCorrectedEnergy(this,e0);
//    fESi75 = -TMath::Abs(fESi75);
//    SetECode(kECode2);
// }
//
// void KVFAZIAReconNuc::CalculateChIoDEFromResidualEnergy(Double_t ERES)
// {
//    // calculate fEChIo from residual energy
//    Double_t e0 = GetChIo()->GetDeltaEFromERes(GetZ(),GetA(),ERES);
//    GetChIo()->SetEResAfterDetector(ERES);
//    fEChIo = GetChIo()->GetCorrectedEnergy(this,e0);
//    fEChIo = -TMath::Abs(fEChIo);
//    SetECode(kECode2);
// }
//
// //_________________________________________________________________________________
// void KVFAZIAReconNuc::CalibrateRings10To17()
// {
//     // Special calibration for particles in rings 10 to 17
//     // We set the energy calibration code for the particle here
//     //    kECode0 = no calibration (e.g. gammas)
//     //    kECode1 = everything OK
//     //    kECode2 = small warning, for example if energy loss in a detector is calculated
//     //    kECode15 = bad, calibration is no good
//     // The contributions from ChIo & CsI are stored in member variables fEChIo, fECsI
//    // If the contribution is calculated rather than measured, it is stored as a negative value
//
//     fECsI=fEChIo=fESi75=fESiLi=0;
//     if(GetCodes().TestIDCode(kIDCode_Gamma)){
//        DoGammaCalibration();
//        return;
//     }
//     // change fUseFullChioenergyforcalib for "coherency" particles
//     // we assume they are calibrated after all other particles in group have
//     // been identified, calibrated, and their energy contributions removed
//     // from the ChIo
//     if(GetCodes().TestIDCode(kIDCode6) || GetCodes().TestIDCode(kIDCode7) || GetCodes().TestIDCode(kIDCode8))
//        fUseFullChIoEnergyForCalib=kTRUE;
//
//     SetECode(kECode1);
//     Bool_t stopped_in_chio=kTRUE;
//     if(GetCsI()){
//        stopped_in_chio=kFALSE;
//        /* CSI ENERGY CALIBRATION */
//        if( GetCodes().TestIDCode(kIDCode_CsI) && GetZ()==4 && GetA()==8 ){
//           DoBeryllium8Calibration();
//        }
//        else
//           fECsI = GetCsI()->GetCorrectedEnergy(this, -1., kFALSE);
//
//        if(fECsI<=0){
//           SetBadCalibrationStatus();// bad - no CsI energy
//           return;
//        }
//     }
//     if(fIncludeEtalonsInCalibration){
//         if(GetSiLi()){
//             Double_t ERES = fECsI;
//             if(!fPileupSiLi && GetSiLi()->IsCalibrated()){
//                 Bool_t si_transmission=kTRUE;
//                 if(StoppedInSiLi()){
//                     si_transmission=kFALSE;
//                 }
//                 else
//                 {
//                     GetSiLi()->SetEResAfterDetector(ERES);
//                 }
//                 fESiLi = GetSiLi()->GetCorrectedEnergy(this,-1.,si_transmission);
//                 if(fESiLi<=0) {
//                     if(!StoppedInSiLi() && ERES>0.0){
//                        CalculateSiLiDEFromResidualEnergy(ERES);
//                     }
//                     else
//                     {
//                        SetBadCalibrationStatus();
//                        return;
//                     }
//                 }
//             }
//             else
//             {
//                CalculateSiLiDEFromResidualEnergy(ERES);
//             }
//         }
//         if(GetSi75()){
//             Double_t ERES = fECsI + TMath::Abs(fESiLi);
//             if(!fPileupSi75 &&!fPileupSiLi && GetSi75()->IsCalibrated()){
//                 Bool_t si_transmission=kTRUE;
//                 if(StoppedInSi75()){
//                     si_transmission=kFALSE;
//                 }
//                 else
//                 {
//                     GetSi75()->SetEResAfterDetector(ERES);
//                 }
//                 fESi75 = GetSi75()->GetCorrectedEnergy(this,-1.,si_transmission);
//                 if(fESi75<=0) {
//                    if(!StoppedInSi75() && ERES>0.0){
//                       CalculateSi75DEFromResidualEnergy(ERES);
//                    }
//                    else
//                    {
//                       SetBadCalibrationStatus();
//                       return;
//                    }
//                 }
//             }
//             else
//             {
//                CalculateSi75DEFromResidualEnergy(ERES);
//             }
//         }
//     }
//     if(GetChIo()){
//     /* IONISATION CHAMBER ENERGY CONTRIBUTION */
//     // if fUseFullChIoEnergyForCalib = kFALSE, ChIo was hit by other particles in group
//     //     therefore we have to estimate the ChIo energy for this particle using the CsI energy
//     // if fPileupChIo = kTRUE, there appears to be another particle stopped in the ChIo
//     //     therefore we have to estimate the ChIo energy for this particle using the CsI energy
//         Double_t ERES = fECsI+TMath::Abs(fESiLi)+TMath::Abs(fESi75);
//         if(!fPileupChIo && fUseFullChIoEnergyForCalib && GetChIo()->IsCalibrated()){
//            // all is apparently well
//            Bool_t ci_transmission=kTRUE;
//            if(stopped_in_chio){
//               ci_transmission=kFALSE;
//            }
//            else{
//               GetChIo()->SetEResAfterDetector(ERES);
//            }
//            fEChIo = GetChIo()->GetCorrectedEnergy(this,-1.,ci_transmission);
//            if(fEChIo<=0) {
//               if(!stopped_in_chio && ERES>0){
//                  CalculateChIoDEFromResidualEnergy(ERES);
//               }
//            }
//         }
//         else{
//             if(!stopped_in_chio && ERES>0){
//                 CalculateChIoDEFromResidualEnergy(ERES);
//             }
//         }
//     }
//
//
//      SetEnergy( fECsI + TMath::Abs(fESiLi) + TMath::Abs(fESi75) + TMath::Abs(fEChIo) );
// }
//
// //________________________________________________________________________________//
//
// void KVFAZIAReconNuc::CheckCsIEnergy()
// {
//    // Check calculated CsI energy loss of particle.
//    // If it is greater than the maximum theoretical energy loss
//    // (depending on the length of CsI, the Z & A of the particle)
//    // we set the energy calibration code to kECode3 (historical VEDA code
//    // for particles with E_csi > E_max_csi)
//
//    KVDetector* csi = GetCsI();
//     if(csi && GetZ()>0 && GetZ()<3 && (csi->GetEnergy() > csi->GetMaxDeltaE(GetZ(), GetA()))) SetECode(kECode3);
// }
//
// Int_t KVFAZIAReconNuc::GetIDSubCode(const Char_t * id_tel_type,KVIDSubCode &) const
// {
//     // OBSOLETE METHOD
//
//     KVINDRAIDTelescope *idtel;
//     if (strcmp(id_tel_type, ""))
//         idtel =
//             (KVINDRAIDTelescope *) GetIDTelescopes()->
//             FindObjectByType(id_tel_type);
//     else
//         idtel = (KVINDRAIDTelescope *)GetIdentifyingTelescope();
//     if (!idtel)
//         return -65535;
// //    return idtel->GetIDSubCode(code);
//     return -65535;
// }
//
// //______________________________________________________________________________________________//
//
// const Char_t *KVFAZIAReconNuc::GetIDSubCodeString(const Char_t *id_tel_type,KVIDSubCode &) const
// {
//     // OBSOLETE METHOD
//
//     KVINDRAIDTelescope *idtel;
//     if (strcmp(id_tel_type, ""))
//         idtel =
//             (KVINDRAIDTelescope *) GetIDTelescopes()->
//             FindObjectByType(id_tel_type);
//     else
//         idtel = (KVINDRAIDTelescope *)GetIdentifyingTelescope();
//     if (!idtel) {
//         if (strcmp(id_tel_type, ""))
//             return Form("No identification attempted in %s", id_tel_type);
//         else
//             return
//                 Form("Particle unidentified. Identifying telescope not set.");
//     }
//     //return idtel->GetIDSubCodeString(code);
//     return id_tel_type;
// }
void KVFAZIAReconNuc::MakeDetectorList()
{
   // Protected method, called when required to fill fDetList with pointers to
   // the detectors whose names are stored in fDetNames.
   // If gMultiDetArray=0x0, fDetList list will be empty.
   KVFAZIADetector* det = 0;
   Double_t val = -1;
   fDetList.Clear();
   if (gMultiDetArray) {
      fDetNames.Begin("/");
      while (!fDetNames.End()) {
         det = (KVFAZIADetector*)gMultiDetArray->GetDetector(fDetNames.Next(kTRUE));
         if (det) {
            fDetList.Add(det);
            if (!strcmp(det->GetLabel(), "SI1")) {
               val = GetParameters()->GetDoubleValue(Form("%s.QL1.Amplitude", det->GetName()));
               det->SetQL1Amplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.QL1.RawAmplitude", det->GetName()));
               det->SetQL1RawAmplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.QL1.BaseLine", det->GetName()));
               det->SetQL1BaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.QL1.SigmaBaseLine", det->GetName()));
               det->SetQL1SigmaBaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.QL1.RiseTime", det->GetName()));
               det->SetQL1RiseTime(val);

               val = GetParameters()->GetDoubleValue(Form("%s.QH1.Amplitude", det->GetName()));
               det->SetQH1Amplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.QH1.RawAmplitude", det->GetName()));
               det->SetQH1RawAmplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.QH1.BaseLine", det->GetName()));
               det->SetQH1BaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.QH1.SigmaBaseLine", det->GetName()));
               det->SetQH1SigmaBaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.QH1.RiseTime", det->GetName()));
               det->SetQH1RiseTime(val);

               val = GetParameters()->GetDoubleValue(Form("%s.I1.Amplitude", det->GetName()));
               det->SetI1Amplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.I1.RawAmplitude", det->GetName()));
               det->SetI1RawAmplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.I1.BaseLine", det->GetName()));
               det->SetI1BaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.I1.SigmaBaseLine", det->GetName()));
               det->SetI1SigmaBaseLine(val);

            } else if (!strcmp(det->GetLabel(), "SI2")) {

               val = GetParameters()->GetDoubleValue(Form("%s.Q2.Amplitude", det->GetName()));
               det->SetQ2Amplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q2.RawAmplitude", det->GetName()));
               det->SetQ2RawAmplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q2.BaseLine", det->GetName()));
               det->SetQ2BaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q2.SigmaBaseLine", det->GetName()));
               det->SetQ2SigmaBaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q2.RiseTime", det->GetName()));
               det->SetQ2RiseTime(val);

               val = GetParameters()->GetDoubleValue(Form("%s.I2.Amplitude", det->GetName()));
               det->SetI2Amplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.I2.RawAmplitude", det->GetName()));
               det->SetI2RawAmplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.I2.BaseLine", det->GetName()));
               det->SetI2BaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.I2.SigmaBaseLine", det->GetName()));
               det->SetI2SigmaBaseLine(val);
            } else if (!strcmp(det->GetLabel(), "CSI")) {
               val = GetParameters()->GetDoubleValue(Form("%s.Q3.Amplitude", det->GetName()));
               det->SetQ3Amplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q3.RawAmplitude", det->GetName()));
               det->SetQ3RawAmplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q3.FastAmplitude", det->GetName()));
               det->SetQ3FastAmplitude(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q3.BaseLine", det->GetName()));
               det->SetQ3BaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q3.SigmaBaseLine", det->GetName()));
               det->SetQ3SigmaBaseLine(val);
               val = GetParameters()->GetDoubleValue(Form("%s.Q3.RiseTime", det->GetName()));
               det->SetQ3RiseTime(val);

            }

         }
      }
   }
}
