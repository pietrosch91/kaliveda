//Created by KVClassFactory on Thu Jun 17 14:20:16 2010
//Author: bonnet

#include "KVPartitionGenerator.h"
#include "TMath.h"
#include "KVCouple.h"
#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"
#include "KVIntegerList.h"
#include "TEventList.h"
#include "TList.h"

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
	ndim = 0;
	tabz = 0;
	ztot = 0;
	mtot = 0;
	
	
	kcurrent = 0;
	kzf = 0;
	kmf = 0;
	kzm = 0;
	mshift=0;
	zshift=0;
	to_be_checked=kFALSE;
	
	npar=0;
	
	file=0;
	tree=0;
	
	levt=0;
	levt_collect=0;
	levt_z1=0;
	
	evt=0;
	
	nl_zf=0;
	nl_mf=0;
	nl_zm=0;
	nl_z1=0;
}

KVPartitionGenerator::~KVPartitionGenerator()
{
   // Destructor
	if (kcurrent) {delete [] kcurrent; kcurrent = 0;}
	if (tabz) {delete [] tabz; tabz = 0;}
	
	
	if (nl_zf) delete nl_zf;
	if (nl_mf) delete nl_mf;
	if (nl_zm) delete nl_zm;
	if (nl_z1) delete nl_z1;

}

void KVPartitionGenerator::PreparTree(const Char_t* filename,const Char_t* treename,Int_t Ndim,Option_t* option){

	KVString snom;

	SaveAndCloseFile();
	
	if (nl_zf) nl_zf->Clear(); else nl_zf = new KVIntegerList();
	if (nl_mf) nl_mf->Clear(); else nl_mf = new KVIntegerList();
	if (nl_zm) nl_zm->Clear(); else nl_zm = new KVIntegerList();
	if (nl_z1) nl_z1->Clear(); else nl_z1 = new KVIntegerList();
	
	if (filename) file = new TFile(filename,option);
	else {
		snom.Form("From_%s.root",this->Class_Name());
		file = new TFile(snom.Data(),option);
	}
	
	snom.Form("From_%s",this->Class_Name());
	if (treename) tree = new TTree(treename,snom.Data());
	else tree = new TTree("tree",snom.Data());
	
	if (tabz) delete [] tabz;
	ndim = Ndim;
	tabz = new Int_t[ndim];

	tree->Branch("ztot",			&ztot,	"ztot/I");
	tree->Branch("mtot",			&mtot,	"mtot/I");
	tree->Branch("tabz",			tabz,		"tabz[mtot]/I");
	
	
	levt = new TList(); snom.Form("TEventListFor_%s",tree->GetName());
	levt->SetName(snom.Data());
	levt->SetOwner(kTRUE);
	
	npar = 0;
	
	levt_collect = new TList();
	levt_z1 = new TList();
	
	bsup = -100;
	binf = Ndim;

}

void KVPartitionGenerator::Break_Using_Zf_Zinf_Criterion(Int_t Zfrag,Int_t Zinf){

	Int_t mfmin = 1;
	Int_t mfmax = Zfrag/Zinf;

	for (Int_t Mfrag=mfmin;Mfrag<=mfmax;Mfrag+=1){
		SetConditions(Zfrag,Mfrag,Zinf);
		Process();
	}

}

void KVPartitionGenerator::Break_Using_Zf_Zmax_Zinf_Criterion(Int_t Zfrag,Int_t Zmax,Int_t Zinf){

	if (Zfrag==Zmax){	
		SetConditions(Zfrag,1,Zinf);
		Process();
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
			Process();
		}
		
	}
	else {
		printf("%d %d ??? \n",Zfrag-Zmax,Zinf);
	}
}

void KVPartitionGenerator::Break_Using_Mf_Zmax_Zinf_Criterion(Int_t Mfrag,Int_t Zmax,Int_t Zinf){

	
	Int_t zfmin = Zmax+(Mfrag-1)*Zinf; 
	Int_t zfmax = Zmax*Mfrag;
	
	for (Int_t Zfrag=zfmin;Zfrag<=zfmax;Zfrag+=1){
		mshift=1;
		zshift=Zmax;
		to_be_checked=kTRUE;
		SetConditions(Zfrag-zshift,Mfrag-mshift,Zinf);
		Process();
	}
	
}

void KVPartitionGenerator::Break_Using_Zf_Mf_Zinf_Criterion(Int_t Zfrag,Int_t Mfrag,Int_t Zinf){

	SetConditions(Zfrag,Mfrag,Zinf);
	Process();
	
}

void KVPartitionGenerator::SetConditions(Int_t Zfrag,Int_t Mfrag,Int_t Zinf){

	
	kzf = Zfrag;
	kmf = Mfrag;
	kzm = Zinf;
	if (kcurrent) delete kcurrent;
	kcurrent = new Int_t[kmf]; for (Int_t mm=0;mm<kmf;mm+=1) kcurrent[mm]=0;
	
	mtot = kmf+mshift;
	if (mtot>ndim){
		Warning("SetConditions","la multiplicité %d est superieure a la val max du tableau (%d)",mtot,ndim);
	
	}
	ztot = kzf+zshift;
	
	if (mshift) tabz[0] = zshift;
	
	npar_zf_mf = 0;
	
	nl_zf->Add(ztot);
	nl_mf->Add(mtot);
	nl_zm->Add(kzm);
	
	KVString snom;
	snom.Form("ztot_%d_mtot_%d_zinf_%d",ztot,mtot,Zinf);
	levt->Add( new TEventList(snom.Data(),tree->GetName()) );
	evt = (TEventList* )levt->Last();
	
	Int_t entries = levt_z1->GetEntries();
	TEventList* e1=0;
	for (Int_t nn=0;nn<entries;nn+=1){
		e1 = (TEventList* )levt_z1->RemoveAt(0);
		if (e1->GetN()>0){
			levt_collect->Add(e1);
			/*
			if (nn>bsup) bsup=nn;
			if (nn<binf) binf=nn;
			*/
		}
		else {
			delete e1;	
		}
	}
	
	for (Int_t ii=0;ii<=ztot;ii+=1){
		snom.Form("ztot_%d_mtot_%d_zmax_%d_zinf_%d",ztot,mtot,ii,Zinf);
		levt_z1->Add(new TEventList(snom.Data(),tree->GetName()));
	}
}

void KVPartitionGenerator::TreatePartition(){
		
	npar_zf_mf += 1; 

	tree->Fill();
	evt->Enter(npar);
	((TEventList* )levt_z1->At(tabz[0]))->Enter(npar);
	
	npar+=1;

}


void KVPartitionGenerator::Process(void){


	if (kmf==1){
		tabz[0+mshift] = kzf;
		TreatePartition();
		
		return;
	}
	
	Int_t zutilise = (kmf*kzm);   
  	Int_t zdispo = kzf-zutilise;
	
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
			coup[nc] = new KVCouple(zdispo,zsup,kmf-nc);
			
			ncouple[nc] = coup[nc]->GetNbreCouples();
			niter[nc] = 0;
		}
		if (niter[nc]<ncouple[nc]){
			
			zdispo = coup[nc]->GetZ2(niter[nc]);
			zsup = coup[nc]->GetZ1(niter[nc]);
			kcurrent[nc] = coup[nc]->GetZ1(niter[nc]);
			kcurrent[nc+1] = coup[nc]->GetZ2(niter[nc]);
			
		}
	}
	
	Bool_t finish = kFALSE;
	while (!finish){
		
		nc = nb_cassure-1;
		while (niter[nc]<ncouple[nc]){
			
			kcurrent[nc] = coup[nc]->GetZ1(niter[nc]);
			kcurrent[nc+1] = coup[nc]->GetZ2(niter[nc]);
			
			for (Int_t ii=0;ii<kmf;ii+=1)
				tabz[ii+mshift] = kcurrent[ii]+kzm;
			
			if (to_be_checked) { 
				if (tabz[0]>=tabz[mshift])	{
					TreatePartition();
				}
			}
			else {
				TreatePartition();
			}
			
			niter[nc]+=1;
		}
		delete coup[nc]; coup[nc]=0;
	
		Int_t previous = nc-1;
		while ( niter[previous] == (ncouple[previous]-1) && previous>=0 ){
			
			delete coup[previous]; coup[previous]=0;
			previous -= 1;
			
			if (previous<0) break;
		}	
		
		if (previous<0) { 
			finish=kTRUE; 
		}
		else {
			niter[previous] += 1;
			zdispo = coup[previous]->GetZ2(niter[previous]);
			zsup = coup[previous]->GetZ1(niter[previous]);
	
			kcurrent[previous] = coup[previous]->GetZ1(niter[previous]);
			
			for (Int_t ncbis=previous+1; ncbis<nb_cassure; ncbis+=1){	
				if (!coup[ncbis]) {
					coup[ncbis] = new KVCouple(zdispo,zsup,kmf-ncbis);
					
					ncouple[ncbis] = coup[ncbis]->GetNbreCouples();
					niter[ncbis] = 0;
				}
				else { }
				if (niter[ncbis]<ncouple[ncbis]){
			
					zdispo = coup[ncbis]->GetZ2(niter[ncbis]);
					zsup = coup[ncbis]->GetZ1(niter[ncbis]);
					kcurrent[ncbis] = coup[ncbis]->GetZ1(niter[ncbis]);
					kcurrent[ncbis+1] = coup[ncbis]->GetZ2(niter[ncbis]);
				}
			}		
		}														  
	}
	
	Info("Process","zfrag:%d en mfrag:%d avec zlim:%d donne %d combinaisons (total %d)",kzf,kmf,kzm,npar_zf_mf,npar);
	

}

void KVPartitionGenerator::WriteInfo(){

	tree->GetUserInfo()->Add(new TNamed("Ztot Range",nl_zf->GetName()));
	tree->GetUserInfo()->Add(new TNamed("Mtot Range",nl_mf->GetName()));
	tree->GetUserInfo()->Add(new TNamed("Zinf Range",nl_zm->GetName()));
	
	TEventList* e1=0;
	Int_t entries = levt_z1->GetEntries();
	for (Int_t nn=0;nn<entries;nn+=1){
		e1 = (TEventList* )levt_z1->RemoveAt(0);
		if (e1->GetN()>0){
			levt_collect->Add(e1);
		}
		else {
			delete e1;	
		}
	}
	//nl_z1->SetMinMax(nl_zm->First(),nl_zf->Last());
	tree->GetUserInfo()->Add(new TNamed("Zmax Range",nl_z1->GetName()));
	
}

void KVPartitionGenerator::SaveAndCloseFile(){

	if (file && file->IsOpen()) {
		file->cd();
		if (tree && file->IsWritable()) {
			Info("SaveAndCloseFile","Ecriture du fichier %s avec l'arbre %s (%d entrees)",file->GetName(),tree->GetName(),tree->GetEntries());
			
			file->Write();
		}
		Info("SaveAndCloseFile","Fermeture de %s",file->GetName());
		file->Close();
		
	}
}
