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
//________________________________________________________________

Int_t KVIDQALine::InsertMarker(Int_t a){
	// Insert a new Q-A identification marker at the mouse position

	Int_t idx = InsertPoint();
	if( idx>=0 ){
 		KVIDQAMarker *m = new KVIDQAMarker( this, a);
		AddMarker( m );
		IncrementPtIdxOfMarkers( idx );
		m->SetPointIndex( idx );
		m->SetX( fX[idx] );
		m->SetY( fY[idx] );
		m->Draw();
		Info("InsertMarker","Marker inserted at X=%f, Y=%f",fX[idx],fY[idx]);  
	}
	return idx;
}
//________________________________________________________________

void KVIDQALine::IncrementPtIdxOfMarkers( Int_t idx, Int_t ival ){

	TIter next( fMarkers );
	KVIDQAMarker *m = NULL;
	while( (m =(KVIDQAMarker *)next()) ){
		Int_t low, up;
 		m->GetPointIndexes(low, up);
		if( low>=idx ){
			low += ival;
			up  += ival;
			if( low < 0 ) low = 0;
			if( up  > fNpoints-1 ) up = fNpoints-1;

 		   	m->SetPointIndexes( low, up );
		}
 	}
}

