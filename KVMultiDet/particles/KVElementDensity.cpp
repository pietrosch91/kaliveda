//Created by KVClassFactory on Thu Sep 27 17:23:23 2012
//Author: John Frankland,,,

#include "KVElementDensity.h"

ClassImp(KVElementDensity)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVElementDensity</h2>
<h4>Atomic element with name, symbol and density</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVElementDensity::KVElementDensity()
{
   // Default constructor
   fIsGas=kFALSE;
}

//________________________________________________________________

KVElementDensity::KVElementDensity (const KVElementDensity& obj)  : KVNuclData()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVElementDensity::~KVElementDensity()
{
   // Destructor
}

//________________________________________________________________

void KVElementDensity::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVElementDensity::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVNuclData::Copy(obj);
   //KVElementDensity& CastedObj = (KVElementDensity&)obj;
}

