//Created by KVClassFactory on Wed Jul  9 15:38:39 2014
//Author: Guilain ADEMARD

#include "KVIDQAMarker.h"

ClassImp(KVIDQAMarker)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDQAMarker</h2>
<h4>Base class for identification markers corresponding to differents couples of mass and charge state</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDQAMarker::KVIDQAMarker()
{
   // Default constructor
   init();
}
//________________________________________________________________

KVIDQAMarker::KVIDQAMarker( KVIDQALine *parent, Int_t a){
	init();
	fA = a;
	fParent = parent;
}
//________________________________________________________________

KVIDQAMarker::~KVIDQAMarker(){
   // Destructor
}
//________________________________________________________________

void KVIDQAMarker::init(){
	// Initialization of data members. Called by constructors.
	fA = 0;
}
//________________________________________________________________

void KVIDQAMarker::Copy(TObject& obj) const{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVIDQAMarker::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   TMarker::Copy(obj);
   KVIDQAMarker& CastedObj = (KVIDQAMarker&)obj;
   CastedObj.fA = fA;
}
