//Created by KVClassFactory on Thu Jun 17 14:20:16 2010
//Author: bonnet

#ifndef __KVPartitionGenerator_H
#define __KVPartitionGenerator_H
#include "KVString.h"

class KVPartition;
class TFile;
class TTree;

class KVPartitionGenerator
{

   public:
   KVPartitionGenerator();
   void SetConditions(Int_t Zfrag,Int_t Mfrag,Int_t Zinf);
	
	KVPartition* par;
	TFile* file;
	TTree* tt;
	
	Int_t mshift;
	Int_t zshift;
	Bool_t to_be_checked;
	
	Int_t mtot,ztot;
	Int_t* tabz;	//[mtot]
	
	Int_t kzf,kmf,kzm;
	Int_t* kcurrent;
	
	virtual ~KVPartitionGenerator();
	
	void DefineTree(KVString file_name);
	
	void Process(KVString file_name="test.root");
	
	void Test_Zf_Zinf(Int_t Zfrag,Int_t Zinf);
	void Test_Zf_Zmax_Zinf(Int_t Zfrag,Int_t Zmax,Int_t Zinf);
   void Test_Mf_Zmax_Zinf(Int_t Mfrag,Int_t Zmax,Int_t Zinf);
	void Test_Zf_Mf_Zinf(Int_t Zfrag,Int_t Mfrag,Int_t Zinf);
	
	ClassDef(KVPartitionGenerator,1)//Calcul numeriquement toutes les partitions d'un couple Zfrag/Mfrag donne
};

#endif
