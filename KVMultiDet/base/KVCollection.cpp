//Created by KVClassFactory on Fri Jun 19 18:51:28 2009
//Author: John Frankland,,,

#include "KVCollection.h"
#include "TClass.h"
#include "TROOT.h"
#include "KVBase.h"
#include "TMethodCall.h"
#include "Riostream.h"
#include "TKey.h"

ClassImp(KVCollection)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVCollection</h2>
<h4>KaliVeda extensions to ROOT collections</h4>
This class adds functionalities such as
<ul>
<li>FindObjectByType</li>
<li>FindObjectByLabel</li>
<li>GetSublistWithType</li>
etc. to the standard ROOT collection classes. The actual collection is embedded and
referenced through a TSeqCollection base pointer. The class of the embedded object for any given
instance is passed as an argument to the constructor:
<code>
KVCollection* my_coll = new KVCollection("THashList");
</code>
Any collection class derived from TSeqCollection is valid: this means all ordered collections, for which the order in which
objects are added is preserved. These lists can also be sorted.<br>
Any object derived from TObject can be stored in the collection, but only objects
derived from KVBase can be sought using their 'type', 'label', or 'number' attributes
(these characteristics are not defined for TObject).
<h3>Signals &amp; slots: monitoring list modification</h3>
If SendModifiedSignals(kTRUE) is called, then every time an object is added to or removed from the list, it will emit
the signal "Modified()". This can be used to monitor any changes to the
state of the list.<br>
<h3>FindObjectBy...</h3>
In addition to the standard TList::FindObject(const Char_t* name) and
TList::FindObject(TObject*) methods, KVCollection adds methods to find
objects based on several different properties, such as type, label,
number, or class. Note that type, label and number are only defined for
objects derived from KVBase; if the list contains a mixture of TObject-
and KVBase-derived objects, the TObject-derived objects will be ignored
if type, label or number are used to search.<br>
Finally, the very general FindObjectWithMethod method can search for an
object using any valid method.<br>
<h3>Sublists</h3>
The GetSubListWith... methods allow to generate new lists containing a
subset of the objects in the main list, according to their name, label,
type, etc. These sublists do not own their objects (they are supposed
to be owned by the main list), and must be deleted by the user in order
to avoid memory leaks.<br>
<h3>Execute</h3>
The Execute methods can be used to execute a given method with the same
arguments for every object in the list.<br>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVCollection::KVCollection()
{
    // Default constructor
    fCollection=0;
    ResetBit(kSignals);
}

KVCollection::KVCollection(const KVCollection& col)
        : TSeqCollection()
{
    // Copy constructor
    // See KVCollection::Copy

    fCollection=col.NewCollectionLikeThisOne();
    col.Copy(*this);
}

KVCollection::KVCollection(const Char_t* collection_classname)
{
    // Create new extended collection of class "collection_classname".
    // Must be the name of a class derived from TCollection.

    fCollection=0;
	 SetCollection(collection_classname);
	 if(!fCollection) MakeZombie();
    ResetBit(kSignals);
}

void KVCollection::SetCollection(const Char_t* class_name)
{
	 // Create TSeqCollection-derived object of class 'class_name'
	 // and set as the embedded collection fCollection.
    TClass* cl = TClass::GetClass(class_name);
    if (!cl)
    {
        Error("SetCollection(const Char_t*)", "Called for unknown class: %s",
              class_name);
        return;
    }
    if (!cl->InheritsFrom("TSeqCollection"))
    {
        Error("SetCollection(const Char_t*)",
				  "Called for class %s which does not inherit from TSeqCollection",
              class_name);
        return;
    }
    fCollection = (TSeqCollection*)cl->New();
}

KVCollection::~KVCollection()
{
    // Destructor
    SafeDelete(fCollection);
}

void KVCollection::Copy(TObject & obj) const
{
    // Copy a list of objects.
    // If this list owns its objects, we make new Clones of all objects in the list
    // (N.B. the Clone() method must work correctly for the objects in question)
    // and put them in the copy list, the copy will own these new objects.
    // Copy will have same IsOwner and IsCleanup status as this list.
    // If this list sends Modified() signal, the copy will do too.

    TSeqCollection::Copy(obj);            //in fact this calls TObject::Copy, no Copy method defined for collection classes
    KVCollection & copy = (KVCollection&) obj;

    //clear any pre-existing objects in copy list
    if (copy.IsOwner()) copy.Delete();
    else copy.Clear();

    //set ownership
    copy.SetOwner(IsOwner());
    //set cleanup status
    copy.SetCleanup(IsCleanup());
    //set signal&slot status
    copy.SendModifiedSignals(IsSendingModifiedSignals());

    //copy or clone list members
    TObject *b;
    TIter next(fCollection);
    while ((b = next()))
    {
        if (IsOwner())
            copy.Add(b->Clone());
        else
            copy.Add(b);
    }
}

KVCollection* KVCollection::NewCollectionLikeThisOne() const
{
    // PROTECTED method
    // Creates and returns pointer to a new (empty) KVCollection
	 // (or derived class) with the same characteristics
    // as this one :
    //   - class of embedded collection
    //   - collection is owner of objects ?
    //   - objects are in cleanup list ?

    KVCollection *newCol = (KVCollection*)IsA()->New();
	 if(!newCol->fCollection) newCol->SetCollection(fCollection->ClassName());
    newCol->SetOwner( IsOwner() );
    newCol->SetCleanup( IsCleanup() );
    return newCol;
}

void KVCollection::Add(TObject *obj)
{
    // Add an object to the list.
    fCollection->Add(obj);
    Changed();
}

void KVCollection::Clear(Option_t *option)
{
    // Clear the list of objects.
    fCollection->Clear(option);
    Changed();
}

void KVCollection::Delete(Option_t *option)
{
    // Delete all heap-based objects in the list.
    fCollection->Delete(option);
    Changed();
}

TObject** KVCollection::GetObjectRef(const TObject *obj) const
{
    // Return reference to object.
    return fCollection->GetObjectRef(obj);
}

TIterator* KVCollection::MakeIterator(Bool_t dir) const
{
    // Make and return iterator for the list.
    return fCollection->MakeIterator(dir);
}

TObject* KVCollection::Remove(TObject *obj)
{
    // Remove object from list.
    return fCollection->Remove(obj);
    Changed();
}

void	KVCollection::PrintCollectionHeader(Option_t* option) const
{
    // Overrides TCollection::PrintCollectionHeader to show the class name of the embedded list
    TROOT::IndentLevel();
    printf("Collection name='%s', embedded list class='%s', size=%d\n", GetName(),
           fCollection->ClassName(), GetSize());
}

TObject* KVCollection::FindObjectByType(const Char_t* type) const
{
    // Will return object with given type (value of KVBase::GetType() method).
    // Objects in list which do not inherit KVBase do not have GetType() method,
    // and are ignored.

    TIter next(fCollection);
    TObject *obj;
    while ( (obj = next()) )
    {
        if ( obj->TestBit(KVBase::kIsKaliVedaObject) )
        {
            if ( ((KVBase*)obj)->IsType(type) )
                return obj;
        }
    }
    return 0;
}

TObject* KVCollection::FindObjectByClass(const TClass* cl) const
{
    // Return (first) object in embedded list with given class.

    TIter next(fCollection);
    TObject *obj;
    while ( (obj = next()) )
    {
        if ( obj->IsA() == cl ) return obj;
    }
    return 0;
}

TObject* KVCollection::FindObjectByClass(const Char_t* cl) const
{
    // Return (first) object in embedded list with given class.

    return FindObjectByClass( TClass::GetClass(cl) );
}

TObject* KVCollection::FindObjectByLabel(const Char_t* label) const
{
    // Will return object with given label (value of KVBase::GetLabel() method).
    // Objects in list which do not inherit KVBase do not have GetLabel() method,
    // and are ignored.

    TIter next(fCollection);
    TObject *obj;
    while ( (obj = next()) )
    {
        if ( obj->TestBit(KVBase::kIsKaliVedaObject) )
        {
            if ( !strcmp(((KVBase*)obj)->GetLabel(), label ) ) return obj;
        }
    }
    return 0;
}


TObject* KVCollection::FindObjectByNumber(UInt_t num) const
{
    // Will return object with given number (value of KVBase::GetNumber() method).
    // Objects in list which do not inherit KVBase do not have GetNumber() method,
    // and are ignored.

    TIter next(fCollection);
    TObject *obj;
    while ( (obj = next()) )
    {
        if ( obj->TestBit(KVBase::kIsKaliVedaObject) )
        {
            if ( ((KVBase*)obj)->GetNumber() == num ) return obj;
        }
    }
    return 0;
}

TObject *KVCollection::FindObjectWithNameAndType(const Char_t * name, const Char_t * type) const
{
    // Return object with specified name AND type (value of KVBase::GetType() method).
    // Objects in list which do not inherit KVBase do not have GetType() method,
    // and are ignored.

    TIter next(fCollection);
    TObject *obj;
    while ( (obj = next()) )
    {
        if ( obj->TestBit(KVBase::kIsKaliVedaObject) && !strcmp(obj->GetName(),name) )
        {
            if ( !strcmp(((KVBase*)obj)->GetType(), type ) ) return obj;
        }
    }
    return 0;
}

//_______________________________________________________________________________
TObject *KVCollection::FindObjectWithMethod(const Char_t* retvalue,const Char_t* method) const
{
    // Find the first object in the list for which the given method returns the given return value:
    //   e.g. if method = "GetName" and retvalue = "john", we return the
    //   first object in this list for which GetName() returns "john".
    //
    // For each object of the list, the existence of the given method is checked using TMethodCall::IsValid()
    // if the method is valid and the return value is equal to the input one (retvalue) object is returned
    // Supported return types are those defined in TMethodCall::ReturnType()

    if (retvalue && method)
    {
        KVString RV(retvalue);
        KVString MTH(method);
        Bool_t wildcard = RV.Contains("*");
        TIter next(fCollection);
        TObject* obj;
        while ( (obj = next()) )
        {
            TMethodCall mt;
            mt.InitWithPrototype(obj->IsA(), MTH.Data(),"");
            if (mt.IsValid())
            {
                if (mt.ReturnType()==TMethodCall::kString)
                {
                    Char_t *ret;
                    mt.Execute(obj,"",&ret);
                    if (!wildcard)
                    {
                        if (RV==ret)
                        {
                            return obj;
                        }
                    }
                    else
                    {
                        if (KVString(ret).Match(RV))
                        {
                            return obj;
                        }
                    }
                }
                else if (mt.ReturnType()==TMethodCall::kLong)
                {
                    Long_t ret;
                    mt.Execute(obj,"",ret);
                    if (ret==RV.Atoi())
                    {
                        return obj;
                    }
                }
                else if (mt.ReturnType()==TMethodCall::kDouble)
                {
                    Double_t ret;
                    mt.Execute(obj,"",ret);
                    if (ret==RV.Atof())
                    {
                        return obj;
                    }
                }
                else Error("FindObjectWithMethod", "Return type %s is not supported", mt.ReturnType());
            }
        }
    }
    return 0;

}

void KVCollection::Execute(const char *method, const char *params, Int_t * error)
{
    //Redefinition of TObject::Execute(const char *, const char *, Int_t *) method.
    //TObject::Execute is called for each object in the embedded list in order, meaning that for each
    //object the method "method" is executed with arguments "params".

    TIter next(fCollection);
    TObject *obj;
    while ((obj = next()))
    {
        obj->Execute(method, params, error);
    }
}

//_______________________________________________________________________________

void KVCollection::Execute(TMethod * method, TObjArray * params, Int_t * error)
{
    //Redefinition of TObject::Execute(TMethod *, TObjArray *, Int_t *) method.
    //TObject::Execute is called for each object in the embedded list in order, meaning that for each
    //object the method "method" is executed with arguments "params".

    TIter next(fCollection);
    TObject *obj;
    while ((obj = next()))
    {
        obj->Execute(method, params, error);
    }
}

//_______________________________________________________________________________

TObject* KVCollection::FindObjectAny(const Char_t *att, const Char_t *keys, Bool_t contains_all, Bool_t case_sensitive) const
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
    if ( !strcmp(att,"name") ) char_test = kName;
    else if (!strcmp(att,"title")) char_test = kTitle;
    else if (!strcmp(att,"type")) char_test = kType;
    else if (!strcmp(att,"label")) char_test =kLabel;
    else if (!strcmp(att,"class")) char_test = kClass;
    else return 0;

    TString::ECaseCompare casecmp;
    if (case_sensitive) casecmp = TString::kExact;
    else casecmp = TString::kIgnoreCase;

    TString _keys(keys);
    TObjArray* keywords = _keys.Tokenize(' ');
    if (!keywords) return 0;
    int nkeys;
    if (!(nkeys = keywords->GetEntries()))
    {
        delete keywords;
        return 0;
    }

    int nmatches;
    TIter next(fCollection);
    TString _att;
    TObject *obj=0;
    KVBase *kvobj=0;
    while ((obj=next()))
    {

        if (char_test>kClass && !obj->TestBit(KVBase::kIsKaliVedaObject) )
        {
            continue;
        }
        switch (char_test)
        {
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
        for (int i=0;i<nkeys;i++)
        {
            nmatches += ( _att.Contains( ((TObjString*)keywords->At(i))->String() , casecmp));
        }
        if ( (nmatches && !contains_all) || ((nmatches==nkeys) && contains_all) )
        {
            delete keywords;
            return obj;
        }
    }
    delete keywords;
    return 0;
}

KVCollection *KVCollection::GetSubListWithClass(const TClass* _class)
{
    // Create and fill a (sub)list with objects in this list of the given class.
    // This new list will be of the same kind as this one.
    // The objects in the sublist do not belong to the sublist.
    //
    //  *** WARNING *** : DELETE the KVCollection returned by this method after using it !!!

    KVCollection *sublist = NewCollectionLikeThisOne();
    sublist->SetOwner(kFALSE);
    if (_class)
    {
        TIter next(fCollection);
        TObject* ob;
        while ((ob=next()))
        {
            if ( _class == ob->IsA() ) sublist->Add(ob);
        }
    }
    return sublist;
}

//_______________________________________________________________________________
KVCollection *KVCollection::GetSubListWithClass(const Char_t* class_name)
{
    // Create and fill a (sub)list with objects in this list of the given class.
    // This new list will be of the same kind as this one.
    // The objects in the sublist do not belong to the sublist.
    //
    //  *** WARNING *** : DELETE the KVList returned by this method after using it !!!

    if (class_name)
    {
        return GetSubListWithClass(TClass::GetClass(class_name));
    }
    else return NULL;
}

//_______________________________________________________________________________
KVCollection *KVCollection::GetSubListWithMethod(const Char_t* retvalue,const Char_t* method)
{
    // Create and fill a (sub)list with objects in this list for which the
    // given method returns the given return value:
    //   e.g. if method = "GetName" and retvalue = "john", we return the
    //    (sub)list of objects in this list for which GetName() returns "john".
    //
    // This new list will be of the same kind as this one.
    // The objects in the sublist do not belong to the sublist.
    //  *** WARNING *** : DELETE the KVList returned by this method after using it !!!
    //
    // For each object of the list, the existence of the given method is checked using TMethodCall::IsValid()
    // if the method is valid and the return value is equal to the input one (retvalue) object is added to the subKVList
    // return type supported are those defined in TMethodCall::ReturnType()

    KVCollection* sublist = NewCollectionLikeThisOne();
    sublist->SetOwner(kFALSE);
    if (retvalue && method)
    {
        KVString RV(retvalue);
        KVString MTH(method);
        Bool_t wildcard = RV.Contains("*");
        TIter next(fCollection);
        TObject* ob;
        while ((ob=next()))
        {

            TMethodCall mt;
            mt.InitWithPrototype(ob->IsA(), MTH.Data(),"");
            if (mt.IsValid())
            {
                //cout << "it is valid" << endl;
                if (mt.ReturnType()==TMethodCall::kString)
                {
                    Char_t *ret;
                    mt.Execute(ob,"",&ret);
                    if (!wildcard)
                    {
                        if (RV==ret) sublist->Add(ob);
                    }
                    else
                    {
                        if (KVString(ret).Match(RV)) sublist->Add(ob);
                    }
                }
                else if (mt.ReturnType()==TMethodCall::kLong)
                {
                    Long_t ret;
                    mt.Execute(ob,"",ret);
                    if (ret==RV.Atoi()) sublist->Add(ob);
                }
                else if (mt.ReturnType()==TMethodCall::kDouble)
                {
                    Double_t ret;
                    mt.Execute(ob,"",ret);
                    if (ret==RV.Atof()) sublist->Add(ob);
                }
                else cout << "this type is not supported " << mt.ReturnType() << endl;
            }
        }
    }
    return sublist;
}

//_______________________________________________________________________________
KVCollection *KVCollection::GetSubListWithName(const Char_t* retvalue)
{
    // Create and fill a (sub)list with all objects in this list whose name
    // (i.e. string returned by GetName()) is "retvalue"
    // This new list will be of the same kind as this one.
    // The objects in the sublist do not belong to the sublist.
    //
    //  *** WARNING *** : DELETE the KVList returned by this method after using it !!!

    return GetSubListWithMethod(retvalue,"GetName");
}

//_______________________________________________________________________________
KVCollection *KVCollection::GetSubListWithLabel(const Char_t* retvalue)
{
    // Create and fill a (sub)list with all objects in this list whose label
    // (i.e. string returned by GetLabel()) is "retvalue"
    // This new list will be of the same kind as this one.
    // The objects in the sublist do not belong to the sublist.
    //
    //  *** WARNING *** : DELETE the KVList returned by this method after using it !!!

    return GetSubListWithMethod(retvalue,"GetLabel");
}

//_______________________________________________________________________________
KVCollection *KVCollection::GetSubListWithType(const Char_t* retvalue)
{
    // Create and fill a (sub)list with all objects in this list whose type
    // (i.e. string returned by GetType()) is "retvalue"
    // This new list will be of the same kind as this one.
    // The objects in the sublist do not belong to the sublist.
    //
    //  *** WARNING *** : DELETE the KVList returned by this method after using it !!!

    return GetSubListWithMethod(retvalue,"GetType");
}

KVCollection* KVCollection::MakeListFromFile(TFile *file)
{
    //Static method create a list containing all objects contain of a file
    //The file can be closed after this method, objects stored in the
    //list still remains valid
    //if file=NULL, the current directory is considered
    //
    //  *** WARNING *** : DELETE the KVCollection returned by this method after using it !!!

    KVCollection *ll = new KVCollection("TList");
    ll->SetOwner(kFALSE);

    TKey* key=0;
    if (!file)
    {
        TIter next_ps(gDirectory->GetListOfKeys());
        while ( (key = (TKey *) next_ps()) ) ll->Add(key->ReadObj());
    }
    else
    {
        TIter next_ps(file->GetListOfKeys());
        while ( (key = (TKey *) next_ps()) ) ll->Add(key->ReadObj());

    }
    return ll;
}

//_______________________________________________________________________________
KVCollection* KVCollection::MakeListFromFileWithMethod(TFile *file,const Char_t* retvalue,const Char_t* method)
{
    //Static method create a list containing all objects whose "method" returns "retvalue" in a file
    //WARNING list has to be empty with KVCollection::Clear() method before closing file
    //if file=NULL, the current directory is considered
    //
    //  *** WARNING *** : DELETE the KVList returned by this method after using it !!!

    KVCollection* l1 = MakeListFromFile(file);
    KVCollection* l2 = l1->GetSubListWithMethod(retvalue,method);
    l1->Clear();
    delete l1;
    return l2;
}

//_______________________________________________________________________________
KVCollection *KVCollection::MakeListFromFileWithClass(TFile *file,const TClass* _class)
{
    //Static method create a list containing all objects of given class in a file
    //WARNING list has to be empty with KVList::Clear() method before closing file
    //if file=NULL, the current directory is considered
    //
    //  *** WARNING *** : DELETE the KVList returned by this method after using it !!!

    KVCollection* l1 = MakeListFromFile(file);
    KVCollection* l2 = l1->GetSubListWithClass(_class);
    l1->Clear();
    delete l1;
    return l2;
}

//_______________________________________________________________________________
KVCollection *KVCollection::MakeListFromFileWithClass(TFile *file,const Char_t* class_name)
{
    //Static method create a list containing all objects of given class in a file
    //WARNING list has to be empty with KVList::Clear() method before closing file
    //if file=NULL, the current directory is considered
    //
    //  *** WARNING *** : DELETE the KVList returned by this method after using it !!!

    KVCollection* l1 = MakeListFromFile(file);
    KVCollection* l2 = l1->GetSubListWithClass(class_name);
    l1->Clear();
    delete l1;
    return l2;
}
