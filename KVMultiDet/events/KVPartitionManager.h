//Created by KVClassFactory on Thu Apr  1 14:11:10 2010
//Author: bonnet

#ifndef __KVPARTITIONMANAGER_H
#define __KVPARTITIONMANAGER_H
#include "KVList.h"
#include "KVGenParList.h"

class KVIntegerList;
class KVString;
class TMethodCall;
class TH1F;
class TTree;
class TStopwatch;

class KVPartitionManager : public KVList
{

   protected:
	KVList* listdepassage;	//->
	Int_t Nmax;
	Int_t nombre_diff, nombre_total;
	void ReduceWithOne(KVList* sl);
	/*
	Int_t* GetValues_Long(TMethodCall& meth);	
	Double_t* GetValues_Double(TMethodCall& meth);
	*/
	KVGenParList lgen;
	Bool_t kcheck;
	
	public:
   KVPartitionManager();
	KVPartitionManager(KVString option);
   virtual ~KVPartitionManager();
	void init();
	void CreationSousListe();
	void SetFillingMode(KVString opt){
		if (opt=="Check") kcheck = kTRUE;
		else 					kcheck = kFALSE;
	}
	void SetNmax(Int_t nmax){ Nmax=nmax; }
	Int_t GetNmax() {return Nmax;}
	
	Bool_t TestPartition(KVIntegerList* par);
	Bool_t FillPartition(KVIntegerList* par);
	Bool_t Fill(KVIntegerList* par);
	
	void ReduceSubLists();
	void TransfertToMainList();
	void UpdateCompteurs(KVList* current=0);
	void PrintInfo();
	
	Int_t GetNombrepartitionsTotale() {return nombre_total;}
	Int_t GetNombrepartitionsDifferentes() {return nombre_diff;}
	
	KVList* GetIntermediate() {return listdepassage;}
	
	KVIntegerList* GetPartition(Int_t kk){  return (KVIntegerList* )this->At(kk); }
	KVIntegerList* FindPartition(const char* name){  return (KVIntegerList* )this->FindObject(name); }
	
	virtual void Reset();
	
	/*
	Int_t* GetIndex(KVString method, Bool_t down = kTRUE);
	TH1F* GenereHisto(KVString method,Int_t nb,Double_t min,Double_t max);
	TTree* GenereTree(KVString tree_name,Bool_t Compress=kTRUE,Bool_t AdditionalValues=kTRUE);
	*/
	TTree* GenereTree(KVString treename,Bool_t Compress=kTRUE);
	void SaveAsTree(KVString filename,KVString treename,Bool_t Compress=kTRUE,Option_t* option = "recreate");
	void SaveAsList(KVString filename,KVString listname,Option_t* option = "recreate"){
			
	
	
	}
	
   ClassDef(KVPartitionManager,1)//Permet d'enregistrer, de classer et compter des partitions d'entiers via la classe KVPartition
};

#endif
