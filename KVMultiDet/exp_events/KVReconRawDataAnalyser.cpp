#include "KVReconRawDataAnalyser.h"
#include "KVDataSet.h"

#include <KVClassFactory.h>

ClassImp(KVReconRawDataAnalyser)

void KVReconRawDataAnalyser::preInitAnalysis()
{
   TClass* recev_cl = TClass::GetClass(GetDataSet()->GetReconstructedEventClassName());
   fRecev = ((KVReconstructedEvent*)recev_cl->New());

   Info("preInitAnalysis", "Reconstructed event container class: %s", recev_cl->GetName());
}

void KVReconRawDataAnalyser::preInitRun()
{
   fEvRecon.reset(new KVEventReconstructor(gMultiDetArray, fRecev));
}

void KVReconRawDataAnalyser::preAnalysis()
{
   if (gMultiDetArray->HandledRawData()) {
      fEvRecon->ReconstructEvent(gMultiDetArray->GetFiredDataParameters());
      fEvRecon->GetEvent()->SetNumber(GetEventNumber());
   }
}

void KVReconRawDataAnalyser::Make(const Char_t* kvsname)
{
   //Automatic generation of derived class for raw data analysis

   KVClassFactory cf(kvsname, "User reconstructed raw data analysis class", "KVReconRawDataAnalyser");
   cf.AddMethod("InitAnalysis", "void");
   cf.AddMethod("InitRun", "void");
   cf.AddMethod("Analysis", "Bool_t");
   cf.AddMethod("EndRun", "void");
   cf.AddMethod("EndAnalysis", "void");
   KVString body;
   //initanalysis
   body = "   //Initialisation of e.g. histograms, performed once at beginning of analysis";
   cf.AddMethodBody("InitAnalysis", body);
   //initrun
   body = "   //Initialisation performed at beginning of each run\n";
   body += "   //  GetRunNumber() returns current run number";
   cf.AddMethodBody("InitRun", body);
   //Analysis
   body = "   //Analysis method called for each event\n";
   body += "   //  GetEventNumber() returns current event number\n";
   body += "   //  if gMultiDetArray->HandledRawData() returns kTRUE, an event was reconstructed and\n";
   body += "   //  can be accessed through GetReconstructedEvent()\n";
   body += "   //  Processing will stop if this method returns kFALSE\n";
   body += "   return kTRUE;";
   cf.AddMethodBody("Analysis", body);
   //endrun
   body = "   //Method called at end of each run";
   cf.AddMethodBody("EndRun", body);
   //endanalysis
   body = "   //Method called at end of analysis: save/display histograms etc.";
   cf.AddMethodBody("EndAnalysis", body);
   cf.GenerateCode();
}

