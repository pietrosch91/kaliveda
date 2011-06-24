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
#include "KVAvailableRunsFile.h"

ClassImp(KVINDRAReconDataAnalyser)
////////////////////////////////////////////////////////////////////////////////
// For analysing reconstructed INDRA data
//
////////////////////////////////////////////////////////////////////////////////
KVINDRAReconDataAnalyser::KVINDRAReconDataAnalyser()
{
   //Default constructor
   fDataSelector="none";
   theChain=0;
   theRawData=0;
   ParVal=0;
   ParNum=0;
}

void KVINDRAReconDataAnalyser::Reset()
{
   //Reset task variables
   KVDataAnalyser::Reset();
   fDataSelector="none";
   theChain=0;
   theRawData=0;
   ParVal=0;
   ParNum=0;
}

KVINDRAReconDataAnalyser::~KVINDRAReconDataAnalyser()
{
   //Destructor
   if(ParVal) delete [] ParVal;
   if(ParNum) delete [] ParNum;
}

//_________________________________________________________________

Bool_t KVINDRAReconDataAnalyser::CheckTaskVariables()
{
   // Checks the task variables 
   
   if(!KVDataAnalyser::CheckTaskVariables()) return kFALSE;
   
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
   
   theChain = new TChain("ReconstructedEvents");
   theChain->SetDirectory(0); // we handle delete
   
   fRunList.Begin(); Int_t run;
   while( !fRunList.End() ){
      run = fRunList.Next();
      cout << "Adding file " << gDataSet->GetFullPathToRunfile(fDataType.Data(),run);
      cout << " to the TChain." << endl;
      theChain->Add(gDataSet->GetFullPathToRunfile(fDataType.Data(),run));
   }
   
   TString option("");
   if (fDataSelector.Length()) {
      option.Form("DataSelector=%s", fDataSelector.Data());
      cout << "Data Selector : " << fDataSelector.Data() << endl;
   }
   
   TSelector *selector = (TSelector*)GetInstanceOfUserClass();
   
   if(!selector || !selector->InheritsFrom("TSelector"))
    {
    	cout << "The selector \"" << GetUserClass() << "\" is not valid." << endl;
    	cout << "Process aborted." << endl;
    	if(selector) {
    		delete selector;
    		selector=0;
    	}
    }
   else
    {
      if (nbEventToRead) {
         theChain->Process(GetUserClass(), option.Data(),nbEventToRead);
      } else {
         theChain->Process(GetUserClass(), option.Data());
      }
    }
   if(selector) delete selector;
   delete theChain;
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
   
   KVDataAnalyser::WriteBatchEnvFile(jobname, kFALSE);
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

KVNumberList KVINDRAReconDataAnalyser::PrintAvailableRuns(KVString & datatype)
{
   //Prints list of available runs, sorted according to multiplicity
   //trigger, for selected dataset, data type/analysis task, and system
   //Returns list containing all run numbers

   KVNumberList all_runs=
       fDataSet->GetRunList(datatype.Data(), fSystem);
   KVINDRADBRun *dbrun;
   
   //first read list and find what triggers are available
   int triggers[10], n_trigs = 0;
   all_runs.Begin();
   while ( !all_runs.End() ) {
      dbrun = (KVINDRADBRun *)fDataSet->GetDataBase()->GetTable("Runs")->GetRecord(all_runs.Next());
      if (!KVBase::
          ArrContainsValue(n_trigs, triggers, dbrun->GetTrigger())) {
         triggers[n_trigs++] = dbrun->GetTrigger();
      }
   }
   //sort triggers in ascending order
   int ord_trig[10];
   TMath::Sort(n_trigs, triggers, ord_trig, kFALSE);

   int trig = 0;
   while (trig < n_trigs) {
      cout << " ---> Trigger M>" << triggers[ord_trig[trig]] << endl;
      all_runs.Begin();
      while ( !all_runs.End() ) {
         dbrun = (KVINDRADBRun *)fDataSet->GetDataBase()->GetTable("Runs")->GetRecord(all_runs.Next());
         if (dbrun->GetTrigger() == triggers[ord_trig[trig]]) {
            cout << "    " << Form("%4d", dbrun->GetNumber());
            cout << Form("\t(%7d events)", dbrun->GetEvents());
            cout << "\t[File written: " << dbrun->GetDatime().
                AsString() << "]";
            if (dbrun->GetComments())
               cout << "\t" << dbrun->GetComments();
            cout << endl;
         }
      }
      trig++;
      cout << endl;
   }
   return all_runs;
}

void KVINDRAReconDataAnalyser::preInitAnalysis()
{
	// Called by currently-processed KVSelector before user's InitAnalysis() method.
	// We build the multidetector for the current dataset in case informations on
	// detector are needed e.g. to define histograms in InitAnalysis().
	// Note that at this stage we are not analysing a given run, so the parameters
	// of the array are not set (they will be set in preInitRun()).
		
	if( !gIndra ) gDataSet->BuildMultiDetector();
}


void KVINDRAReconDataAnalyser::preInitRun()
{
	// Called by currently-processed KVSelector when a new file in the TChain is opened.
	// We call gIndra->SetParameters for the current run.
	
	Int_t run = GetRunNumberFromFileName( theChain->GetCurrentFile()->GetName() );
	gIndra->SetParameters(run);
	ConnectRawDataTree();
}

void KVINDRAReconDataAnalyser::preAnalysis()
{
	// Read and set raw data for this event
	
	if(!theRawData) return;
	theRawData->GetEntry(Entry);
	Entry++;
	for(int i=0; i<NbParFired; i++){
		KVACQParam* par = gIndra->GetACQParam((*parList)[ParNum[i]]->GetName());
		if(par) par->SetData(ParVal[i]);
	}
}

void KVINDRAReconDataAnalyser::ConnectRawDataTree()
{
	// Called by preInitRun().
	// When starting to read a new run (=new file), we look for the TTree "RawData" in the
	// current file (it should have been created by KVINDRARawDataReconstructor).
	// If found, it will be used by ReadRawData() to set the values of all acquisition parameters
	// for each event.
	
	theRawData=(TTree*)theChain->GetCurrentFile()->Get("RawData");
	if(!theRawData){
		Warning("ConnectRawDataTree", "RawData tree not found in file; raw data parameters of detectors will not be available in analysis");
		return;
	}
	else
		Info("ConnectRawDataTree", "Found RawData tree in file");
	Int_t maxNopar = theRawData->GetMaximum("NbParFired");
   if(ParVal) delete [] ParVal;
   if(ParNum) delete [] ParNum;
	ParVal = new UShort_t[maxNopar];	
	ParNum = new UInt_t[maxNopar];	
	parList = (TObjArray*)theRawData->GetUserInfo()->FindObject("ParameterList");
	theRawData->SetBranchAddress("NbParFired", &NbParFired);
	theRawData->SetBranchAddress("ParNum", ParNum);
	theRawData->SetBranchAddress("ParVal", ParVal);
	Info("ConnectRawDataTree", "Connected raw data parameters");
	Entry=0;
}
