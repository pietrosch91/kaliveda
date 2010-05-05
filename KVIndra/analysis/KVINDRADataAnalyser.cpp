/*
$Id: KVINDRADataAnalyser.cpp,v 1.9 2007/11/13 08:58:31 franklan Exp $
$Revision: 1.9 $
$Date: 2007/11/13 08:58:31 $
*/

//Created by KVClassFactory on Wed Apr  5 23:50:04 2006
//Author: John Frankland

#include "KVINDRADataAnalyser.h"
#include "KVINDRADBRun.h"
#include "KVDataAnalysisTask.h"
#include "KVDataSet.h"
#include "TChain.h"
#include "TSelector.h"
#include "TEnv.h"
#include "THashList.h"
#include "KVString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "KVINDRA.h"


ClassImp(KVINDRADataAnalyser)
////////////////////////////////////////////////////////////////////////////////
// For analysing INDRA data
//
//The difference from the basic KVDataAnalyser is that
//when lists of runs are presented to the user, they are
//grouped according to the multiplicity trigger of the
//acquisition.
////////////////////////////////////////////////////////////////////////////////
KVINDRADataAnalyser::KVINDRADataAnalyser()
{
}


KVINDRADataAnalyser::~KVINDRADataAnalyser()
{
   //Destructor
}

//_________________________________________________________________

KVNumberList KVINDRADataAnalyser::PrintAvailableRuns(KVString & datatype)
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

//_________________________________________________________________
void KVINDRADataAnalyser::WriteBatchInfo(TTree* tt){


tt->GetUserInfo()->Add(new TEnv());		
TEnv* kvenv = (TEnv* )tt->GetUserInfo()->FindObject("TEnv");
		
//----
THashList* hh = gEnv->GetTable();
KVString tamp;
for (Int_t kk=0;kk<hh->GetEntries();kk+=1){
	tamp.Form("%s",hh->At(kk)->GetName());
	if (tamp.BeginsWith("Plugin.")){}
	else kvenv->SetValue(hh->At(kk)->GetName(),((TEnvRec* )hh->At(kk))->GetValue(),kEnvUser);
}
		
kvenv->SetValue("gIndra->GetKVRoot()",gIndra->GetKVRoot(),kEnvUser);
kvenv->SetValue("gIndra->GetKVVersion()",gIndra->GetKVVersion(),kEnvUser);
kvenv->SetValue("gIndra->GetKVBuildDate()",gIndra->GetKVBuildDate(),kEnvUser);
kvenv->SetValue("gIndra->GetKVBuildUser()",gIndra->GetKVBuildUser(),kEnvUser);
kvenv->SetValue("gIndra->GetKVSourceDir()",gIndra->GetKVSourceDir(),kEnvUser);
kvenv->SetValue("gIndra->GetKVRootDir()",gIndra->GetKVRootDir(),kEnvUser);
kvenv->SetValue("gIndra->GetKVBinDir()",gIndra->GetKVBinDir(),kEnvUser);
kvenv->SetValue("gIndra->GetKVFilesDir()",gIndra->GetKVFilesDir(),kEnvUser);

kvenv->SetValue("gIndra->bzrRevisionId()",gIndra->bzrRevisionId(),kEnvUser);
kvenv->SetValue("gIndra->bzrRevisionDate()",gIndra->bzrRevisionDate(),kEnvUser);
kvenv->SetValue("gIndra->bzrBranchNick()",gIndra->bzrBranchNick(),kEnvUser);
kvenv->SetValue("gIndra->bzrRevisionNumber()",gIndra->bzrRevisionNumber());
kvenv->SetValue("gIndra->bzrIsBranchClean()",gIndra->bzrIsBranchClean());

kvenv->SetValue("gROOT->GetVersion()",gROOT->GetVersion(),kEnvUser);

kvenv->SetValue("gSystem->GetBuildArch()",gSystem->GetBuildArch(),kEnvUser);
kvenv->SetValue("gSystem->GetBuildCompiler()",gSystem->GetBuildCompiler(),kEnvUser);
kvenv->SetValue("gSystem->GetBuildCompilerVersion()",gSystem->GetBuildCompilerVersion(),kEnvUser);
kvenv->SetValue("gSystem->GetBuildNode()",gSystem->GetBuildNode(),kEnvUser);
kvenv->SetValue("gSystem->GetBuildDir()",gSystem->GetBuildDir(),kEnvUser);

kvenv->SetValue("gSystem->GetUserInfo()->fUser",gSystem->GetUserInfo()->fUser,kEnvUser);
kvenv->SetValue("gSystem->HostName()",gSystem->HostName(),kEnvUser);
		
if ( fBatchEnv ){
	THashList* hh = fBatchEnv->GetTable();
	for (Int_t kk=0;kk<hh->GetEntries();kk+=1){
		tamp.Form("%s",hh->At(kk)->GetName());
		if ( !strcmp(kvenv->GetValue(hh->At(kk)->GetName(),"rien"),"rien") )
			kvenv->SetValue(hh->At(kk)->GetName(),((TEnvRec* )hh->At(kk))->GetValue(),kEnvUser);
	}
}


}
