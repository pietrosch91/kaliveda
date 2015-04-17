//Created by KVClassFactory on Fri Apr 17 10:19:02 2015
//Author: John Frankland,,,

#include "KVGenPhaseSpace.h"

ClassImp(KVGenPhaseSpace)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGenPhaseSpace</h2>
<h4>Generate momenta for an event using microcanonical phase space sampling</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVGenPhaseSpace::KVGenPhaseSpace()
{
   // Default constructor
}

//________________________________________________________________

KVGenPhaseSpace::KVGenPhaseSpace(const Char_t* name, const Char_t* title) : KVBase(name, title)
{
   // Write your code here
}

KVGenPhaseSpace::~KVGenPhaseSpace()
{
   // Destructor
}

//________________________________________________________________

void KVGenPhaseSpace::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVGenPhaseSpace::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVBase::Copy(obj);
   //KVGenPhaseSpace& CastedObj = (KVGenPhaseSpace&)obj;
}

