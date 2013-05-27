//Created by KVClassFactory on Tue May  7 09:22:02 2013
//Author: John Frankland,,,

#include "KVGeoStrucElement.h"
#include "Riostream.h"
#include <TROOT.h>
using namespace std;

ClassImp(KVGeoStrucElement)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGeoStrucElement</h2>
<h4>A multidetector array geometry structural element</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVGeoStrucElement::init()
{
    // Default initialisations
    fStructures.SetOwner();
    fStructures.SetCleanup();
    fDetectors.SetCleanup();
    fParentStrucList.SetCleanup();
}

void KVGeoStrucElement::AddParentStructure(KVGeoStrucElement *e)
{
    fParentStrucList.Add(e);
}

void KVGeoStrucElement::RemoveParentStructure(KVGeoStrucElement *e)
{
    fParentStrucList.Remove(e);
}

KVGeoStrucElement::KVGeoStrucElement()
{
   // Default constructor
    init();
}

//________________________________________________________________

KVGeoStrucElement::KVGeoStrucElement(const Char_t* name, const Char_t* type) : KVBase(name, type)
{
   // Create a geometry structure element with given name and type
    init();
}

KVGeoStrucElement::~KVGeoStrucElement()
{
   // Destructor
    Clear();
}

void KVGeoStrucElement::Add(KVBase *element)
{
    // Add a detector or a daughter structure to this structure.
    // Note that daughter structures are owned by their mother and will be
    // deleted by her destructor.

    if(element->InheritsFrom(KVDetector::Class())){
        fDetectors.Add(element);
        dynamic_cast<KVDetector*>(element)->AddParentStructure(this);
    }
    else if(element->InheritsFrom(KVGeoStrucElement::Class())){
        fStructures.Add(element);
        dynamic_cast<KVGeoStrucElement*>(element)->AddParentStructure(this);
    }
    else{
        Error("Add", "Cannot add elements of class %s", element->ClassName());
    }
}

void KVGeoStrucElement::Remove(KVBase *element)
{
    // Remove a detector or structure element from this structure

    if(element->InheritsFrom(KVDetector::Class())){
        fDetectors.Remove(element);
        dynamic_cast<KVDetector*>(element)->RemoveParentStructure(this);
    }
    else if(element->InheritsFrom(KVGeoStrucElement::Class())){
        fStructures.Remove(element);
        dynamic_cast<KVGeoStrucElement*>(element)->RemoveParentStructure(this);
    }
    else{
        Error("Add", "Cannot add elements of class %s", element->ClassName());
    }
}

void KVGeoStrucElement::Clear(Option_t *opt)
{
    fDetectors.Clear();
    fStructures.Clear();
    fParentStrucList.Clear();
}

KVGeoStrucElement *KVGeoStrucElement::GetStructure(const Char_t *type, const Char_t *name) const
{
    // Get structure with type and name
    return (KVGeoStrucElement *)fStructures.FindObjectWithNameAndType(name,type);
}

KVGeoStrucElement* KVGeoStrucElement::GetParentStructure(const Char_t* type, const Char_t* name) const
{
    // Get parent geometry structure element of given type.
    // Give unique name of structure if more than one element of same type is possible.
    KVGeoStrucElement* el=0;
    if(strcmp(name,"")){
        KVSeqCollection* strucs = fParentStrucList.GetSubListWithType(type);
        el = (KVGeoStrucElement*)strucs->FindObject(name);
        delete strucs;
    }
    else
        el = (KVGeoStrucElement*)fParentStrucList.FindObjectByType(type);
    return el;
}

void KVGeoStrucElement::Print(Option_t *option) const
{
    TROOT::IndentLevel();
    cout << ClassName() << "::" << GetName() << " [TYPE=" << GetType() << "]" << endl << endl;
    TROOT::IncreaseDirLevel();
    if(GetDetectors()->GetEntries()){
        TROOT::IndentLevel();
        cout << "DETECTORS : " << endl << endl;
        TIter next(GetDetectors());
        TObject* d;
        TROOT::IncreaseDirLevel();
        while( (d = next()) ) {TROOT::IndentLevel(); cout << "    " << d->GetName() << endl;}
        cout << endl;
        TROOT::DecreaseDirLevel();
    }
    TIter next(GetStructures());
    KVGeoStrucElement* el;
    //TROOT::IncreaseDirLevel();
    while( (el = (KVGeoStrucElement*)next()) ){
        el->Print(option);
    }
    TROOT::DecreaseDirLevel();
    cout << endl;
}
