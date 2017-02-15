//Created by KVClassFactory on Tue Feb 14 11:13:53 2017
//Author: John Frankland,,,

#include "KVSimDirAnalyser.h"
#include "KVDataAnalysisTask.h"
#include <KVSimFile.h>
#include <KVSimDir.h>
#include "TSystem.h"

ClassImp(KVSimDirAnalyser)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimDirAnalyser</h2>
<h4>Analysis of trees containing simulated events</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimDirAnalyser::KVSimDirAnalyser()
   : KVDataAnalyser(), fListOfSimFiles(nullptr), fAnalysisChain(nullptr)
{
   // Default constructor
}

//____________________________________________________________________________//

KVSimDirAnalyser::~KVSimDirAnalyser()
{
   // Destructor
}

void KVSimDirAnalyser::SubmitTask()
{
   // Set up and run the analysis

   BuildChain();

   Bool_t read_all_events = (GetNbEventToRead() == 0);
   Long64_t nevents = (!read_all_events ? GetNbEventToRead() : fAnalysisChain->GetEntries());
   Long64_t update_interval = (nevents > 10 ? nevents / 10 : 1);
   TString results_file_name;
   KVSimFile* first_file = (KVSimFile*)fListOfSimFiles->First();
   results_file_name.Form("%s_%s", GetUserClass(), first_file->GetName());
   TString options;
   options.Form("EventsReadInterval=%lld,BranchName=%s,CombinedOutputFile=%s,SimulationInfos=%s",
                update_interval, first_file->GetBranchName(), results_file_name.Data(), first_file->GetTitle());
   // Add any user-defined options
   if (GetUserClassOptions() != "") {
      options += ",";
      options += GetUserClassOptions();
   }

   // Check compilation of user class & run
   if (CheckIfUserClassIsValid()) {
      Info("SubmitTask", "Beginning TChain::Process...");
#ifdef WITH_CPP11
      if (GetProofMode() != KVDataAnalyser::EProofMode::None) fAnalysisChain->SetProof(kTRUE);
#else
      if (GetProofMode() != KVDataAnalyser::None) fAnalysisChain->SetProof(kTRUE);
#endif
      if (read_all_events) {
         fAnalysisChain->Process(Form("%s%s", GetUserClassImp().Data(), GetACliCMode()), options.Data());
      } else {
         fAnalysisChain->Process(Form("%s%s", GetUserClassImp().Data(), GetACliCMode()), options.Data(), GetNbEventToRead());
      }
   }
   delete fAnalysisChain;
   fAnalysisChain = nullptr;
}

void KVSimDirAnalyser::BuildChain()
{
   // Build a TChain with all files/trees to be analysed

   TIter next(fListOfSimFiles);
   KVSimFile* file;
   while ((file = (KVSimFile*)next())) {
      if (!fAnalysisChain) {
         fAnalysisChain = new TChain(file->GetTreeName());
      }
      TString fullpath;
      AssignAndDelete(fullpath, gSystem->ConcatFileName(file->GetSimDir()->GetDirectory(), file->GetName()));
      fAnalysisChain->Add(fullpath);
   }
}

