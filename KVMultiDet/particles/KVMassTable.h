//Created by KVClassFactory on Mon Jul 27 11:39:45 2009
//Author: John Frankland,,,

#ifndef __KVMASSTABLE_H
#define __KVMASSTABLE_H

#include "KVBase.h"

class KVMassTable : public KVBase
{
	
   public:
   KVMassTable();
   virtual ~KVMassTable();

   virtual void Initialize()=0;
   virtual Double_t GetMassExcess(Int_t /*Z*/, Int_t /*A*/)=0;
   virtual Bool_t IsKnown(Int_t /*Z*/, Int_t /*A*/)=0;

   ClassDef(KVMassTable,1)//ABC for nuclear mass tables
};

#endif
