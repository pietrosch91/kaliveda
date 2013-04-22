/*
$Id: KVIVReconIdent.cpp,v 1.3 2007/11/21 11:22:59 franklan Exp $
$Revision: 1.3 $
$Date: 2007/11/21 11:22:59 $
*/

//Created by KVClassFactory on Thu Jun  7 13:52:25 2007
//Author: John Frankland

#include "KVIVReconIdent.h"
#include "Analysisv.h"
#include "LogFile.h"
#include "KVBatchSystem.h"
#include "KVDataSet.h"

#include "Riostream.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include "KVFocalPlanVamos.h"

#include "KVReconstructedNucleus.h"
#include "KVINDRAReconNuc.h"
#include "KVIdentificationResult.h"
#include "KVCsI.h"
#include "KVSilicon.h"

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

KVIVReconIdent::KVIVReconIdent()
{
   //Default constructor
}

KVIVReconIdent::~KVIVReconIdent()
{
   //Destructor
}

void KVIVReconIdent::InitAnalysis(void)
{
   //Create VAMOS Analysisv object
   fLogV = new LogFile;
   if( gBatchSystem ){
   fLogV->Open( Form( "%s_vamos.log", gBatchSystem->GetJobName() ) );
   }
   else{ 
   fLogV->Open("Calibration_vamos.log");
   }
   //get VAMOS calibrator for current dataset
   fAnalyseV = Analysisv::NewAnalyser( gDataSet->GetName(), fLogV );

   LoadGrids();
   
   gIndra->SetIdentifications();
   gIndra->InitializeIDTelescopes();   
}

//_____________________________________
void KVIVReconIdent::InitRun(void)
{
   //Connect VAMOS branches in input and output trees  
   
   //fAnalyseV->SetRunFlag(0);

   KVINDRAReconIdent::InitRun();
   fAnalyseV->OpenOutputTree(fIdentTree);
   fAnalyseV->outAttach();
   fAnalyseV->CreateHistograms();
   fAnalyseV->OpenInputTree(fChain->GetTree());
   fAnalyseV->inAttach();

   ifstream in;   
   Int_t run1;
   TString sline;
   
    /*kvd_si = new KVSiliconVamos(530.*KVUnits::um);   
    gap = new KVDetector("C4H10", 136.5*KVUnits::mm);
    kvd_csi = new KVCsIVamos(1.);*/
    
    kvd_si = new KVDetector("Si",530.*KVUnits::um);   
    gap = new KVDetector("C4H10", 136.5*KVUnits::mm);
    kvd_csi = new KVDetector("CsI",1.*KVUnits::cm);
    
    gap->SetPressure(40.*KVUnits::mbar); 
    
    fAnalyseV->SetTel1(kvd_si);    
    fAnalyseV->SetTel2(gap);        
    fAnalyseV->SetTel3(kvd_csi);
            
    fIdentTree->Branch("M_INDRA",&M_INDRA,"M_INDRA/I");
    event=1;
}

//_____________________________________
Bool_t KVIVReconIdent::Analysis(void)
{
   //Identification of INDRA events and ident/reconstruction of VAMOS data
   
   //fLogV->Log<<"-----------"<<endl;
   fLogV->Log<<"ev num vamos="<<event<<endl;
   //fLogV->Log<<"GetMult		"<<GetEvent()->GetMult()<<endl;
   //fLogV->Log<<"-----------"<<endl;

// Identification of INDRA events
      
   fEventNumber = GetEvent()->GetNumber();
   if (GetEvent()->GetMult() > 0) {

      GetEvent()->IdentifyEvent();
      GetEvent()->CalibrateEvent();      
      
      /*fLogV->Log<<"======"<<endl;
      fLogV->Log<<"Mult. indra	: "<<GetEvent()->GetMult()<<endl;
      fLogV->Log<<"======"<<endl;*/   	
      M_INDRA = GetEvent()->GetMult();
      while(part = GetEvent()->GetNextParticle("ok")){
      
        /*fLogV->Log<<"ring indra	: "<<part->GetRingNumber()<<endl;
	fLogV->Log<<"module indra	: "<<part->GetModuleNumber()<<endl;
      	fLogV->Log<<"Z indra		: "<<part->GetZ()<<endl; 
	fLogV->Log<<"Telescope	: "<<part->GetIdentifyingTelescope()<<endl;
	fLogV->Log<<"Bool		: "<<part->IsIdentified()<<endl;*/				
	
      } 
   }


// Ident/Reconstruction of VAMOS data
   
   fAnalyseV->Treat();
   		
   fIdentTree->Fill();
   fAnalyseV->FillHistograms();
   
	event++;
    	
    fLogV->Log<<"___________________________________________"<<endl;
    fLogV->Log<<"___________________________________________"<<endl;
      
       return kTRUE;
}

//_____________________________________
void KVIVReconIdent::EndAnalysis(void)
{
	
   	fLogV->Close();
}


Bool_t KVIVReconIdent::LoadGrids(){

    const char *grid_map= "grid_vamos.dat";
    
    printf("Attempting to load grids from %s...\n", grid_map);
    
    if(gIDGridManager == 0){
        printf("gIDGridManager not running\n");
        return 1;
    }

    char ds_path[256];
    sprintf(ds_path, "null");
    
    if(gDataSet->GetDataSetDir() == 0){
        printf("Failed to retrieve data set dir string\n");  
        return 1;
    }

    sprintf(ds_path, "%s/%s", gDataSet->GetDataSetDir(), grid_map);
    
    printf("Reading grid map: %s\n", grid_map);    
    if(gIDGridManager->ReadAsciiFile(ds_path) != 0){
        printf("All Grids from Vamos loaded\n");
    }else{
        return 1;
    }

    return 0;
}

