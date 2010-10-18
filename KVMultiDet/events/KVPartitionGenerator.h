//Created by KVClassFactory on Thu Jun 17 14:20:16 2010
//Author: bonnet

#ifndef __KVPartitionGenerator_H
#define __KVPartitionGenerator_H
#include "KVString.h"

class TTree;
class TFile;
class TList;
class TEventList;
class KVIntegerList;

class KVPartitionGenerator
{

   public:
   KVPartitionGenerator();
   void SetConditions(Int_t Zfrag,Int_t Mfrag,Int_t Zinf);
	
	Int_t ndim;
	Int_t mshift;
	Int_t zshift;
	Bool_t to_be_checked;
	
	Int_t mtot,ztot;
	Int_t* tabz;	//[ndim]
	
	Int_t kzf,kmf,kzm;
	Int_t* kcurrent;
	
	TFile* file;
	TTree* tree;
	TList* levt;
	TEventList* evt;
	TList* levt_collect;
	TList* levt_z1;
	
	Int_t bsup;
	Int_t binf;

	UInt_t npar;
	UInt_t npar_zf_mf;
	
	KVIntegerList* nl_zf;
	KVIntegerList* nl_mf;
	KVIntegerList* nl_zm;
	KVIntegerList* nl_z1;
	
	virtual ~KVPartitionGenerator();
	
	void Process();
	
	void Break_Using_Zf_Zinf_Criterion(Int_t Zfrag,Int_t Zinf);
	void Break_Using_Zf_Zmax_Zinf_Criterion(Int_t Zfrag,Int_t Zmax,Int_t Zinf);
   void Break_Using_Mf_Zmax_Zinf_Criterion(Int_t Mfrag,Int_t Zmax,Int_t Zinf);
	void Break_Using_Zf_Mf_Zinf_Criterion(Int_t Zfrag,Int_t Mfrag,Int_t Zinf);
	
	void TreatePartition();
	void SaveAndCloseFile();
	void WriteInfo();
	
	void PreparTree(const Char_t* filename,const Char_t* treename,Int_t Ndim=1000,Option_t* option="recreate");
	ClassDef(KVPartitionGenerator,1)//Calcul numeriquement toutes les partitions d'un couple Zfrag/Mfrag donne
};

#endif
