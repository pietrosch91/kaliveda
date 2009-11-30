/*
$Id: KVINDRARawIdent.cpp,v 1.2 2008/11/12 16:15:49 franklan Exp $
$Revision: 1.2 $
$Date: 2008/11/12 16:15:49 $
*/

//Created by KVClassFactory on Wed Feb 20 10:52:20 2008
//Author: franklan

#include "KVINDRARawIdent.h"
#include "KVINDRA.h"
#include "KVDataSet.h"
#include "KVINDRADB.h"
#include "KVDataRepositoryManager.h"
#include "KVDataRepository.h"

ClassImp(KVINDRARawIdent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRARawIdent</h2>
<h4>User raw data analysis class</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRARawIdent::KVINDRARawIdent()
{
   // Default constructor
   file = 0;
   tree = genetree = 0;
   recev=0;
   nb_recon=0;
}

KVINDRARawIdent::~KVINDRARawIdent()
{
   // Destructor
   if(recev) delete recev;
}

//________________________________________________________________

void KVINDRARawIdent::InitAnalysis () 
{
   // Create new KVINDRAReconEvent used to reconstruct & store events
   // The condition used to seed new reconstructed particles (see KVReconstructedEvent::AnalyseTelescopes)
   // is set by reading the value of the environment variables:
   //     Reconstruction.DataAnalysisTask.ParticleSeedCond:        [all/any]
   //     [name of dataset].Reconstruction.DataAnalysisTask.ParticleSeedCond:     [all/any]
   // If no value is set for the current dataset (second variable), the value of the
   // first variable will be used.
   
   recev = new KVINDRAReconEvent;
   recev->SetPartSeedCond( gDataSet->GetDataSetEnv("Reconstruction.DataAnalysisTask.ParticleSeedCond") );
}

//________________________________________________________________

void KVINDRARawIdent::InitRun () 
{
   // Creates new ROOT file with TTree for reconstructed/calibrated events.
   // By default this file will be written in the same data repository as the raw data file we are reading.
   // This can be changed by setting the environment variable(s):
   //
   //     RawIdent.DataAnalysisTask.OutputRepository:     [name of repository]
   //     [name of dataset].RawIdent.DataAnalysisTask.OutputRepository:         [name of repository]
   //
   // If no value is set for the current dataset (second variable), the value of the
   // first variable will be used. If neither is defined, the new file will be written in the same repository as
   // the raw file (if possible, i.e. if repository is not remote).
   
   // get dataset to which we must associate new run
   KVDataSet* OutputDataset =
      gDataRepositoryManager->GetDataSet(
         gDataSet->GetDataSetEnv("RawIdent.DataAnalysisTask.OutputRepository", gDataRepository->GetName()),
         gDataSet->GetName() );
      
		file = OutputDataset->NewRunfile("ident", fRunNumber);
      
		cout << "Writing \"ident\" events in ROOT file " << file->GetName() << endl;
      
      //tree for reconstructed events
		tree = new TTree("ReconstructedEvents", Form("%s : %s : ident events created from raw data",
			 	gIndraDB->GetRun(fRunNumber)->GetName(),
            gIndraDB->GetRun(fRunNumber)->GetTitle())
            );
		tree->SetAutoSave(30000000);
      
      //leaves for reconstructed events
      tree->Branch("RunNumber", &fRunNumber, "RunNumber/I");
      tree->Branch("EventNumber", &fEventNumber, "EventNumber/I");
		tree->Branch("INDRAReconEvent", "KVINDRAReconEvent", &recev, 64000, 0)->SetAutoDelete(kFALSE);
      
		if( FileHasUnknownParameters() ){
         
         //we add a leaf for each unknown data parameter
         TIter next_unknown( fRunFile->GetUnknownParameters() );
         KVACQParam* acqpar;
         while( (acqpar = (KVACQParam*)next_unknown()) ){
            tree->Branch( acqpar->GetName(), *(acqpar->ConnectData()), Form("%s/S", acqpar->GetName()));
         }         
      }
      
      Info("InitRun", "Created reconstructed data tree %s : %s", tree->GetName(), tree->GetTitle());
      if( FileHasUnknownParameters() ){
         Info("InitRun", "Reconstructed data tree contains %d unknown (non-INDRA) parameters", 
               (fRunFile->GetUnknownParameters()->GetSize()));
      }
      
      //tree for raw data
		rawtree = new TTree("RawData", Form("%s : %s : raw data",
			 	gIndraDB->GetRun(fRunNumber)->GetName(), gIndraDB->GetRun(fRunNumber)->GetTitle()));
      rawtree->Branch("RunNumber", &fRunNumber, "RunNumber/I");
      rawtree->Branch( "EventNumber", &fEventNumber, "EventNumber/I");
      //we add to the 'raw tree' a branch for every acquisition parameter
      TIter next_rawpar( fRunFile->GetRawDataParameters() );
      KVACQParam* acqpar;
      while( (acqpar = (KVACQParam*)next_rawpar()) ){
         rawtree->Branch( acqpar->GetName(), *(acqpar->ConnectData()), Form("%s/S", acqpar->GetName()));
      }
      Info("InitRun", "Created raw data tree (%s : %s) for %d parameters",
            rawtree->GetName(), rawtree->GetTitle(), rawtree->GetNbranches());
      
      //tree for gene data
		genetree = new TTree("GeneData", Form("%s : %s : gene data",
			 	gIndraDB->GetRun(fRunNumber)->GetName(), gIndraDB->GetRun(fRunNumber)->GetTitle()));
      
      //we add to the 'gene tree' a branch for every acquisition parameter of the detector
      genetree->Branch("RunNumber", &fRunNumber, "RunNumber/I");
      genetree->Branch( "EventNumber", &fEventNumber, "EventNumber/I");
      genetree->Branch( "TriggerInfo", "KVINDRATriggerInfo", &fTrig, 32000, 0 );
      TIter next_acqpar( gIndra->GetACQParams() );
      while( (acqpar = (KVACQParam*)next_acqpar()) ){
         genetree->Branch( acqpar->GetName(), *(acqpar->ConnectData()), Form("%s/S", acqpar->GetName()));
      }
      
      Info("InitRun", "Created pulser/laser data tree (%s : %s) for %d parameters",
            genetree->GetName(), genetree->GetTitle(), genetree->GetNbranches());
      //initialise number of reconstructed events
      nb_recon = 0;
      
      // initialise identifications
      gIndra->InitializeIDTelescopes();
   
      // print status of identifications
      gIndra->PrintStatusOfIDTelescopes();
      // print status of calibrations
      gIndra->PrintCalibStatusOfDetectors();
}

//________________________________________________________________

Bool_t KVINDRARawIdent::Analysis () 
{
   // Reconstruct, identify and calibrate an event.
   
	rawtree->Fill();
	
   if( IsINDRAEvent() ){
      
      if( ((KVINDRARawDataReader*)fRunFile)->IsPhysics() ){
         
         recev->ReconstructEvent( fDetEv );
         recev->SetNumber( fEventNumber );
         if (recev->GetMult() > 0) {
            recev->IdentifyEvent();
            recev->CalibrateEvent();
         }
         //set trigger info in event
		   recev->EventTrigger().SetSTAT_EVE( fTrig->GetSTAT_EVE() );
		   recev->EventTrigger().SetR_DEC( fTrig->GetR_DEC() );
		   recev->EventTrigger().SetCONFIG( fTrig->GetCONFIG() );
         
         tree->Fill();
         recev->Clear();
         nb_recon++;
      }
      else
      {
         genetree->Fill();
      }
   }
   else if( FileHasUnknownParameters() ){
      tree->Fill();
   }
   
   return kTRUE;
}

//________________________________________________________________

void KVINDRARawIdent::EndRun () 
{
   //Method called at end of each run
      cout << endl << " *** Number of reconstructed INDRA events : "
            << nb_recon << " ***" << endl<< endl;
		file->cd();
		gIndra->Write("INDRA");//write INDRA to file
		gDataAnalyser->WriteBatchInfo(tree);
		tree->Write();//write tree to file
      rawtree->Write();
      genetree->Write();
      
      // get dataset to which we must associate new run
      KVDataSet* OutputDataset =
         gDataRepositoryManager->GetDataSet(
            gDataSet->GetDataSetEnv("RawIdent.DataAnalysisTask.OutputRepository", gDataRepository->GetName()),
            gDataSet->GetName() );
		//add new file to repository
		OutputDataset->CommitRunfile("ident", fRunNumber, file);
}

//________________________________________________________________

void KVINDRARawIdent::EndAnalysis () 
{
   //Method called at end of analysis: save/display histograms etc.
}

