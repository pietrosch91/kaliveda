//Created by KVClassFactory on Thu Apr  1 14:11:10 2010
//Author: bonnet

#include "KVPartitionManager.h"
#include "KVString.h"
#include "KVPartition.h"
#include "TMethodCall.h"
#include "TMath.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TTree.h"

ClassImp(KVPartitionManager)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVPartitionManager</h2>
<h4>Count, Store and Sort partitions</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVPartitionManager::KVPartitionManager()
{
   // Default constructor
	init();
}

KVPartitionManager::~KVPartitionManager()
{
   // Destructor
	delete listdepassage;
	lgen.Clear();	
}

void KVPartitionManager::init(){
	//
	listdepassage = new KVList(kFALSE);
	CreationSousListe();
	Nmax=200;
	nombre_diff=0;
	nombre_total=0;
	SetName("MainListe");
	lgen.Clear();
	
}

void KVPartitionManager::CreationSousListe(){

	GetIntermediate()->Add(new KVList(kFALSE));
	KVList* sl = (KVList* )GetIntermediate()->Last();
	KVString snom; snom.Form("SousListe_%d",GetIntermediate()->GetEntries());
	sl->SetName(snom.Data());
	
}

Bool_t KVPartitionManager::TestPartition(KVPartition* par){

	Bool_t IsNew=kFALSE;
	KVList* sl = (KVList* )GetIntermediate()->Last();
	KVPartition* pp = 0;
	if ( !(pp = (KVPartition* )sl->FindObject(par->GetName())) ){
		sl->Add(par);
		IsNew=kTRUE;
		if (sl->GetEntries()==Nmax){
			CreationSousListe();
		}
	}	
	else pp->AddPopulation(par->GetPopulation());
	
	return IsNew;

}

void KVPartitionManager::ReduceSubLists(){

	KVList* ll = GetIntermediate();
	Info("ReduceSubLists","nbre de sous listes : %d",ll->GetEntries());

	KVList *sl0,*sl1;
	Int_t passage=2;
	Int_t nsl = ll->GetEntries();
	
	while (nsl>1){
		
		Int_t pas =1;
		for (Int_t nn=0;nn<ll->GetEntries();nn+=pas){
			
			sl0  = (KVList* )ll->At(nn);
			
			if (nn+1<ll->GetEntries()){
				sl1  = (KVList* )ll->At(nn+1);
				if (!sl1) Info("ReduceSubLists","pas de liste sl1 a la position %d",nn+1);
			
				KVPartition* par0=0,*par1=0;
				while (sl1->GetEntries()>0){
			
					par1 = (KVPartition* )sl1->RemoveAt(0);
			
					if ( (par0 = (KVPartition* )sl0->FindObject(par1->GetName())) ){
						par0->AddPopulation(par1->GetPopulation());
						delete par1;
					}
					else{
						sl0->Add(par1);
					}
				}
				delete (KVList* )ll->RemoveAt(nn+1);
			}
			
		}
		
		passage+=1;
		nsl = ll->GetEntries();
		
		Info("ReduceSubLists","nbre de sous-liste : %d",nsl);	
	}
		
	UpdateCompteurs((KVList* )ll->At(0));

}

void KVPartitionManager::TransfertToMainList(){
	
	KVList* sl  = (KVList* )GetIntermediate()->RemoveAt(0);
	if (GetEntries()==0) {
		Int_t tot = sl->GetEntries();
		for (Int_t mm=0;mm<tot;mm+=1){
			Add(sl->RemoveAt(0));
		}
	}
	else {
		ReduceWithOne(sl);
	}
	
	delete sl;
	CreationSousListe();
	
	UpdateCompteurs();
	
	Info("TransfertToMainList","%d partitions enregistrees dont %d differentes",
		GetNombrepartitionsTotale(), 
		GetNombrepartitionsDifferentes()
	);

}

void KVPartitionManager::ReduceWithOne(KVList* sl){

	KVPartition* par0=0,*par1=0;
	while (sl->GetEntries()>0){
		par1 = (KVPartition* )sl->RemoveAt(0);
		if ( (par0 = FindPartition(par1->GetName())) ){
			par0->AddPopulation(par1->GetPopulation());
			delete par1;
		}
		else{
			Add(par1);
		}
	}
}

void KVPartitionManager::UpdateCompteurs(KVList* current){
	
	if (!current) {
		nombre_diff = GetEntries();
		nombre_total = 0;
		Int_t ztot_min=10000,z_min=10000,mtot_min=10000;
		Int_t ztot_max=-10000,z_max=-10000,mtot_max=-10000;
		
		KVPartition* par = 0;
		for (Int_t nn=0;nn<nombre_diff;nn+=1) {
			par = GetPartition(nn);
			nombre_total += par->GetPopulation();
			if (par->GetZtot()>ztot_max) 	ztot_max=par->GetZtot();
			if (par->GetZtot()<ztot_min) 	ztot_min=par->GetZtot();
			if (par->GetMtot()>mtot_max) 	mtot_max=par->GetMtot();
			if (par->GetMtot()<mtot_min)	mtot_min=par->GetMtot();
			if (par->GetZmax()>z_max) 		z_max=par->GetZmax();
			if (par->GetZmin()<z_min) 		z_min=par->GetZmin();
		}
		
		lgen.SetValue("Ztot_max",ztot_max);
		lgen.SetValue("Ztot_min",ztot_min);
		lgen.SetValue("Mtot_max",mtot_max);
		lgen.SetValue("Mtot_min",mtot_min);
		lgen.SetValue("Z_max",z_max);
		lgen.SetValue("Z_min",z_min);
	}
	else {
		Int_t n_diff=0;
		Int_t n_total=0;	
		n_diff = current->GetEntries();
		for (Int_t nn=0;nn<n_diff;nn+=1) n_total += ((KVPartition* )current->At(nn))->GetPopulation();
	
		Info("UpdateCompteurs","%s %d partitions enregistrees dont %d differentes",
			current->GetName(),n_total,n_diff);
	}

}

void KVPartitionManager::Reset(){

	KVList* li = GetIntermediate();
	Int_t tot = li->GetEntries();
	for (Int_t ii=0; ii<tot; ii+=1) {
		KVList* sl = (KVList* )li->RemoveAt(0);	
		Int_t toti = sl->GetEntries();
		for (Int_t jj=0;jj<toti; jj+=1)
			delete (KVPartition* )sl->RemoveAt(0);
		delete sl;
	}
	
	Clear();
	CreationSousListe();
	UpdateCompteurs();
	
}


TH1F* KVPartitionManager::GenereHisto(KVString method,Int_t nb,Double_t min,Double_t max){

	if (GetEntries()==0) return 0;
	
	TMethodCall meth;
   meth.InitWithPrototype(this->At(0)->IsA(), method.Data(),"");
	
	KVString snom; snom.Form("%s_%s",this->At(0)->Class_Name(),method.Data());
	
	TH1F* h1 = (TH1F* )gROOT->FindObject(snom.Data());
	if (h1) delete h1;
	h1 = new TH1F(snom.Data(),"From_KVParticleManager",nb,min,max);
	
	if (meth.IsValid()){
		if ( meth.ReturnType()==TMethodCall::kLong) {
			Int_t* val_Long = GetValues_Long(meth);
			for (Int_t nn=0;nn<GetEntries();nn+=1)
				h1->Fill(val_Long[nn]);
			delete [] val_Long;	
		}
		else if (meth.ReturnType()==TMethodCall::kDouble) {
			Double_t* val_Double = GetValues_Double(meth);
			for (Int_t nn=0;nn<GetEntries();nn+=1)
				h1->Fill(val_Double[nn]);
			delete [] val_Double;	
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
	
	return h1;

}



Int_t* KVPartitionManager::GetIndex(KVString method, Bool_t down){

	if (GetEntries()==0) return 0;
	
	TMethodCall meth;
   meth.InitWithPrototype(this->At(0)->IsA(), method.Data(),"");
	Int_t* idx = new Int_t[GetEntries()];
	
	
	if (meth.IsValid()){
		if ( meth.ReturnType()==TMethodCall::kLong) {
			Int_t* val_Long = GetValues_Long(meth);
			TMath::Sort(GetEntries(),val_Long,idx,down);
			delete [] val_Long;
		}
		else if ( meth.ReturnType()==TMethodCall::kDouble) {
			Double_t* val_Double = GetValues_Double(meth);
			TMath::Sort(GetEntries(),val_Double,idx,down);
			delete [] val_Double;
		}
		else {
			delete idx;
			return 0;
		}
	}
	else {
		delete idx;
		return 0;
	}
	
	return idx;

}


Int_t* KVPartitionManager::GetValues_Long(TMethodCall& meth){

	Int_t dim=GetEntries();
	Int_t* val = new Int_t[dim];
	Long_t ret;
	for (Int_t kk=0;kk<dim;kk+=1){
		meth.Execute(At(kk),"",ret);
		val[kk]=ret;
	}
	return val;
}
	
	
Double_t* KVPartitionManager::GetValues_Double(TMethodCall& meth){

	Int_t dim=GetEntries();
	Double_t* val = new Double_t[dim];
	Double_t ret;
	for (Int_t kk=0;kk<dim;kk+=1){
		meth.Execute(At(kk),"",ret);
		val[kk]=ret;
	}
	return val;
}
	
TTree* KVPartitionManager::GenereTree(KVString tree_name,Bool_t Compress,Bool_t AdditionalValue){

	if (GetEntries()==0) return 0;
	
	Int_t mmax = lgen.GetIntValue("Mtot_max");
	Int_t* val = new Int_t[mmax];
	Int_t n_val;
	Int_t pop;
	
	TTree* tree = new TTree(tree_name.Data(),"FromKVPartitionManager");
	tree->Branch("n_val",			&n_val,	"n_val/I");
	tree->Branch("val",				val,		"val[n_val]/I");
	if (Compress)
		tree->Branch("pop",	&pop,		"pop/I");
	
	KVPartition* par;
	
	KVGenParList* add=0;
	Double_t* tab_add=0;
	
	if (AdditionalValue){
		par = GetPartition(0);
		par->CalculValeursAdditionnelles();
		add = par->GetParametersList();
		Info("GenereTree","Parametres additionels");
		add->Print();
		tab_add = new Double_t[add->GetNPar()];
	
		for (Int_t np=0;np<add->GetNPar();np+=1){
			KVString snom1; snom1.Form("%s",add->GetParameter(np)->GetName());
			KVString snom2; snom2.Form("%s/D",add->GetParameter(np)->GetName());
			tree->Branch(snom1.Data(),				&tab_add[np],		snom2.Data());
		}
	}
	
	for (Int_t kk=0;kk<GetEntries();kk+=1){
		par = GetPartition(kk);
		
		n_val = par->GetMtot();
		for (Int_t nn=0;nn<n_val;nn+=1)
			val[nn] = par->GetValeur(nn);
		
		pop = par->GetPopulation();
		
		if (AdditionalValue){
			par->CalculValeursAdditionnelles();
			add = par->GetParametersList();
			for (Int_t np=0;np<add->GetNPar();np+=1){
				tab_add[np] = add->GetDoubleValue(add->GetParameter(np)->GetName());
			}
		}
		
		
		if (Compress){
			tree->Fill();
		}
		else{
			for (Int_t pp=0; pp<pop; pp+=1) {
				tree->Fill();
			}
		}
	}	
	
	delete [] val;
	if (tab_add) delete [] tab_add;
	
	return tree;

}

void KVPartitionManager::PrintInfo(){

	Info("PrintInfo","Intervalles des partitions enregistrees");
	printf(" - Ztot : %d %d\n",lgen.GetIntValue("Ztot_min"),lgen.GetIntValue("Ztot_max"));
	printf(" - Mtot : %d %d\n",lgen.GetIntValue("Mtot_min"),lgen.GetIntValue("Mtot_max"));
	printf(" - Z    : %d %d\n",lgen.GetIntValue("Z_min"),lgen.GetIntValue("Z_max"));


}
