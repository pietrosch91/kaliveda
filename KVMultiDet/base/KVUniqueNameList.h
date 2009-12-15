//Created by KVClassFactory on Thu Dec  3 15:49:33 2009
//Author: John Frankland,,,

#ifndef __KVUNIQUENAMELIST_H
#define __KVUNIQUENAMELIST_H

#include "KVHashList.h"

class KVUniqueNameList : public KVHashList
{

   public:
   KVUniqueNameList();
   virtual ~KVUniqueNameList();
    virtual void      AddFirst(TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(!FindObject(obj->GetName())) KVHashList::AddFirst(obj);
    };
    virtual void      AddLast(TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(!FindObject(obj->GetName())) KVHashList::AddLast(obj);
    };
    virtual void      AddAt(TObject *obj, Int_t idx)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(!FindObject(obj->GetName())) KVHashList::AddAt(obj,idx);
    };
    virtual void      AddAfter(const TObject *after, TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(!FindObject(obj->GetName())) KVHashList::AddAfter(after,obj);
    };
    virtual void      AddBefore(const TObject *before, TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(!FindObject(obj->GetName())) KVHashList::AddBefore(before,obj);
    };
    virtual void       Add(TObject *obj)
    {
       // Add an object to the list if it is not already in it
       // (no object with same name in list)
       
       if(!FindObject(obj->GetName())) KVHashList::Add(obj);
    };

   ClassDef(KVUniqueNameList,1)//Optimised list in which objects can only be placed once
};

#endif
