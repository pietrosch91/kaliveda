#define INDRAnalysis_cxx
#include "INDRAnalysis.h"
#include "TStyle.h"
#include "Riostream.h"
#include "KVINDRAReconEvent.h"
#include "KVINDRAReconNuc.h"
#include "KVChIo.h"
#include "KVSilicon.h"
#include "KVGroup.h"
#include "TTree.h"
#include "TFile.h"
#include "KVBatchSystem.h"

ClassImp(INDRAnalysis)

void INDRAnalysis::InitAnalysis()
{
   // Function called before starting the event loop.
   ///////////////////////////////////////////////////////////////
   //DEFINE HISTOGRAMS, NTUPLES, TTREES ETC. HERE               //
   ///////////////////////////////////////////////////////////////
//       
//    t__chiosi = new TTree("t__chiosi","ChIo-Si(300) ID matrices in MeV");
//    t__chiosi->Branch("E_si", &E_si, "E_si/F");
//    t__chiosi->Branch("E_chio", &E_chio, "E_chio/F");
//    t__chiosi->Branch("Ring_si", &Ring_si, "Ring_si/I");
   t__chiosi = new TTree("t__chiosi","ChIo-Si(300) ID matrices in MeV");
   t__chiosi->Branch("E_si", &E_si, "E_si/F");
   t__chiosi->Branch("E_chio", &E_chio, "E_chio/F");
   t__chiosi->Branch("Ring_si", &Ring_si, "Ring_si/I");
}

void INDRAnalysis::InitRun()
{
}

Bool_t INDRAnalysis::Analysis()
{
   // Processing function. This function is called
   // to process an event. 
   ///////////////////////////////////////////////
   //  TREATMENT OF SELECTED EVENTS             //
   //   Here you fill histos, ntuples, etc.     //
   ///////////////////////////////////////////////
//    
//    KVINDRAReconNuc *part = 0;
//    while( (part = GetEvent()->GetNextParticle()) ){
// 		   if(part->StoppedInSi() && part->GetStatus()==0){
// 			   if(part->GetChIo()){
// 				   if( part->GetSi()->GetThickness() == 300.){
// 					   Ring_si = part->GetRingNumber();
// 					   E_si = part->GetEnergySi();
// 					   E_chio = part->GetEnergyChIo();
// 					   t__chiosi->Fill();
// 				   }
// 			   }
// 		   }
//    }
//    
//    return kTRUE;
   KVINDRAReconNuc *part = 0;
   while( (part = GetEvent()->GetNextParticle()) ){
		   if(part->StoppedInSi() && part->GetStatus()==0){
			   if(part->GetChIo()){
				   if( part->GetSi()->GetThickness() == 300.){
					   Ring_si = part->GetRingNumber();
					   E_si = part->GetEnergySi();
					   E_chio = part->GetEnergyChIo();
					   t__chiosi->Fill();
				   }
			   }
		   }
   }
   
   return kTRUE;
}

void INDRAnalysis::EndRun()
{
}

void INDRAnalysis::EndAnalysis()
{
   // Function called at the end of the event loop.
//
	///////////////////////////////////////////////////////
	//  SAVE HISTOS, NTUPLES ETC. IN FILE HERE  //
	///////////////////////////////////////////////////////
// 	
//     TFile* file;
//     if( gBatchSystem ){ //analysis running as batch job
//       file = new TFile( Form("%s.root", gBatchSystem->GetJobName()) , "recreate");
//     } else { //interactive analysis
//        file = new TFile("INDRAnalysis.root", "recreate");
//     }
//     
//     //write TTree in file
//     file->cd();	
//     t__chiosi->Write();
// 	
//     //close file
//     delete file;
	
	
    TFile* file;
    if( gBatchSystem ){ //analysis running as batch job
      file = new TFile( Form("%s.root", gBatchSystem->GetJobName()) , "recreate");
    } else { //interactive analysis
       file = new TFile("INDRAnalysis.root", "recreate");
    }
    
    //write TTree in file
    file->cd();	
    t__chiosi->Write();
	
    //close file
    delete file;
}
