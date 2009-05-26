#define AnalyseCamp1_cxx
#include "AnalyseCamp1.h"
#include "TStyle.h"
#include "Riostream.h"
#include "KVINDRA.h"
#include "KVINDRAReconEvent.h"
#include "KVINDRAReconNuc.h"
#include "KVChIo.h"
#include "KV2Body.h"
#include "KVSilicon.h"
#include "KVZmax.h"
#include "KVTensPCM.h"
#include "KVBatchSystem.h"

ClassImp(AnalyseCamp1)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>AnalyseCamp1</h2>
<h4>Simple analysis class with a few global variables</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void AnalyseCamp1::InitAnalysis()
{
// 	Function called before starting the event loop.
// 	///////////////////////////////////////////////////////////////
// 	//DEFINE HISTOGRAMS, TTREES, GLOBAL VARIABLES ETC. HERE      //
// 	///////////////////////////////////////////////////////////////
// 	
// 	//define the global variables necessary for the analysis
// 	AddGV( "KVZmax", "Zmax" );
// 	AddGV( "KVTensPCM","tensor" );
// 
// 	//create file and tree for results of analysis
//    if( gBatchSystem ){  //analysis running as batch job
//       tree_file = new TFile( Form("%s.root", gBatchSystem->GetJobName()) ,"RECREATE");
//    }  
//    else {   //interactive analysis
//    	tree_file = new TFile("AnalyseCamp1.root","RECREATE");
//    }
// 	
//    	t__data = new TTree("t__data","Xe+Sn 50AMeV codes 2-4 and ecodes 1&2");
//    	t__data->Branch("mult", &Mult, "Mult/I");
//    	t__data->Branch("Zmax", &Zmax, "Zmax/I");
//    	t__data->Branch("QP_Estar", &Excit, "Excit/F");
//    	t__data->Branch("QP_Vpar", &VQProj, "VQProj/F");
//    	t__data->Branch("QP_Theta", &Theta_QP, "Theta_QP/F");
//    	t__data->Branch("QP_Z", &ZQProj, "ZQProj/I");
//    	t__data->Branch("Theta_flow", &Theta_flot, "Theta_flot/F");
//    	t__data->Branch("Sphericity", &Sphericity, "Sphericity/F");
//    	//autosave TTree every 1Mb
//    	t__data->SetAutoSave(1000000);
// 	
// 	//create bidims for alpha particle vper-vpar
// 	if(!Valph_lab) Valph_lab = new TH2F("Valph_lab","V_{#perp} - V_{//} #alpha lab",200,-5.,15.,200,-10.,10.);
// 	if(!Valph_cm) Valph_cm = new TH2F("Valph_cm","V_{#perp} - V_{//} #alpha cm",200,-10.,10.,200,-10.,10.);
// 	if(!Valph_qp) Valph_qp = new TH2F("Valph_qp","V_{#perp} - V_{//} #alpha qp",200,-15.,5.,200,-10.,10.);

	//define the global variables necessary for the analysis
 	AddGV( "KVZmax", "Zmax" );
 	AddGV( "KVTensPCM","tensor" );

	//create file and tree for results of analysis
   if( gBatchSystem ){  //analysis running as batch job
      tree_file = new TFile( Form("%s.root", gBatchSystem->GetJobName()) ,"RECREATE");
   }  
   else {   //interactive analysis
   	tree_file = new TFile("AnalyseCamp1.root","RECREATE");
   }
   
   	t__data = new TTree("t__data","Xe+Sn 50AMeV codes 2-4 and ecodes 1&2");
   	t__data->Branch("mult", &Mult, "Mult/I");
   	t__data->Branch("Zmax", &Zmax, "Zmax/I");
   	t__data->Branch("QP_Estar", &Excit, "Excit/F");
   	t__data->Branch("QP_Vpar", &VQProj, "VQProj/F");
   	t__data->Branch("QP_Theta", &Theta_QP, "Theta_QP/F");
   	t__data->Branch("QP_Z", &ZQProj, "ZQProj/I");
   	t__data->Branch("Theta_flow", &Theta_flot, "Theta_flot/F");
   	t__data->Branch("Sphericity", &Sphericity, "Sphericity/F");
   	//autosave TTree every 1Mb
   	t__data->SetAutoSave(1000000);
	
	//create bidims for alpha particle vper-vpar
	if(!Valph_lab) Valph_lab = new TH2F("Valph_lab","V_{#perp} - V_{//} #alpha lab",200,-5.,15.,200,-10.,10.);
	if(!Valph_cm) Valph_cm = new TH2F("Valph_cm","V_{#perp} - V_{//} #alpha cm",200,-10.,10.,200,-10.,10.);
	if(!Valph_qp) Valph_qp = new TH2F("Valph_qp","V_{#perp} - V_{//} #alpha qp",200,-15.,5.,200,-10.,10.);
}

//___________________________________________________________________________//

void AnalyseCamp1::InitRun()
{
	//Called at beginning of each new run
	//Set e.g. code mask for good particles here (has to be reset at start
	//of each new run).
// 
// 	//We keep ID codes 2-4 & 6 and E codes 1 & 2
//    	GetEvent()->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);
//    	GetEvent()->AcceptECodes(kECode1 | kECode2);

	//We keep ID codes 2-4 & 6 and E codes 1 & 2
   	GetEvent()->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);
   	GetEvent()->AcceptECodes(kECode1 | kECode2);
}

//___________________________________________________________________________//

Bool_t AnalyseCamp1::Analysis()
{
   // Processing function. This function is called
   // to process an event
   ///////////////////////////////////////////////
   //  TREATMENT OF SELECTED EVENTS             //
   //   Here you fill histos, ntuples, etc.     //
   ///////////////////////////////////////////////
// 	//keep events with at least Mtrig particles with correct codes
// 	if(!GetEvent()->IsOK()) return kTRUE;
// 	
// 	//We could also use (same):
// 	//if(GetEvent()->GetMult("ok") < gIndra->GetTrigger()) return kTRUE;
// 	
//    Mult = GetEvent()->GetMult("ok");			//multiplicity of particles with correct codes
// 
//    Zmax = (int)GetGV("Zmax")->GetValue();	//Z of largest nucleus with correct codes
//       
//    KVNucleus QProj;		//for source reconstruction/calorimetry
//    
//    KVINDRAReconNuc *part;
//    
//    //c.m. velocity of system being studied 
//    Double_t Vcm = gIndra->GetSystem()->GetKinematics()->GetCMVelocity().z();
//    
//    while( (part = GetEvent()->GetNextParticle("ok")) ){	//loop over particles with correct codes
// 	   
// 		   //for calorimetry, take all products with v// > vcm and
// 	   	   //add them to the QP source
// 		   if( part->GetVpar() > Vcm ){
// 			   
// 			   QProj += (*part);
// 				part->AddGroup("QP"); //label particle as belonging to QP
// 			   
// 		   }
// 		   //label alpha particles
// 		   if( part->GetZ()==2 && part->GetA()==4 ){
// 			   
// 			   part->AddGroup("alpha");
// 			   
// 		   }
// 		   
//    }
//    
//    //"Quasiprojectile" characteristics
//    Excit = QProj.GetExcitEnergy();
//    VQProj = QProj.GetVpar();
//    ZQProj = QProj.GetZ();
//    Theta_QP = QProj.GetTheta();
//    
//    //Informations from event tensor
//    KVTensPCM* tens = (KVTensPCM*)GetGV("tensor");
//    Theta_flot = tens->GetValue();
//    Sphericity = tens->GetValue("Sphericite");
//    
//    t__data->Fill();
//    
//    //Velocity diagrams for alpha particles
//    //The reaction CM frame is defined by default. (GetFrame("CM"))
//    //We are going to define also the QP rest frame.
//    GetEvent()->SetFrame("QP", QProj.GetV());
//    
//    while( (part = GetEvent()->GetNextParticle("alpha")) ){//loop over alpha particles
// 
// 	   //vper-vpar in the lab.
// 	   //notice that GetVperp() automatically returns a -ve value if
// 	   //the y-component of velocity is negative	   
// 	   if(TMath::Abs(part->GetVperp())>0.) Valph_lab->Fill( part->GetVpar(), part->GetVperp(), 1./TMath::Abs(part->GetVperp()) );
// 	   
// 	   //in the reaction CM frame
// 	   if(TMath::Abs(part->GetFrame("CM")->GetVperp())>0.) Valph_cm->Fill( part->GetFrame("CM")->GetVpar(), part->GetFrame("CM")->GetVperp(), 1./TMath::Abs(part->GetFrame("CM")->GetVperp()) );
// 	   
// 	   //in the QP frame
// 		//we only include alpha particles belonging to the QP
// 	   if(part->BelongsToGroup("QP") && TMath::Abs(part->GetFrame("QP")->GetVperp())>0.)
// 			Valph_qp->Fill( part->GetFrame("QP")->GetVpar(), part->GetFrame("QP")->GetVperp(), 1./TMath::Abs(part->GetFrame("QP")->GetVperp()) );
//    } 
// 
//    return kTRUE;
	
	//keep events with at least Mtrig particles with correct codes
	if(!GetEvent()->IsOK()) return kTRUE;
	
	//We could also use (same):
	//if(GetEvent()->GetMult("ok") < gIndra->GetTrigger()) return kTRUE;
	
   Mult = GetEvent()->GetMult("ok");			//multiplicity of particles with correct codes

   Zmax = (int)GetGV("Zmax")->GetValue();	//Z of largest nucleus with correct codes
      
   KVNucleus QProj;		//for source reconstruction/calorimetry
   
   KVINDRAReconNuc *part;
   
   //c.m. velocity of system being studied 
   Double_t Vcm = gIndra->GetSystem()->GetKinematics()->GetCMVelocity().z();
   
   while( (part = GetEvent()->GetNextParticle("ok")) ){	//loop over particles with correct codes
	   
		   //for calorimetry, take all products with v// > vcm and
	   	   //add them to the QP source
		   if( part->GetVpar() > Vcm ){
			   
			   QProj += (*part);
				part->AddGroup("QP"); //label particle as belonging to QP
			   
		   }
		   //label alpha particles
		   if( part->GetZ()==2 && part->GetA()==4 ){
			   
			   part->AddGroup("alpha");
			   
		   }
		   
   }
   
   //"Quasiprojectile" characteristics
   Excit = QProj.GetExcitEnergy();
   VQProj = QProj.GetVpar();
   ZQProj = QProj.GetZ();
   Theta_QP = QProj.GetTheta();
   
   //Informations from event tensor
   KVTensPCM* tens = (KVTensPCM*)GetGV("tensor");
   Theta_flot = tens->GetValue();
   Sphericity = tens->GetValue("Sphericite");
   
   t__data->Fill();
   
   //Velocity diagrams for alpha particles
   //The reaction CM frame is defined by default. (GetFrame("CM"))
   //We are going to define also the QP rest frame.
   GetEvent()->SetFrame("QP", QProj.GetV());
   
   while( (part = GetEvent()->GetNextParticle("alpha")) ){//loop over alpha particles

	   //vper-vpar in the lab.
	   //notice that GetVperp() automatically returns a -ve value if
	   //the y-component of velocity is negative	   
	   if(TMath::Abs(part->GetVperp())>0.) Valph_lab->Fill( part->GetVpar(), part->GetVperp(), 1./TMath::Abs(part->GetVperp()) );
	   
	   //in the reaction CM frame
	   if(TMath::Abs(part->GetFrame("CM")->GetVperp())>0.) Valph_cm->Fill( part->GetFrame("CM")->GetVpar(), part->GetFrame("CM")->GetVperp(), 1./TMath::Abs(part->GetFrame("CM")->GetVperp()) );
	   
	   //in the QP frame
		//we only include alpha particles belonging to the QP
	   if(part->BelongsToGroup("QP") && TMath::Abs(part->GetFrame("QP")->GetVperp())>0.)
			Valph_qp->Fill( part->GetFrame("QP")->GetVpar(), part->GetFrame("QP")->GetVperp(), 1./TMath::Abs(part->GetFrame("QP")->GetVperp()) );
   } 

   return kTRUE;
}

//___________________________________________________________________________//

void AnalyseCamp1::EndRun()
{
	//Function called at end of each run
	Info("EndRun","");
}

//___________________________________________________________________________//

void AnalyseCamp1::EndAnalysis()
{
   // Function called at the end of the event loop.
	///////////////////////////////////////////////////////
	//  SAVE HISTOS, NTUPLES ETC. IN ROOT FILE HERE  //
	///////////////////////////////////////////////////////
// 	
// 	//write contents of file on disk (tree + histograms) and close file
//     tree_file->Write();
//     delete tree_file;
// 
	//write contents of file on disk (tree + histograms) and close file
    tree_file->Write();
    delete tree_file;
}

//___________________________________________________________________________//

