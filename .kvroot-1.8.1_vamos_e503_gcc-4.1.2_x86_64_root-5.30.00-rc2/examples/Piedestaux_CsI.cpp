/*
$Id: Piedestaux_CsI.cpp,v 1.1 2007/06/25 15:58:41 franklan Exp $
$Revision: 1.1 $
$Date: 2007/06/25 15:58:41 $
*/

//Created by KVClassFactory on Mon Jun 25 17:09:12 2007
//Author: franklan

#include "Piedestaux_CsI.h"
#include <TROOT.h>
#include <KVINDRA.h>
#include <KVDetector.h>
#include <TH1.h>
#include <TF1.h>
#include <TList.h>
#include <KVACQParam.h>
#include <Riostream.h>
#include "TStyle.h"

ClassImp(Piedestaux_CsI)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Piedestaux_CsI</h2>
<h4>User raw data analysis class</h4>
Example of raw data analysis script, used to extract the CsI pedestals from the
appropriate pedestal run.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Piedestaux_CsI::Piedestaux_CsI()
{
   //Default constructor
}

Piedestaux_CsI::~Piedestaux_CsI()
{
   //Destructor
}


//________________________________________________________________
void Piedestaux_CsI::InitAnalysis () 
{
   //Initialisation of e.g. histograms, performed once at beginning of analysis
	if( gBatchSystem ){ // batch mode
      file = new TFile( Form( "%s.root", gBatchSystem->GetJobName()) , "recreate" );
   } else {
      file = new TFile("Piedestaux_CsI.root", "recreate");
   }
   //initialise tree pointer to 0
   t=0;
}


//________________________________________________________________
void Piedestaux_CsI::InitRun () 
{
   //Initialisation performed at beginning of each run
   //  Int_t fRunNumber contains current run number
//     For the first run, the tree is created with a branch for each acquisition parameter.
//     For subsequent runs, we reset the branch addresses in case the acquisition parameters
//     have "moved" i.e. changed address.
   
    //make sure our ROOT file is current working directory
    file->cd();
    Bool_t make_branches = kFALSE;
    if(!t){
	   //create tree if not already done => first run  
      t = new TTree("t","CsI acquisition parameters");
      make_branches = kTRUE;
    }
	KVDetector* det;
	TIter next_csi(gIndra->GetListOfCsI()); 
	while( (det = (KVDetector*)next_csi()) ){		
		   //parametre R
		   KVACQParam* acq_par_gg = det->GetACQParam("R");
         if( make_branches ){
		      t->Branch( acq_par_gg->GetName(), *(acq_par_gg->ConnectData()), Form("%s/S", acq_par_gg->GetName()));
         } else {
            t->SetBranchAddress( acq_par_gg->GetName(), *(acq_par_gg->ConnectData()) );
         }
		   //parametre L
	   	KVACQParam* acq_par_pg = det->GetACQParam("L");
         if( make_branches ){
		      t->Branch( acq_par_pg->GetName(), *(acq_par_pg->ConnectData()), Form("%s/S", acq_par_pg->GetName()));
         } else {
            t->SetBranchAddress( acq_par_pg->GetName(), *(acq_par_pg->ConnectData()) );
         }
	}
}


//________________________________________________________________
Bool_t Piedestaux_CsI::Analysis () 
{
   //Analysis method called for each event
   //  Long64_t fEventNumber contains current event number
   //  KVINDRATriggerInfo* fTrig contains informations on INDRA trigger for event
   //  KVDetectorEvent* fDetEv gives list of hit groups for current event
   //  Processing will stop if this method returns kFALSE
	//Fill all branches with data from acquisition parameters
	
	t->Fill();
   return kTRUE;
}


//________________________________________________________________
void Piedestaux_CsI::EndRun () 
{
   //Method called at end of each run
}


//________________________________________________________________
void Piedestaux_CsI::EndAnalysis () 
{
   //Method called at end of analysis: save/display histograms etc.
	//On ecrit l'arbre dans le fichier ROOT
   //On fitte tous les pics et écrit les valeurs dans le fichier "piedestaux_csi.dat"
   //Si l'analyse ne s'effectue pas en mode batch, on affiche et imprime les résultats des fits.
	file->cd();
   t->Write();
	
	//fit all branches of tree and write results in file
	ofstream results;
	results.open("piedestaux_csi.dat"); //output text file
	
	if( !gBatchSystem ){
      //create canvas with A4 aspect ratio and divide into 12 pads
	   //pour afficher/controler les fits
	      can=new TCanvas("can","",297,210);
	      gStyle->SetPaperSize(29.7,21.);
	      can->Divide(4,3);
   }
   
	TIter next_csi(gIndra->GetListOfCsI()); KVDetector* det;
	TH1F*htemp=0;
	
	int ipad = 1; int iprint=1;
	
	while( (det = (KVDetector*)next_csi()) ){ // boucle sur tous les CsI
		
		if( !gBatchSystem )can->cd(ipad++);
		
		cout << det->GetName() << endl;
		KVACQParam* acq_par_gg = det->GetACQParam("R");
		t->Fit( "gaus", acq_par_gg->GetName(), "", "V","e");
		htemp=(TH1F*)gROOT->FindObject("htemp");
		htemp->SetTitle(acq_par_gg->GetName());
		if( !gBatchSystem )htemp->Draw("e1");
		TF1*fit=htemp->GetFunction("gaus");
		if( !gBatchSystem ){fit->SetLineStyle(2); fit->SetLineWidth(1);fit->SetLineColor(2); gPad->Modified();gPad->Update();}
		results<<acq_par_gg->GetName()<<" ";
		results<<fit->GetParameter("Mean")<<" "<<fit->GetParError(fit->GetParNumber("Mean"))<<" ";
		results<<fit->GetParameter("Sigma")<<" "<<fit->GetParError(fit->GetParNumber("Sigma"))<<" ";
		results<<fit->GetChisquare()<<" "<<fit->GetNDF() << endl;
		
		if( !gBatchSystem )can->cd(ipad++);
		
		KVACQParam* acq_par_pg = det->GetACQParam("L");
		t->Fit( "gaus", acq_par_pg->GetName(), "", "V","e");
		htemp=(TH1F*)gROOT->FindObject("htemp");
		htemp->SetTitle(acq_par_pg->GetName());
		if( !gBatchSystem )htemp->Draw("e1");
		fit=htemp->GetFunction("gaus");
		if( !gBatchSystem ){fit->SetLineStyle(2); fit->SetLineWidth(1);fit->SetLineColor(2); gPad->Modified();gPad->Update();}
		results<<acq_par_pg->GetName()<<" ";
		results<<fit->GetParameter("Mean")<<" "<<fit->GetParError(fit->GetParNumber("Mean"))<<" ";
		results<<fit->GetParameter("Sigma")<<" "<<fit->GetParError(fit->GetParNumber("Sigma"))<<" ";
		results<<fit->GetChisquare()<<" "<<fit->GetNDF() << endl;
		
		if( !gBatchSystem ){
         if(ipad==13){
			   can->Modified(); can->Update(); can->Print(Form("fits_pied_csi_%d.eps", iprint++)); ipad=1;
		   }
      }
	}
	results.close();

   //close ROOT file
	delete file;
}

