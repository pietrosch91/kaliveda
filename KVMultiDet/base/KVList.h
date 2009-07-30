#ifndef KVLIST_H
#define KVLIST_H

#include "KVCollection.h"

class KVList:public KVCollection
{

public:

    KVList(Bool_t owner = kTRUE) : KVCollection("TList")
    {
        // Default ctor.
        // By default, a KVList owns its objects.
        SetOwner(owner);
    };
    virtual ~ KVList(){};

    ClassDef(KVList, 3)  // KV wrapper for ROOT TList classes
};
#endif
