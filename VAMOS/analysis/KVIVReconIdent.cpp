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


   //Chargement des grilles de E503	
   LoadGrids_e503(); 	
   
   //gIndra->SetIdentifications();
   //gIndra->InitializeIDTelescopes();   
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
   
   //Chargement des cuts graphiques, détecteurs
   Init_e503();

}

//_____________________________________
Bool_t KVIVReconIdent::Analysis(void)
{
   //Identification of INDRA events and ident/reconstruction of VAMOS data
   
   //fLogV->Log<<"-----------"<<endl;
   //fLogV->Log<<"ev num vamos="<<event<<endl;
   //fLogV->Log<<"GetMult		"<<GetEvent()->GetMult()<<endl;
   //fLogV->Log<<"-----------"<<endl;

// Identification of INDRA events
      
   fEventNumber = GetEvent()->GetNumber();
   if (GetEvent()->GetMult() > 0) {

      GetEvent()->IdentifyEvent();
      GetEvent()->CalibrateEvent();      
   }


// Ident/Reconstruction of VAMOS data
   
   fAnalyseV->Treat();   		
   fIdentTree->Fill();
   fAnalyseV->FillHistograms();
    	
    //fLogV->Log<<"___________________________________________"<<endl;
    //fLogV->Log<<"___________________________________________"<<endl;
      
       return kTRUE;
}

//_____________________________________
void KVIVReconIdent::EndAnalysis(void)
{
	
   	fLogV->Close();
}

void KVIVReconIdent::Init_e503(){

Int_t sys;

if(gIndra->GetCurrentRunNumber()>321 && gIndra->GetCurrentRunNumber()<379)
{
	sys=4840;
}
if(gIndra->GetCurrentRunNumber()>378 && gIndra->GetCurrentRunNumber()<425)
{
	sys=4848;
}
if(gIndra->GetCurrentRunNumber()>510 && gIndra->GetCurrentRunNumber()<551)
{
	sys=4040;
}
if((gIndra->GetCurrentRunNumber()>454 && gIndra->GetCurrentRunNumber()<510) || (gIndra->GetCurrentRunNumber()>582 && gIndra->GetCurrentRunNumber()<592))
{
	sys=4048;
}
fLogV->Log<<"sys : "<<sys<<endl;

// VAMOS Part : Q Graphical Cuts

//==========================================================================
// Identification Si-CsI
fcoup = new TFile(Form("$KVROOT/KVFiles/INDRA_e503/cuts_brho_%d.root",sys));
list = new TList();
	if(fcoup->IsZombie()==0){
		q20=NULL;
		q19=NULL;
		q18=NULL;
		q17=NULL;
		q16=NULL;
		q15=NULL;
		q14=NULL;
		q13=NULL;
		q12=NULL;
		q11=NULL;
		q10=NULL;
		q9=NULL;
		q8=NULL;
		q7=NULL;
		q6=NULL;
		q5=NULL;
		
		if(fcoup->Read("q20")!=0 ) q20=(TCutG *)fcoup->Get("q20");
		if(fcoup->Read("q19")!=0 ) q19=(TCutG *)fcoup->Get("q19");
		if(fcoup->Read("q18")!=0 ) q18=(TCutG *)fcoup->Get("q18");
		if(fcoup->Read("q17")!=0 ) q17=(TCutG *)fcoup->Get("q17");
		if(fcoup->Read("q16")!=0 ) q16=(TCutG *)fcoup->Get("q16");  
		if(fcoup->Read("q15")!=0 ) q15=(TCutG *)fcoup->Get("q15");
		if(fcoup->Read("q14")!=0 ) q14=(TCutG *)fcoup->Get("q14");
		if(fcoup->Read("q13")!=0 ) q13=(TCutG *)fcoup->Get("q13");
		if(fcoup->Read("q12")!=0 ) q12=(TCutG *)fcoup->Get("q12");
		if(fcoup->Read("q11")!=0 ) q11=(TCutG *)fcoup->Get("q11");
		if(fcoup->Read("q10")!=0 ) q10=(TCutG *)fcoup->Get("q10");
		if(fcoup->Read("q9")!=0 ) q9=(TCutG *)fcoup->Get("q9");
		if(fcoup->Read("q8")!=0 ) q8=(TCutG *)fcoup->Get("q8"); 
		if(fcoup->Read("q7")!=0 ) q7=(TCutG *)fcoup->Get("q7");
		if(fcoup->Read("q6")!=0 ) q6=(TCutG *)fcoup->Get("q6");
		if(fcoup->Read("q5")!=0 ) q5=(TCutG *)fcoup->Get("q5");
		
		if(q20!=NULL)list->Add(q20);
		if(q19!=NULL)list->Add(q19);			  
		if(q18!=NULL)list->Add(q18);
		if(q17!=NULL)list->Add(q17);
		if(q16!=NULL)list->Add(q16);			
		if(q15!=NULL)list->Add(q15);
		if(q14!=NULL)list->Add(q14);
		if(q13!=NULL)list->Add(q13);			
		if(q12!=NULL)list->Add(q12);
		if(q11!=NULL)list->Add(q11);
		if(q10!=NULL)list->Add(q10);			
		if(q9!=NULL)list->Add(q9);
		if(q8!=NULL)list->Add(q8);
		if(q7!=NULL)list->Add(q7);
		if(q6!=NULL)list->Add(q6);  			
		if(q5!=NULL)list->Add(q5);  					
				
		fcoup->Close();
	}
	else{
		fLogV->Log<<"Error : not reading cuts (Si-CsI)..."<<endl;
	}

fAnalyseV->SetFileCut(list);
//==========================================================================
fcoup2 = new TFile(Form("$KVROOT/KVFiles/INDRA_e503/cuts_brho_chiosi_%d.root",sys));
list2 = new TList();
	if(fcoup2->IsZombie()==0){		
		q20cs=NULL;
		q19cs=NULL;
		q18cs=NULL;
		q17cs=NULL;
		q16cs=NULL;
		q15cs=NULL;
		q14cs=NULL;
		q13cs=NULL;
		q12cs=NULL;
		q11cs=NULL;
		q10cs=NULL;
		q9cs=NULL;
		q8cs=NULL;
		q7cs=NULL;
		q6cs=NULL;
		q5cs=NULL;
		
		if(fcoup2->Read("q20")!=0 ) q20cs=(TCutG *)fcoup2->Get("q20");
		if(fcoup2->Read("q19")!=0 ) q19cs=(TCutG *)fcoup2->Get("q19");
		if(fcoup2->Read("q18")!=0 ) q18cs=(TCutG *)fcoup2->Get("q18");
		if(fcoup2->Read("q17")!=0 ) q17cs=(TCutG *)fcoup2->Get("q17");
		if(fcoup2->Read("q16")!=0 ) q16cs=(TCutG *)fcoup2->Get("q16");  
		if(fcoup2->Read("q15")!=0 ) q15cs=(TCutG *)fcoup2->Get("q15");
		if(fcoup2->Read("q14")!=0 ) q14cs=(TCutG *)fcoup2->Get("q14");
		if(fcoup2->Read("q13")!=0 ) q13cs=(TCutG *)fcoup2->Get("q13");
		if(fcoup2->Read("q12")!=0 ) q12cs=(TCutG *)fcoup2->Get("q12");
		if(fcoup2->Read("q11")!=0 ) q11cs=(TCutG *)fcoup2->Get("q11");
		if(fcoup2->Read("q10")!=0 ) q10cs=(TCutG *)fcoup2->Get("q10");
		if(fcoup2->Read("q9")!=0 ) q9cs=(TCutG *)fcoup2->Get("q9");
		if(fcoup2->Read("q8")!=0 ) q8cs=(TCutG *)fcoup2->Get("q8");
		if(fcoup2->Read("q7")!=0 ) q7cs=(TCutG *)fcoup2->Get("q7"); 
		
		if(q20cs!=NULL)list2->Add(q20cs);
		if(q19cs!=NULL)list2->Add(q19cs);				
		if(q18cs!=NULL)list2->Add(q18cs);
		if(q17cs!=NULL)list2->Add(q17cs);
		if(q16cs!=NULL)list2->Add(q16cs);				
		if(q15cs!=NULL)list2->Add(q15cs);
		if(q14cs!=NULL)list2->Add(q14cs);
		if(q13cs!=NULL)list2->Add(q13cs);				
		if(q12cs!=NULL)list2->Add(q12cs);
		if(q11cs!=NULL)list2->Add(q11cs);
		if(q10cs!=NULL)list2->Add(q10cs);				
		if(q9cs!=NULL)list2->Add(q9cs);
		if(q8cs!=NULL)list2->Add(q8cs);	   
		if(q7cs!=NULL)list2->Add(q7cs);						 
				
		fcoup2->Close();
	}
	else{
		fLogV->Log<<"not reading cuts (Chio-Si)..."<<endl;
	}

fAnalyseV->SetFileCutChioSi(list2);
//==========================================================================

//==========================================================================
fcoup3 = new TFile(Form("$KVROOT/KVFiles/INDRA_e503/cuts_brho_sitof_%d.root",sys));
list3 = new TList();
	if(fcoup3->IsZombie()==0){		
		q20st=NULL;
		q19st=NULL;
		q18st=NULL;
		q17st=NULL;
		q16st=NULL;
		q15st=NULL;
		q14st=NULL;
		q13st=NULL;
		q12st=NULL;
		q11st=NULL;
		q10st=NULL;
		q9st=NULL;
		q8st=NULL;
		q7st=NULL;
		q6st=NULL;
		q5st=NULL;
		
		if(fcoup3->Read("q20")!=0 ) q20st=(TCutG *)fcoup3->Get("q20");
		if(fcoup3->Read("q19")!=0 ) q19st=(TCutG *)fcoup3->Get("q19");
		if(fcoup3->Read("q18")!=0 ) q18st=(TCutG *)fcoup3->Get("q18");
		if(fcoup3->Read("q17")!=0 ) q17st=(TCutG *)fcoup3->Get("q17");
		if(fcoup3->Read("q16")!=0 ) q16st=(TCutG *)fcoup3->Get("q16");  
		if(fcoup3->Read("q15")!=0 ) q15st=(TCutG *)fcoup3->Get("q15");
		if(fcoup3->Read("q14")!=0 ) q14st=(TCutG *)fcoup3->Get("q14");
		if(fcoup3->Read("q13")!=0 ) q13st=(TCutG *)fcoup3->Get("q13");
		if(fcoup3->Read("q12")!=0 ) q12st=(TCutG *)fcoup3->Get("q12");
		if(fcoup3->Read("q11")!=0 ) q11st=(TCutG *)fcoup3->Get("q11");
		if(fcoup3->Read("q10")!=0 ) q10st=(TCutG *)fcoup3->Get("q10");
		if(fcoup3->Read("q9")!=0 ) q9st=(TCutG *)fcoup3->Get("q9");
		if(fcoup3->Read("q8")!=0 ) q8st=(TCutG *)fcoup3->Get("q8");
		if(fcoup3->Read("q7")!=0 ) q7st=(TCutG *)fcoup3->Get("q7"); 
		if(fcoup3->Read("q6")!=0 ) q6st=(TCutG *)fcoup3->Get("q6");	       
		
		if(q20st!=NULL)list3->Add(q20st);
		if(q19st!=NULL)list3->Add(q19st);			       
		if(q18st!=NULL)list3->Add(q18st);
		if(q17st!=NULL)list3->Add(q17st);
		if(q16st!=NULL)list3->Add(q16st);			       
		if(q15st!=NULL)list3->Add(q15st);
		if(q14st!=NULL)list3->Add(q14st);
		if(q13st!=NULL)list3->Add(q13st);			       
		if(q12st!=NULL)list3->Add(q12st);
		if(q11st!=NULL)list3->Add(q11st);
		if(q10st!=NULL)list3->Add(q10st);			       
		if(q9st!=NULL)list3->Add(q9st);
		if(q8st!=NULL)list3->Add(q8st);    
		if(q7st!=NULL)list3->Add(q7st); 
		if(q6st!=NULL)list3->Add(q6st);  							
				
		fcoup3->Close();
	}
	else{
		fLogV->Log<<"not reading cuts (Si-Tof)..."<<endl;
	}

fAnalyseV->SetFileCutChioSi(list3);
//==========================================================================
    
//============================================
//Layers definition used inside Identificationv class
//Those layers will be used to add a correction to the total energy
		
    tgt = 0;
    tgt = new KVTarget("C",0.012); //mg/cm2

//=======================
//Drift Chamber #1    
    dcv1 = 0;    
    dcv1 = new KVDetector;

    // Entrance window 
	KVMaterial *win;
    win = new KVMaterial("Myl", 1.5*KVUnits::um);

    // Gas Layer
	KVMaterial *gas;
    gas = new KVMaterial("C4H10", 130.*KVUnits::mm);
    gas->SetPressure(18.*KVUnits::mbar);

    // Exit window
	KVMaterial *wout;    
    wout = new KVMaterial("Myl", 1.5*KVUnits::um);
    
    dcv1->AddAbsorber(win);
    dcv1->AddAbsorber(gas);
    dcv1->AddAbsorber(wout);    
//=======================

    sed = 0;
    sed = new KVMaterial("Myl", 0.9*KVUnits::um); 

//=======================
//Drift Chamber #2      
    dcv2 = 0;    
    dcv2 = new KVDetector;
    
    // Entrance window 
	KVMaterial *win2;
    win2 = new KVMaterial("Myl", 1.5*KVUnits::um);

    // Gas Layer
	KVMaterial *gas2;
    gas2 = new KVMaterial("C4H10", 130.*KVUnits::mm);
    gas2->SetPressure(18.*KVUnits::mbar);

    // Exit window
	KVMaterial *wout2;    
    wout2 = new KVMaterial("Myl", 1.5*KVUnits::um);    
    dcv2->AddAbsorber(win2);
    dcv2->AddAbsorber(gas2);
    dcv2->AddAbsorber(wout2);    
//=======================
    
//=======================
//Ionisation Chamber
    ic = 0;
    ic = new KVDetector;

    // Entrance window 
	KVMaterial *winic;
    winic = new KVMaterial("Myl", 1.5*KVUnits::um);

    // Gas Layer
	KVMaterial *dead;
    dead = new KVMaterial("C4H10", 20.68*KVUnits::mm);
    dead->SetPressure(40.*KVUnits::mbar);
    
	KVMaterial *active;    
    active = new KVMaterial("C4H10", 104.57*KVUnits::mm);
    active->SetPressure(40.*KVUnits::mbar);

    ic->AddAbsorber(winic);
    ic->AddAbsorber(dead);
    ic->AddAbsorber(active);
//=======================
       
    isogap1 = 0;
    isogap1 = new KVMaterial("C4H10", 12.78*KVUnits::mm);

    isogap1->SetPressure(40.*KVUnits::mbar);

    ssi = 0;
    ssi = new KVMaterial("Si", 530.*KVUnits::um);

    isogap2 = 0;
    isogap2 = new KVMaterial("C4H10", 136.5*KVUnits::mm);
    
    isogap2->SetPressure(40.*KVUnits::mbar);

    ccsi = 0;
    ccsi = new KVMaterial("CsI", 1.0*KVUnits::cm);    
//============================================
// VAMOS Part : Total Energy Correction
 
	
	fAnalyseV->SetTarget(tgt);
	fAnalyseV->SetDC1(dcv1);
	fAnalyseV->SetSed(sed);
	fAnalyseV->SetDC2(dcv2);
	fAnalyseV->SetIC(ic);
	fAnalyseV->SetGap1(isogap1);
	fAnalyseV->SetSi(ssi);
	fAnalyseV->SetGap2(isogap2);	   
	fAnalyseV->SetCsI(ccsi);
	
//============================================ 

    
//============================================    
    kvd_si = new KVDetector("Si",530.*KVUnits::um);	//define a Silicon detector of 500 microm thick
    kvd_csi = new KVDetector("CsI",1.);	            //define a CsI detector of 1 cm thick
    gap = new KVDetector("C4H10", 136.5*KVUnits::mm);
    gap->SetPressure(40.*KVUnits::mbar);
        
    fAnalyseV->SetTel1(kvd_si);    
    fAnalyseV->SetTel2(gap);	    
    fAnalyseV->SetTel3(kvd_csi);
//============================================

return;
}

Bool_t KVIVReconIdent::LoadGrids_e503(){

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
        printf("Grids Si-CsI from Vamos loaded\n");
    }else{
        return 1;
    }
//============================================================================    
    const char *grid_map2= "grid_chio_si_vamos.dat";
    
    printf("Attempting to load grids from %s...\n", grid_map2);
    
    if(gIDGridManager == 0){
        printf("gIDGridManager not running\n");
        return 1;
    }

    char ds_path2[256];
    sprintf(ds_path2, "null");
    
    if(gDataSet->GetDataSetDir() == 0){
        printf("Failed to retrieve data set dir string\n");  
        return 1;
    }

    sprintf(ds_path2, "%s/%s", gDataSet->GetDataSetDir(), grid_map2);
    
    printf("Reading grid map: %s\n", grid_map2);    
    if(gIDGridManager->ReadAsciiFile(ds_path2) != 0){
        printf("Grids Chio-Si from Vamos loaded\n");
    }else{
        return 1;
    }
//============================================================================    
    const char *grid_map3= "grid_si_tof_vamos.dat";
    
    printf("Attempting to load grids from %s...\n", grid_map3);
    
    if(gIDGridManager == 0){
        printf("gIDGridManager not running\n");
        return 1;
    }

    char ds_path3[256];
    sprintf(ds_path3, "null");
    
    if(gDataSet->GetDataSetDir() == 0){
        printf("Failed to retrieve data set dir string\n");  
        return 1;
    }

    sprintf(ds_path3, "%s/%s", gDataSet->GetDataSetDir(), grid_map3);
    
    printf("Reading grid map: %s\n", grid_map3);    
    if(gIDGridManager->ReadAsciiFile(ds_path3) != 0){
        printf("Grids Si-Tof from Vamos loaded\n");
    }else{
    	cout<<"Can't find Si-Tof Grids"<<endl;	
        return 1;
    }
    
    return 0;
    
}

