//Created by KVClassFactory on Mon Mar 29 14:58:00 2010
//Author: bonnet

#include "KVIntegerList.h"
#include "KVEvent.h"
#include "KVNucleus.h"
#include "TClass.h"
#include "KVPartition.h"

ClassImp(KVIntegerList)

////////////////////////////////////////////////////////////////////////////////
/*
BEGIN_HTML
<h2>KVIntegerList</h2>
<h4>Permet de gerer des partitions de nombres entiers</h4>
END_HTML
Cette classe trouve une application dans la gestion de partition de charges 
BEGIN_LATEX
{Z_{i},i=1,M_{tot}}
END_LATEX
Elle a deux parametres d'entree qui sont accessibles par le constructeur
Il s'agit de :
- la taille maximale du systeme a partitionner (val_max) et/ou de la taille du plus gros cluster
- l'ordre supérireure des moments (mommax) qui seront calcules automatiquement KVIntegerList::Compute()
BEGIN_LATEX
Moment d'ordre n : M_{n} = \Sigma_{i=1,Mtot} P( Z_{i} ) * Z_{i}^{ n}
END_LATEX

Les methodes type Fill...(...) permettent de remplir et de classer la partition dans le meme temps
A chaque appel de ces methodes les contenus de la classe sont reinititialises et recalcules

Voici un petit exemple pour remplir une KVIntegerList de deux facons

BEGIN_HTML
<pre>
<code>
void test()
{
Int_t* taInt_t* tab=0;

KVIntegerList* par = new KVIntegerList(100,3);
par->Fill("12 13 46 20 89");
par->Print("Moments");
par->Print("Partition");

tab = new Int_t[12];
for (Int_t nn=0;nn<12;nn+=1){
	tab[nn] = TMath::Nint(gRandom->Uniform(0.9,20.1));
}
	par->Fill(tab,12);
	par->Print("Moments");
	par->Print("Partition");

delete [] tab;
delete par;
}
</code>
</pre>
END_HTML
Des methodes type Compare...(...) permettent de comparer une partition a une autre en utilisant differents
ingredients

Les methodes type Remove..(..) permettent de retirer des valeurs et de recalculer les grandeurs a partir
des restantes

Des grandeurs calculables a partir des moments peuvent etre definies dans la methode
KVIntegerList::CalculValeursAdditionnelles() en la derivant dans une classe fille
Ces grandeurs additionnelles sont stockées dans une KVGenParList
pour y acceder on peut utiliser les methodes GetValeursEnPlus(...)
*/
////////////////////////////////////////////////////////////////////////////////

void KVIntegerList::init()
{

	fPop = 0;
	fRegle = 0;
	fLimiteRegle = 0;
	fMult = 0;

}

KVIntegerList::KVIntegerList()
{
   // Default constructor
	init();
}

KVIntegerList::~KVIntegerList()
{
   // Destructor
	
	EraseRegle();
	init();

}

void KVIntegerList::SetPartition(const Char_t* par){
		
KVString st(par);
st.Begin(" ");
TObjArray* toks = 0;
while (!st.End()){
	KVString tamp = st.Next();
	Int_t val;
	Int_t freq;
	if (tamp.Contains("(")){
		if (!tamp.Contains(")")) {
			Warning("SetPartition","%s ->pb de coherence dans les parentheses",tamp.Data());
			return;
		}
		else {
			toks = tamp.Tokenize("(");
			val = ((TObjString* )toks->At(0))->GetString().Atoi();
			freq= ((TObjString* )toks->At(1))->GetString().Atoi();
			delete toks;
		}
	}
	else {
		val = tamp.Atoi();
		freq = 1;
	}
	Add(val,freq);
}

}

void KVIntegerList::Update(){
		
	KVString snom="",stamp="";
	fMult=0;	
	for (Int_t ii = fLimiteRegle; ii>=0; ii-=1){
		Int_t contenu = fRegle->At(ii);
		if (contenu>0){
			fMult+=contenu;
			//Formattage du nom de la partition
			stamp.Form("%d",ii);	snom += stamp;
			if (contenu>1){
				stamp.Form("(%d)",contenu);
				snom += stamp; 
			}
			snom+=" ";
		}
	}
	if (snom!="") snom.Remove(snom.Length()-1);	
	SetName(snom.Data());
	SetBit(kHastobeComputed,kFALSE);
		
}
	
void KVIntegerList::Fill(Int_t* tab,Int_t mult){

	ResetPartition();
	Add(tab,mult);
	Update();	

}

void KVIntegerList::Fill(Double_t* tab,Int_t mult){

	ResetPartition();
	Add(tab,mult);
	Update();	

}

void KVIntegerList::Fill(KVEvent* evt,Option_t* opt){

	if (!evt) return;
	ResetPartition();
	
	KVNucleus* nuc=0;
	while ( (nuc = (KVNucleus* )evt->GetNextParticle(opt)) ) 
		Add(nuc->GetZ());
	Update();	
	
}

void KVIntegerList::Add(Int_t val,Int_t freq){
	
	if (!fRegle) { 
		fRegle = new TArrayI(val+1); 
		fLimiteRegle=val;
		SetPopulation(1);
	}
	else {
		if (val>fLimiteRegle) { fRegle->Set(val+1); fLimiteRegle=val; }
		else {}
	}
	
	fRegle->AddAt(fRegle->At(val)+freq,val);
	SetBit(kHastobeComputed,kTRUE);

}

Bool_t KVIntegerList::Remove(Int_t val){
		
	if (!fRegle) { return kFALSE; }
	else if (val>fLimiteRegle){
		return kFALSE;
	}
	else if (fRegle->At(val)==0) { return kFALSE; }
	else {
		fRegle->AddAt(fRegle->At(val)-1,val);
		SetBit(kHastobeComputed,kTRUE);
		return kTRUE;
	}

}

KVPartition* KVIntegerList::CreateKVPartition(Int_t mom_max){
	KVPartition* par = new KVPartition(fLimiteRegle,mom_max);
	Int_t* tabz = GetTableOfValues();
	par->Fill(tabz,fMult);
	delete tabz;
	return par;
}
	
void KVIntegerList::Streamer(TBuffer &R__b)
{
   
   if (R__b.IsReading()) {
      TNamed::Class()->ReadBuffer(R__b, this);
  		DeducePartitionFromTNamed();   
	} else {
   	SetTitle(Form("%d",GetPopulation()));   
		TNamed::Class()->WriteBuffer(R__b, this);
	}
}

Int_t* KVIntegerList::GetTableOfValues(){  
		
	Int_t* tabz = new Int_t[GetMultiplicity()];
	Int_t mm=0;
	for (Int_t ii = fLimiteRegle; ii>=0; ii-=1){
		Int_t contenu = fRegle->At(ii);
		for (Int_t cc=0;cc<contenu;cc+=1)
			tabz[mm++]=ii;
	}
	return tabz;		

}
