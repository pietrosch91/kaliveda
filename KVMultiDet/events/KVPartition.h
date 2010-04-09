//Created by KVClassFactory on Mon Mar 29 14:58:00 2010
//Author: bonnet

#ifndef __KVPARTITION_H
#define __KVPARTITION_H
#include "KVString.h"

class KVNumberList;
class KVEvent;
class KVGenParList;

class KVPartition : public TObject
{
	
	protected:
	
	void AddToRegle(Int_t val);
	void Compute();
	
	Int_t val_max; 
	Int_t mom_max;
	Int_t population;
	
	Int_t* regle;			//[val_max]
	Double_t* moments;	//[mom_max]
	
	Int_t nbre_val_diff;
	Int_t nbre_val;
	
	Int_t* valeurs;			//[nbre_val]
	Int_t* valeurs_diff;		//[nbre_val_diff]
	
	KVString name;
   
	KVGenParList* lgen;	//->
	
	public:
  	
	enum {
      kHastobeComputed = BIT(14)	//Variables has to be recalculated
  	};
	
	virtual void init(Int_t valmax=0,Int_t mommax=0);
	
	KVPartition();
	KVPartition(Int_t valmax,Int_t mommax=5);

   virtual ~KVPartition();
	
	void SetName(KVString snom) { name=snom; }
	const char* GetName() const { return name.Data(); }
	
	const char* GetTitle() const { KVString stit; stit.Form("%d",GetPopulation()); return stit.Data(); }
	
	void Fill(Int_t* tab,Int_t mult);
	void Fill(KVNumberList nl);
	void Fill(KVEvent* evt,Option_t* opt = "");

	void Reset();
	void ResetPopulation() { population=0; }
	void AddOne() { AddPopulation(1); }
	Int_t GetPopulation() const { return population; }
	void AddPopulation(Int_t pop) { population+=pop; }
	
	void Print(Option_t* option = "") const;

	Int_t GetMultDiff(void) const {return nbre_val_diff; }
	Int_t* GetValeurs() const { return valeurs; }
	Int_t GetValeur(Int_t rang) const { return valeurs[rang]; }	
	Int_t GetFrequence(Int_t rang) const { return regle[GetValeur(rang)]; }	
	Bool_t Contains(Int_t valeur) const { return regle[valeur]>0; }	
	
	Double_t GetMoment(Int_t ordre) const {
		return ( (ordre<GetMomentOrdreMax()) ? moments[ordre] : -1 );
	}
	
	Double_t GetMomentNormalise(Int_t ordre) const { return moments[ordre]/GetMoment(0); }	
	Int_t GetMomentOrdreMax(void) const {return mom_max; }
	
	Int_t GetZmax(Int_t rang=0) const;
	Int_t GetZmin(Int_t rang=0) const;
	
	Double_t GetZtot() const {return GetMoment(1); }
	Double_t GetMtot() const {return GetMoment(0); }
	Double_t GetZmean() const {return GetMomentNormalise(1); }
	Int_t GetZ1() const {return GetZmax(0); }
	Int_t GetZ2() const {return GetZmax(1); }
	
	KVGenParList* GetParametersList() const { return lgen; }
	
	Int_t Compare(const TObject* obj) const;
	Int_t CompareMoments(KVPartition* par) const;
	Int_t CompareMult(KVPartition* par) const;
	Int_t CompareValeurs(KVPartition* par) const;
	Int_t CompareName(KVPartition* par) const;
	
	virtual void CalculValeursAdditionnelles();
	
   ClassDef(KVPartition,1)//a partition of integer
};

#endif
