//Created by KVClassFactory on Thu Sep 27 14:48:55 2012
//Author: John Frankland,,,

#include "KVRangeYanez.h"
#include "KVRangeYanezMaterial.h"
#include "KVElementDensity.h"
#include "KVNDTManager.h"
#include "TString.h"

ClassImp(KVRangeYanez)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVRangeYanez</h2>
<h4>Interface to Range dE/dx and range library (Ricardo Yanez)</h4>
See <a href="http://www.calel.org/range.html">here</a> for details.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVHashList* KVRangeYanez::fMaterials = 0x0;

KVRangeYanez::KVRangeYanez()
      : KVIonRangeTable("RANGE",
      "Interface to Range dE/dx and range library (Ricardo Yanez)")
{
   // Default constructor
}

//________________________________________________________________

KVRangeYanez::KVRangeYanez (const KVRangeYanez& obj)  : KVIonRangeTable()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVRangeYanez::~KVRangeYanez()
{
   // Destructor
}

//________________________________________________________________

void KVRangeYanez::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVRangeYanez::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVIonRangeTable::Copy(obj);
   //KVRangeYanez& CastedObj = (KVRangeYanez&)obj;
}

KVIonRangeTableMaterial* KVRangeYanez::GetMaterial(const Char_t* material)
{
   // Returns pointer to material of given name or type.
   KVIonRangeTableMaterial* M = (KVIonRangeTableMaterial*)fMaterials->FindObject(material);
   if (!M) {
      M = (KVIonRangeTableMaterial*)fMaterials->FindObjectByType(material);
   }
   return M;
}

void KVRangeYanez::Print(Option_t*) const
{
   printf("KVRangeYanez::%s\n%s\n", GetName(), GetTitle());
   Int_t n = (fMaterials ? fMaterials->GetEntries() : 0);
   if(n){
      printf("\nEnergy loss & range tables loaded for %d materials:\n\n", fMaterials->GetEntries());
      fMaterials->ls();
   }
   else
      printf("\nEnergy loss & range tables loaded for 0 materials.\n");
}

TObjArray* KVRangeYanez::GetListOfMaterials()
{
   // Create and fill a list of all materials for which range tables exist.
   // Each entry is a TNamed with the name and type (title) of the material.
   // User's responsibility to delete list after use (it owns its objects).
   
   TObjArray* list = new TObjArray(fMaterials->GetEntries());
   list->SetOwner(kTRUE);
   TIter next(fMaterials);
   KVIonRangeTableMaterial* mat;
   while( (mat = (KVIonRangeTableMaterial*)next()) ){
      list->Add(new TNamed(mat->GetName(), mat->GetType()));
   }
   return list;
}
void KVRangeYanez::CheckMaterialsList()
{
   if(!fMaterials) {
      fMaterials=new KVHashList;
      fMaterials->SetName("RANGE materials list");
      fMaterials->SetOwner();
   }
}
void KVRangeYanez::AddElementalMaterial(Int_t z, Int_t a)
{
   // Adds a material composed of a single isotope of a chemical element.
   KVElementDensity*ed = (KVElementDensity*)gNDTManager->GetData(z,a,"ElementDensity");
   if(!ed){
      Error("AddElementalMaterial", 
            "No element found in ElementDensity NDT-table with Z=%d, A=%d",z,a);
      return;
   }
   TString state = "solid";
   if(ed->IsGas()) state="gas";
   KVRangeYanezMaterial* mat = 
         new KVRangeYanezMaterial(this, ed->GetElementName(),ed->GetElementSymbol(),
            state,z,a,ed->GetValue());
   mat->Initialize();
   CheckMaterialsList();
   fMaterials->Add(mat);
}

void KVRangeYanez::AddCompoundMaterial(
         const Char_t* name, const Char_t* symbol,
         Int_t nelem, Int_t* z, Int_t* a, Int_t* natoms, Double_t density)
{
   // Adds a compound material with a simple formula composed of different elements
   // For solids, give the density (in g/cm**3)
   
   TString state="gas";
   if(density>0) state="solid";
   KVRangeYanezMaterial* mat = 
         new KVRangeYanezMaterial(this, name, symbol, state, -1, -1, density);
   for(int i=0;i<nelem;i++){
      mat->AddCompoundElement(z[i],a[i],natoms[i]);
   }
   mat->Initialize();
   CheckMaterialsList();
   fMaterials->Add(mat);
}

void KVRangeYanez::AddMixedMaterial(
         const Char_t* name, const Char_t* symbol,
         Int_t nelem, Int_t* z, Int_t* a, Int_t* natoms, Double_t* weight, Double_t density)
{
}
