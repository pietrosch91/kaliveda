#include "KVIVReconIdent.h"
#include "KVIVReconEvent.h"
#include "KVVAMOS.h"
#include "TFile.h"
#include "TTree.h"
#include "KVDataRepository.h"
#include "KVDataSet.h"
#include "KVDataRepositoryManager.h"
#include "KVDataAnalyser.h"
#include "KVINDRADB.h"

using namespace std;

ClassImp(KVIVReconIdent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIVReconIdent</h2>
<h4>INDRA identification and calibration, VAMOS trajectory reconstruction,
calibration and identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVIVReconIdent::InitAnalysis(void){
}
//_____________________________________

void KVIVReconIdent::InitRun(void){
   //When each run is opened, we create a new ROOT file for the identified events we
   //are going to generate from the reconstructed events we are reading.
   // By default this file will be written in the same data repository as the recon data file we are reading.
   // This can be changed by setting the environment variable(s):
   //
   //     ReconIdent.DataAnalysisTask.OutputRepository:     [name of repository]
   //     [name of dataset].ReconIdent.DataAnalysisTask.OutputRepository:         [name of repository]
   //
   // If no value is set for the current dataset (second variable), the value of the
   // first variable will be used. If neither is defined, the new file will be written in the same repository as
   // the recon file (if possible, i.e. if repository is not remote).

   // get dataset to which we must associate new run
   KVDataSet* OutputDataset =
      gDataRepositoryManager->GetDataSet(
         gDataSet->GetDataSetEnv("ReconIdent.DataAnalysisTask.OutputRepository", gDataRepository->GetName()),
         gDataSet->GetName() );

   //create new ROOT file for identified events
   fRunNumber = gIndra->GetCurrentRunNumber();
   fIdentFile = OutputDataset->NewRunfile("ident", fRunNumber);


		fIdentTree = new TTree("ReconstructedEvents", Form("%s : %s : ident events created from recon data",
			 	gIndraDB->GetRun(fRunNumber)->GetName(),
            gIndraDB->GetRun(fRunNumber)->GetTitle())
            );
#if ROOT_VERSION_CODE > ROOT_VERSION(5,25,4)
#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,1)
   // The TTree::OptimizeBaskets mechanism is disabled, as for ROOT versions < 5.26/00b
   // this lead to a memory leak
   fIdentTree->SetAutoFlush(0);
#endif
#endif
      //leaves for reconstructed events
      TBranch *recon_br = (TBranch *)fChain->GetListOfBranches()->First();
      KVEvent::MakeEventBranch(fIdentTree,recon_br->GetName(),recon_br->GetClassName(),GetEventReference());

	 // set flag if this branch contains a KVIVReconEvent object
	fIsIVevent = TClass::GetClass(recon_br->GetClassName())->InheritsFrom("KVIVReconEvent");

      Info("InitRun", "Created identified/calibrated data tree %s : %s", fIdentTree->GetName(), fIdentTree->GetTitle());

   // initialise identifications
   gIndra->InitializeIDTelescopes();
   gVamos->InitializeIDTelescopes();
 
   cout << endl <<setw(20)<<""<<"----------------------"<<endl;
   cout         <<setw(20)<<""<<"|  STATUS FOR INDRA  |"<<endl;
   cout         <<setw(20)<<""<<"----------------------"<<endl<<endl;
   // print status of identifications
   gIndra->PrintStatusOfIDTelescopes();
   // print status of calibrations
   gIndra->PrintCalibStatusOfDetectors();

   cout << endl <<setw(20)<<""<<"----------------------"<<endl;
   cout         <<setw(20)<<""<<"|  STATUS FOR VAMOS  |"<<endl;
   cout         <<setw(20)<<""<<"----------------------"<<endl<<endl;
   // print status of identifications
   gVamos->PrintStatusOfIDTelescopes();
   // print status of calibrations
   gVamos->PrintCalibStatusOfDetectors();
}
//_____________________________________

Bool_t KVIVReconIdent::Analysis(void){
   //For each event we:
   //     perform primary event identification and calibration and fill tree

   fEventNumber = GetEvent()->GetNumber();

   //Analyse INDRA event
   if (GetEvent()->GetMult() > 0) {
      GetEvent()->IdentifyEvent();
      GetEvent()->CalibrateEvent();
   }

   //Analyse VAMOS event
   if( fIsIVevent ){ // condition set for backwards compatibility with 
	                 // old recon ROOT files

	   KVIVReconEvent *IVevent = (KVIVReconEvent *)GetEvent();
	   // Z-identification
	   IVevent->IdentifyVAMOSEvent_Z();

	   // first calibration with only Z known and mean A from mass formula
	   IVevent->CalibrateVAMOSEvent();
//
//	   // firt A-identification
//	   IVevent->IdentifyVAMOSevent_A();
//
//	   // second calibration with Z and A known
//	   IVevent->CalibrateVAMOSevent();
//
//	   //second A-identification
//	   IVevent->IdentifyVAMOSevent_A();
   }

   //Fill Ident tree
   fIdentTree->Fill();

   return kTRUE;
}
//_____________________________________

void KVIVReconIdent::EndAnalysis(void){
}
