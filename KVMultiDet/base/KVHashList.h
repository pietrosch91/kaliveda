//Created by KVClassFactory on Mon Nov 30 15:00:00 2009
//Author: John Frankland,,,

#ifndef __KVHASHLIST_H
#define __KVHASHLIST_H

#include "KVSeqCollection.h"

class KVHashList : public KVSeqCollection
{

   public:
   KVHashList(Int_t capacity = TCollection::kInitHashTableCapacity, Int_t rehash = 0);
   virtual ~KVHashList();

   ClassDef(KVHashList,1) //Extended version of ROOT THashList
};

#endif
