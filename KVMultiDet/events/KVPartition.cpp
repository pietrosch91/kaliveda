//Created by KVClassFactory on Mon Mar 29 14:58:00 2010
//Author: bonnet

#include "KVPartition.h"
#include "TMath.h"
#include "Riostream.h"
#include "KVNumberList.h"
#include "KVEvent.h"
#include "KVNucleus.h"
#include "KVNameValueList.h"

ClassImp(KVPartition)

////////////////////////////////////////////////////////////////////////////////
/*
BEGIN_HTML
<h2>KVPartition</h2>
<h4>Permet de gerer des partitions de nombres entiers et le calcul de grandeurs associees</h4>
END_HTML
Cette classe trouve une application dans la gestion de partition de charges 
BEGIN_LATEX
{Z_{i},i=1,M_{tot}}
END_LATEX
Elle a deux parametres d'entree qui sont accessibles par le constructeur
Il s'agit de :
- la taille maximale du systeme a partitionner (val_max) et/ou de la taille du plus gros cluster
- l'ordre supérireure des moments (mommax) qui seront calcules automatiquement KVPartition::Compute()
BEGIN_LATEX
Moment d'ordre n : M_{n} = \Sigma_{i=1,Mtot} P( Z_{i} ) * Z_{i}^{ n}
END_LATEX

Les methodes type Fill...(...) permettent de remplir et de classer la partition dans le meme temps
A chaque appel de ces methodes les contenus de la classe sont reinititialises et recalcules

Voici un petit exemple pour remplir une KVPartition de deux facons

BEGIN_HTML
<pre>
<code>
void test()
{
Int_t* taInt_t* tab=0;

KVPartition* par = new KVPartition(100,3);
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
KVPartition::CalculValeursAdditionnelles() en la derivant dans une classe fille
Ces grandeurs additionnelles sont stockées dans une KVGenParList
pour y acceder on peut utiliser les methodes GetValeursEnPlus(...)
*/
////////////////////////////////////////////////////////////////////////////////

void KVPartition::init(Int_t valmax,Int_t mommax)
{
	
	val_max=valmax;
	mom_max=mommax;
	
	population=0;
	
	regle = new Int_t[val_max+1];			for (Int_t mm=0; mm<=val_max; mm+=1)	regle[mm]=0;
	moments = new Double_t[mom_max+1];	for (Int_t mm=0; mm<=mom_max; mm+=1)	moments[mm]=0.;
	
	nbre_val_diff=0;
	nbre_val=0;
	
	valeurs_diff=0;
	valeurs=0;
	
	//SetBit(kCanDelete,kTRUE);
	
	lgen = new KVNameValueList();
	SetName("");
}

KVPartition::KVPartition(Int_t valmax,Int_t mommax)
{
   // Default constructor
	init(valmax,mommax);
}

KVPartition::KVPartition()
{
   // Default constructor
	init();
}

KVPartition::~KVPartition()
{
   // Destructor
	Info("~KVPartition()","Je suis dans le destructeur");
	Reset();
	
	delete [] regle;		regle=0;
	delete [] moments;	moments=0;
	delete lgen; 			lgen=0;

}

void KVPartition::Copy(TObject & obj) const{

	if  ( ((KVPartition &) obj).GetMomMax()!=GetMomMax() || ((KVPartition &) obj).GetValMax()!=GetValMax() )
	{
		delete ((KVPartition &) obj).regle; 
		delete ((KVPartition &) obj).moments;
		delete ((KVPartition &) obj).lgen;
		 
		((KVPartition &) obj).init(GetValMax(),GetMomMax());
	}
	
	((KVPartition &) obj).FillWithRegle(regle,GetValMax());

}

void KVPartition::Compute(){
	
	valeurs = new Int_t[nbre_val];   
	valeurs_diff = new Int_t[nbre_val_diff]; 
	
	ComputeValues();
	
	SetBit(kHastobeComputed,kFALSE);
	
	AddOne();

}

void KVPartition::ComputeValues(){
	
	
	KVString snom,stamp;
	Int_t mdiff=0,mtot=0;
	//Boucle sur les valeurs de 0 a val_max
	for (Int_t nn=val_max; nn>=0; nn-=1){
		if (regle[nn]>0){
			//Calcul des moments
			for (Int_t ordre=0; ordre<=mom_max; ordre+=1)	
				moments[ordre] += regle[nn] * TMath::Power(nn,ordre);
			
			//Formattage du nom de la partition
			stamp.Form("%d",nn);	snom += stamp;
			if (regle[nn]>1){
				stamp.Form("(%d)",regle[nn]);
				snom += stamp; 
			}
			
			//Enregistrement des valeurs
			//dans les deux tableaux 
			valeurs_diff[mdiff++]=nn;
			for (Int_t mm=0;mm<regle[nn];mm+=1){
				valeurs[mtot++]=nn;
			}
			snom+=" ";
		}
	}

	SetName(snom);
	
	CalculValeursAdditionnelles();

}

void KVPartition::Fill(Int_t* tab,Int_t mult){

	if (!tab) return;
	Reset();
	
	for (Int_t mm=0;mm<mult;mm+=1)
		AddToRegle(tab[mm]);
	
	Compute();

}

void KVPartition::Fill(Double_t* tab,Int_t mult){

	if (!tab) return;
	Reset();
	
	for (Int_t mm=0;mm<mult;mm+=1)
		AddToRegle(TMath::Nint(tab[mm]));
	
	Compute();

}

void KVPartition::FillWithConditions(Int_t* tab,Int_t mult,Int_t zmin,Int_t zmax){

	if (!tab) return;
	Reset();
	Bool_t select_min = ( (zmin==-1) ? kFALSE : kTRUE);
	Bool_t select_max = ( (zmax==-1) ? kFALSE : kTRUE);
	
	for (Int_t mm=0;mm<mult;mm+=1)
		if ( ( !select_min || (select_min && tab[mm]>=zmin) ) && ( !select_max || (select_max && tab[mm]<=zmax) ) ) 
			AddToRegle(tab[mm]);
	
	Compute();

}

void KVPartition::Fill(KVNumberList nl){

	Reset();
	
	Int_t mult;
	Int_t* tab = nl.GetArray(mult);
	for (Int_t mm=0;mm<mult;mm+=1)
		AddToRegle(tab[mm]);
	
	delete [] tab;
	
	Compute();

}

void KVPartition::Fill(KVEvent* evt,Option_t* opt){

	if (!evt) return;
	
	Reset();
	
	KVNucleus* nuc=0;
	while ( (nuc = (KVNucleus* )evt->GetNextParticle(opt)) ) 
		AddToRegle(nuc->GetZ());
	
	Compute();

}

void KVPartition::FillWithRegle(Int_t* regl,Int_t vmax){

	Reset();
	
	for (Int_t mm=0;mm<=vmax;mm+=1)
		for (Int_t nb=0;nb<regl[mm];nb+=1)
			AddToRegle(mm);
	
	Compute();

}

void KVPartition::AddToRegle(Int_t val){
	if (regle[val]==0)
		nbre_val_diff+=1;
	nbre_val+=1;
	
	regle[val]+=1;
}

void KVPartition::Reset(){

	for (Int_t mm=0;mm<nbre_val_diff;mm+=1){
		regle[valeurs_diff[mm]]=0;
	}
	
	if (valeurs_diff) delete [] valeurs_diff; valeurs_diff=0;
	if (valeurs) 		delete [] valeurs;		valeurs=0;
	
	nbre_val_diff=0;
	nbre_val=0;
	
	ResetMoments();
	ResetPopulation();
	SetName("");

}

void KVPartition::ResetMoments(){
	for (Int_t mm=0;mm<=mom_max;mm+=1) 
		moments[mm]=0;
}

void KVPartition::Print(Option_t* option) const {

	if (!strcmp(option,"Moments")) {
		Info("Print","Moments #Sigma Z^{ordre}");
		printf("Nombres de moments calcules %d\n",mom_max);
		for (Int_t mm=0;mm<=mom_max;mm+=1){
			printf("Moments d'ordre %d -> %1.0lf\n",mm,moments[mm]);
		}
	}
	else if (!strcmp(option,"Partition")) {
		Info("Print","Partitions");
		printf("Multiplicite %1.0lf\n",moments[0]);
		printf("Nombre de charges differentes %d\n",nbre_val_diff);
		for (Int_t mm=0;mm<nbre_val_diff;mm+=1){
			printf("%d",valeurs_diff[mm]);
			if (regle[valeurs_diff[mm]]>1)
				printf("(%d)",regle[valeurs_diff[mm]]);
			printf(" ");	
		}
		printf("\n");
		
	}
	else {
		printf("KVPartition : %s Population %s\n",GetName(),GetTitle());
	}

}


Int_t KVPartition::Compare(const TObject* obj) const {

	return CompareName( (KVPartition* )obj );

}

Int_t KVPartition::CompareMoments(KVPartition* par) const {
	//renvoie 1 si les moments sont identiques 0 sinon
	//Attention la comparaison ne s'eefectue que jusqu'a l'ordre max defini
	//pour les deux partitions
	Int_t ordremax = TMath::Min(par->GetMomentOrdreMax(),this->GetMomentOrdreMax());
	for (Int_t ord=0; ord<=ordremax; ord+=1){
		if (par->GetMoment(ord) != this->GetMoment(ord)) 
			return 0;
	}
	return 1;

}

Int_t KVPartition::CompareMult(KVPartition* par) const {

	//renvoie 1 si toutes les multiplicites sont identiques 0 sinon
	return Int_t(par->GetMult()==this->GetMult() && par->GetMultDiff()==this->GetMultDiff());

}

Int_t KVPartition::CompareValeurs(KVPartition* par) const {

	//renvoie 1 si toutes les valeurs sont identiques 0 sinon
	if (this->CompareMult(par)==0) return 0;
	for (Int_t mm=0; mm<this->GetMultDiff(); mm+=1){
		Int_t m1=this->GetValeurDiff(mm);
		Int_t m2=par->GetValeurDiff(mm);
		if ( (m1 != m2) || (this->GetFrequenceOf(m1) != par->GetFrequenceOf(m2)) ){
			return 0;	
		}
	}
	return 1;
}

Int_t KVPartition::CompareName(KVPartition* par) const {

	//renvoie 1 si nom identique 0 sinon
	if ( !strcmp(this->GetName(),par->GetName()) ) return 1;
	else return 0;
	
}

void KVPartition::CalculValeursAdditionnelles(){
	
	//Exemple de valeurs additionnelles
	//qui peuvent etre calculees en plus
	//cette routine peut etre redefinie dans les classes filles
	Double_t m1 = GetMomentNormalise(1); 
	Double_t m2 = GetMomentNormalise(2); 
	Double_t m3 = GetMomentNormalise(3); 
	Double_t m4 = GetMomentNormalise(4); 
	
	lgen->SetValue("Mean",m1);
	lgen->SetValue("RMS",TMath::Sqrt( m2 - TMath::Power(m1,2.) ) ); //Same as TH1::GetRMS()
	Double_t rms = lgen->GetDoubleValue("RMS");
	//Same as TH1::GetSkewness()
	lgen->SetValue("Skewness",(m3 - 3*m1*m2 +2*TMath::Power(m1,3.))/TMath::Power(rms,3.) );
	//Same as TH1::GetKurtosis()+3
	lgen->SetValue("Kurtosis",(m4 - 4*m1*m3 +6*TMath::Power(m1,2.)*m2 -3*TMath::Power(m1,4.))/TMath::Power(rms,4.) );
	
	lgen->SetValue("GenAsym",( (GetMoment(0)>1) ? (1./TMath::Sqrt(GetMoment(0)-1.))*(rms/m1) : -1.) );
	
	lgen->SetValue("Z1",GetZ1());
	lgen->SetValue("Z2",GetZ2());
	lgen->SetValue("Z3",GetZmax(2));
	
	lgen->SetValue("Ztot",GetZtot());
	lgen->SetValue("Mdiff",GetMultDiff());
	
}

Double_t  KVPartition::GetZmax(Int_t rang) const {
	//Donne le 1er, 2eme ... plus gros de la partition
	//Attention notation C++ 
	// - le premier -> 0
	// - le dernier -> N-1 ou N est la multiplicite
	
	return ( (rang<GetMoment(0)) ? Double_t(GetValeur(rang)) : -1. );

}

Double_t  KVPartition::GetZmin(Int_t rang) const {
	//Donne le 1er, 2eme ... plus petit de la partition
	//Attention notation C++ 
	// - le premier -> 0
	// - le dernier -> N-1 ou N est la multiplicite

	Int_t inverse = Int_t(GetMoment(0))-1-rang;
	return ( (inverse<GetMoment(0)) ? Double_t(GetValeur(inverse)) : -1. );

}

Bool_t KVPartition::RemoveAt(Int_t rang) {

	//return kTRUE if the indicated rank is occupied
	Int_t val = Int_t(GetZmax(rang));
	if (val==-1) return kFALSE;
	
	regle[val]-=1;
	if (regle[val]==0) {	nbre_val_diff-=1; }
	nbre_val-=1;
	
	ResetMoments();
	ComputeValues();
	
	return kTRUE;

}

Bool_t KVPartition::RemoveValue(Int_t val) {

	//return kTRUE if the indicated value belong to the partition
	if (regle[val]>0){
		Int_t nval = regle[val];
		regle[val] = 0;
		nbre_val -= nval;
		nbre_val_diff -= 1;
	}
	else return kFALSE;
	
	ResetMoments();
	ComputeValues();
	
	return kTRUE;

}

Double_t KVPartition::GetValeursEnPlus(KVString sname){

	return GetParametersList()->GetDoubleValue(sname.Data());

}

Double_t KVPartition::GetValeursEnPlus(const Char_t* sname){

	return GetParametersList()->GetDoubleValue(sname);

}
