/***************************************************************************
                          kvlist.h  -  description
                             -------------------
    begin                : Sat May 18 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVList.h,v 1.21 2009/01/20 15:15:54 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVLIST_H
#define KVLIST_H

#define KVLIST_NOT_WITH_THIS_TYPE "No object found with type %s"
#define KVLIST_NOT_WITH_THIS_NAME_AND_TYPE "No object found with name %s and type %s"

#include "TList.h"
#include "TFile.h"
#include "KVConfig.h"

class KVBase;

class KVList:public TList {
 public:
   KVList(Bool_t owner = kTRUE);
   KVList(const KVList &);
    virtual ~ KVList();

   virtual TObject *FindObjectAny(const Char_t *att, const Char_t *keys, Bool_t contains_all=kFALSE, Bool_t case_sensitive=kTRUE) const;
   
   virtual KVBase *FindObjectByName(const Char_t *);
   virtual KVBase *FindObjectByType(const Char_t *);
   virtual KVBase *FindObjectByClass(const Char_t *);
   virtual KVBase *FindObjectByClass(const TClass *);
   virtual KVBase *FindObjectByLabel(const Char_t *);
   virtual KVBase *FindObject(UInt_t num);
   virtual KVBase *FindObject(KVBase * obj);
   virtual KVBase *FindObject(const Char_t * name, const Char_t * type);
   virtual TObject *FindObject(const char *name) const {
      return TList::FindObject(name);
   };
   virtual TObject *FindObject(const TObject * name) const {
      return TList::FindObject(name);
   };

	virtual TObject *FindObjectWithMethod(const Char_t* retvalue,const Char_t* method);
   
	virtual KVList *GetSubListWithMethod(const Char_t* retvalue,const Char_t* method);
	
	virtual KVList *GetSubListWithClass(const TClass* _class);
	virtual KVList *GetSubListWithClass(const Char_t* class_name);
	
	virtual KVList *GetSubListWithName(const Char_t* retvalue);
	virtual KVList *GetSubListWithLabel(const Char_t* retvalue);
	virtual KVList *GetSubListWithType(const Char_t* retvalue);
	
	static KVList* MakeListFromFile(TFile *file);	
	static KVList* MakeListFromFileWithMethod(TFile *file,const Char_t* retvalue,const Char_t* method);	
	static KVList* MakeListFromFileWithClass(TFile *file,const TClass* _class);
	static KVList* MakeListFromFileWithClass(TFile *file,const Char_t* class_name);
	
	virtual void Execute(const char *method, const char *params,
                        Int_t * error = 0);
   virtual void Execute(TMethod * method, TObjArray * params,
                        Int_t * error = 0) {
      TObject::Execute(method, params, error);
   };

   virtual void Print(Option_t * opt = "") const;
#ifdef __WITH_NEW_TCOLLECTION_PRINT
   virtual void       Print(Option_t *option, Int_t recurse) const {TCollection::Print(option,recurse);};
   virtual void       Print(Option_t *option, const char* wildcard, Int_t recurse=1) const{TCollection::Print(option,wildcard,recurse);};
   virtual void       Print(Option_t *option, TPRegexp& regexp, Int_t recurse=1) const{TCollection::Print(option,regexp,recurse);};
#else
#ifdef __WITH_TCOLLECTION_PRINT_WILDCARD
   virtual void Print(Option_t * wildcard, Option_t * opt) const {
      TCollection::Print(wildcard, opt);
   };
#endif	
#endif
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif
   
#ifdef __WITHOUT_TCOLLECTION_GETENTRIES
   //we add the GetEntries method added to TCollection from ROOT v4.03/04 onwards
   virtual Int_t GetEntries() const { return GetSize(); };
#endif
   
   ClassDef(KVList, 1)          // KV wrapper for ROOT TList classes
};

#if ROOT_VERSION_CODE < ROOT_VERSION(5,11,2)

//---- R__FOR_EACH macro -----------------------------------------------------------

// Macro to loop over all elements of a list of type "type" while executing
// procedure "proc" on each element

#define R__FOR_EACH(type,proc) \
    SetCurrentCollection(); \
    TIter _NAME3_(nxt_,type,proc)(TCollection::GetCurrentCollection()); \
    type *_NAME3_(obj_,type,proc); \
    while ((_NAME3_(obj_,type,proc) = (type*) _NAME3_(nxt_,type,proc)())) \
       _NAME3_(obj_,type,proc)->proc

#endif

#endif
