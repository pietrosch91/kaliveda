//Created by KVClassFactory on Fri Jul  2 15:16:15 2010
//Author: bonnet

#ifndef __KVSIMREADER_HIPSE_H
#define __KVSIMREADER_HIPSE_H

#include "KVSimReader.h"
#include "KVNameValueList.h"

class KVSimReader_HIPSE : public KVSimReader
{

	
	public:
   KVSimReader_HIPSE();
	KVSimReader_HIPSE(KVString filename);
	
   virtual ~KVSimReader_HIPSE();
	
	void init(){
		tree_name = "HIPSE";
		Info("init","%s",branch_name.Data());
	}

	virtual void ReadFile();
	virtual Bool_t ReadHeader();
	virtual Bool_t ReadEvent();
	virtual Bool_t ReadNucleus();

   ClassDef(KVSimReader_HIPSE,1)//Reading class of HIPSE generator
};

#endif
