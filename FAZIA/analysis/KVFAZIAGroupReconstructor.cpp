#include "KVFAZIAGroupReconstructor.h"

#include <KVFAZIA.h>
#include <KVFAZIADetector.h>
#include <KVPSAResult.h>
#include <KVSignal.h>
#include <KVLightEnergyCsIFull.h>
#include <KVLightEnergyCsI.h>
#include <KVCalibrator.h>

ClassImp(KVFAZIAGroupReconstructor)

//////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIAGroupReconstructor</h2>
<h4>Reconstruct particles in FAZIA telescopes</h4>
<!-- */
// --> END_HTML
//////////////////////////////////////////////////////////////////////////////////
//KVReconstructedNucleus* KVFAZIAGroupReconstructor::ReconstructTrajectory(const KVGeoDNTrajectory* traj, const KVGeoDetectorNode* node)
//{
//   // Specialised event reconstruction for FAZIA data
//   // Triggered CsI detectors are checked: if it is a gamma, we count it (parameter "FAZIA_GAMMA_MULT")
//   // and add the name of the detector to the parameter "FAZIA_GAMMA_DETS"
//   // but do not reconstruct a particle.

//   if (node->GetDetector()->IsType("CsI")) {
//      if (node->GetDetector()->Fired()) {
//         ++nfireddets;
//         KVIDTelescope* idt = (KVIDTelescope*)traj->GetIDTelescopes()->FindObjectByType("CsI");
//         if (idt) {
//            KVIdentificationResult idr;
//            if (idt->IsReadyForID()) {
//               idt->Identify(&idr);
//               if (idr.IDOK && idr.IDcode == 0) {
//                  GetEventFragment()->GetParameters()->IncrementValue("FAZIA_GAMMA_MULT", 1);
//                  GetEventFragment()->GetParameters()->IncrementValue("FAZIA_GAMMA_DETS", node->GetName());
//                  node->GetDetector()->SetAnalysed();
//                  return nullptr;
//               }
//            }
//         }
//         return GetEventFragment()->AddParticle();
//      }
//      return nullptr;
//   }
//   return KVGroupReconstructor::ReconstructTrajectory(traj, node);
//}

void KVFAZIAGroupReconstructor::CalibrateParticle(KVReconstructedNucleus* PART)
{
//   // Perform energy calibration of (previously identified) particle


   KVNucleus avatar;
   //printf("start Calibrate\n");
   Int_t ntot = PART->GetReconstructionTrajectory()->GetN();
   if (ntot < 1)
      return;
   Bool_t punch_through = kFALSE;
   Bool_t incoherency = kFALSE;
   Bool_t pileup = kFALSE;
   Bool_t check_error = kFALSE;

   double error_si1 = 0, error_si2 = 0; // error_csi=0;
   Double_t* eloss = new Double_t[ntot];
   for (Int_t ii = 0; ii < ntot; ii += 1) eloss[ii] = 0;

   KVFAZIADetector* det = 0;
   Int_t ndet = 0;
   Int_t ndet_calib = 0;
   Double_t etot = 0;
   Double_t fESI1, fESI2, fECSI;
   fESI1 = fESI2 = fECSI = 0;

   PART->GetReconstructionTrajectory()->IterateFrom();
   KVGeoDetectorNode* node;
   while ((node = PART->GetReconstructionTrajectory()->GetNextNode())) {
      det = (KVFAZIADetector*)node->GetDetector();

      if (det->IsCalibrated()) {
         if (det->GetIdentifier() == KVFAZIADetector::kCSI) {
            if (det->GetCalibrator("Channel-Energy")->InheritsFrom("KVLightEnergyCsIFull")) {
               KVLightEnergyCsIFull* calib = (KVLightEnergyCsIFull*)det->GetCalibrator("Channel-Energy");
               calib->SetZ(PART->GetZ());
               calib->SetA(PART->GetA());
               eloss[ntot - ndet - 1] = calib->Compute(det->GetQ3Amplitude());
            }
            else if (det->GetCalibrator("Channel-Energy")->InheritsFrom("KVLightEnergyCsI")) {
               KVLightEnergyCsI* calib = (KVLightEnergyCsI*)det->GetCalibrator("Channel-Energy");
               calib->SetZ(PART->GetZ());
               calib->SetA(PART->GetA());
               eloss[ntot - ndet - 1] = calib->Compute(det->GetQ3Amplitude());
            }
         }
         else eloss[ntot - ndet - 1] = det->GetEnergy();

         if (det->GetIdentifier() == KVFAZIADetector::kSI1)      fESI1 = eloss[ntot - ndet - 1];
         else if (det->GetIdentifier() == KVFAZIADetector::kSI2) fESI2 = eloss[ntot - ndet - 1];
         else if (det->GetIdentifier() == KVFAZIADetector::kCSI) fECSI = eloss[ntot - ndet - 1];
         etot += eloss[ntot - ndet - 1];
         ndet_calib += 1;
      }
      ndet += 1;
   }
   if (ndet == ndet_calib) {
      Double_t E_targ = 0;
      PART->SetEnergy(etot);

      if (PART->IsAMeasured()) {
         Double_t etot_avatar = 0;
         Double_t chi2 = 0;
         avatar.SetZAandE(PART->GetZ(), PART->GetA(), PART->GetKE());
         for (Int_t nn = ntot - 1; nn >= 0; nn -= 1) {
            det = (KVFAZIADetector*)PART->GetDetector(nn);
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
         }
         else if (chi2 > 10.) {
            //Warning("Calibrate", "Incoherence %lf != %lf", etot, etot_avatar);
            incoherency = kTRUE;
         }
      }

      if (PART->GetZ() && PART->GetEnergy() > 0) {
         E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(PART);
         PART->SetTargetEnergyLoss(E_targ);
      }

      Double_t E_tot = PART->GetEnergy() + E_targ;
      PART->SetEnergy(E_tot);
      // set particle momentum from telescope dimensions (random)
      PART->GetAnglesFromStoppingDetector();
      PART->SetECode(0);
      if (punch_through)   PART->SetECode(2);
      if (incoherency)     PART->SetECode(3);
      if (check_error)     PART->SetECode(5); //
      if (pileup)          PART->SetECode(4); //

      PART->SetIsCalibrated();
   }
   else {
      if (((KVFAZIADetector*)PART->GetStoppingDetector())->GetIdentifier() == KVFAZIADetector::kCSI  && !(PART->GetStoppingDetector()->IsCalibrated()) && ndet_calib == 2) {
         if (PART->GetZ() > 0) {
//            if (!PART->IsAMeasured()) {
//               if (GetZ() == 1)       SetA(1);
//               else if (GetZ() == 2)  SetA(4);
//               else if (GetZ() == 20) SetA(48);
//               else {
//                  SetA(1.04735 + 1.99941 * GetZ() + 0.00683224 * TMath::Power(GetZ(), 2.));
//               }
//            }

            Double_t E_targ = 0;
            fECSI = ((KVDetector*)PART->GetStoppingDetector()->GetNode()->GetDetectorsInFront()->First())->GetEResFromDeltaE(PART->GetZ(), PART->GetA(), fESI2);
            PART->SetEnergy(fECSI + fESI1 + fESI2);

            E_targ = ((KVMultiDetArray*)GetGroup()->GetArray())->GetTargetEnergyLossCorrection(PART);
            Double_t E_tot = PART->GetEnergy() + E_targ;
            PART->SetECode(1);
            PART->SetIsCalibrated();
            PART->SetEnergy(E_tot);
            PART->GetAnglesFromStoppingDetector();
         }
      }
   }

//   if(PART->IsCalibrated()) Info("CalibrateParticle","calibrate particle (%d,%d) E = %lf MeV/A",PART->GetZ(),PART->GetA(), PART->GetKE()/PART->GetA());


   delete [] eloss;
}

void KVFAZIAGroupReconstructor::PostReconstructionProcessing()
{
   // Copy FPGA energy values to reconstructed particle parameter lists
   // Set values in detectors for identification/calibration procedures

   for (KVEvent::Iterator it = GetEventFragment()->begin(); it != GetEventFragment()->end(); ++it) {
      KVReconstructedNucleus* rnuc = it.pointer<KVReconstructedNucleus>();

      rnuc->GetReconstructionTrajectory()->IterateFrom();

      KVGeoDetectorNode* node;
      while ((node = rnuc->GetReconstructionTrajectory()->GetNextNode())) {

         KVFAZIADetector* det = (KVFAZIADetector*)node->GetDetector();
         TIter next_s(det->GetListOfSignals());
         KVSignal* sig;
         while ((sig = (KVSignal*)next_s())) {
            if (sig->HasFPGA()) {
               for (Int_t ii = 0; ii < sig->GetNFPGAValues(); ii += 1) {
                  KVString label = "";
                  if (ii == 0) label = "FPGAEnergy";
                  if (ii == 1) label = "FPGAFastEnergy"; //only for CsI Q3
                  Double_t ene = ((KVFAZIA*)GetGroup()->GetArray())->GetFPGAEnergy(
                                    det->GetBlockNumber(),
                                    det->GetQuartetNumber(),
                                    det->GetTelescopeNumber(),
                                    sig->GetType(),
                                    ii
                                 );

                  rnuc->SetParameter(
                     Form("%s.%s.%s", det->GetName(), sig->GetName(), label.Data()), ene
                  );
               }
            }
            if (!sig->PSAHasBeenComputed()) {
               sig->TreateSignal();
            }

            unique_ptr<KVPSAResult> psa(sig->GetPSAResult());
            if (psa.get()) {
               *(rnuc->GetParameters()) += (KVNameValueList&) * psa;
            }
         }

         if (!strcmp(det->GetLabel(), "SI1")) {
            double val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.Amplitude", det->GetName()));
            det->SetQL1Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.RawAmplitude", det->GetName()));
            det->SetQL1RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.BaseLine", det->GetName()));
            det->SetQL1BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.SigmaBaseLine", det->GetName()));
            det->SetQL1SigmaBaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.RiseTime", det->GetName()));
            det->SetQL1RiseTime(val);

            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.FPGAEnergy", det->GetName()));
            det->SetQH1FPGAEnergy(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.Amplitude", det->GetName()));
            det->SetQH1Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.RawAmplitude", det->GetName()));
            det->SetQH1RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.BaseLine", det->GetName()));
            det->SetQH1BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.SigmaBaseLine", det->GetName()));
            det->SetQH1SigmaBaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.RiseTime", det->GetName()));
            det->SetQH1RiseTime(val);

            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I1.Amplitude", det->GetName()));
            det->SetI1Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I1.RawAmplitude", det->GetName()));
            det->SetI1RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I1.BaseLine", det->GetName()));
            det->SetI1BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I1.SigmaBaseLine", det->GetName()));
            det->SetI1SigmaBaseLine(val);

         }
         else if (!strcmp(det->GetLabel(), "SI2")) {

            double val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.FPGAEnergy", det->GetName()));
            det->SetQ2FPGAEnergy(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.Amplitude", det->GetName()));
            det->SetQ2Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.RawAmplitude", det->GetName()));
            det->SetQ2RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.BaseLine", det->GetName()));
            det->SetQ2BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.SigmaBaseLine", det->GetName()));
            det->SetQ2SigmaBaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.RiseTime", det->GetName()));
            det->SetQ2RiseTime(val);

            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I2.Amplitude", det->GetName()));
            det->SetI2Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I2.RawAmplitude", det->GetName()));
            det->SetI2RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I2.BaseLine", det->GetName()));
            det->SetI2BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I2.SigmaBaseLine", det->GetName()));
            det->SetI2SigmaBaseLine(val);
         }
         else if (!strcmp(det->GetLabel(), "CSI")) {
            double val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.FPGAEnergy", det->GetName()));
            det->SetQ3FPGAEnergy(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.FPGAFastEnergy", det->GetName()));
            det->SetQ3FastFPGAEnergy(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.Amplitude", det->GetName()));
            det->SetQ3Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.RawAmplitude", det->GetName()));
            det->SetQ3RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.FastAmplitude", det->GetName()));
            det->SetQ3FastAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.BaseLine", det->GetName()));
            det->SetQ3BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.SigmaBaseLine", det->GetName()));
            det->SetQ3SigmaBaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.RiseTime", det->GetName()));
            det->SetQ3RiseTime(val);

         }

      }
   }

}
