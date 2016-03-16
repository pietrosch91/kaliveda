//Created by KVClassFactory on Wed Jan 28 09:54:34 2015
//Author: Eric Bonnet

#include "KVFAZIASelector.h"
#include "KVReconstructedNucleus.h"
#include "KVBatchSystem.h"
#include "KVFAZIA.h"
#include "KVDataSet.h"
#include "KVDataAnalyser.h"
#include "KVFAZIAReconDataAnalyser.h"
#include "KVFAZIADetector.h"
#include "KVSignal.h"
#include "KVClassFactory.h"

#include "KVDataRepositoryManager.h"
#include "KVDataSetManager.h"
#include "KVFAZIADBRun.h"
#include "KVFAZIADB.h"

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIASelector</h2>
<h4>selector to analyze FAZIA reconstructed data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

// This class is derived from the KaliVeda class KVSelector.
// The following members functions are called by the TTree::Process() functions:
//    InitRun():       called everytime a run starts
//    EndRun():        called everytime a run ends
//    InitAnalysis():  called at the beginning of the analysis
//                     a convenient place to create your histograms.
//    Analysis():      called for each event. In this function you
//                     fill your histograms.
//    EndAnalysis():   called at the end of a loop on the tree,
//                     a convenient place to draw/fit your histograms.
//
// Modify these methods as you wish in order to create your analysis class.
// Don't forget that for every class used in the analysis, you must put a
// line '#include' at the beginning of this file.
// E.g. if you want to access informations on the FAZIA multidetector
// through the global pointer gIndra, you must AT LEAST add the line
//   #include "KVFAZIA.h"

void KVFAZIASelector::SlaveBegin(TTree* tree)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   SetReadingOfRawData(kFALSE);
   gDataAnalyser->preInitAnalysis();

   //
   // ParseOptions : Manage options passed as arguments
   // done in KVEventSelector::SlaveBegin
   KVEventSelector::SlaveBegin(tree);

}

void KVFAZIASelector::Init(TTree* tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer

   Event = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   if (strcmp(GetBranchName(), "")  && fChain->GetBranch(GetBranchName())) {
      Info("Init", "Analysing data in branch : %s", GetBranchName());
      fChain->SetBranchAddress(GetBranchName() , &Event, &b_Event);
   }
   if (NeedToReadRawData())
      LinkRawData();

   //user additional branches addressing
   SetAdditionalBranchAddress();
   fEventsRead = 0;
   fOutputFile = 0;

}

Bool_t KVFAZIASelector::Notify()
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
   fCurrentRun = ((KVFAZIAReconDataAnalyser*)gDataAnalyser)->GetCurrentRunNumber();

   if (fAutomatic)
      CreateOutputFile();
   InitRun();  //user initialisations for run

   return kTRUE;
}

//_____________________________________
void KVFAZIASelector::LinkRawData()
{
   Info("LinkRawData", "Enable reading of raw data : %s\n", rawdatabranchname.Data());
   RawEvent = 0;
   fChain->SetBranchAddress(rawdatabranchname.Data(), &RawEvent);
}

//_____________________________________
void KVFAZIASelector::ConnectSignalsToDetectors()
{
   KVFAZIADetector* det = 0;
   KVSignal* sig = 0;

   TIter next_s(RawEvent->GetSignals());
   while ((sig = (KVSignal*)next_s())) {
      sig->DeduceFromName();
      det = (KVFAZIADetector*)gFazia->GetDetector(sig->GetDetectorName());
      det->SetSignal(sig, sig->GetType());
   }

}

//_____________________________________
void KVFAZIASelector::ParseOptions()
{
   KVEventSelector::ParseOptions();

   //check if the reading of raw data is needed
   if (IsOptGiven("ReadRawData")) SetReadingOfRawData(GetOpt("ReadRawData").Atoi());

   //Force the name of the KVEvent branch after the call to ParseOption
   SetBranchName("FAZIAReconEvent");
}

//_____________________________________
void KVFAZIASelector::Make(const Char_t* kvsname)
{
   // Automatic generation of KVSelector-derived class for KaliVeda analysis
   KVClassFactory cf(kvsname, "User analysis class", Class_Name(), kTRUE);
   cf.AddImplIncludeFile("KVReconstructedNucleus.h");
   cf.AddImplIncludeFile("KVBatchSystem.h");
   cf.AddImplIncludeFile("KVFAZIA.h");
   cf.GenerateCode();
}

//_____________________________________
void KVFAZIASelector::SetAutomaticManagementOfOutputs(Bool_t automatic)
{
   //indicate if outputs (histograms, trees or other objects
   //has to be managed automatically by the selector
   //
   //to be active FAZIA_SELECTOR_OUTPUTS shell variable
   //has to be defined in your .cshrc type configuration file
   //the format of the output file will be :
   // FAZIA_SELECTOR_OUTPUTS/nameofthekvfaziaselector/dataset/R%d.root
   fOutputPath = "";
   if (automatic) {
      TString output_path = gSystem->Getenv("FAZIASELECTOR_OUTPUTS");
      fOutputPath.Form("%s/%s/%s", output_path.Data(), GetName(), gDataSet->GetName());
      if (output_path == "") {
         Error("CreateOutputFile", "FAZIASELECTOR_OUTPUTS shell variable has to defined first");
         fAutomatic = kFALSE;
         fOutputPath = "";
         return;
      } else if (gSystem->Exec(Form("test -d %s", fOutputPath.Data())) != 0) {
         Info("SetAutomaticManagementOfOutputs", "Creation of %s directory", fOutputPath.Data());
         if (gSystem->mkdir(fOutputPath.Data(), kTRUE) != 0) {
            Info("SetAutomaticManagementOfOutputs", "Error in the %s directory creation", fOutputPath.Data());
            fAutomatic = kFALSE;
            fOutputPath = "";
            return;
         }
      }
   }
   fAutomatic = automatic;
}
//_____________________________________
Bool_t KVFAZIASelector::CreateOutputFile()
{
   //create ROOT file where all outputs coming from
   //the selector will be stores
   //to be active FAZIA_SELECTOR_OUTPUTS shell variable
   //has to be defined in your .cshrc type configuration file
   //the format of the output file will be :
   // FAZIA_SELECTOR_OUTPUTS/nameofthekvfaziaselector/dataset/R%d.root
   //When it is created, the output file name is tmp_R%d.root
   //and renamed at the end of the run R%d.root
   //This helps to know if the entire run has been read
   //
   //
   fOutputFile = new TFile(Form("%s/tmp_R%d.root", fOutputPath.Data(), GetCurrentRunNumber()), "recreate");
   if (fOutputFile && fOutputFile->IsOpen())
      return kTRUE;
   return kFALSE;
}
//_____________________________________
Bool_t KVFAZIASelector::SaveAndCloseOutputFile()
{
   //write in and close the ROOT file created by the CreateOutputFile
   //method
   //
   if (!fOutputFile) {
      Error("SaveAndCloseOutputFile", "No file open for writing outputs");
      return kFALSE;
   }

   fOutputFile->cd();
   fOutputFile->Write();
   fOutputFile->Close();

   TString fmt1, fmt2;
   fmt1.Form("%s/tmp_R%d.root", fOutputPath.Data(), GetCurrentRunNumber());
   fmt2.Form("%s/R%d.root", fOutputPath.Data(), GetCurrentRunNumber());

   gROOT->ProcessLine(Form(".! mv %s %s", fmt1.Data(), fmt2.Data()));
   return kTRUE;
}

//_____________________________________
void KVFAZIASelector::CheckEndOfRun()
{
   // Testing whether EndRun() should be called
   if (AtEndOfRun()) {
      Info("Process", "End of file reached after %lld events", fEventsRead);
      EndRun();
      if (fAutomatic)
         SaveAndCloseOutputFile();
      fNotifyCalled = kFALSE;//Notify will be called when next file is opened (in TChain)
   }

}

//_____________________________________
void KVFAZIASelector::CheckListOfWellEndedRuns(const Char_t* kvsname, const Char_t* dsname)
{
   //static method
   //List well ended files in the corresponding directory :
   //FAZIA_SELECTOR_OUTPUTS/[kvsname]/[dsname]/
   //Only available if the selector you ran
   //had the option SetAutomaticManagementOfOutputs(kTRUE)
   //
   TString output_path = gSystem->Getenv("FAZIASELECTOR_OUTPUTS");
   TString op;
   op.Form("%s/%s/%s", output_path.Data(), kvsname, dsname);
   if (output_path == "") {
      printf("Error in CheckListOfEndedRuns : FAZIASELECTOR_OUTPUTS shell variable is not defined\n");
      return;
   }
   if (gSystem->Exec(Form("test -d %s", op.Data())) != 0) {
      printf("directory %s does not exist\n", op.Data());
      return;
   }
   KVNumberList lr;
   KVString lruns = gSystem->GetFromPipe(Form("ls %s/R*.root", op.Data()));
   lruns.Begin("\n");
   Int_t run = -1;
   while (!lruns.End()) {
      KVString srun = lruns.Next();
      srun = gSystem->BaseName(srun.Data());
      Int_t rep = sscanf(srun.Data(), "R%d.root", &run);
      if (rep == 1)
         lr.Add(run);
   }
   if (lr.GetNValues() > 0)
      printf("Ended runs for the selector %s and the dataset %s are:\n%s\n", kvsname, dsname, lr.AsString());
   else
      printf("No runs ended for the selector %s and the dataset %s\n", kvsname, dsname);
}
//_____________________________________
void KVFAZIASelector::MergeListOfEndedRuns(const Char_t* kvsname, const Char_t* dsname)
{
   //static method
   //Gather well ended files in the corresponding directory :
   //FAZIA_SELECTOR_OUTPUTS/[kvsname]/[dsname]/
   //Files are merged using the hadd command
   //the -f option is set, so previous output file is deleted
   //
   //To avoid mixing, one merging is done for each system
   //
   //The output file is deduced from system name
   //Example for "48Ca + 48Ca 35.0 MeV/A" gives 48Ca48Ca35.0
   //
   //An additional TEnv file is created with the format
   // listruns_48Ca48Ca35.0 to follow the previous example
   //It is a summary of info coming from the merging :
   //       System:                                  48Ca + 48Ca 35.0 MeV/A
   //       DataSet:                                 FAZIASYM
   //       Selector:                                CheckCsIIdentification
   //       Number:                                  34
   //       RunList:                                 14588-14621
   //
   //Only available if the selector you ran
   //had the option SetAutomaticManagementOfOutputs(kTRUE)
   //
   TString output_path = gSystem->Getenv("FAZIASELECTOR_OUTPUTS");
   TString op;
   op.Form("%s/%s/%s", output_path.Data(), kvsname, dsname);
   if (output_path == "") {
      printf("Error in CheckListOfEndedRuns : FAZIASELECTOR_OUTPUTS shell variable is not defined\n");
      return;
   }
   if (gSystem->Exec(Form("test -d %s", op.Data())) != 0) {
      printf("directory %s does not exist\n", op.Data());
      return;
   }

   if (!(gDataSet)) {
      new KVDataRepositoryManager();
      gDataRepositoryManager->Init();
      gDataSetManager->GetDataSet(dsname)->cd();
   }

   KVNumberList lall;
   TList* lsyst = new TList();
   lsyst->SetOwner(kTRUE);
   KVNumberList* lpersyst = 0;
   KVString lruns = gSystem->GetFromPipe(Form("ls %s/R*.root", op.Data()));
   lruns.Begin("\n");
   Int_t run = -1;
   while (!lruns.End()) {
      KVString srun = lruns.Next();
      srun = gSystem->BaseName(srun.Data());
      Int_t rep = sscanf(srun.Data(), "R%d.root", &run);
      if (rep == 1) {
         KVFAZIADBRun* fr =  gFaziaDB->GetRun(run);
         lall.Add(run);
         if (!(lpersyst = (KVNumberList*)lsyst->FindObject(fr->GetSystemName()))) {
            lpersyst = new KVNumberList();
            lpersyst->SetName(fr->GetSystemName());
            printf("creation de la liste associee a : %s\n", fr->GetSystemName());
            lsyst->Add(lpersyst);
         }
         lpersyst->Add(run);
      }
   }


   TIter next(lsyst);
   while ((lpersyst = (KVNumberList*)next())) {
      TEnv fruns;

      printf("%s: %s\n", lpersyst->GetName(), lpersyst->AsString());

      TString slist(lpersyst->GetExpandedList());
      slist.ReplaceAll(" ", ",");
      TString sname(lpersyst->GetName());
      sname.ReplaceAll("+ ", "");
      sname.ReplaceAll(" MeV/A", "");
      sname.ReplaceAll(" ", "");
      TString inst = "";
      if (lpersyst->GetNValues() > 1) {
         inst.Form(".! hadd -f %s/%s.root %s/R{%s}.root",
                   op.Data(),
                   sname.Data(),
                   op.Data(),
                   slist.Data()
                  );
      } else if (lpersyst->GetNValues() == 1) {
         inst.Form(".! hadd -f %s/%s.root %s/R%d.root",
                   op.Data(),
                   sname.Data(),
                   op.Data(),
                   lpersyst->First()
                  );
      }


      if (inst != "") {
         printf("%s\n", inst.Data());
         KVString sroot;
         sroot.Form("%s/%s.root",
                    op.Data(),
                    sname.Data()
                   );
         //if (!gSystem->IsFileInIncludePath(sroot.Data()) || (gSystem->IsFileInIncludePath(sroot.Data()) && erase_prev))
         //{
         fruns.SetValue("System", lpersyst->GetName());
         fruns.SetValue("DataSet", dsname);
         fruns.SetValue("Selector", kvsname);
         fruns.SetValue("Number", lpersyst->GetNValues());
         fruns.SetValue("RunList", lpersyst->AsString());
         gROOT->ProcessLine(inst.Data());
         //}
         fruns.WriteFile(
            Form("%s/listruns_%s.env",
                 op.Data(),
                 sname.Data()
                )
         );
      }

   }


   delete lsyst;

}
//_____________________________________
void KVFAZIASelector::GetMissingRunsListing(const Char_t* kvsname, const Char_t* dsname)
{
   //static method
   //
   TString output_path = gSystem->Getenv("FAZIASELECTOR_OUTPUTS");
   TString op;
   op.Form("%s/%s/%s", output_path.Data(), kvsname, dsname);
   if (output_path == "") {
      printf("Error in CheckListOfEndedRuns : FAZIASELECTOR_OUTPUTS shell variable is not defined\n");
      return;
   }
   if (gSystem->Exec(Form("test -d %s", op.Data())) != 0) {
      printf("directory %s does not exist\n", op.Data());
      return;
   }

   if (!(gDataSet)) {
      new KVDataRepositoryManager();
      gDataRepositoryManager->Init();
      gDataSetManager->GetDataSet(dsname)->cd();
   }

   KVString lenv = gSystem->GetFromPipe(Form("ls %s/*env", op.Data()));
   lenv.Begin("\n");
   while (!lenv.End()) {
      TEnv env;
      KVString senv = lenv.Next();
      env.ReadFile(senv.Data(), kEnvAll);
      TString syst(env.GetValue("System", ""));
      TString sruns(env.GetValue("RunList", ""));
      KVDBSystem* dbsys = gFaziaDB->GetSystem(syst.Data());
      if (dbsys) {
         KVNumberList ltot;
         dbsys->GetRunList(ltot);
         ltot.Remove(sruns.Data());
         if (ltot.GetNValues() > 0)
            printf("System: %s, Missing Runs %s\n", dbsys->GetName(), ltot.AsString());
         else
            printf("System: %s, All runs done\n", dbsys->GetName());
      }
   }

}
