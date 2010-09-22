//Created by KVClassFactory on Thu Jun 17 14:20:16 2010
//Author: bonnet

#include "KVPartitionGenerator.h"
#include "TMath.h"
#include "KVCouple.h"
#include "Riostream.h"
#include "KVPartition.h"
#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"
#include "TSystem.h"
#include "TROOT.h"

ClassImp(KVPartitionGenerator)
////////////////////////////////////////////////////////////////////////////////
/*
BEGIN_HTML
<h2>KVPartitionGenerator</h2>
<h4>Calcul numeriquement toutes les partitions d'un couple Zfrag/Mfrag donne</h4>
END_HTML
Compute all possible partitions for a given number in a given number of pieces
For exmaple the break up of a total charge Zfrag in Mfrag charges, assuming a minimum size for these charges (Zinf)
Beta Version ...
Seems to work as to be tested
Different ways of break-up
	Test_Zf_Zinf -> Only Zf as a constraint for the total charge
	Test_Zf_Zmax_Zinf -> Zf as the total charge and Zmax as the biggest charge
	Test_Mf_Zmax_Zinf -> Mf as the number of pieces and Zmax as the biggest charge
	Test_Zf_Mf_Zinf -> Zf as the total charge and Mf the number of pieces
All these methods call the Process() method which use the KVCouple class to iteratively break the total initial charge in the given number of pieces

*/
////////////////////////////////////////////////////////////////////////////////

KVPartitionGenerator::KVPartitionGenerator()
{
   // Default constructor
	kcurrent = 0;
	tabz = 0;
	kzf = 0;
	kmf = 0;
	kzm = 0;
	mshift=0;
	zshift=0;
	to_be_checked=kFALSE;

}

KVPartitionGenerator::~KVPartitionGenerator()
{
   // Destructor
	if (kcurrent) {delete [] kcurrent; kcurrent = 0;}

}

void KVPartitionGenerator::Test_Zf_Zinf(Int_t Zfrag,Int_t Zinf){

	Int_t mfmin = 1;
	Int_t mfmax = Zfrag/Zinf;

	for (Int_t Mfrag=mfmin;Mfrag<=mfmax;Mfrag+=1){
		SetConditions(Zfrag,Mfrag,Zinf);
		Process("test_Zf_Zinf.root");
	}
	file->Close();

}

void KVPartitionGenerator::Test_Zf_Zmax_Zinf(Int_t Zfrag,Int_t Zmax,Int_t Zinf){

	if (Zfrag==Zmax){	
		SetConditions(Zfrag,1,Zinf);
		Process("test_Zf_Zmax_Zinf.root");
		file->Close();
	}
	else if (Zfrag-Zmax>=Zinf){
		
		Int_t mfmin=2;
		Int_t np=0;
		KVCouple* cp = new KVCouple(Zfrag,Zmax,mfmin);
		np=cp->GetNbreCouples();
		while (np<=0){
			if (cp) delete cp; cp = 0;
			mfmin+=1;
			cp = new KVCouple(Zfrag,Zmax,mfmin);
			np = cp->GetNbreCouples();
		}
		printf("a priori mfmin=%d OK\n",mfmin);
		for (Int_t ii=0;ii<cp->GetNbreCouples();ii+=1){
			printf("%d %d\n",cp->GetZ1(ii),cp->GetZ2(ii));
		}
		if (cp) delete cp; cp=0;
		
		
		Int_t mfmax=((Zfrag-Zmax)/Zinf)+1;
		np=0;
		cp = new KVCouple(Zfrag,Zmax,mfmax);
		np=cp->GetNbreCouples();
		while (np<=0){
			if (cp) delete cp; cp = 0;
			mfmax-=1;
			cp = new KVCouple(Zfrag,Zmax,mfmax);
			np = cp->GetNbreCouples();
		}
		printf("a priori mfmax=%d OK\n",mfmax);
		for (Int_t ii=0;ii<cp->GetNbreCouples();ii+=1){
			printf("%d %d\n",cp->GetZ1(ii),cp->GetZ2(ii));
		}
		if (cp) delete cp; cp=0;
		
		
		for (Int_t Mfrag=mfmin;Mfrag<=mfmax;Mfrag+=1){
			mshift=1;
			zshift=Zmax;
			to_be_checked=kTRUE;
			SetConditions(Zfrag-zshift,Mfrag-mshift,Zinf);
			Process("test_Test_Zf_Zmax_Zinf.root");
		}
		file->Close();
		
	}
	else {
		printf("%d %d ??? \n",Zfrag-Zmax,Zinf);
	}
}

void KVPartitionGenerator::Test_Mf_Zmax_Zinf(Int_t Mfrag,Int_t Zmax,Int_t Zinf){

	
	Int_t zfmin = Zmax+(Mfrag-1)*Zinf; 
	Int_t zfmax = Zmax*Mfrag;
	
	for (Int_t Zfrag=zfmin;Zfrag<=zfmax;Zfrag+=1){
		mshift=1;
		zshift=Zmax;
		to_be_checked=kTRUE;
		SetConditions(Zfrag-zshift,Mfrag-mshift,Zinf);
		Process("test_Mf_Zmax_Zinf.root");
	}
	file->Close();
	
}

void KVPartitionGenerator::Test_Zf_Mf_Zinf(Int_t Zfrag,Int_t Mfrag,Int_t Zinf){

	SetConditions(Zfrag,Mfrag,Zinf);
	Process("test_Zf_Mf_Zinf.root");
	file->Close();
	
}

void KVPartitionGenerator::SetConditions(Int_t Zfrag,Int_t Mfrag,Int_t Zinf){

	
	kzf = Zfrag;
	kmf = Mfrag;
	kzm = Zinf;
	if (kcurrent) delete kcurrent;
	kcurrent = new Int_t[kmf]; for (Int_t mm=0;mm<kmf;mm+=1) kcurrent[mm]=0;
	
	mtot = kmf+mshift;
	ztot = kzf+zshift;
	
	if (tabz) delete tabz;
	tabz = new Int_t[mtot];		for (Int_t mm=0;mm<mtot;mm+=1) tabz[mm]=0;
	if (mshift) tabz[0] = zshift;
	

}


void KVPartitionGenerator::DefineTree(KVString file_name){

	
	KVString snom;
	if ( !(file = gROOT->GetFile(file_name.Data())) ){
		file = new TFile(file_name.Data(),"recreate");
	}
	
	snom.Form("tree_Zfra%d_Mfra%d_Zinf%d",kzf+zshift,kmf+mshift,kzm);
	tt = new TTree(snom.Data(),"From KVPartitionGenerator");
	
	tt->Branch("mtot",&mtot,"mtot/I");
	tt->Branch("ztot",&ztot,"ztot/I");
	tt->Branch("tabz",tabz,"tabz[mtot]/I");
	
	/*
	KVString stit;
	stit.Form("%d",kzf);	tt->GetUserInfo()->Add(new TNamed("zf",stit.Data())); 
	stit.Form("%d",kmf);	tt->GetUserInfo()->Add(new TNamed("mf",stit.Data())); 
	stit.Form("%d",kzm);	tt->GetUserInfo()->Add(new TNamed("zmin",stit.Data())); 
	*/

}


void KVPartitionGenerator::Process(KVString file_name){

	DefineTree(file_name);

	if (kmf==1){
		tabz[0+mshift] = kzf;
		tt->Fill();
		file->cd();
		tt->Write();
		
		return;
	}
	
	//printf("---\n---\n---\n");
	Int_t zutilise = (kmf*kzm);   
  	Int_t zdispo = kzf-zutilise;
	//printf("zutilise:%d zdispo:%d\n",zutilise,zdispo);
	
	Int_t nb_cassure = kmf-1;
	KVCouple* coup[nb_cassure];
	Int_t ncouple[nb_cassure]; 
	Int_t niter[nb_cassure]; 
	for (Int_t nc=0;nc<nb_cassure;nc+=1) {
		ncouple[nc]=0;
		coup[nc]=0;
		niter[nc]=0;
	}
	
	Int_t nc;
	Int_t zsup = zdispo;
	for (nc=0;nc<nb_cassure;nc+=1){
		
		if (!coup[nc]) {
			//printf("Creation du couple %d\n",nc);
			coup[nc] = new KVCouple(zdispo,zsup,kmf-nc);
			
			ncouple[nc] = coup[nc]->GetNbreCouples();
			niter[nc] = 0;
		}
		if (niter[nc]<ncouple[nc]){
			
			//printf("%d %d %d - %d %d - %d %d\n",nc,zdispo,zsup,niter[nc],ncouple[nc],coup[nc]->GetZ2(niter[nc]),coup[nc]->GetZ1(niter[nc]));
			zdispo = coup[nc]->GetZ2(niter[nc]);
			zsup = coup[nc]->GetZ1(niter[nc]);
			kcurrent[nc] = coup[nc]->GetZ1(niter[nc]);
			kcurrent[nc+1] = coup[nc]->GetZ2(niter[nc]);
			
		}
	}
	
	//for (Int_t time=0;time<2000;time+=1){
	UInt_t npar = 0;
	//const long long npar2 = 0;
	Bool_t finish = kFALSE;
	while (!finish){
		
		nc = nb_cassure-1;
		while (niter[nc]<ncouple[nc]){
			
			kcurrent[nc] = coup[nc]->GetZ1(niter[nc]);
			kcurrent[nc+1] = coup[nc]->GetZ2(niter[nc]);
			//transfert des charges
			
			for (Int_t ii=0;ii<kmf;ii+=1)
				tabz[ii+mshift] = kcurrent[ii]+kzm;
			
			if (to_be_checked) { 
				if (tabz[0]>=tabz[mshift])	{
					npar += 1; 
					tt->Fill(); 
				}
			}
			else {
				npar += 1;
				tt->Fill();
			}
			
			niter[nc]+=1;
		}
		delete coup[nc]; coup[nc]=0;
	
		Int_t previous = nc-1;
		//printf("entree while : previous:%d iterations:%d ncouples:%d\n",previous,niter[previous],ncouple[previous]);
		while ( niter[previous] == (ncouple[previous]-1) && previous>=0 ){
			
			delete coup[previous]; coup[previous]=0;
			previous -= 1;
			
			//printf("boucle while : previous:%d iterations:%d ncouples:%d\n",previous,niter[previous],ncouple[previous]);
			if (previous<0) break;
		}	
		if (previous<0) { 
			//printf("sortie break : %d previous=%d nb_cassure=%d\n",0,previous,nb_cassure); 
			file->cd();
			tt->Write();
			//file->Close();
			finish=kTRUE; 
		}
		else {
			niter[previous] += 1;
			zdispo = coup[previous]->GetZ2(niter[previous]);
			zsup = coup[previous]->GetZ1(niter[previous]);
	
			kcurrent[previous] = coup[previous]->GetZ1(niter[previous]);
			//kcurrent[nc+1] = coup[nc-remonte]->GetZ2(niter[nc-remonte]);
	
			//printf("boucle entre %d %d\n",previous+1,nb_cassure);
			for (Int_t ncbis=previous+1; ncbis<nb_cassure; ncbis+=1){	
				if (!coup[ncbis]) {
					//printf("Creation du couple %d\n",ncbis);
					coup[ncbis] = new KVCouple(zdispo,zsup,kmf-ncbis);
					ncouple[ncbis] = coup[ncbis]->GetNbreCouples();
					niter[ncbis] = 0;
				}
				else {
					//printf("le couple %d existe deja\n",ncbis);
				}
				if (niter[ncbis]<ncouple[ncbis]){
			
					//printf("%d %d %d - %d %d - %d %d\n",ncbis,zdispo,zsup,niter[ncbis],ncouple[ncbis],coup[ncbis]->GetZ2(niter[ncbis]),coup[ncbis]->GetZ1(niter[ncbis]));
					zdispo = coup[ncbis]->GetZ2(niter[ncbis]);
					zsup = coup[ncbis]->GetZ1(niter[ncbis]);
					kcurrent[ncbis] = coup[ncbis]->GetZ1(niter[ncbis]);
					kcurrent[ncbis+1] = coup[ncbis]->GetZ2(niter[ncbis]);
					//printf("partition : "); for (Int_t nn=0;nn<kmf;nn+=1) printf("%d ",kcurrent[nn]+kzm); printf("\n");
		
				}
			}		
		}														  
	}
	
	printf("zfrag:%d en mfrag:%d avec zlim:%d donne %d combinaisons\n",kzf,kmf,kzm,npar);
	

	
	
}
