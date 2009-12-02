#ifndef KVLIST_H
#define KVLIST_H

#include "KVSeqCollection.h"

class KVList:public KVSeqCollection
{

public:

    KVList(Bool_t owner = kTRUE);
    virtual ~ KVList();

    ClassDef(KVList, 3)//Extended version of ROOT TList
};
#endif
