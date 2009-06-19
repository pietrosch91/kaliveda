//Created by KVClassFactory on Fri Jun 19 18:51:28 2009
//Author: John Frankland,,,

#include "KVCollection.h"
#include "TClass.h"

ClassImp(KVCollection)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVCollection</h2>
<h4>KaliVeda extension to ROOT collections</h4>
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
derived from KVBase can be sought using their type or label attribute (these characteristics
are not defined for TObject).
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
   if(!cl){
       Error("KVCollection(const Char_t*)", "Called for unknown class: %s",
       collection_classname);
       MakeZombie();
       return;
   }
   if(!cl->InheritsFrom("TCollection")){
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
}

void KVCollection::Clear(Option_t *option)
{
    // Clear the list of objects.
    fCollection->Clear(option);
}

void KVCollection::Delete(Option_t *option)
{
    // Delete all heap-based objects in the list.
    fCollection->Delete(option);
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
}
