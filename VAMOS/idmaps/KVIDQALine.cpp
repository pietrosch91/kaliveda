//Created by KVClassFactory on Fri Jun 20 12:07:39 2014
//Author: Guilain ADEMARD

#include "KVIDQALine.h"
#include "TPad.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TSpectrum.h"

ClassImp(KVIDQALine)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDQALine</h2>
<h4>Base class for identification ridge lines and spots corresponding to different charge states and masses respectively</h4>
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

 		   	m->SetPointIndexes( low, up, m->GetDelta() );
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
//________________________________________________________________

void KVIDQALine::FindAMarkers(TH1 *h){

   //find mass peaks in histogram h.
   //Use TSpectrum to find the peak candidates.
   //Build KVIDQAMarker for each peak found.
   
   TSpectrum *s = new TSpectrum(60);
   Int_t nfound = s->Search(h,2,"goff");

   Info("FindAMarkers","%d masses found in histogram %s for %s line",nfound,h->GetName(),GetName());

   // Clear list of IDQAMarkers to build a new one
   fMarkers->Clear();

   // Build new IDQAMarkers
   Float_t *xpeaks = s->GetPositionX();
   for (int p=0;p<nfound;p++){
      Double_t x = xpeaks[p];
 	  KVIDQAMarker *m = new KVIDQAMarker;
	  m->SetParent( this );
	  m->SetX( x );
	  AddMarker( m );
   }

   // sort fMarkers list from X coordinate
   fMarkers->Sort();

   // find neighbours simply looping  all points
   // and find also the 2 adjacent points: (low2 < low < x < up < up2 )
   // needed in case x is outside the graph ascissa interval
   Int_t low  = -1;
   Int_t low2 = -1;
   TIter next( fMarkers );
   KVIDQAMarker *m = NULL;

   Int_t a = 0; // we assum that xpeaks gives A/Q
   Bool_t stop = kFALSE;
   KVList del_list;
   while( (m =(KVIDQAMarker *)next()) ){

	   Double_t x = m->GetX();
	   Double_t tmp = TMath::Nint(x*GetQ());
	   if( a<=tmp ) a=tmp;

	   m->SetA( a++);
	   Int_t up  = -1;
   	   Int_t up2 = -1;

	   if( !stop ){
	   	   for (Int_t i = (low>-1 ? low : 0 ); i < fNpoints; ++i) {
         	   if (fX[i] < x){
            	   if (low == -1 || fX[i] > fX[low])  {
               		   low2 = low;
               		   low = i;
            	   } else if (low2 == -1) low2 = i;
         	   } else if (fX[i] > x) {
            	   if (up  == -1 || fX[i] < fX[up])  {
               		   up2 = up;
               		   up = i;
            	   } else if (up2 == -1) up2 = i;
         	   } else{ // case x == fX[i]
			 	   low = up = i;
			 	   break;
		 	   }
      	   }
	   }

 	   // treat cases when x is outside graph min max abscissa
       if (up == -1)  {
           up  = low;
           low = low2;
       }
       if (low == -1) {
           low = up;
           up  = up2;
       }
       // treat cases when x is outside line min max abscissa
       if ((up == -1) || (low==-1)){
		   // list the markers which are outside line to be deleted
		   del_list.Add(m);
		   stop = kTRUE; 
		   continue;
       }
	  
	   // set indexes of neighbour points and the X coordinate
	   // to the marker
   	   m->SetPointIndexesAndX( low, up, x );
	   m->Draw();
   }
 
   if( del_list.GetEntries() ){
	   Warning("FindAMarkers","markers will be deleted (outside of %s-line)",GetName());
	   del_list.ls();
   }
}
//________________________________________________________________

