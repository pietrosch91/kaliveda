#define KVFAZIAReader_cxx
// The class definition in KVFAZIAReader.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("KVFAZIAReader.C")
// Root > T->Process("KVFAZIAReader.C","some options")
// Root > T->Process("KVFAZIAReader.C+")
//

#include "KVFAZIAReader.h"
#include "KVDataAnalyser.h"
#include "KVFAZIARawDataAnalyser.h"
#include "KVFAZIA.h"
#include "KVClassFactory.h"

#include "TSystem.h"

ClassImp(KVFAZIAReader)

void KVFAZIAReader::SlaveBegin(TTree* /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   Info("KVFAZIAReader::SlaveBegin", "called");
   gDataAnalyser->preInitAnalysis();
}

Int_t KVFAZIAReader::GetEntry(Long64_t entry, Int_t getall)
{

   return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0;

}

Bool_t KVFAZIAReader::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either KVFAZIAReader::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

   GetDetectorEvent()->Clear();
   fEventsRead += 1;

   RawEvent->Clear();

   GetEntry(entry);
   gFazia->GetDetectorEvent(GetDetectorEvent(), RawEvent->GetSignals());

   if (GetNumberOfReadEntries() % 10000 == 0) {
      Info("KVFAZIAReader::Process", "%d read entries", GetNumberOfReadEntries());
      ProcInfo_t pid;
      if (gSystem->GetProcInfo(&pid) == 0) {
         cout << "     ------------- Process infos -------------" << endl;
         printf(" CpuSys = %f  s.    CpuUser = %f s.    ResMem = %f MB   VirtMem = %f MB\n",
                pid.fCpuSys, pid.fCpuUser, pid.fMemResident / 1024., pid.fMemVirtual / 1024.);
      }
   }
   return Analysis();
}


void KVFAZIAReader::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
   Info("KVFAZIAReader::Terminate", "%d events read", GetNumberOfReadEntries());
   EndRun();
   EndAnalysis();

}

void KVFAZIAReader::Init(TTree* tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   Info("KVFAZIAReader::Init", "called");
   RawEvent = 0;

   if (!tree)
      return;
   fChain = tree;
   fChain->SetMakeClass(1);
   fChain->SetBranchAddress("rawevent", &RawEvent);
   fEventsRead = 0;
   fNotifyCalled = kFALSE;

}

Bool_t KVFAZIAReader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.
   if (fNotifyCalled) return kTRUE; // avoid multiple calls at beginning of analysis
   fNotifyCalled = kTRUE;

   Info("Notify", "Beginning analysis of file %s (%lld events)", fChain->GetCurrentFile()->GetName(), fChain->GetTree()->GetEntries());
   gDataAnalyser->preInitRun();
   fCurrentRun = ((KVFAZIARawDataAnalyser*)gDataAnalyser)->GetCurrentRunNumber();
   InitRun();  //user initialisations for run

   return kTRUE;
}

void KVFAZIAReader::Make(const Char_t* kvsname)
{
   // Automatic generation of KVFAZIAReader-derived class for KaliVeda analysis

   KVClassFactory cf(kvsname, "User analysis class", Class_Name(), kTRUE);
   cf.AddImplIncludeFile("KVSignal.h");
   cf.AddImplIncludeFile("KVPSAResult.h");
   cf.AddImplIncludeFile("KVNameValueList.h");
   cf.AddImplIncludeFile("TClonesArray.h");
   cf.AddImplIncludeFile("KVBatchSystem.h");
   cf.AddImplIncludeFile("KVFAZIA.h");
   cf.GenerateCode();
}
