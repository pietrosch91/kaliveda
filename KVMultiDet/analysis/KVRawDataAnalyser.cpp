//Created by KVClassFactory on Thu Sep 24 11:07:45 2009
//Author: John Frankland,,,

#include "KVRawDataAnalyser.h"
#include "KVMultiDetArray.h"
#include "KVClassFactory.h"

ClassImp(KVRawDataAnalyser)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVRawDataAnalyser</h2>
<h4>Abstract base class for user analysis of raw data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVRawDataAnalyser::KVRawDataAnalyser()
{
   // Default constructor
   fRunFile = 0;
   fDetEv = 0;
}

KVRawDataAnalyser::~KVRawDataAnalyser()
{
   // Destructor
}

void KVRawDataAnalyser::ProcessRun()
{
   // Perform treatment of a given run
	// Before processing each run, after opening the associated file, user's InitRun() method is called.
	// After each run, user's EndRun() is called.
	// For each event of each run, user's Analysis() method is called.
	//
	// For further customisation, the pre/post-methods are called just before and just after
	// each of these methods (preInitRun(), postAnalysis(), etc. etc.)

   //Open data file
	KVString raw_file = gDataSet->GetFullPathToRunfile("raw", fRunNumber);
   fRunFile = (KVRawDataReader*)gDataSet->OpenRunfile( "raw", fRunNumber );
   if( (!fRunFile) || fRunFile->IsZombie() ){
      //skip run if file cannot be opened
      if(fRunFile) delete fRunFile;
      return;
   }
	//warning! real number of run may be different from that deduced from file name
	//we get the real run number from gMultiDetArray and use it to name any new files
	Int_t newrun = gMultiDetArray->GetCurrentRunNumber();
	if(newrun!=fRunNumber){
		cout << " *** WARNING *** run number read from file = " << newrun << endl;
		fRunNumber = newrun;
	}

	fEventNumber=1;//event number

   Long64_t nevents = GetNbEventToRead();
	if(nevents<=0){
		nevents=1000000000;
		cout << endl << "Reading all events from file " << raw_file.Data() << endl;
	} else {
		cout << endl << "Reading " << nevents << " events from file " << raw_file.Data() << endl;
	}

	cout << "Starting analysis of run " << fRunNumber << " on : ";
	TDatime now;
	cout <<  now.AsString() << endl << endl;

	preInitRun();
   //call user's beginning of run
   InitRun();
	postInitRun();

   fDetEv = new KVDetectorEvent;

   //loop over events in file
	while( fRunFile->GetNextEvent() && nevents--)
	{
      //reconstruct hit groups
      KVSeqCollection* fired = fRunFile->GetFiredDataParameters();
      gMultiDetArray->GetDetectorEvent(fDetEv, fired);

		preAnalysis();
      //call user's analysis. stop if returns kFALSE.
      if(!Analysis()) break;
		postAnalysis();

       fDetEv->Clear();

		if(!((fEventNumber)%10000)) cout<< " ++++ " << fEventNumber << " events read ++++ " << endl;
		fEventNumber+=1;
   }

    delete fDetEv;

	cout << "Ending analysis of run " << fRunNumber << " on : ";
	TDatime now2;
	cout <<  now2.AsString() << endl << endl;
	cout << endl << "Finished reading " << fEventNumber-1 << " events from file " << raw_file.Data() << endl << endl;

	preEndRun();
	//call user's end of run function
	EndRun();
	postEndRun();

   delete fRunFile;
}


void KVRawDataAnalyser::SubmitTask()
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

   //loop over runs
   GetRunList().Begin();
   while( !GetRunList().End() ){
      fRunNumber = GetRunList().Next();
      ProcessRun();
   }

	preEndAnalysis();
   //call user's end of analysis
   EndAnalysis();
	postEndAnalysis();
}


//_______________________________________________________________________//

void KVRawDataAnalyser::Make(const Char_t * kvsname)
{
   //Automatic generation of derived class for raw data analysis

   KVClassFactory cf(kvsname, "User raw data analysis class", "KVRawDataAnalyser");
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
   body+= "   //  GetRunNumber() returns current run number";
   cf.AddMethodBody("InitRun", body);
   //Analysis
   body = "   //Analysis method called for each event\n";
   body+= "   //  GetEventNumber() returns current event number\n";
   body+= "   //  GetDetectorEvent() gives pointer to list of hit groups (KVDetectorEvent) for current event\n";
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
