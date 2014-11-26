//Created by KVClassFactory on Wed Jul  9 15:38:39 2014
//Author: Guilain ADEMARD

#include "KVIDQAMarker.h"
#include "KVIDQALine.h"
#include "TROOT.h"
#include "TClass.h"

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
	SetParent(parent);
	SetA(a);
}
//________________________________________________________________

KVIDQAMarker::~KVIDQAMarker(){
   // Destructor
}
//________________________________________________________________

void KVIDQAMarker::init(){
	// Initialization of data members. Called by constructors.
	SetA(0);
	fPtIdxLow = fPtIdxUp = -1;
	SetMarkerStyle( 21 );
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
   CastedObj.SetParent(fParent);
   CastedObj.SetA(fA);
   CastedObj.fPtIdxLow = fPtIdxLow;
   CastedObj.fPtIdxUp  = fPtIdxUp;
}
//________________________________________________________________

void KVIDQAMarker::SetParent( KVIDQALine *parent ){ fParent = parent; }
//________________________________________________________________


 Int_t KVIDQAMarker::GetQ() const{ 
	return (fParent ? fParent->GetQ() : 0); 
}
//________________________________________________________________

void KVIDQAMarker::ls(Option_t *) const
{
   // List TNamed name and title.

   TROOT::IndentLevel();
   cout <<"OBJ: " << IsA()->GetName() << "\t" << GetName() <<" : "
	   <<Form("PtIdxLow= %d PtIdxUp= %d X=%f Y=%f marker type=%d :",fPtIdxLow,fPtIdxUp,fX,fY,fMarkerStyle)
        << Int_t(TestBit(kCanDelete)) << " at: "<<this<< endl;
}

