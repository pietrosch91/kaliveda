#include <KVDataRepositoryManager.h>
#include <KVDataSetManager.h>
#include <KVEventReconstructor.h>
#include <KVMFMDataFileReader.h>
#include <iostream>
#include "TApplication.h"
#include "TROOT.h"
#include "KVINDRA.h"
#include "KVFAZIA.h"
#include "TH2.h"
using namespace std;

int main(int argc, char* argv[])
{
   KVBase::InitEnvironment();

   //TApplication myapp("myapp", &argc, argv);

   KVDataRepositoryManager drm;
   drm.Init();
   gDataSetManager->GetDataSet("INDRAFAZIA")->cd();

   KVMultiDetArray::MakeMultiDetector(gDataSet->GetName());
   gMultiDetArray->SetIdentifications();
   gMultiDetArray->SetGridsInTelescopes(12);
   gMultiDetArray->InitializeIDTelescopes();
   gMultiDetArray->PrintStatusOfIDTelescopes();

   unique_ptr<KVMFMDataFileReader> raw_file(KVMFMDataFileReader::Open(argv[1]));

   KVEventReconstructor erec(gMultiDetArray, new KVReconstructedEvent);
   //KVGroupReconstructor::SetDoIdentification(false);
   KVGroupReconstructor::SetDoCalibration(false);

   int first_frame = TString(argv[2]).Atoi();
   TFile f(Form("INDRAFAZIArecon_%d.root", first_frame), "recreate");

   TTree* tree = new TTree("ReconEvents", "Reconstructed INDRA-FAZIA events");
   KVEvent::MakeEventBranch(tree, "ReconEvent", "KVReconstructedEvent", erec.GetEventReference());

   int i = 0;
   while (raw_file->GetNextEvent()) {
      ++i;
      if (i != first_frame) continue;
      first_frame += 20;
      //cout << "=================================================================" << endl;
      if (gMultiDetArray->HandleRawDataEvent(raw_file.get())) {
         erec.ReconstructEvent(gMultiDetArray->GetFiredDataParameters());
         erec.GetEvent()->SetNumber(i + 1);
         if (gFazia->HandledRawData()) {
            for (KVEvent::Iterator it = erec.GetEvent()->begin(); it != erec.GetEvent()->end(); ++it) {
               KVReconstructedNucleus& n = it.reference<KVReconstructedNucleus>();
               if (n.GetParameters()->GetTStringValue("ARRAY") == "FAZIA") n.Print();
            }
         }
         tree->Fill();
      }
      else {
         cout << "Frame " << i << " not handled by anybody:" << endl;
         raw_file->PrintFrameRead();
      }
      if (!(i % 10000)) cout << "Treated " << i << " frames..." << endl;
   }

   f.Write();
}
