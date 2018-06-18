#include <KVDataRepositoryManager.h>
#include <KVDataSetManager.h>
#include <KVEventReconstructor.h>
#include <KVMFMDataFileReader.h>
#include <iostream>
#include "TApplication.h"
#include "TROOT.h"
using namespace std;

int main(int argc, char* argv[])
{
   KVBase::InitEnvironment();

   //TApplication myapp("myapp", &argc, argv);

   KVDataRepositoryManager drm;
   drm.Init();
   gDataSetManager->GetDataSet("INDRAFAZIA")->cd();
   //data/eindraX/fazia/acquisition/run/run_0048.dat.19-04-18_15h17m46s

   KVMultiDetArray::MakeMultiDetector(gDataSet->GetName());

   unique_ptr<KVMFMDataFileReader> raw_file(KVMFMDataFileReader::Open("/data/eindraX/fazia/acquisition/run/run_0048.dat.19-04-18_15h17m46s"));

   KVEventReconstructor erec(gMultiDetArray, new KVReconstructedEvent);
   KVGroupReconstructor::SetDoIdentification(false);
   KVGroupReconstructor::SetDoCalibration(false);

   int nframes = 100;
   if (argc > 1) nframes = TString(argv[1]).Atoi();
   for (int i = 0; i < nframes; ++i) {
      std::cout << std::endl << "==============================================EVENT " << i << std::endl << std::endl;
      if (!raw_file->GetNextEvent()) break;
      //if (i < 25) continue;
      gMultiDetArray->HandleRawDataEvent(raw_file.get());
      erec.ReconstructEvent(gMultiDetArray->GetFiredDataParameters());
      erec.GetEvent()->SetNumber(i + 1);
      erec.GetEvent()->ls();
   }
}
