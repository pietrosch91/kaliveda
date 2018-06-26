//Created by KVClassFactory on Tue Jan 27 11:37:39 2015
//Author: ,,,

#include "KVFAZIA.h"
#include "KVGeoImport.h"
#include "KVSignal.h"
#include "KVFAZIADetector.h"
#include "KVGroup.h"
#include "KVFAZIABlock.h"
#include "KVDetectorEvent.h"
#include "KVTarget.h"
#include "TSystem.h"
#include "KVDataSet.h"
#include "KVConfig.h"

//#include "TGeoBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoEltu.h"

#include <KVReconstructedNucleus.h>

#ifdef WITH_MFM
#include "MFMFaziaFrame.h"
#endif

#ifdef WITH_PROTOBUF
#include "FzEventSet.pb.h"
#endif

ClassImp(KVFAZIA)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIA</h2>
<h4>Base class for description of the FAZIA set up</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
KVFAZIA* gFazia;

KVFAZIA::KVFAZIA(const Char_t* title)
   : KVMultiDetArray("FAZIA", title)
{
   // Default constructor
   fStartingBlockNumber = 0;
   gFazia = this;
   fDetectorLabels = "";
   fSignalTypes = "QL1,I1,QH1,Q2,I2,Q3";
   SetGeometryImportParameters();
   CreateCorrespondence();
}

KVFAZIA::~KVFAZIA()
{
   // Destructor

   if (gFazia == this) gFazia = nullptr;
}

void KVFAZIA::AddDetectorLabel(const Char_t* label)
{
   if (fDetectorLabels == "") fDetectorLabels += label;
   else if (!fDetectorLabels.Contains(label)) fDetectorLabels += Form(",%s", label);
}

void KVFAZIA::GenerateCorrespondanceFile()
{
   // Look for the geometry object <-> detector name correspondance file in the dataset directory
   //  If not found, we create it

   fCorrespondanceFile = gDataSet->GetFullPathToDataSetFile(Form("%s.names", ClassName()));
   if (fCorrespondanceFile != "") return;

#ifdef WITH_GNU_INSTALL
   fCorrespondanceFile.Form("%s/%s.names", KVBase::WorkingDirectory(), ClassName());
#else
   fCorrespondanceFile.Form("%s/%s.names", gDataSet->GetDataSetDir(), ClassName());
#endif
   Info("GenerateCorrespondanceFile", "Creation de %s", fCorrespondanceFile.Data());
   KVEnv env;

   fDetectorLabels = "SI1,SI2,CSI";

   SetNameOfDetectors(env);
   if (env.GetTable() && env.GetTable()->GetEntries() > 0) {
      env.AddCommentLine(Form("Automatic generated file by %s::GenerateCorrespondanceFile", ClassName()));
      env.AddCommentLine("Make link between geometric ROOT objects and detector names");
      env.WriteFile(fCorrespondanceFile.Data());
   }
   fDetectorLabels = "";
}

void KVFAZIA::SetNameOfDetectors(KVEnv& env)
{
   //define the format of detectors name
   // label-index
   // where index = block*100+quartet*10+telescope
   // example :
   // SI1-123 is the Silicon 1 of the block 1, the quartet 2 and the telescope 3
   //

   for (Int_t bb = fStartingBlockNumber; bb < fNblocks; bb += 1) {
      for (Int_t qq = 1; qq <= 4; qq += 1) {
         for (Int_t tt = 1; tt <= 4; tt += 1) {
            fDetectorLabels.Begin(",");
            while (!fDetectorLabels.End()) {
               KVString sdet = fDetectorLabels.Next();
               env.SetValue(
                  Form("BLOCK_%d_QUARTET_%d_%s-T%d", bb, qq, sdet.Data(), tt),
                  Form("%s-%d", sdet.Data(), bb * 100 + qq * 10 + tt)
               );
            }
         }
      }
   }
}

void KVFAZIA::PerformClosedROOTGeometryOperations(Int_t run)
{
   // Finalise description of array performing all operations which require ROOT
   // geometry to be closed

   KVGeoImport imp(gGeoManager, KVMaterial::GetRangeTable(), this, kTRUE);
   imp.SetDetectorPlugin(ClassName());
   imp.SetNameCorrespondanceList(fCorrespondanceFile.Data());
   // any additional structure name formatting definitions
   DefineStructureFormats(imp);

   // the following parameters are optimized for a 12-block compact
   // geometry placed at 80cm with rings 1-5 of INDRA removed.
   // make sure that the expected number of detectors get imported!
   imp.ImportGeometry(fImport_dTheta, fImport_dPhi, fImport_ThetaMin, fImport_PhiMin, fImport_ThetaMax, fImport_PhiMax);

   SetCalibrators();
   SetIdentifications();

   SetDetectorThicknesses();
   SetBit(kIsBuilt);

   if (run != -1) {
      SetParameters(run);
   }
}

void KVFAZIA::GetGeometryParameters()
{
   //Called by the Build method
   Info("GetGeometryParameters", "to be defined in child class ...");
}

void KVFAZIA::BuildFAZIA()
{
   //Called by the Build method
   Info("BuildFAZIA", "to be defined in child class ...");

}

void KVFAZIA::BuildTarget()
{

   KVMaterial target_holder_mat("Al");
   new TGeoBBox("TARGET_FRAME", 3., 3., 0.1 / 2.);
   new TGeoEltu("TARGET_HOLE", 2., 2., 0.1 / 2.);
   TGeoCompositeShape* cs = new TGeoCompositeShape("TARGET_FRAME", "TARGET_FRAME - TARGET_HOLE");
   TGeoVolume* target_frame = new TGeoVolume("TARGET_FRAME", cs, target_holder_mat.GetGeoMedium());
   gGeoManager->GetTopVolume()->AddNode(target_frame, 1);

   KVTarget* T = GetTarget();
   if (T) {
      KVMaterial* targMat = (KVMaterial*)T->GetLayers()->First();
      TGeoVolume* target = gGeoManager->MakeEltu("TARGET", targMat->GetGeoMedium(), 2., 2., targMat->GetThickness() / 2.);
      gGeoManager->GetTopVolume()->AddNode(target, 1);
   }
}

void KVFAZIA::Build(Int_t run)
{
   // Build the FAZIA array
   GetGeometryParameters();
   GenerateCorrespondanceFile();

   CreateGeoManager();

   BuildFAZIA();

   if (fBuildTarget)
      BuildTarget();

   if (fCloseGeometryNow) {
      gGeoManager->DefaultColors();
      gGeoManager->CloseGeometry();
      PerformClosedROOTGeometryOperations(run);
   }
}

void KVFAZIA::GetDetectorEvent(KVDetectorEvent* detev, const TSeqCollection* sigs)
{
   // First step in event reconstruction based on current status of detectors in array.
   // Fills the given KVDetectorEvent with the list of all groups which have fired.
   // i.e. loop over all groups of the array and test whether KVGroup::Fired() returns true or false.
   //
   // If the list of fired acquisition parameters 'sigs' is not given,
   // KVMultiDetArray::GetDetectorEvent is called. We check also if the internal fFiredACQParams
   // list contains data.
   //

   if (!sigs || !sigs->GetEntries()) {
      if (fFiredACQParams.GetEntries()) sigs = &fFiredACQParams;
   }
   if (sigs && sigs->GetEntries()) {
      // check type of first object in list
      // if it is a detector, we are in ze bidouille for indrafazia tests
      if (sigs->First()->InheritsFrom("KVDetector")) {
         TIter next_det(sigs);
         KVDetector* det = 0;
         KVGroup* grp = 0;
         while ((det = (KVDetector*)next_det())) {
            if ((grp = det->GetGroup())) {
               detev->AddGroup(grp);
            }
         }
         return;
      }
      // list of fired acquisition parameters given
      TIter next_par(sigs);

      KVSignal* par = 0;
      KVDetector* det = 0;
      KVGroup* grp = 0;
      while ((par = (KVSignal*)next_par())) {
         if (!(par->GetN() > 0))
            Info("GetDetectorEvent", "%s empty", par->GetName());
         par->DeduceFromName();
         //          if (!(det = GetDetector(par->GetDetectorName()))) {
         //             det = GetDetector(KVFAZIADetector::GetNewName(par->GetDetectorName()));
         //          }
         det = GetDetector(par->GetDetectorName());
         if (det) {
            ((KVFAZIADetector*)det)->SetSignal(par, par->GetType());
            if ((!(((KVFAZIADetector*)det)->GetSignal(par->GetType())->GetN() > 0)))
               Warning("Error", "%s %s empty signal is returned", det->GetName(), par->GetType());
            if ((grp = det->GetGroup())) {
               detev->AddGroup(grp);
            }
         }
         else {
            Error("GetDetectedEvent", "Unknown detector %s !!!", par->GetDetectorName());
         }
      }
   }
   else {
      KVMultiDetArray::GetDetectorEvent(detev, 0);
   }

}

void KVFAZIA::FillDetectorList(KVReconstructedNucleus* rnuc, KVHashList* DetList, const KVString& DetNames)
{
   // Protected method, called when required to fill fDetList with pointers to
   // the detectors whose names are stored in fDetNames.
   // Also set all raw data values in the detectors.

   KVFAZIADetector* det = 0;
   Double_t val = -1;
   DetList->Clear();
   DetNames.Begin("/");
   while (!DetNames.End()) {
      KVString sdet = DetNames.Next(kTRUE);
      det = (KVFAZIADetector*)GetDetector(sdet.Data());
      if (!det) {
         det = (KVFAZIADetector*)GetDetector(KVFAZIADetector::GetNewName(sdet.Data()));
      }

      if (det) {
         DetList->Add(det);
         if (!strcmp(det->GetLabel(), "SI1")) {
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.Amplitude", sdet.Data()));
            det->SetQL1Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.RawAmplitude", sdet.Data()));
            det->SetQL1RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.BaseLine", sdet.Data()));
            det->SetQL1BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.SigmaBaseLine", sdet.Data()));
            det->SetQL1SigmaBaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QL1.RiseTime", sdet.Data()));
            det->SetQL1RiseTime(val);

            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.FPGAEnergy", sdet.Data()));
            det->SetQH1FPGAEnergy(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.Amplitude", sdet.Data()));
            det->SetQH1Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.RawAmplitude", sdet.Data()));
            det->SetQH1RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.BaseLine", sdet.Data()));
            det->SetQH1BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.SigmaBaseLine", sdet.Data()));
            det->SetQH1SigmaBaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.QH1.RiseTime", sdet.Data()));
            det->SetQH1RiseTime(val);

            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I1.Amplitude", sdet.Data()));
            det->SetI1Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I1.RawAmplitude", sdet.Data()));
            det->SetI1RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I1.BaseLine", sdet.Data()));
            det->SetI1BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I1.SigmaBaseLine", sdet.Data()));
            det->SetI1SigmaBaseLine(val);

         }
         else if (!strcmp(det->GetLabel(), "SI2")) {

            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.FPGAEnergy", sdet.Data()));
            det->SetQ2FPGAEnergy(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.Amplitude", sdet.Data()));
            det->SetQ2Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.RawAmplitude", sdet.Data()));
            det->SetQ2RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.BaseLine", sdet.Data()));
            det->SetQ2BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.SigmaBaseLine", sdet.Data()));
            det->SetQ2SigmaBaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q2.RiseTime", sdet.Data()));
            det->SetQ2RiseTime(val);

            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I2.Amplitude", sdet.Data()));
            det->SetI2Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I2.RawAmplitude", sdet.Data()));
            det->SetI2RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I2.BaseLine", sdet.Data()));
            det->SetI2BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.I2.SigmaBaseLine", sdet.Data()));
            det->SetI2SigmaBaseLine(val);
         }
         else if (!strcmp(det->GetLabel(), "CSI")) {
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.FPGAEnergy", sdet.Data()));
            det->SetQ3FPGAEnergy(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.FPGAFastEnergy", sdet.Data()));
            det->SetQ3FastFPGAEnergy(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.Amplitude", sdet.Data()));
            det->SetQ3Amplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.RawAmplitude", sdet.Data()));
            det->SetQ3RawAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.FastAmplitude", sdet.Data()));
            det->SetQ3FastAmplitude(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.BaseLine", sdet.Data()));
            det->SetQ3BaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.SigmaBaseLine", sdet.Data()));
            det->SetQ3SigmaBaseLine(val);
            val = rnuc->GetParameters()->GetDoubleValue(Form("%s.Q3.RiseTime", sdet.Data()));
            det->SetQ3RiseTime(val);

         }

      }
   }
}

#ifdef WITH_PROTOBUF
void KVFAZIA::treat_event(const DAQ::FzEvent& e)
{
   for (int b = 0; b < e.block_size(); ++b) {
      for (int f = 0; f < e.block(b).fee_size(); ++f) {
         for (int h = 0; h < e.block(b).fee(f).hit_size(); ++h) {
            int detnum = 100 * e.block(b).blkid()
                         + 10 * fQuartet[e.block(b).fee(f).feeid()][e.block(b).fee(f).hit(h).telid()]
                         + fTelescope[e.block(b).fee(f).feeid()][e.block(b).fee(f).hit(h).telid()];
            TString det;
            switch (e.block(b).fee(f).hit(h).detid()) {
               case ::DAQ::FzHit_FzDetector::FzHit_FzDetector_Si1:
                  det = "SI1-";
                  break;
               case ::DAQ::FzHit_FzDetector::FzHit_FzDetector_Si2:
                  det = "SI2-";
                  break;
               case ::DAQ::FzHit_FzDetector::FzHit_FzDetector_CsI:
                  det = "CSI-";
                  break;
               default:
                  Error("treat_hit", "Unknown detector type %d", e.block(b).fee(f).hit(h).detid());
                  return;
            }
            det += Form("%d", detnum);
            if (GetDetector(det)) {
               fFiredACQParams.Add(GetDetector(det));
               ((KVFAZIADetector*)GetDetector(det))->SetFired();
            }
         }
      }
   }
}
#endif

#ifdef WITH_MFM
Bool_t KVFAZIA::handle_raw_data_event_mfmframe(const MFMCommonFrame& f)
{
   // Treatment of raw data in MFM frames with type MFM_FAZIA_FRAME_TYPE

   fFiredACQParams.Clear();

   if (f.GetFrameType() != MFM_FAZIA_FRAME_TYPE) return kFALSE;

#ifdef WITH_PROTOBUF
   DAQ::FzEventSet fazia_set;
   DAQ::FzEvent fazia_event;
   // Parse protobuf data in MFM frame
   if (fazia_set.ParseFromArray(f.GetPointUserData(), ((MFMFaziaFrame&)f).GetEventSize())) {
      // Parsed an event set
      for (int i = 0; i < fazia_set.ev_size(); ++i) treat_event(fazia_set.ev(i));
   }
   else if (fazia_event.ParseFromArray(f.GetPointUserData(), ((MFMFaziaFrame&)f).GetEventSize())) {
      // Parsed an event
      treat_event(fazia_event);
   }
#endif
   return kTRUE;
}
#endif

void KVFAZIA::CreateCorrespondence()
{
   // set up correspondence between FPGA number/FEE number (from acquisition)
   // and Quartet/Telescope numbers

   TString DataFilePath;
   if (!KVBase::SearchKVFile("ElecDetLink.env", DataFilePath, "data")) {
      Error("CreateCorrespondence", "ElecDetLink.env not found");
      return;
   }
   KVEnv DetLink;
   DetLink.ReadFile(DataFilePath, kEnvUser);
   for (int t = 1; t <= 4; t++) {
      for (int q = 1; q <= 4; q++) {
         TString elec = DetLink.GetValue(Form("T%1d-Q%1d", t, q), " ");
         if (!elec.IsWhitespace()) {
            int fee, fpga;
            sscanf(elec.Data(), "FPGA%d-FE%d", &fpga, &fee);
            fQuartet[fee][fpga] = q;
            fTelescope[fee][fpga] = t;
         }
         else {
            Error("CreateCorrespondence", "Problem reading FAZIA ElecDetLink.env file : T%1d-Q%1d = %s", t, q, elec.Data());
         }
      }
   }
}
