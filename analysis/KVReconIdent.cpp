#define KVReconIdent_cxx

#include "KVReconIdent.h"
#include "TFile.h"
#include "TTree.h"
#include "KVDataRepository.h"
#include "KVDataSet.h"
#include "KVDataRepositoryManager.h"

ClassImp(KVReconIdent)

///////////////////////////////////////////////////////////
// Analysis class used to identify previously
// reconstructed events and write the new data files in the data repository
//

void KVReconIdent::InitAnalysis(void)
{
}

//_____________________________________
void KVReconIdent::InitRun(void)
{
   //When each run is opened, we create a new ROOT file for the identified events we
   //are going to generate from the reconstructed events we are reading.
   // By default this file will be written in the same data repository as the recon data file we are reading.
   // This can be changed by setting the environment variable(s):
   //
   //     Identification1.DataAnalysisTask.OutputRepository:     [name of repository]
   //     [name of dataset].Identification1.DataAnalysisTask.OutputRepository:         [name of repository]
   //
   // If no value is set for the current dataset (second variable), the value of the
   // first variable will be used. If neither is defined, the new file will be written in the same repository as
   // the recon file (if possible, i.e. if repository is not remote).

   // get dataset to which we must associate new run
   KVDataSet* OutputDataset =
      gDataRepositoryManager->GetDataSet(
         gDataSet->GetDataSetEnv("Identification1.DataAnalysisTask.OutputRepository", gDataRepository->GetName()),
         gDataSet->GetName() );
         
   //create new ROOT file for identified events
   fRunNumber = gIndra->GetCurrentRunNumber();
   fIdentFile = OutputDataset->NewRunfile("ident", fRunNumber);
   
   KVString tree_title = gIndra->GetCurrentRun()->GetName();
   if(gIndra->GetSystem()) tree_title += Form(" : %s", gIndra->GetSystem()->GetName());
   tree_title += " : ident events created from recon data";
   fIdentTree = new TTree("ReconstructedEvents", tree_title.Data());
   fIdentTree->SetAutoSave(30000000);
   fIdentTree->Branch("RunNumber", &fRunNumber, "RunNumber/I");
   fIdentTree->Branch("EventNumber", &fEventNumber, "EventNumber/I");
   fIdentTree->Branch("INDRAReconEvent", "KVINDRAReconEvent", GetEventReference(),
                      64000, 0)->SetAutoDelete(kFALSE);
   
   // initialise identifications
   gIndra->InitializeIDTelescopes();
   
   // print status of identifications
   gIndra->PrintStatusOfIDTelescopes();
   // print status of calibrations
   gIndra->PrintCalibStatusOfDetectors();
}

//_____________________________________
Bool_t KVReconIdent::Analysis(void)
{
   //For each event we:
   //     perform primary event identification and calibration and fill tree
   //Events with zero reconstructed multiplicity are excluded, but no selection is made
   //based on the results of the identification/calibration

   fEventNumber = GetEvent()->GetNumber();
   if (GetEvent()->GetMult() > 0) {
      GetEvent()->IdentifyEvent();
      GetEvent()->CalibrateEvent();
      fIdentTree->Fill();
   }
   return kTRUE;
}

//_____________________________________
void KVReconIdent::EndRun(void)
{
   //At the end of each run we:
   //      write the tree and INDRA into the new file
   //      close the file
   //      copy the file into the required repository (see InitRun)
   //      update the available runlist

   fIdentFile->cd();
   gIndra->Write("INDRA");      //write INDRA to file
   fIdentTree->Write();         //write tree to file
   if(GetRawData()) GetRawData()->Write("RawData"); //copy raw data tree to file (if it exists)
   GetGeneData()->Write("GeneData"); //copy pulser & laser (gene) tree to file
   //add file to repository
   // get dataset to which we must associate new run
   KVDataSet* OutputDataset =
      gDataRepositoryManager->GetDataSet(
         gDataSet->GetDataSetEnv("Identification1.DataAnalysisTask.OutputRepository", gDataRepository->GetName()),
         gDataSet->GetName() );
         
   OutputDataset->CommitRunfile("ident", gIndra->GetCurrentRunNumber(),
                           fIdentFile);
   fIdentFile = 0;
   fIdentTree = 0;
}

//_____________________________________
void KVReconIdent::EndAnalysis(void)
{
}
