/*
$Id: KVINDRAReconDataAnalyser.cpp,v 1.4 2007/11/15 14:59:45 franklan Exp $
$Revision: 1.4 $
$Date: 2007/11/15 14:59:45 $
*/

//Created by KVClassFactory on Wed Apr  5 23:50:04 2006
//Author: John Frankland

#include "KVBase.h"
#include "KVINDRAReconDataAnalyser.h"
#include "KVINDRADBRun.h"
#include "KVDataAnalysisTask.h"
#include "KVDataSet.h"
#include "TChain.h"
#include "TObjString.h"
#include "TChain.h"
#include "KVSelector.h"

ClassImp(KVINDRAReconDataAnalyser)
////////////////////////////////////////////////////////////////////////////////
// For analysing reconstructed INDRA data
//
////////////////////////////////////////////////////////////////////////////////
KVINDRAReconDataAnalyser::KVINDRAReconDataAnalyser()
{
   //Default constructor
   fDataSelector="none";
}

void KVINDRAReconDataAnalyser::Reset()
{
   //Reset task variables
   KVDataAnalyser::Reset();
   fDataSelector="none";
}

KVINDRAReconDataAnalyser::~KVINDRAReconDataAnalyser()
{
   //Destructor
}

//_________________________________________________________________

Bool_t KVINDRAReconDataAnalyser::CheckTaskVariables()
{
   // Checks the task variables 
   
   if(!KVINDRADataAnalyser::CheckTaskVariables()) return kFALSE;
   
   if (fDataSelector == "none") {
      ChooseKVDataSelector();
   }

   cout << "============> Analysis summary <=============" << endl;
   cout << "Analysis of runs " << fRunList.
       GetList() << " with the KVSelector ";
   cout << "\"" << GetUserClass() << "\"." << endl;
   if (nbEventToRead) {
      cout << nbEventToRead << " events will be processed." << endl;
   } else {
      cout << "All events will be processed." << endl;
   }
   cout << "=============================================" << endl;
   
   return kTRUE;
}

//_________________________________________________________________

void KVINDRAReconDataAnalyser::SubmitTask()
{
   //Run the interactive analysis
   
   //make the chosen dataset the active dataset ( = gDataSet; note this also opens database
   //and positions gDataBase & gIndraDB).
   fDataSet->cd();
   
   TChain *t = new TChain("ReconstructedEvents");
   t->SetDirectory(0); // we handle delete
   
   fRunList.Begin(); Int_t run;
   while( !fRunList.End() ){
      
      run = fRunList.Next();
      cout << "Adding file " << gDataSet->GetFullPathToRunfile(fDataType.
                                                               Data(),
                                                               run);
      cout << " to the TChain." << endl;
      if( !t->Add(gDataSet->GetFullPathToRunfile(fDataType.Data(), run), -1) ){
         //tree is not called "ReconstructedEvents". this must be an old file. tree is called "tree".
         t->SetName("tree");
         t->Add(gDataSet->GetFullPathToRunfile(fDataType.Data(), run), -1);
      }
   }
   
   TString option("");
   if (fDataSelector.Length()) {
      option.Form("DataSelector=%s", fDataSelector.Data());
      cout << "Data Selector : " << fDataSelector.Data() << endl;
   }
   
   KVSelector *selector = (KVSelector*)GetInstanceOfUserClass();
   
   if(!selector || !selector->InheritsFrom("KVSelector"))
    {
    cout << "The selector \"" << GetUserClass() << "\" is not valid." << endl;
    cout << "Process aborted." << endl;
    }
   else
    {
      //check name of branch with reconstructed INDRA events
      if( !t->GetBranch("INDRAReconEvent") ){
         selector->SetINDRAReconEventBranchName("data");
      }
      if (nbEventToRead) {
         t->Process(GetUserClass(), option.Data(),nbEventToRead);
      } else {
         t->Process(GetUserClass(), option.Data());
      }
    }
   if(selector) delete selector;
   delete t;
}

//_________________________________________________________________

void KVINDRAReconDataAnalyser::ChooseKVDataSelector()
{
   // Choose the KVDataSelector. Look for source files in current working directory.
   SetKVDataSelector();
   cout << "Give the name of the Data Selector: [<RET>=none]" << endl;
   fDataSelector.ReadToDelim(cin);
   if(fDataSelector!="") KVBase::FindClassSourceFiles(fDataSelector.Data(), fDataSelectorImp, fDataSelectorDec);
}

//__________________________________________________________________________________//

void KVINDRAReconDataAnalyser::WriteBatchEnvFile(const Char_t* jobname, Bool_t save)
{
   //Save (in the TEnv fBatchEnv) all necessary information on analysis task which can be used to execute it later
   //(i.e. when batch processing system executes the job).
   //If save=kTRUE (default), write the information in a file whose name is given by ".jobname"
   //where 'jobname' is the name of the job as given to the batch system.
   
   KVINDRADataAnalyser::WriteBatchEnvFile(jobname, kFALSE);
   if(fDataSelector!="none"&&fDataSelector!=""){
      fBatchEnv->SetValue("KVDataSelector", fDataSelector.Data());
      if( fDataSelectorImp!="" ) fBatchEnv->SetValue("KVDataSelectorImp", fDataSelectorImp.Data());
      if( fDataSelectorDec!="" ) fBatchEnv->SetValue("KVDataSelectorDec", fDataSelectorDec.Data());
   }
   if(save) fBatchEnv->SaveLevel(kEnvUser);
}

//_________________________________________________________________

Bool_t KVINDRAReconDataAnalyser::ReadBatchEnvFile(const Char_t* filename)
{
   //Read the batch env file "filename" and initialise the analysis task using the
   //informations in the file
   //Returns kTRUE if all goes well
   
   Bool_t ok = kFALSE;
   
   if(!KVDataAnalyser::ReadBatchEnvFile(filename)) return ok;
   
   fDataSelector = fBatchEnv->GetValue("KVDataSelector", "");
   if( fDataSelector != "" && fDataSelector!="none" ){
      //if names of source files for selector are known, and if current working directory
      //is not the same as the launch directory, we have to copy the user's files here
      fDataSelectorImp = fBatchEnv->GetValue("KVDataSelectorImp", "");
      fDataSelectorDec = fBatchEnv->GetValue("KVDataSelectorDec", "");
      TString launchDir = fBatchEnv->GetValue("LaunchDirectory", gSystem->WorkingDirectory());
      if((fDataSelectorImp!="") && (launchDir != gSystem->WorkingDirectory())){
         TString path_src, path_trg;
         //copy user's implementation file
         AssignAndDelete(path_src, gSystem->ConcatFileName(launchDir.Data(), fDataSelectorImp.Data()));
         AssignAndDelete(path_trg, gSystem->ConcatFileName(gSystem->WorkingDirectory(), fDataSelectorImp.Data()));
         gSystem->CopyFile(path_src.Data(), path_trg.Data());
         //copy user's header file
         AssignAndDelete(path_src, gSystem->ConcatFileName(launchDir.Data(), fDataSelectorDec.Data()));
         AssignAndDelete(path_trg, gSystem->ConcatFileName(gSystem->WorkingDirectory(), fDataSelectorDec.Data()));
         gSystem->CopyFile(path_src.Data(), path_trg.Data());
      }
   }
   
   ok = kTRUE;
   
   return ok;
}

//_________________________________________________________________

void KVINDRAReconDataAnalyser::SetKVDataSelector(const Char_t * kvs)
{
   //Set name of data selector to use. Look for source files in current working directory.
   fDataSelector = kvs;
   if(fDataSelector!="") KVBase::FindClassSourceFiles(kvs, fDataSelectorImp, fDataSelectorDec);
}

//_________________________________________________________________

const Char_t* KVINDRAReconDataAnalyser::ExpandAutoBatchName(const Char_t* format)
{
   //Replace any 'special' symbols in "format" with their current values
   //
   //  $Date   : current date and time
   //  $System  :  name of system to be analysed
   //  $User  :  name of user
   //  $UserClass or $Selector :  name of user's analysis class (KVSelector)
   //  $DataSelector  :  name of user's data selector (KVDataSelector)
   
   static KVString tmp;
   tmp = KVDataAnalyser::ExpandAutoBatchName(format);
   tmp.ReplaceAll("$Selector", GetUserClass());
   tmp.ReplaceAll("$DataSelector", GetKVDataSelector());
   return tmp.Data();
}

