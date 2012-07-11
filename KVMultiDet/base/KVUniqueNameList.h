//Created by KVClassFactory on Thu Dec  3 15:49:33 2009
//Author: John Frankland,,,

#ifndef __KVUNIQUENAMELIST_H
#define __KVUNIQUENAMELIST_H

#include "KVHashList.h"

class KVUniqueNameList : public KVHashList
{
   Bool_t fReplace;// if kTRUE, objects with same name are replaced
   
   Bool_t checkObjInList(TObject* obj);
   
   public:
   KVUniqueNameList(Bool_t R=kFALSE);
   virtual ~KVUniqueNameList();
    virtual void      AddFirst(TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(checkObjInList(obj)) KVHashList::AddFirst(obj);
    };
    virtual void      AddLast(TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(checkObjInList(obj)) KVHashList::AddLast(obj);
    };
    virtual void      AddAt(TObject *obj, Int_t idx)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(checkObjInList(obj)) KVHashList::AddAt(obj,idx);
    };
    virtual void      AddAfter(const TObject *after, TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(checkObjInList(obj)) KVHashList::AddAfter(after,obj);
    };
    virtual void      AddBefore(const TObject *before, TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(checkObjInList(obj)) KVHashList::AddBefore(before,obj);
    };
    virtual void       Add(TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(checkObjInList(obj)) KVHashList::Add(obj);
    };

   ClassDef(KVUniqueNameList,2)//Optimised list in which objects can only be placed once
};

#endif
