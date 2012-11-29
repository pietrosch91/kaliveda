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

   //LoadGrids();
   
   gIndra->SetIdentifications();
   gIndra->InitializeIDTelescopes();   
}

//_____________________________________
void KVIVReconIdent::InitRun(void)
{
   //Connect VAMOS branches in input and output trees  

   KVINDRAReconIdent::InitRun();
   fAnalyseV->OpenOutputTree(fIdentTree);
   fAnalyseV->outAttach();
   fAnalyseV->CreateHistograms();
   fAnalyseV->OpenInputTree(fChain->GetTree());
   fAnalyseV->inAttach();

   ifstream in;   
   Int_t run1;
   TString sline;
            
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
   }


// Ident/Reconstruction of VAMOS data
   
   //fAnalyseV->Treat();
   		
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

