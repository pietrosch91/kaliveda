/*
$Id: KVINDRAPulserDataTree.h,v 1.4 2009/03/26 16:33:01 franklan Exp $
$Revision: 1.4 $
$Date: 2009/03/26 16:33:01 $
*/

//Created by KVClassFactory on Wed Jan 21 11:56:26 2009
//Author: franklan

#ifndef __KVINDRAPULSERDATATREE_H
#define __KVINDRAPULSERDATATREE_H

#include "KVBase.h"
#include "TTree.h"
#include "THashTable.h"
#include "Riostream.h"
#include "KVString.h"
class TFile;

class KVINDRAPulserDataTree : public KVBase
{
	protected:
	TTree* fArb;//!tree containing pulser data
	Bool_t fHaveGeneData;//!true if gene data directory found
	Bool_t fHavePinData;//!true if pin data directory found
	KVString fPathGeneDataDir;//!full path to gene data directory
	KVString fPathPinDataDir;//!full path to pin data directory
	Bool_t fGeneTGZ;//!set to kTRUE if gene data is extracted from '.tgz' archive
	Bool_t fPinTGZ;//!set to kTRUE if pin data is extracted from '.tgz' archive
	Int_t fRun;//!run number used to build tree
	Float_t* fVal;//!array of floats used to fill tree
	Int_t fTab_siz;//!size of array
	THashTable *fIndex;//!associate name of branch to index in fVal
	TList* fRunlist;//!list of runs given by database

	Bool_t CheckDirectory(const Char_t*, KVString&);
	void CheckDirectories();
	void DeleteDirectories();
	void CreateTree();
	void ReadData();
	void ReadData(Int_t);
	void ReadFile(ifstream&);
	Bool_t OpenGeneData(Int_t , ifstream &);
	Bool_t OpenPinData(Int_t , ifstream &);
		
   public:
   KVINDRAPulserDataTree();
   virtual ~KVINDRAPulserDataTree();
	
	virtual void Build();
	TTree* GetTree() const { return fArb; };
	
	void ReadTree(TFile *);
	void WriteTree(TFile *);
	
	Int_t GetIndex(const Char_t* branchname) const
	{
		if(fIndex){
			KVBase* iob = (KVBase*)fIndex->FindObject(branchname);
			if(iob) return (Int_t)iob->GetNumber();
		}
		return -1;
	};
	void SetRunList(TList*runs) { fRunlist = runs; };
	
	Float_t GetMean(const Char_t*, Int_t);

   ClassDef(KVINDRAPulserDataTree,1)//Handles TTree with mean pulser data for every run
};

#endif
