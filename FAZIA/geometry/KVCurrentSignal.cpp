//Created by KVClassFactory on Tue Jan 13 15:11:11 2015
//Author: ,,,

#include "KVCurrentSignal.h"

ClassImp(KVCurrentSignal)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVCurrentSignal</h2>
<h4>digitized intensity signal</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVCurrentSignal::KVCurrentSignal()
{
   // Default constructor
}

//________________________________________________________________

KVCurrentSignal::KVCurrentSignal(const char* name) : KVSignal(name, "Current")
{
   // Write your code here
}

//________________________________________________________________

KVCurrentSignal::~KVCurrentSignal()
{
   // Destructor
}

//________________________________________________________________

void KVCurrentSignal::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVCurrentSignal::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVSignal::Copy(obj);
   //KVCurrentSignal& CastedObj = (KVCurrentSignal&)obj;
}

