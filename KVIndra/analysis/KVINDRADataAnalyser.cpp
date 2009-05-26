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

