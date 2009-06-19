//Created by KVClassFactory on Fri Jun 19 18:51:28 2009
//Author: John Frankland,,,

#ifndef __KVCOLLECTION_H
#define __KVCOLLECTION_H

#include "TCollection.h"

class KVCollection : public TCollection
{
    KVCollection(const KVCollection &) : TCollection() {};    //private and not-implemented, collections
    void operator=(const KVCollection &){}; //are too complex to be automatically copied

    protected:
    TCollection* fCollection;//Pointer to embedded ROOT collection

    public:
    KVCollection();
    KVCollection(const Char_t* collection_classname);
    virtual ~KVCollection();

    virtual void       Add(TObject *obj);
    virtual void       Clear(Option_t *option="");
    virtual void       Delete(Option_t *option="");
    virtual TObject  **GetObjectRef(const TObject *obj) const;
    virtual TIterator *MakeIterator(Bool_t dir = kIterForward) const;
    virtual TObject   *Remove(TObject *obj);

    const Char_t* CollectionClassName() const
    {
        // Return classname of embedded collection object
        return fCollection->ClassName();
    };

   ClassDef(KVCollection,1)//KaliVeda extension to ROOT collections
};

#endif
