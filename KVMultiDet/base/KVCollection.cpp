//Created by KVClassFactory on Fri Jun 19 18:51:28 2009
//Author: John Frankland,,,

#include "KVCollection.h"
#include "TClass.h"
#include "TROOT.h"
#include "KVBase.h"
#include "TMethodCall.h"

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
referenced through a TCollection base pointer. The class of the embedded object for any given
instance is passed as an argument to the constructor:
<code>
KVCollection* my_coll = new KVCollection("THashList");
</code>
Any collection class derived from TCollection is valid.<br>
Any object derived from TObject can be stored in the collection, but only objects
derived from KVBase can be sought using their 'type', 'label', or 'number' attributes
(these characteristics are not defined for TObject).
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVCollection::KVCollection()
{
    // Default constructor
    fCollection=0;
}

KVCollection::KVCollection(const Char_t* collection_classname)
{
    // Create new extended collection of class "collection_classname".
    // Must be the name of a class derived from TCollection.

    fCollection=0;
    TClass* cl = TClass::GetClass(collection_classname);
    if (!cl)
    {
        Error("KVCollection(const Char_t*)", "Called for unknown class: %s",
              collection_classname);
        MakeZombie();
        return;
    }
    if (!cl->InheritsFrom("TCollection"))
    {
        Error("KVCollection(const Char_t*)", "Called for class %s which does not inherit from TCollection",
              collection_classname);
        MakeZombie();
        return;
    }
    fCollection = (TCollection*)cl->New();
}

KVCollection::~KVCollection()
{
    // Destructor
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

TObject *KVCollection::At(Int_t idx) const
{
    // If the embedded list inherits from TSeqCollection, this method will return the
    // object with the given index in the list. Otherwise we return 0x0.

    TSeqCollection *sc = dynamic_cast<TSeqCollection*>(fCollection);
    if (sc) return sc->At(idx);
    return 0;
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

	if (retvalue && method) {
		KVString RV(retvalue); KVString MTH(method);
		Bool_t wildcard = RV.Contains("*");
      TIter next(fCollection); TObject* obj;
      while ( (obj = next()) ) {
			TMethodCall mt;
         mt.InitWithPrototype(obj->IsA(), MTH.Data(),"");
			if (mt.IsValid()) {
				if (mt.ReturnType()==TMethodCall::kString){
					Char_t *ret;
					mt.Execute(obj,"",&ret);
					if (!wildcard) {if (RV==ret) { return obj; } }
					else { if (KVString(ret).Match(RV)) { return obj; } }
				}
				else if (mt.ReturnType()==TMethodCall::kLong){
					Long_t ret;
					mt.Execute(obj,"",ret);
					if (ret==RV.Atoi()) { return obj; }
				}
				else if (mt.ReturnType()==TMethodCall::kDouble){
					Double_t ret;
					mt.Execute(obj,"",ret);
					if (ret==RV.Atof()) { return obj; }
				}
				else Error("FindObjectWithMethod", "Return type %s is not supported", mt.ReturnType());
			}
	   }
   }
	return 0;

}
