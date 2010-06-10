//Created by KVClassFactory on Mon Nov 30 15:00:00 2009
//Author: John Frankland,,,

#ifndef __KVHASHLIST_H
#define __KVHASHLIST_H

#include "KVSeqCollection.h"
#include "THashList.h"

class KVHashList : public KVSeqCollection
{

   public:
   KVHashList(Int_t capacity = TCollection::kInitHashTableCapacity, Int_t rehash = 2);
   virtual ~KVHashList();

   Float_t AverageCollisions() const;
   void Rehash(Int_t newCapacity = 0);
   TList     *GetListForObject(const char *name) const;
   TList     *GetListForObject(const TObject *obj) const;
    void Sort(Bool_t order = kSortAscending)
    {
    	((THashList*)fCollection)->Sort(order);
    };

   ClassDef(KVHashList,1) //Extended version of ROOT THashList
};

#endif
