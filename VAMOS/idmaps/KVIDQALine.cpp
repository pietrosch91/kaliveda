//Created by KVClassFactory on Fri Jun 20 12:07:39 2014
//Author: Guilain ADEMARD

#include "KVIDQALine.h"

ClassImp(KVIDQALine)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDQALine</h2>
<h4>Base class for identification ridge lines and spots corresponding to different masses and charge states respectively</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDQALine::KVIDQALine(){
   // Default constructor
	init();
}
//________________________________________________________________

KVIDQALine::~KVIDQALine(){
   // Destructor
}
//________________________________________________________________

void KVIDQALine::init(){
	// Initialization, used by constructors.

	fMarkers = new KVList;
	fMarkers->SetCleanup();

}
//________________________________________________________________

void KVIDQALine::Copy(TObject& obj) const{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVIDQALine::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVIDZALine::Copy(obj);
   KVIDQALine& CastedObj = (KVIDQALine&)obj;
   fMarkers->Copy((TObject &) (*CastedObj.GetMarkers()));
}
//________________________________________________________________

void KVIDQALine::IdentA( Double_t x, Double_t y, Int_t &A, Int_t &realA ){

	// TO BE IMPLEMENTED
}
