//Created by KVClassFactory on Fri Jun 20 12:07:39 2014
//Author: Guilain ADEMARD

#include "KVIDQALine.h"
#include "TPad.h"
#include "TROOT.h"

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

   	fMarkers->Delete();
   	SafeDelete( fMarkers );
}
//________________________________________________________________

void KVIDQALine::init(){
	// Initialization, used by constructors.

	fMarkers = new KVList;
	
	//-----------------------
	static Int_t iMarker = 0;
	fMarkers->SetName( Form( "fMarkers%d__%p",iMarker++,fMarkers) );
	//-----------------------

	fMarkers->SetCleanup();
	fNextA = 1;
	fNextAinc = kTRUE;

	//no draw points of this line, just draw its KVIDQAMarkers
	SetMarkerStyle(1);
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
   CastedObj.fNextA = fNextA;
   CastedObj.fNextAinc = fNextAinc;
   fMarkers->Copy((TObject &) (*CastedObj.GetMarkers()));
   CastedObj.GetMarkers()->R__FOR_EACH(KVIDQAMarker,SetParent)((&CastedObj));

}
//________________________________________________________________

void KVIDQALine::ExecuteEvent(Int_t event, Int_t px, Int_t py){

	KVIDZALine::ExecuteEvent( event, px, py );

	static Bool_t motion = kFALSE;
	switch (event)
    {
		case kButton1Motion:
			motion = kTRUE;
			break;
		case kButton1Up:
			if( motion ){
				motion = kFALSE;
				fMarkers->R__FOR_EACH(KVIDQAMarker,UpdateXandY)();
			}
			break;
	}
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
		if( (fNextAinc ? 1 : -1)*(a-fNextA)<0) fNextAinc = !fNextAinc;
		fNextA=a+( fNextAinc ? 1 : -1);

 		KVIDQAMarker *m = new KVIDQAMarker( this, a);
		AddMarker( m );
		m->SetPointIndex( idx );
		m->Draw();
//		fMarkers->ls();
//		Info("InsertMarker","Marker inserted at X=%f, Y=%f",fX[idx],fY[idx]);  
	}
	return idx;
}
//________________________________________________________________

Int_t KVIDQALine::RemoveMarker(Int_t a){

	Int_t idx = -1;
	KVIDQAMarker *m = NULL;
	TIter next( fMarkers );
	
	// look for a marker with 'a'
	if( a>0 ){
		while( (m =(KVIDQAMarker *)next()) ){
			if( a == m->GetA() ) break;
		}
	}
	// look for a marker at mouse position
	else{
		Int_t px = gPad->GetEventX();
   		Int_t py = gPad->GetEventY();

   		//localize makrer to be deleted
   		// start with a small window (in case the mouse is very close to one marker)
		while( (m =(KVIDQAMarker *)next()) ){
      		Int_t dpx = px - gPad->XtoAbsPixel(gPad->XtoPad( m->GetX() ));
      		Int_t dpy = py - gPad->YtoAbsPixel(gPad->YtoPad( m->GetY() ));
      		if (dpx * dpx + dpy * dpy < 100) break;
   		}
	}

	// if a marker is found ...
	if( m ){
		//... remove it from the marker list
		fMarkers->Remove( m );
		delete m;
	}
	return idx;
};
//________________________________________________________________

Int_t KVIDQALine::InsertPoint(){
	Int_t i = KVIDZALine::InsertPoint();
	if(i>=0) IncrementPtIdxOfMarkers(i);
//	Info("InsertPoint","Point %d inserted at X=%f, Y=%f",i,fX[i],fY[i]);  
	return i;
}
//________________________________________________________________

Int_t KVIDQALine::InsertPoint( Int_t i, Double_t x, Double_t y){

	if( (i<0) || (i>fNpoints)) return -1;
 	Double_t **ps = ExpandAndCopy(fNpoints + 1, i);
   CopyAndRelease(ps, i, fNpoints++, i + 1);

   // To avoid redefenitions in descendant classes
   FillZero(i, i+ 1);

   fX[i] = x;
   fY[i] = y;
	IncrementPtIdxOfMarkers(i);
//	Info("InsertPoint","Point %d inserted at X=%f, Y=%f",i,fX[i],fY[i]);  
	return i;
}
//________________________________________________________________

Int_t KVIDQALine::RemovePoint(Int_t i){
	// Delete point number ipoint and the associated ID marker
	// if it has one.
	
	KVIDQAMarker *m = NULL;
	TIter next( fMarkers );
	while( (m =(KVIDQAMarker *)next()) ){
		if( i == m->GetPointIndex() ) break;
	}
	if( m ){
 	   	fMarkers->Remove( m );
		delete m;
	}
	else{
		i = KVIDZALine::RemovePoint(i);
		if(i>=0) IncrementPtIdxOfMarkers(i,-1);
	}
	return i;
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
//________________________________________________________________

void KVIDQALine::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVIDQALine.
   // Resets the 'fParent' pointer of each KVIDQAMarker after reading them in.
   //
   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(KVIDQALine::Class(),this);
   	  fMarkers->R__FOR_EACH(KVIDQAMarker,SetParent)((this));
   } else {
      R__b.WriteClassBuffer(KVIDQALine::Class(),this);
   }
}
