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
   	runFlag = -1;
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

//ReadModuleMap(); 	//"module_map.dat"       
//fAnalyseV->SetModuleMap(module_map);
LoadGrids();
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
   
    kvd_si = new KVSiliconVamos(530.*KVUnits::um);   
    gap = new KVDetector("C4H10", 136.5*KVUnits::mm);
    kvd_csi = new KVCsIVamos(1.);
    
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
      
      fLogV->Log<<"======"<<endl;
      fLogV->Log<<"Mult. indra	: "<<GetEvent()->GetMult()<<endl;
      fLogV->Log<<"======"<<endl;   	
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
   fAnalyseV->SetRunFlag(-5);	
   	delete fLogV;
}

Int_t KVIVReconIdent::SetRunFlag(Int_t flag){
 	runFlag = flag;
	return 0;
}

Int_t KVIVReconIdent::GetRunFlag(){
return runFlag;
}

Int_t KVIVReconIdent::ReadModuleMap(){	//const Char_t *map	

    //printf("Reading the module map...\n");

ifstream f;
if(!gDataSet->OpenDataSetFile("module_map.dat",f))
  {
     cout << "Could not open module_map.dat file !!!" << endl;
     exit(-1);
  }
  
else 
  {

    //printf("Initialising null module_map...\n");
    for(Int_t i=0; i<18; i++){
        for(Int_t j=0; j<80; j++){
            module_map[i][j] = "null";
        }
    }

    //printf("Reading the data...\n");

    Int_t si_num = 0;
    Int_t csi_num = 0;

    string module_name;
    string line;

    //ifstream f(map);

    //if (!f.good()){
    //    cout << "Error: Module map file not good (" << map << ")" << endl;
    //    exit(-1);
    //}

    while(getline(f, line)){
        if(line[0] != '#'){
            if(line[0] == '!'){
                si_num++;
            }else{
                istringstream s (line);
                s >> module_name >> csi_num;	//csi_num is real number (1-80) and si_num is (0-17)
                //printf("Processing Si(%02i) CsI(%02i)...\n", si_num+1, csi_num); // +1 on si to get real
                module_map[si_num][csi_num-1] = module_name;
            }
        }
    }
}

    f.close();
    //printf("Module map has been read successfully\n");

    return 0;
}

Bool_t KVIVReconIdent::LoadGrids(){

    const char *grid_map= "grid_vamos.dat";
    const char *grid_map2= "SI_CSI_C4-9.dat";
    
    printf("Attempting to load grids from %s...\n", grid_map);
    printf("Attempting to load grids from %s...\n", grid_map2);
    
    if(gIDGridManager == 0){
        printf("gIDGridManager not running\n");
        return 1;
    }

    char ds_path[256];
    sprintf(ds_path, "null");
    char ds_path2[256];
    sprintf(ds_path2, "null");
    
    if(gDataSet->GetDataSetDir() == 0){
        printf("Failed to retrieve data set dir string\n");  
        return 1;
    }

    sprintf(ds_path, "%s/%s", gDataSet->GetDataSetDir(), grid_map);
    sprintf(ds_path2, "%s/%s", gDataSet->GetDataSetDir(), grid_map2);
    
    printf("Reading grid map: %s\n", grid_map);    
    printf("Reading grid map: %s\n", grid_map2);
    if(gIDGridManager->ReadAsciiFile(ds_path) != 0 && gIDGridManager->ReadAsciiFile(ds_path2) != 0){
        printf("All Grids from Indra and Vamos loaded\n");
    }else{
        return 1;
    }

    return 0;
}

