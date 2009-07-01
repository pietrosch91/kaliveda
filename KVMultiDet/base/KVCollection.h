//Created by KVClassFactory on Fri Jun 19 18:51:28 2009
//Author: John Frankland,,,

#ifndef __KVCOLLECTION_H
#define __KVCOLLECTION_H

#include "TCollection.h"
#include <RQ_OBJECT.h>

class KVCollection : public TCollection
{
    RQ_OBJECT("KVList")

    enum
    {
        kSignals = BIT(14) // bit flag for sending 'Modified()' signal on changes
    };

    KVCollection(const KVCollection &) : TCollection() {};    //private and not-implemented, collections
    void operator=(const KVCollection &){}; //are too complex to be automatically copied

protected:
    TCollection* fCollection;//Pointer to embedded ROOT collection

    virtual void Changed()
    {
        // Called by Add/RemoveObject, Clear and Delete.
        // We make it emit the "Modified()" signal, if
        // the kSignals bit has been set using SendModifiedSignals(kTRUE).

        if (TestBit(kSignals)) Modified();
    };
    virtual void	PrintCollectionHeader(Option_t* option) const;

public:
    KVCollection();
    KVCollection(const Char_t* collection_classname);
    virtual ~KVCollection();

    virtual void Modified()
    {
        Emit("Modified()");
    };  // *SIGNAL*
    virtual void SendModifiedSignals(Bool_t yes = kTRUE)
    {
        // yes=kTRUE: turns on signals-and-slots mechanism, list will
        //           emit 'Modified()' signal every time a change
        //           occurs (objects added or removed).
        // yes=kFALSE: turns off signals-and-slots mechanism
        //
        // By default, the 'Modified()' signal is NOT enabled.
        SetBit(kSignals,yes);
    };
    virtual Bool_t IsSendingModifiedSignals()
    {
        // returns kTRUE if 'Modified()' signal is active
        // (see SendModifiedSignals).
        return TestBit(kSignals);
    };

    virtual TObject*	At(Int_t idx) const;
    virtual void       Add(TObject *obj);
    virtual void       Clear(Option_t *option="");
    virtual void       Delete(Option_t *option="");
    virtual TObject  **GetObjectRef(const TObject *obj) const;
    virtual TIterator *MakeIterator(Bool_t dir = kIterForward) const;
    virtual TObject   *Remove(TObject *obj);
    virtual Int_t GetSize() const
    {
        return fCollection->GetSize();
    };

    const Char_t* CollectionClassName() const
    {
        // Return classname of embedded collection object
        return fCollection->ClassName();
    };

    virtual TObject *FindObjectByName(const Char_t *name) const
    {
        return fCollection->FindObject(name);
    };
    virtual TObject *FindObjectByType(const Char_t *) const;
    TObject *FindObjectByClass(const Char_t *) const;
    TObject *FindObjectByClass(const TClass *) const;
    virtual TObject *FindObjectByLabel(const Char_t *) const;
    virtual TObject *FindObjectByNumber(UInt_t num) const;
    virtual TObject *FindObjectWithNameAndType(const Char_t * name, const Char_t * type) const;
    virtual TObject *FindObjectWithMethod(const Char_t* retvalue,const Char_t* method) const;

    ClassDef(KVCollection,1)//KaliVeda extensions to ROOT collections
};

#endif
