//Created by KVClassFactory on Wed Jul  7 12:15:56 2010
//Author: bonnet

#ifndef __KVSIMREADER_HIPSE_H
#define __KVSIMREADER_HIPSE_H

#include "KVSimReader.h"

class KVSimReader_HIPSE : public KVSimReader
{

  	protected:
	TH1F* h1;
	
	public:
   KVSimReader_HIPSE();
   KVSimReader_HIPSE(KVString filename);
	
	virtual ~KVSimReader_HIPSE();

	void init(){
		tree_name = "HIPSE";
		Info("init","%s",branch_name.Data());
		h1 = 0;
	}
	
	virtual void ReadFile();
	virtual Bool_t ReadHeader();
	virtual Bool_t ReadEvent();
	virtual Bool_t ReadNucleus();
	
	void SetPotentialHardness(Double_t val){  
		KVString sval; sval.Form("%lf",val);
		AddObjectToBeWrittenWithTree(new TNamed("Hardness of the potential",sval.Data()));  
	}
	void SetExchangePercentage(Double_t val){  
		KVString sval; sval.Form("%lf",val);
		AddObjectToBeWrittenWithTree(new TNamed("Percentage of exchange",sval.Data()));  
	}
	void SetNNCollisionPercentage(Double_t val){  
		KVString sval; sval.Form("%lf",val);
		AddObjectToBeWrittenWithTree(new TNamed("Percentage of N-N collisions",sval.Data()));  
	}

   ClassDef(KVSimReader_HIPSE,1)//Read ascii file for events of the HIPSE code after clusterization
};

#endif
