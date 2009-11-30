/*
$Id: KVINDRARawDataAnalyser.cpp,v 1.4 2009/01/14 16:13:49 franklan Exp $
$Revision: 1.4 $
$Date: 2009/01/14 16:13:49 $
*/

//Created by KVClassFactory on Fri May  4 15:26:02 2007
//Author: franklan

#include "KVINDRARawDataAnalyser.h"
#include "KVString.h"
#include "KVINDRA.h"
#include "KVDataSet.h"
#include "KVClassFactory.h"

ClassImp(KVINDRARawDataAnalyser)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRARawDataAnalyser</h2>
<h4>Analysis of raw INDRA data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRARawDataAnalyser::KVINDRARawDataAnalyser()
{
   //Default constructor
   fTrig = 0;
}

KVINDRARawDataAnalyser::~KVINDRARawDataAnalyser()
{
   //Destructor
}

void KVINDRARawDataAnalyser::ProcessRun()
{
   // Perform treatment of a given run
	// Before processing each run, after opening the associated file, user's InitRun() method is called.
	// After each run, user's EndRun() is called.
	// For each event of each run, user's Analysis() method is called.
	//
	// For further customisation, the pre/post-methods are called just before and just after
	// each of these methods (preInitRun(), postAnalysis(), etc. etc.)
   
   //Open data file
	KVString dlt_file = gDataSet->GetFullPathToRunfile("raw", fRunNumber);
   fRunFile = (KVINDRARawDataReader*)gDataSet->OpenRunfile( "raw", fRunNumber );
   if( (!fRunFile) || fRunFile->IsZombie() ){
      //skip run if file cannot be opened
      if(fRunFile) delete fRunFile;
      return;
   }
	//warning! real number of run may be different from that deduced from file name
	//we get the real run number from gIndra and use it to name any new files
	Int_t newrun = gIndra->GetCurrentRunNumber();
	if(newrun!=fRunNumber){
		cout << " *** WARNING *** run number read from file = " << newrun << endl;
		fRunNumber = newrun;
	}
   
	fEventNumber=0;//event number
   
   Long64_t nevents = GetNbEventToRead();
	if(nevents<=0){
		nevents=1000000000;
		cout << endl << "Reading all events from file " << dlt_file.Data() << endl;
	} else {
		cout << endl << "Reading " << nevents << " events from file " << dlt_file.Data() << endl;
	}
	
	cout << "Starting analysis of run " << fRunNumber << " on : ";
	TDatime now;
	cout <<  now.AsString() << endl << endl;
   
	preInitRun();
   //call user's beginning of run
   InitRun();
	postInitRun();
   
   Int_t INDRA_events = 0; Int_t gene_events = 0; Int_t other_events = 0;
   
   fDetEv = 0;
   
   //loop over events in file
	while( fRunFile->GetNextEvent() && nevents--)
	{
      fEventNumber++;
      
      if( ((KVINDRARawDataReader*)fRunFile)->IsINDRAEvent() ){
        	
         INDRA_events++;
         
         if( ((KVINDRARawDataReader*)fRunFile)->IsGene() ) gene_events++;
        
		   //set trigger info for event
		   fTrig->SetSTAT_EVE( ((KVINDRARawDataReader*)fRunFile)->GetSTAT_EVE() );
		   fTrig->SetR_DEC( ((KVINDRARawDataReader*)fRunFile)->GetR_DEC() );
		   fTrig->SetCONFIG( ((KVINDRARawDataReader*)fRunFile)->GetCONFIG() );
			
         //reconstruct hit groups
		   fDetEv = gIndra->GetDetectorEvent();
      } else {
         //count non-INDRA events
         other_events++;
      }
      
		preAnalysis();
      //call user's analysis. stop if returns kFALSE.
      if(!Analysis()) break;
		postAnalysis();
      
      if(fDetEv) {delete fDetEv; fDetEv = 0;}
      gIndra->Clear();
      
		if(!((fEventNumber)%10000)) cout<< " ++++ " << fEventNumber << " events read ++++ " << endl;
   }    
    
	cout << endl << "Finished reading " << fEventNumber << " events from file " << dlt_file.Data() << endl;
   cout << " +++ INDRA events : " << INDRA_events << endl;
   cout << " +++ INDRA gene events : " << gene_events << endl;
   cout << " +++ Other (non-INDRA) events : " << other_events << endl << endl;
	cout << "Ending analysis of run " << fRunNumber << " on : ";
	TDatime now2;
	cout <<  now2.AsString() << endl << endl;
	
	preEndRun();
	//call user's end of run function
	EndRun();
	postEndRun();
   
   delete fRunFile;
}

void KVINDRARawDataAnalyser::SubmitTask()
{
   // Perform analysis of chosen runs
	// Before beginning the loop over the runs, the user's InitAnalysis() method is called.
	// After completing the analysis of all runs, the user's EndAnalysis() method is called.
	//
	// Further customisation of the event loop is possible by overriding the methods
	//   preInitAnalysis()
	//   postInitAnalysis()
	//   preEndAnalysis()
	//   postEndAnalysis()
	// which are executed respectively just before and just after the methods.
   
   if(gDataSet != fDataSet) fDataSet->cd();
   gDataSet->BuildMultiDetector();
   
	preInitAnalysis();
   //call user's initialisation
   InitAnalysis();
	postInitAnalysis();
   
   fTrig = new KVINDRATriggerInfo;
   
   //loop over runs
   GetRunList().Begin();
   while( !GetRunList().End() ){
      fRunNumber = GetRunList().Next();
      ProcessRun();
   }
   
   delete fTrig; fTrig = 0;
   
	preEndAnalysis();
   //call user's end of analysis
   EndAnalysis();
	postEndAnalysis();
}

//_______________________________________________________________________//

void KVINDRARawDataAnalyser::Make(const Char_t * kvsname)
{
   //Automatic generation of derived class for raw data analysis
   
   KVClassFactory cf(kvsname, "User raw data analysis class", "KVINDRARawDataAnalyser");
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
   body+= "   //  Int_t fRunNumber contains current run number";
   cf.AddMethodBody("InitRun", body);
   //Analysis
   body = "   //Analysis method called for each event\n";
   body+= "   //  Long64_t fEventNumber contains current event number\n"; 
   body+= "   //  KVINDRATriggerInfo* fTrig contains informations on INDRA trigger for event\n"; 
   body+= "   //  KVDetectorEvent* fDetEv gives list of hit groups for current event\n";
   body+= "   //  Processing will stop if this method returns kFALSE\n";
   body+= "   return kTRUE;";
   cf.AddMethodBody("Analysis", body);
   //endrunù
   body = "   //Method called at end of each run";
   cf.AddMethodBody("EndRun", body);
   //endanalysis
   body = "   //Method called at end of analysis: save/display histograms etc.";
   cf.AddMethodBody("EndAnalysis", body);
   cf.GenerateCode();
}

KVNumberList KVINDRARawDataAnalyser::PrintAvailableRuns(KVString & datatype)
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
