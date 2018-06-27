#include "KVFAZIAGroupReconstructor.h"

#include <KVFAZIA.h>
#include <KVFAZIADetector.h>
#include <KVPSAResult.h>
#include <KVSignal.h>

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
