//Created by KVClassFactory on Thu Sep 13 10:51:51 2012
//Author: Guilain ADEMARD

#include "KVVAMOSReconNuc.h"

ClassImp(KVVAMOSReconNuc)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSReconNuc</h2>
<h4>Nucleus identified by VAMOS spectrometer</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVVAMOSReconNuc::KVVAMOSReconNuc()
{
   // Default constructor
}

//________________________________________________________________

KVVAMOSReconNuc::KVVAMOSReconNuc (const KVVAMOSReconNuc& obj)  : KVReconstructedNucleus()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVVAMOSReconNuc::~KVVAMOSReconNuc()
{
   // Destructor
}

//________________________________________________________________

void KVVAMOSReconNuc::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVVAMOSReconNuc::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVReconstructedNucleus::Copy(obj);
   //KVVAMOSReconNuc& CastedObj = (KVVAMOSReconNuc&)obj;
}

