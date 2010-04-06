//Created by KVClassFactory on Thu Apr  1 14:11:10 2010
//Author: bonnet

#ifndef __KVPARTITIONMANAGER_H
#define __KVPARTITIONMANAGER_H
#include "KVList.h"

class KVPartition;
class KVString;
class TMethodCall;
class TH1F;

class KVPartitionManager : public KVList
{

   protected:
	KVList* listdepassage;	//->
	Int_t Nmax;
	Int_t nombre_diff, nombre_total;
	void ReduceWithOne(KVList* sl);
	Int_t* GetValues_Long(TMethodCall& meth);	
	Double_t* GetValues_Double(TMethodCall& meth);

	public:
   KVPartitionManager();
   virtual ~KVPartitionManager();
	void init();
	void CreationSousListe();
	
	void SetNmax(Int_t nmax){ Nmax=nmax; }
	Int_t GetNmax() {return Nmax;}
	
	Bool_t TestPartition(KVPartition* par);
	void ReduceSubLists();
	void TransfertToMainList();
	void UpdateCompteurs(KVList* current=0);
	
	Int_t GetNombrepartitionsTotale() {return nombre_total;}
	Int_t GetNombrepartitionsDifferentes() {return nombre_diff;}
	
	KVList* GetIntermediate() {return listdepassage;}
	
	virtual void Reset();
	
	Int_t* GetIndex(KVString method, Bool_t down = kTRUE);
	TH1F* GenereHisto(KVString method,Int_t nb,Double_t min,Double_t max);
	
   ClassDef(KVPartitionManager,1)//Count, Store and Sort partitions
};

#endif
