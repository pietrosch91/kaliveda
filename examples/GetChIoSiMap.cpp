/*
$Id: GetChIoSiMap.cpp,v 1.3 2008/01/24 09:53:24 franklan Exp $
$Revision: 1.3 $
$Date: 2008/01/24 09:53:24 $
*/

//Created by KVClassFactory on Mon Jun 25 17:08:35 2007
//Author: franklan

#include "GetChIoSiMap.h"
#include "KVBatchSystem.h"
#include "KVINDRAReconNuc.h"
#include "KVSilicon.h"
#include "KVCsI.h"
#include "KVChIo.h"

ClassImp(GetChIoSiMap)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>GetChIoSiMap</h2>
<h4>User raw data analysis class</h4>
Example of raw data analysis script, used to fill a TTree with the raw data necessary
to draw and test ChIo-Si identification grids
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

GetChIoSiMap::GetChIoSiMap()
{
   //Default constructor
}

GetChIoSiMap::~GetChIoSiMap()
{
   //Destructor
}


//________________________________________________________________
void GetChIoSiMap::InitAnalysis () 
{
   //Initialisation of e.g. histograms, performed once at beginning of analysis
	
   //Create ROOT file containing tree with data
   if( gBatchSystem ){ //running in batch mode
      file = new TFile( Form("%s.root", gBatchSystem->GetJobName()), "recreate" );
   } else { //interactive
	   file = new TFile("GetChIoSiMap.root","recreate");
   }
	tree=new TTree("chio_si_tree","ChIo-Si id maps");
	tree->Branch("ring", &ring, "ring/b");
	tree->Branch("module", &module, "module/b");
	tree->Branch("de_chio", &de_chio, "de_chio/F");
	tree->Branch("e_si", &e_si, "e_si/F");
	tree->Branch("csir", &csir, "csir/S");
	tree->Branch("run", &run, "run/s");
	tree->Branch("si150", &si150, "si150/O");
	
	//create object used to reconstruct events
	rec_ev=new KVINDRAReconEvent;
}


//________________________________________________________________
void GetChIoSiMap::InitRun () 
{
   //Initialisation performed at beginning of each run
   //  Int_t fRunNumber contains current run number
}


//________________________________________________________________
Bool_t GetChIoSiMap::Analysis () 
{
   //Analysis method called for each event
   //  Long64_t fEventNumber contains current event number
   //  KVINDRATriggerInfo* fTrig contains informations on INDRA trigger for event
   //  KVDetectorEvent* fDetEv gives list of hit groups for current event
   //  Processing will stop if this method returns kFALSE
   
	if( !fTrig->IsPhysics() || !fDetEv ) return kTRUE; //check state of trigger for event, exclude 'generateur' events
		
	rec_ev->ReconstructEvent( fDetEv ); //reconstruct particles from detector energy losses
	
	KVINDRAReconNuc* particle;
	while ( (particle = rec_ev->GetNextParticle()) ){ //loop over reconstructed particles
		
		//only particles passing through a ChIo & a Si, and with no other particle in the same group
		if(particle->GetSi()&&particle->GetChIo()&&(!particle->GetStatus())){
			
			ring=particle->GetRingNumber(); module = particle->GetModuleNumber();
			
			si150 = ( particle->GetSi()->GetThickness() < 200 );
			
			KVIDTelescope* chiosi = (KVIDTelescope*)particle->GetIDTelescopes()->FindObjectByType("CI_SI");
			if(chiosi){
				de_chio = chiosi->GetIDMapY(); e_si = chiosi->GetIDMapX();
			} else {
				de_chio = -100.; e_si = -100.;
			}
			if(particle->GetCsI()){
				csir = particle->GetCsI()->GetACQParam("R")->GetCoderData();
			} else {
				csir = -1;
			}
			
			tree->Fill();
		}
	}
	
	rec_ev->Clear();

   return kTRUE;
}


//________________________________________________________________
void GetChIoSiMap::EndRun () 
{
   //Method called at end of each run
}


//________________________________________________________________
void GetChIoSiMap::EndAnalysis () 
{
   //Method called at end of analysis: save/display histograms etc.
   
	//write file to disk
	file->Write();
	delete file;
	
	delete rec_ev;
}

