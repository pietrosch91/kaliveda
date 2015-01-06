//Created by KVClassFactory on Wed Jul  9 15:38:39 2014
//Author: Guilain ADEMARD

#include "KVIDQAMarker.h"
#include "KVIDQALine.h"
#include "TROOT.h"
#include "TClass.h"

#ifndef ROOT_Buttons
#include "Buttons.h"
#endif

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

	// delete associated ID line point.
	Int_t i = GetPointIndex();	
	if(i>=0){
	  if( fParent && (fParent->KVIDZALine::RemovePoint(i)>=0) )
 	  	  fParent->IncrementPtIdxOfMarkers(i,-1);
	}
}
//________________________________________________________________

void KVIDQAMarker::init(){
	// Initialization of data members. Called by constructors.
	fParent = NULL;
	SetA(0);
	fPtIdxLow = fPtIdxUp = -1;
	fDelta = 0;
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
   //CastedObj.SetParent(fParent);
   CastedObj.SetA(fA);
   CastedObj.fPtIdxLow = fPtIdxLow;
   CastedObj.fPtIdxUp  = fPtIdxUp;
   CastedObj.fDelta    = fDelta;
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
//________________________________________________________________

void KVIDQAMarker::UpdateXandY(){
	// Set X and Y of this marker calculated from the coordinates of low point, up point, and fDelta.
	// fDelta is the distance between this marker end the low point,
	// normalized to the distance between low and up points;
	if( !fParent ) return;
	Double_t x_low, y_low, x_up, y_up;
	if((fParent->GetPoint(fPtIdxLow, x_low, y_low)>-1) && (fParent->GetPoint(fPtIdxUp, x_up, y_up)>-1)){
		SetX( x_low + (x_up-x_low)*fDelta );
		SetY( y_low + (y_up-y_low)*fDelta );
 	}
}
//________________________________________________________________

void KVIDQAMarker::ExecuteEvent(Int_t event, Int_t px, Int_t py){

	TMarker::ExecuteEvent( event, px, py );

	static Bool_t motion = kFALSE;


	switch (event)
    {
		case kButton1Motion:
			motion = kTRUE;
			break;
		case kButton1Up:
			if( motion ){
				motion = kFALSE;
				if( fParent && (fPtIdxLow>-1) && (fPtIdxUp>-1)){
					if( fPtIdxLow == fPtIdxUp ){
						fParent->SetPoint( fPtIdxLow, GetX(), GetY() );
					}
					else{
						fPtIdxUp = fPtIdxLow; 
						fDelta   = 0;
						fParent->InsertPoint( fPtIdxUp, GetX(), GetY() );
					}
				}
			}
			break;
	}
}
