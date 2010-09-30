//Created by KVClassFactory on Mon Mar 29 14:58:00 2010
//Author: bonnet

#ifndef __KVINTEGERLIST_H
#define __KVINTEGERLIST_H
#include "KVString.h"
#include "TArrayI.h"
#include "TNamed.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TMath.h"

class KVEvent;
class KVPartition;

class KVIntegerList : public TNamed
{
	
	protected:
	
	TArrayI* fRegle;	//!
	TArrayI* fValues;	//!
	Int_t fLimiteRegle;	//!
	Int_t fPop;	//!
	Int_t fMult;//!
	Ssiz_t fLength; //!
	
	virtual void init();
	Bool_t ToBeUpdated() { return TestBit(kHastobeComputed); } 
	virtual void Update();
	virtual void EraseRegle(){ if (fRegle) delete fRegle; fRegle=0; } 
	virtual void EraseValues(){ if (fValues) delete fValues; fValues=0; } 
	
	virtual void DeducePartitionFromTNamed(){
		DeducePartitionFromName();
		DeducePopulationFromTitle();
	}
	
	virtual void DeducePopulationFromTitle(){
		KVString st(GetTitle());
		SetPopulation(st.Atoi());
	}
	
	virtual void DeducePartitionFromName(){
		SetPartition(GetName());
	}
	
	virtual void SetPartition(const Char_t* par);
	
	public:
	
	enum {
      kHastobeComputed = BIT(14)	//Variables has to be recalculated
  	};
	
	KVIntegerList();
	virtual ~KVIntegerList();
	
	Int_t Compare(KVIntegerList* par) {
		
		/*
		CheckForUpdate();
		par->CheckForUpdate();
		*/
		const char *s1 = this->GetName();
   	const char *s2 = par->GetName();
   	
		Ssiz_t len1 = this->GetLengthName();
   	
		if (len1 != par->GetLengthName()) return -1;
   	
		Int_t ii=0;
		while (s1[ii]==s2[ii]){
			if (ii==len1) return 0;
			ii+=1;
		}
		return -1;
	}
	
	virtual void ResetPartition(){ 	
		if (fRegle) {
			for (Int_t ii=0;ii<fRegle->GetSize();ii+=1)
				fRegle->AddAt(0,ii);
			SetBit(kHastobeComputed,kTRUE);
		}
	}
	
	void CheckForUpdate() {
		if (ToBeUpdated()) 
			Update();
	}
	void Fill(Int_t* tab,Int_t mult);
	void Fill(Double_t* tab,Int_t mult);
	void Fill(KVEvent* evt,Option_t* opt);
	
	void Add(Int_t* tab,Int_t mult){
		for (Int_t ii=0;ii<mult;ii+=1)
			Add(tab[ii]);
	}
	
	void Add(Double_t* tab,Int_t mult){
		for (Int_t ii=0;ii<mult;ii+=1)
			Add(tab[ii]);
	}
	
	void Add(Int_t val) { Add(val,1); }
	void Add(Int_t val,Int_t freq);
	
	void Add(Double_t val) { Add(TMath::Nint(val),1); }
	void Add(Double_t val,Int_t freq){ Add(TMath::Nint(val),freq); }
	
	Bool_t Remove(Int_t val);
	
	//void Copy(TObject& obj) const {}

	void ResetPopulation() { fPop=0; }
	Int_t GetPopulation() const { return fPop; }
	void AddPopulation(Int_t pop) { fPop += pop; }
	void SetPopulation(Int_t pop) { fPop = pop; }
	
	Int_t GetNbre() {/*CheckForUpdate();*/ return fMult;}
	TArrayI* GetTableOfValues();
	void SetTableOfValues();
	
	Int_t GetFrequencyOf(Int_t val){
		CheckForUpdate();
		return (val<=fLimiteRegle ? fRegle->At(val) : -1);
	}
	Bool_t Contains(Int_t val){
		return (GetFrequencyOf(val)>0);
	}
	
	const char* GetPartitionName(){
		CheckForUpdate();
		return GetName();
	}
	
	Ssiz_t GetLengthName(){
		return fLength;
	}
	
	void Reduce(){
		EraseRegle();
		EraseValues();
		fLimiteRegle=0;
	}
	
	void PrintPartition(){
		Info("PrintPartition","%s : population %d",GetPartitionName(),GetPopulation());
	}
	
	KVPartition* CreateKVPartition(Int_t mom_max=5);
	TNamed* CreateTNamed();
	
	
	ClassDef(KVIntegerList,1)//Permet de gerer des partitions de nombres entiers
};

#endif
