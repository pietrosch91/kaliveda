//Created by KVClassFactory on Fri Jan 23 14:17:23 2015
//Author: ,,,

#include "KVFAZIARawDataReconstructor.h"
#include "KVDataAnalyser.h"
#include "KVDataSet.h"
#include "KVFAZIADB.h"
#include "KVDataRepositoryManager.h"
#include "KVDataRepository.h"
#include "RVersion.h"

ClassImp(KVFAZIARawDataReconstructor)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIARawDataReconstructor</h2>
<h4>Handle reconstruction of FAZIA events</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIARawDataReconstructor::KVFAZIARawDataReconstructor()
   : taskname("Reconstruction"), datatype("recon")
{
   //Default constructor
   file = 0;
   tree = 0;
   recev=0;
   nb_recon=0;
}

KVFAZIARawDataReconstructor::~KVFAZIARawDataReconstructor()
{
   // Destructor
   SafeDelete(recev);
}

//______________________________________________________________________________________//

void KVFAZIARawDataReconstructor::InitRun()
{
   // Creates new ROOT file with TTree for reconstructed/calibrated events.
   // By default this file will be written in the same data repository as the raw data file we are reading.
   // This can be changed by setting the environment variable(s):
   //
   //     Reconstruction.DataAnalysisTask.OutputRepository:     [name of repository]
   //     [name of dataset].Reconstruction.DataAnalysisTask.OutputRepository:         [name of repository]
   //
   // If no value is set for the current dataset (second variable), the value of the
   // first variable will be used. If neither is defined, the new file will be written in the same repository as
   // the raw file (if possible, i.e. if repository is not remote).
   
   // Create new KVINDRAReconEvent used to reconstruct & store events
   // The condition used to seed new reconstructed particles (see KVReconstructedEvent::AnalyseTelescopes)
   // is set by reading the value of the environment variables:
   //     Reconstruction.DataAnalysisTask.ParticleSeedCond:        [all/any]
   //     [name of dataset].Reconstruction.DataAnalysisTask.ParticleSeedCond:     [all/any]
   // If no value is set for the current dataset (second variable), the value of the
   // first variable will be used.
 
	if(!recev) recev = new KVReconstructedEvent;
   //recev->SetPartSeedCond( gDataSet->GetDataSetEnv("Reconstruction.DataAnalysisTask.ParticleSeedCond") );
  
   // get dataset to which we must associate new run
   KVDataSet* OutputDataset =
      gDataRepositoryManager->GetDataSet(
         gDataSet->GetDataSetEnv(Form("%s.DataAnalysisTask.OutputRepository",taskname.Data()),
            gDataRepository->GetName()),
         gDataSet->GetName() );
      
		file = OutputDataset->NewRunfile(datatype.Data(), fCurrentRun);
      
		cout << "Writing \"" << datatype.Data() << "\" events in ROOT file " << file->GetName() << endl;

      //tree for reconstructed events
		tree = new TTree("ReconstructedEvents", Form("%s : %s : %s events created from raw data",
			 	gFaziaDB->GetRun(fCurrentRun)->GetName(),
            gFaziaDB->GetRun(fCurrentRun)->GetTitle(),
            datatype.Data())
            );

      //leaves for reconstructed events
		tree->Branch("FAZIAReconEvent", "KVReconstructedEvent", &recev, 10000000, 0)->SetAutoDelete(kFALSE);
      
      Info("InitRun", "Created reconstructed data tree %s : %s", tree->GetName(), tree->GetTitle());
      nb_recon = 0;
		
}

//______________________________________________________________________________________//

Bool_t KVFAZIARawDataReconstructor::Analysis()
{
	// Analysis of event.
	// RawData TTree is filled with values of parameters for event.
	// If event has INDRA trigger information (IsIndraEvent()==kTRUE)
	// then
	//    *) event reconstruction is performed for 'Physics' events
	//    *) or the GeneTree is filled with pulser/laser data for 'Gene' events
	
   
   recev->SetNumber( GetEventNumber() );		         
	//printf("!! Appel de ReconstructedEvent::ReconstructEvent()\n");
   recev->ReconstructEvent( GetDetectorEvent() );
	
   ExtraProcessing();
	if (recev->GetMult()>0){
   	nb_recon++;
      tree->Fill();
   }
   //printf("!! Appel de ReconstructedEvent::Clear()\n");
   recev->Clear();
   //printf("!!! Appel de KVDetectorEvent::Clear()\n");
   GetDetectorEvent()->GetGroups()->Clear();
   
   return kTRUE;
}

//______________________________________________________________________________________//

void KVFAZIARawDataReconstructor::EndRun()
{
   SafeDelete(recev);
   
	cout << endl << " *** Number of reconstructed FAZIA events : "
		<< nb_recon << " ***" << endl<< endl;
	file->cd();
//		gDataAnalyser->WriteBatchInfo(tree);
		tree->Write();//write tree to file
      
      // get dataset to which we must associate new run
      KVDataSet* OutputDataset =
         gDataRepositoryManager->GetDataSet(
            gDataSet->GetDataSetEnv(Form("%s.DataAnalysisTask.OutputRepository",taskname.Data()),
               gDataRepository->GetName()),
            gDataSet->GetName() );
		//add new file to repository
		OutputDataset->CommitRunfile(datatype.Data(), fCurrentRun, file);
}
