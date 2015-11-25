//Created by KVClassFactory on Mon Sep  3 11:29:00 2012
//Author: Guilain ADEMARD

#include "KVDriftChamber.h"

ClassImp(KVDriftChamber)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVDriftChamber</h2>
<h4>Drift Chamber, used at the focal plan of VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
void KVDriftChamber::init()
{
   // Initialise non-persistent pointers
}
//________________________________________________________________

KVDriftChamber::KVDriftChamber()
{
   // Default constructor
   init();
   SetType("FIL");
   SetName(GetArrayName());
}
//________________________________________________________________

KVDriftChamber::KVDriftChamber(const KVDriftChamber& obj)  : KVVAMOSDetector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVDriftChamber::~KVDriftChamber()
{
   // Destructor
}
//________________________________________________________________

void KVDriftChamber::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVDriftChamber::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVVAMOSDetector::Copy(obj);
   //KVDriftChamber& CastedObj = (KVDriftChamber&)obj;
}
//________________________________________________________________

const Char_t* KVDriftChamber::GetArrayName()
{
   // Name of detector given in the form
   // FIL_1 FIL_2
   // to be compatible with GANIL acquisition parameters.
   //
   // The root of the name is the detector type.
   fFName = Form("%s_%d", GetType(), GetNumber());
   return fFName.Data();
}

