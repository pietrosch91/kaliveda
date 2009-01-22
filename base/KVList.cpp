/***************************************************************************
$Id: KVList.cpp,v 1.28 2009/01/15 17:04:23 ebonnet Exp $
                          kvlist.cpp  -  description
                             -------------------
    begin                : Sat May 18 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "TROOT.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TRegexp.h"
#include "TClass.h"
#include "TMethodCall.h"
#include "TDirectory.h"
#include "TKey.h"

#include "KVList.h"
#include "KVBase.h"
#include "KVString.h"

#include "Riostream.h"

ClassImp(KVList);
//_______________________________________________________________________________
// KVList :  Wrapper for a Tlist, but owns its objects.
//
//   Adds a few functionalities such as FindObjectByType/Label(), and also
//  wraps basic method FindObject(name) with FindObjectByName(name) which
//  returns a KVBase* pointer instead of a TObject*.
//
//  WARNING: By default, a KVList owns the objects to which it points,
//  and will delete them when the list is deleted.
//      Use KVList(kFALSE) constructor to have a list which does not own its objects.
//


//_______________________________________________________________________________
KVList::KVList(Bool_t owner)
{
   //Default ctor. If owner=kTRUE (default), makes TList own its objects:
   //they will be deleted if Clear() or Delete() are used, or if the list is
   //deleted itself.
   SetOwner(owner);
}

//_______________________________________________________________________________

KVList::~KVList()
{
}

//_______________________________________________________________________________
KVList::KVList(const KVList & obj)
{
   //copy ctor
   SetOwner(kTRUE);
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVList &) obj).Copy(*this);
#endif
}



//_______________________________________________________________________________
KVList *KVList::GetSubListWithClass(const TClass* _class)
{
	// Create and fill a (sub)list with objects in this list of the given class.
	//
	//  *** WARNING *** : DELETE the KVList returned by this method after using it !!!
	
	KVList *sublist = new KVList(kFALSE); 
	if (_class) {
   	TObjLink *lnk = FirstLink();
   	while (lnk) {
      	if ( _class == lnk->GetObject()->IsA() ) sublist->Add(lnk->GetObject());
      	lnk = lnk->Next();
		}
   }
	
	return sublist;
}

//_______________________________________________________________________________
KVList *KVList::GetSubListWithClass(const Char_t* class_name)
{
	// Create and fill a (sub)list with objects in this list of the given class.
	//
	//  *** WARNING *** : DELETE the KVList returned by this method after using it !!!
	
	if (class_name) {
      return GetSubListWithClass(gROOT->GetClass(class_name));
   }
	else return NULL;
}

//_______________________________________________________________________________
KVList *KVList::GetSubListWithMethod(const Char_t* retvalue,const Char_t* method)
{
	// Create and fill a (sub)list with objects in this list for which the
	// given method returns the given return value:
	//   e.g. if method = "GetName" and retvalue = "john", we return the
	//    (sub)list of objects in this list for which GetName() returns "john".
	//
	//  *** WARNING *** : DELETE the KVList returned by this method after using it !!!
	//
   // For each object of the list, the existence of the given method is checked using TMethodCall::IsValid()
	// if the method is valid and the return value is equal to the input one (retvalue) object is added to the subKVList
	// return type supported are those defined in TMethodCall::ReturnType()
	 
	KVList *sublist = new KVList(kFALSE); 		//owner=kFALSE (see KVList::KVList(Bool_t owner))
	if (retvalue && method) {
      KVString sretvalue; sretvalue.Form("%s",retvalue);
		TObjLink *lnk = FirstLink();
      while (lnk) {
		//cout << "lnk->GetObject()->IsA() = " << lnk->GetObject()->IsA()->GetName() << endl;
	      TMethodCall *mt = new TMethodCall();
         mt->InitWithPrototype(lnk->GetObject()->IsA(),Form("%s",method),"");
			if (mt->IsValid()) {
			//cout << "it is valid" << endl;
				if (mt->ReturnType()==TMethodCall::kString){
					Char_t *ret; 
					mt->Execute(lnk->GetObject(),"",&ret);
					KVString sret; sret.Form("%s",ret);
					if (!sretvalue.Contains("*")) {if (sret==sretvalue) sublist->Add(lnk->GetObject());}
					else { if (sret.Match(sretvalue)) sublist->Add(lnk->GetObject()); }
				}
				else if (mt->ReturnType()==TMethodCall::kLong){
					Long_t ret;
					mt->Execute(lnk->GetObject(),"",ret);
					if (ret==sretvalue.Atoi()) sublist->Add(lnk->GetObject());
				}
				else if (mt->ReturnType()==TMethodCall::kDouble){
					Double_t ret;
					mt->Execute(lnk->GetObject(),"",ret);
					if (ret==sretvalue.Atof()) sublist->Add(lnk->GetObject());
				}
				else cout << "this type is not supported " << mt->ReturnType() << endl;
			}
			lnk = lnk->Next();
         delete mt;
	   }
   }	
	return sublist;
}

//_______________________________________________________________________________
KVList *KVList::GetSubListWithName(const Char_t* retvalue)
{
	// Create and fill a (sub)list with all objects in this list whose name
	// (i.e. string returned by GetName()) is "retvalue"
	//
	//  *** WARNING *** : DELETE the KVList returned by this method after using it !!!
	
	return GetSubListWithMethod(retvalue,"GetName");
}

//_______________________________________________________________________________
KVList *KVList::GetSubListWithLabel(const Char_t* retvalue)
{
	// Create and fill a (sub)list with all objects in this list whose label
	// (i.e. string returned by GetLabel()) is "retvalue"
	//
	//  *** WARNING *** : DELETE the KVList returned by this method after using it !!!
	
	return GetSubListWithMethod(retvalue,"GetLabel");
}

//_______________________________________________________________________________
KVList *KVList::GetSubListWithType(const Char_t* retvalue)
{
	// Create and fill a (sub)list with all objects in this list whose type
	// (i.e. string returned by GetType()) is "retvalue"
	//
	//  *** WARNING *** : DELETE the KVList returned by this method after using it !!!
	
	return GetSubListWithMethod(retvalue,"GetType");
}

//_______________________________________________________________________________
KVList* KVList::MakeListFromFile(TFile *file)
{
	//Static method create a list containing all objects contain of a file
	//The file can be closed after this method, objects stored in the
	//list still remains valid
	//if file=NULL, the current directory is considered
	//User has to delete KVList pointer after use
	
	KVList *ll = new KVList(kFALSE);

	TKey* key=0;
	if (!file){
		TIter next_ps(gDirectory->GetListOfKeys());
		while ( (key = (TKey *) next_ps()) ) ll->Add(key->ReadObj());
	}
	else {
		/*
		if (file->GetList()->GetEntries()<file->GetListOfKeys()->GetEntries())
			file->ReadAll();
		TObjLink *lnk = file->GetList()->FirstLink();
		while (lnk) {
			ll->Add(lnk->GetObject());
			lnk=lnk->Next();
		}
		*/
		TIter next_ps(file->GetListOfKeys());
		while ( (key = (TKey *) next_ps()) ) ll->Add(key->ReadObj());
		
	}
	return ll;
}

//_______________________________________________________________________________
KVList* KVList::MakeListFromFileWithMethod(TFile *file,const Char_t* retvalue,const Char_t* method)
{
	
	KVList* l1 = MakeListFromFile(file);
	KVList* l2 = l1->GetSubListWithMethod(retvalue,method);
	l1->Clear(); delete l1;
	return l2;

}

//_______________________________________________________________________________
KVList *KVList::MakeListFromFileWithClass(TFile *file,const TClass* _class)
{
	KVList* l1 = MakeListFromFile(file);
	KVList* l2 = l1->GetSubListWithClass(_class);
	l1->Clear(); delete l1;
	return l2;
}

//_______________________________________________________________________________
KVList *KVList::MakeListFromFileWithClass(TFile *file,const Char_t* class_name)
{
	KVList* l1 = MakeListFromFile(file);
   KVList* l2 = l1->GetSubListWithClass(class_name);
   l1->Clear(); delete l1;
	return l2;
}

//_______________________________________________________________________________
TObject *KVList::FindObjectWithMethod(const Char_t* retvalue,const Char_t* method)
{
	
	if (retvalue && method) {
      TObjLink *lnk = FirstLink();
      while (lnk) {
			TMethodCall *mt = new TMethodCall();
         mt->InitWithPrototype(lnk->GetObject()->IsA(),Form("%s",method),"");
			if (mt->IsValid()) {
				if (mt->ReturnType()==TMethodCall::kString){
					Char_t *ret; 
					mt->Execute(lnk->GetObject(),"",&ret);
					if (!TString(retvalue).Contains("*")) {if (!strcmp(ret,retvalue)) { delete mt; return lnk->GetObject(); } }
					else { if (KVString(ret).Match(retvalue)) { delete mt; return lnk->GetObject(); } }
				}
				else if (mt->ReturnType()==TMethodCall::kLong){
					Long_t ret;
					mt->Execute(lnk->GetObject(),"",ret);
					if (ret==TString(retvalue).Atoi()) { delete mt; return lnk->GetObject(); }
				}
				else if (mt->ReturnType()==TMethodCall::kDouble){
					Double_t ret;
					mt->Execute(lnk->GetObject(),"",ret);
					if (ret==TString(retvalue).Atof()) { delete mt; return lnk->GetObject(); }
				}
				else cout << "this type is not supported " << mt->ReturnType() << endl;
			}
			lnk = lnk->Next();
         delete mt;
	   }
   }	
	return 0;
	
}

//_______________________________________________________________________________
KVBase *KVList::FindObjectByName(const Char_t * type)
{
//Look for object with name "name" in list
   return (KVBase *) TList::FindObject(type);
}

//_____________________________________________________________________________
KVBase *KVList::FindObjectByType(const Char_t * type)
{
   // Find an object in this list using its type (KVBase::GetType).
   // Requires a sequential scan till the object has been found.
   // Returns 0 if object with specified type is not found.
   
   if (type) {
      TObjLink *lnk = FirstLink();
      while (lnk) {
         KVBase *obj = (KVBase *) lnk->GetObject();
         if (!strcmp(type, obj->GetType()))
            return obj;
         lnk = lnk->Next();
      }
   }
   return 0;
}

//_____________________________________________________________________________
KVBase *KVList::FindObjectByClass(const Char_t * class_name)
{
   // Find an object in this list using its class name (TObject::ClassName()).
   // The first object in the list with the correct class is returned.
   // Requires a sequential scan till the object has been found.
   // Returns 0 if object with specified class is not found.
   
   if (class_name) {
      TClass* _class = gROOT->GetClass(class_name);
      return FindObjectByClass(_class);
   }
   return 0;
}

//_____________________________________________________________________________
KVBase *KVList::FindObjectByClass(const TClass* _class)
{
   // Find an object in this list using its class (TObject::IsA()).
   // The first object in the list with the correct class is returned.
   // Requires a sequential scan till the object has been found.
   // Returns 0 if object with specified class is not found.
   
   if (_class) {
      TObjLink *lnk = FirstLink();
      while (lnk) {
         TObject *obj = (TObject *) lnk->GetObject();
         if ( _class == obj->IsA() ){
            return (KVBase*)obj;
         }
         lnk = lnk->Next();
      }
   }
   return 0;
}

//_______________________________________________________________________________
KVBase *KVList::FindObjectByLabel(const Char_t * label)
{
   // Find an object in this list using its label (KVBase::GetLabel).
   // Requires a sequential scan till the object has been found.
   // Returns 0 if object with specified label is not found.
   
   if (label) {
      TObjLink *lnk = FirstLink();
      while (lnk) {
         KVBase *obj = (KVBase *) lnk->GetObject();
         if (!strcmp(label, obj->GetLabel()))
            return obj;
         lnk = lnk->Next();
      }
   }
   return 0;
}

//_______________________________________________________________________________
KVBase *KVList::FindObject(UInt_t num)
{
   //Find an object in this list using its number (value of GetNumber()).

   TObjLink *lnk = FirstLink();
   while (lnk) {
      KVBase *obj = (KVBase *) lnk->GetObject();
      if (obj->GetNumber() == num)
         return obj;
      lnk = lnk->Next();
   }
   return 0;
}

//_______________________________________________________________________________
KVBase *KVList::FindObject(const Char_t * name, const Char_t * type)
{
   // Find an object in this list using its type and name. Requires a sequential
// scan till the object has been found. Returns 0 if object with specified
// name & type is not found.

   if (type && name) {
      TObjLink *lnk = FirstLink();
      while (lnk) {
         KVBase *obj = (KVBase *) lnk->GetObject();
         if (!strcmp(type, obj->GetType())
             && !strcmp(name, obj->GetName()))
            return obj;
         lnk = lnk->Next();
      }
   }
   return 0;
}

void KVList::Print(Option_t * opt) const
{
   //Print description of all objects in list

   TIter next(this);
   TObject *obj;
   while ((obj = next())) {
#ifdef KV_DEBUG
      if (!obj)
         Error("Print", "Object does not exist: %l", (Long_t) obj);
#endif
      obj->Print(opt);
   }
}

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVList::Copy(TObject & obj) const
#else
void KVList::Copy(TObject & obj)
#endif
{
   //Copy this to obj.
   //As KVList owns its objects, new clones of all objects in list are made

   TList::Copy(obj);            //in fact this calls TObject::Copy, no Copy method defined for collection classes
   KVList & copy = (KVList &) obj;
   //delete any pre-existing objects in copy list
   copy.Delete();
   //clone list members
   TObject *b;
   TIter next(this);
   while ((b = next())) {
      copy.Add(b->Clone());
   }
}

//_______________________________________________________________________________

void KVList::Execute(const char *method, const char *params, Int_t * error)
{
   //Redefinition of TObject::Execute method.
   //TObject::Execute is called for each object in the list in order, meaning that for each
   //object the method "method" is executed with arguments "params".
   TIter next(this);
   TObject *obj;
   while ((obj = next())) {
      obj->Execute(method, params, error);
   }
}

KVBase *KVList::FindObject(KVBase * obj)
{
   return (KVBase *) TList::FindObject(obj);
}

//_______________________________________________________________________________

TObject* KVList::FindObjectAny(const Char_t *att, const Char_t *keys, Bool_t contains_all, Bool_t case_sensitive) const
{
   //Find an object in the list, if one of its characteristics 'att' contains any or all of
   //the keywords contained in the string 'keys'
   //
   // att = "name", "title", "class", "type", "label",
   //      WARNING: when using "type" or "label", any objects in list which do
   //                         not inherit from KVBase are ignored
   // keys = list of keywords, separated by spaces
   //        
   // contains_all = kFALSE (default) : object found if any keyword occurs in the characteristic 'att'
   // contains_all = kTRUE               : object found if all keywords occur in the characteristic 'att'
   //
   // case_sensitive = kTRUE (default) : case-sensitive comparison
   // case_sensitive = kFALSE               : ignore case of keywords

   int char_test=-1;
   enum { kName, kTitle, kClass, kType, kLabel };
   if( !strcmp(att,"name") ) char_test = kName;
   else if(!strcmp(att,"title")) char_test = kTitle;
   else if(!strcmp(att,"type")) char_test = kType;
   else if(!strcmp(att,"label")) char_test =kLabel;
   else if(!strcmp(att,"class")) char_test = kClass;
   else return 0;
   
   TString::ECaseCompare casecmp;
   if(case_sensitive) casecmp = TString::kExact;
   else casecmp = TString::kIgnoreCase;
   
   TString _keys(keys);
   TObjArray* keywords = _keys.Tokenize(' ');
   if(!keywords) return 0;
   int nkeys;
   if(!(nkeys = keywords->GetEntries())) {
      delete keywords; return 0;
   }
   
   int nmatches;
   TObjLink *lnk = FirstLink();
   TString _att;
   TObject *obj=0;
   KVBase *kvobj=0; 
   while (lnk) {
      
      obj = lnk->GetObject();
      if(char_test>kClass){
         kvobj = dynamic_cast<KVBase*>(obj);
         if(!kvobj) {
            lnk = lnk->Next();
            continue;
         }
      }
      switch(char_test){
         case kName:
            _att = obj->GetName();
            break;
         case kTitle:
            _att = obj->GetTitle();
            break;
         case kClass:
            _att = obj->ClassName();
            break;
         case kLabel:
            _att = kvobj->GetLabel();
            break;
         case kType:
            _att = kvobj->GetType();
            break;
      }
      nmatches=0;
      for(int i=0;i<nkeys;i++){         
         nmatches += ( _att.Contains( ((TObjString*)keywords->At(i))->String() , casecmp));
      }
      if( (nmatches && !contains_all) || ((nmatches==nkeys) && contains_all) ){
         delete keywords;
         return obj;
      }
      lnk = lnk->Next();
   }
   delete keywords;
   return 0;
}
