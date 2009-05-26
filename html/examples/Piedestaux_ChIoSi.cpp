/*
$Id: Piedestaux_ChIoSi.cpp,v 1.1 2007/06/25 15:58:41 franklan Exp $
$Revision: 1.1 $
$Date: 2007/06/25 15:58:41 $
*/

//Created by KVClassFactory on Mon Jun 25 17:09:04 2007
//Author: franklan

#include "Piedestaux_ChIoSi.h"
#include "KVBatchSystem.h"
#include "KVINDRA.h"
#include <TStyle.h>
#include <TROOT.h>
#include <KVDetector.h>
#include <TH1.h>
#include <TF1.h>
#include <TList.h>
#include <KVACQParam.h>
#include <Riostream.h>

ClassImp(Piedestaux_ChIoSi)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Piedestaux_ChIoSi</h2>
<h4>User raw data analysis class</h4>
Example of raw data analysis script, used to extract the ChIo and Si pedestals from the
appropriate pedestal run.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Piedestaux_ChIoSi::Piedestaux_ChIoSi()
{
   //Default constructor
}

Piedestaux_ChIoSi::~Piedestaux_ChIoSi()
{
   //Destructor
}


//________________________________________________________________
void Piedestaux_ChIoSi::InitAnalysis () 
{
   //Initialisation of e.g. histograms, performed once at beginning of analysis
	if( gBatchSystem ){ // batch mode
      file = new TFile( Form( "%s.root", gBatchSystem->GetJobName()) , "recreate" );
   } else {
      file = new TFile("Piedestaux_ChIoSi.root", "recreate");
   }
   //initialise tree pointer to 0
   t=0;
}


//________________________________________________________________
void Piedestaux_ChIoSi::InitRun () 
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
	   t = new TTree("t","ChIoSi acquisition parameters");
      make_branches = kTRUE;
    }
	
	KVDetector* det;
	TIter next_chio(gIndra->GetListOfChIo()); 
	while( (det = (KVDetector*)next_chio()) ){ //boucle sur toutes les ChIo
		
		//parametre GG
		KVACQParam* acq_par_gg = det->GetACQParam("GG");
         if( make_branches ){
		      t->Branch( acq_par_gg->GetName(), *(acq_par_gg->ConnectData()), Form("%s/S", acq_par_gg->GetName()));
         } else {
		      t->SetBranchAddress( acq_par_gg->GetName(), *(acq_par_gg->ConnectData()) );
         }
		
		//parametre PG
		KVACQParam* acq_par_pg = det->GetACQParam("PG");
         if( make_branches ){
		      t->Branch( acq_par_pg->GetName(), *(acq_par_pg->ConnectData()), Form("%s/S", acq_par_pg->GetName()));
         } else {
		      t->SetBranchAddress( acq_par_pg->GetName(), *(acq_par_pg->ConnectData()) );
         }
	}
	
 	TIter next_si(gIndra->GetListOfSi());
	while( (det = (KVDetector*)next_si()) ){ //boucle sur tous les siliciums
		
		//parametre GG
		KVACQParam* acq_par_gg = det->GetACQParam("GG");
		if(acq_par_gg){
         if( make_branches ){
            t->Branch( acq_par_gg->GetName(), *(acq_par_gg->ConnectData()), Form("%s/S", acq_par_gg->GetName()));
         } else {
            t->SetBranchAddress( acq_par_gg->GetName(), *(acq_par_gg->ConnectData()) );
         }
      }
		//parametre PG
		KVACQParam* acq_par_pg = det->GetACQParam("PG");
		if(acq_par_pg){
         if( make_branches ){
            t->Branch( acq_par_pg->GetName(), *(acq_par_pg->ConnectData()), Form("%s/S", acq_par_pg->GetName()));
         } else {
            t->SetBranchAddress( acq_par_pg->GetName(), *(acq_par_pg->ConnectData()) );
         }
      }
	}
   
}


//________________________________________________________________
Bool_t Piedestaux_ChIoSi::Analysis () 
{
   //Analysis method called for each event
   //  Long64_t fEventNumber contains current event number
   //  KVINDRATriggerInfo* fTrig contains informations on INDRA trigger for event
   //  KVDetectorEvent* fDetEv gives list of hit groups for current event
   //  Processing will stop if this method returns kFALSE
	//On remplit les branches avec les donnees pour cet evenement
	
	t->Fill();
   return kTRUE;
}


//________________________________________________________________
void Piedestaux_ChIoSi::EndRun () 
{
   //Method called at end of each run
}


//________________________________________________________________
void Piedestaux_ChIoSi::EndAnalysis () 
{
   //Method called at end of analysis: save/display histograms etc.
	//On ecrit l'arbre dans le fichier ROOT
   //On fitte tous les pics et écrit les valeurs dans le fichier "piedestaux_chiosi.dat"
   //Si l'analyse ne s'effectue pas en mode batch, on affiche et imprime les résultats des fits.
   
	file->cd();
	t->Write();
	
   //fit all branches of tree and write results in file
	ofstream results;
	results.open("piedestaux_chiosi.dat"); //output text file
	
   if( !gBatchSystem ){
	   //create canvas with A4 aspect ratio and divide into 12 pads
	   //pour afficher/controler les fits
	   can=new TCanvas("can","",297,210);
	   gStyle->SetPaperSize(29.7,21.);
	   can->Divide(4,3);
   }
   
	TIter next_chio(gIndra->GetListOfChIo()); KVDetector* det;
	TH1F*htemp=0;
	
	int ipad = 1; int iprint=1;
	
	while( (det = (KVDetector*)next_chio()) ){ // boucle sur toutes les ChIo
		
		if( !gBatchSystem ) can->cd(ipad++);
		
		cout << det->GetName() << endl;
		KVACQParam* acq_par_gg = det->GetACQParam("GG");
		t->Fit( "gaus", acq_par_gg->GetName(), "", "V","e");
		htemp=(TH1F*)gROOT->FindObject("htemp");
		htemp->SetTitle(acq_par_gg->GetName());
		if( !gBatchSystem ) htemp->Draw("e1");
		TF1*fit=htemp->GetFunction("gaus");
		if( !gBatchSystem ) {fit->SetLineStyle(2); fit->SetLineWidth(1);fit->SetLineColor(2);gPad->Modified();gPad->Update();}
		results<<acq_par_gg->GetName()<<" ";
		results<<fit->GetParameter("Mean")<<" "<<fit->GetParError(fit->GetParNumber("Mean"))<<" ";
		results<<fit->GetParameter("Sigma")<<" "<<fit->GetParError(fit->GetParNumber("Sigma"))<<" ";
		results<<fit->GetChisquare()<<" "<<fit->GetNDF() << endl;
		
		if( !gBatchSystem ) can->cd(ipad++);
		
		KVACQParam* acq_par_pg = det->GetACQParam("PG");
		t->Fit( "gaus", acq_par_pg->GetName(), "", "V","e");
		htemp=(TH1F*)gROOT->FindObject("htemp");
		htemp->SetTitle(acq_par_pg->GetName());
		if( !gBatchSystem ) htemp->Draw("e1");
		fit=htemp->GetFunction("gaus");
		if( !gBatchSystem ) {fit->SetLineStyle(2); fit->SetLineWidth(1);fit->SetLineColor(2);gPad->Modified();gPad->Update();}
		results<<acq_par_pg->GetName()<<" ";
		results<<fit->GetParameter("Mean")<<" "<<fit->GetParError(fit->GetParNumber("Mean"))<<" ";
		results<<fit->GetParameter("Sigma")<<" "<<fit->GetParError(fit->GetParNumber("Sigma"))<<" ";
		results<<fit->GetChisquare()<<" "<<fit->GetNDF() << endl;
		
		if( !gBatchSystem ) {
         if(ipad==13){
            can->Modified(); can->Update(); can->Print(Form("fits_pied_chiosi_%d.eps", iprint++)); ipad=1;
         }
      }
	}
	TIter next_si(gIndra->GetListOfSi());
	ipad = 1;
	
	htemp=0;
	while( (det = (KVDetector*)next_si()) ){ // boucle sur tous les siliciums
		if( !gBatchSystem ) can->cd(ipad++);
		
		cout << det->GetName() << endl;
		KVACQParam* acq_par_gg = det->GetACQParam("GG");
		if( acq_par_gg ){
			t->Fit( "gaus", acq_par_gg->GetName(), "", "V", "e");
			htemp=(TH1F*)gROOT->FindObject("htemp");
			htemp->SetTitle(acq_par_gg->GetName());
			if( !gBatchSystem ) htemp->Draw("e1");
			TF1*fit=htemp->GetFunction("gaus");
			if( !gBatchSystem ) {fit->SetLineStyle(2); fit->SetLineWidth(1);fit->SetLineColor(2); gPad->Modified();gPad->Update();}
			results<<acq_par_gg->GetName()<<" ";
			results<<fit->GetParameter("Mean")<<" "<<fit->GetParError(fit->GetParNumber("Mean"))<<" ";
			results<<fit->GetParameter("Sigma")<<" "<<fit->GetParError(fit->GetParNumber("Sigma"))<<" ";
			results<<fit->GetChisquare()<<" "<<fit->GetNDF() << endl;
		}
		if( !gBatchSystem ) can->cd(ipad++);
		
		KVACQParam* acq_par_pg = det->GetACQParam("PG");
		if( acq_par_pg ){
			t->Fit( "gaus", acq_par_pg->GetName(), "", "V", "e");
			htemp=(TH1F*)gROOT->FindObject("htemp");
			htemp->SetTitle(acq_par_pg->GetName());
			if( !gBatchSystem ) htemp->Draw("e1");
			TF1*fit=htemp->GetFunction("gaus");
			if( !gBatchSystem ) {fit->SetLineStyle(2); fit->SetLineWidth(1);fit->SetLineColor(2); gPad->Modified();gPad->Update();}
			results<<acq_par_pg->GetName()<<" ";
			results<<fit->GetParameter("Mean")<<" "<<fit->GetParError(fit->GetParNumber("Mean"))<<" ";
			results<<fit->GetParameter("Sigma")<<" "<<fit->GetParError(fit->GetParNumber("Sigma"))<<" ";
			results<<fit->GetChisquare()<<" "<<fit->GetNDF() << endl;
		}
		if( !gBatchSystem ) {
         if(ipad==13){
			   can->Modified(); can->Update(); can->Print(Form("fits_pied_chiosi_%d.eps", iprint++)); ipad=1;
         }
		}
	}
	results.close();

   //close ROOT file
	delete file;
}

