//Created by KVClassFactory on Wed Jul 25 09:43:37 2012
//Author: Guilain ADEMARD

#include "KVVAMOSDetector.h"

ClassImp(KVVAMOSDetector)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSDetector</h2>
<h4>Detectors of VAMOS spectrometer</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVVAMOSDetector::KVVAMOSDetector()
{
   // Default constructor
}
//________________________________________________________________

KVVAMOSDetector::KVVAMOSDetector(UInt_t number, const Char_t* type) : KVSpectroDetector(type){
	// create a new VAMOS detector of a given material.

	SetNumber(number);
}
//________________________________________________________________

KVVAMOSDetector::KVVAMOSDetector (const KVVAMOSDetector& obj)  : KVSpectroDetector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVVAMOSDetector::~KVVAMOSDetector()
{
   // Destructor
}

//________________________________________________________________

void KVVAMOSDetector::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVVAMOSDetector::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVSpectroDetector::Copy(obj);
   //KVVAMOSDetector& CastedObj = (KVVAMOSDetector&)obj;
}

