//Created by KVClassFactory on Mon Feb 17 13:51:39 2014
//Author: John Frankland,,,

#include "KVFAZIAIDTelescope.h"

ClassImp(KVFAZIAIDTelescope)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIAIDTelescope</h2>
<h4>Identification for FAZIA array</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIAIDTelescope::KVFAZIAIDTelescope()
{
   // Default constructor
}

KVFAZIAIDTelescope::~KVFAZIAIDTelescope()
{
   // Destructor
}

//________________________________________________________________

void KVFAZIAIDTelescope::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVFAZIAIDTelescope::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVIDTelescope::Copy(obj);
   //KVFAZIAIDTelescope& CastedObj = (KVFAZIAIDTelescope&)obj;
}

