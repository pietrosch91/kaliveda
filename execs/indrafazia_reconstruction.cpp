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
   gMultiDetArray->SetGridsInTelescopes(1000);
   gMultiDetArray->InitializeIDTelescopes();
   gMultiDetArray->PrintStatusOfIDTelescopes();

   unique_ptr<KVMFMDataFileReader> raw_file(KVMFMDataFileReader::Open(argv[1]));

   KVEventReconstructor erec(gMultiDetArray, new KVReconstructedEvent);
   //KVGroupReconstructor::SetDoIdentification(false);
   KVGroupReconstructor::SetDoCalibration(false);

   int first_frame = TString(argv[2]).Atoi();
   TFile f(Form("INDRAFAZIArecon_%d.root", first_frame), "recreate");

   int i = 0;
   while (raw_file->GetNextEvent()) {
      ++i;
      if (i != first_frame) continue;
      first_frame += 20;
      gMultiDetArray->HandleRawDataEvent(raw_file.get());
      erec.ReconstructEvent(gMultiDetArray->GetFiredDataParameters());
      erec.GetEvent()->SetNumber(i + 1);
      erec.GetEvent()->ls();
      if (!(i % 10000)) cout << "Treated " << i << " frames..." << endl;
   }

   f.Write();
}
