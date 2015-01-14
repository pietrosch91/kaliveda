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

Int_t KVIDQAMarker::Compare(const TObject *obj) const
{
    // Compares numerical valur of X coordinate for sorting lists (such as KVList)
    
	const KVIDQAMarker *other = dynamic_cast<const KVIDQAMarker *>(obj);
    if(!other) return 0;
    // check for equality
    return ((other->GetX())>GetX() ? -1 : 1);
}
//________________________________________________________________

void KVIDQAMarker::SetParent( KVIDQALine *parent ){ fParent = parent; }
//________________________________________________________________

void KVIDQAMarker::SetPointIndexes( Int_t idx_low, Int_t idx_up, Double_t delta ) { 
	if( !fParent ) return;
	fDelta    = delta;
	fPtIdxLow = idx_low;
   	fPtIdxUp  = idx_up;
	UpdateXandY();
}

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
	   <<Form("PtIdxLow= %d, PtIdxUp= %d, fDelta= %f, X=%f, Y=%f, marker type=%d :",fPtIdxLow,fPtIdxUp,fDelta,fX,fY,fMarkerStyle)
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

	if( fParent && !fParent->IsEditable() ) return; 

	static Double_t x_prev;
	static Bool_t motion = kFALSE;

	TMarker::ExecuteEvent( event, px, py );

	switch (event)
    {
		case kButton1Down:
			x_prev = GetX();
			break;

		case kButton1Motion:
			motion = kTRUE;
			break;
		case kButton1Up:
			if( motion ){
				motion = kFALSE;
				if((fPtIdxLow>-1) && (fPtIdxUp>-1)){
					if( fPtIdxLow == fPtIdxUp ){
						fParent->SetPoint( fPtIdxLow, GetX(), GetY() );
						fParent->GetMarkers()->R__FOR_EACH(KVIDQAMarker,UpdateXandY)();
					}
					else{
						fPtIdxUp = fPtIdxLow = fParent->InsertPoint( fPtIdxUp, GetX(), GetY(), x_prev );
						fDelta   = 0;
						fParent->GetMarkers()->R__FOR_EACH(KVIDQAMarker,UpdateXandY)();
					}
				}
			}
			break;
	}
}
//________________________________________________________________

void KVIDQAMarker::SetPointIndexesAndX( Int_t idx_low, Int_t idx_up, Double_t x ){
	// Set indexes of the two neighbour points and the X coordinates of this
	// marker. Then fDelta and Y coordinates will be calculated and set to
	// this marker.

	if( !fParent ) return;

	Double_t x_low, y_low, x_up, y_up;
	if((fParent->GetPoint(idx_low, x_low, y_low)>-1) && (fParent->GetPoint(idx_up, x_up, y_up)>-1)){

		Double_t y;
		if( x_low == y_up ) y = y_low;
		else y = y_up + (x-x_up)*(y_low-y_up)/(x_low-x_up);

		Double_t L     = TMath::Sqrt( TMath::Power(x_up-x_low,2)+TMath::Power(y_up-y_low,2) );
		Double_t l     = TMath::Sqrt( TMath::Power(x-x_low,2)+TMath::Power(y-y_low,2) );

		if( L==0 ){ 
			fPtIdxLow = fPtIdxUp = idx_low;
			fDelta = 0.;
		}
		else{
			fPtIdxLow = idx_low;
   			fPtIdxUp  = idx_up;
			fDelta = l/L;
		}

		SetX( x );
		SetY( y_low + (y_up-y_low)*fDelta );
 	}
}
//________________________________________________________________

void KVIDQAMarker::WriteAsciiFile(ofstream & file)
{
    // Write attributes of this KVIDQAMarker in ascii file

	file<<fA<<"\t"<<fPtIdxLow<<"\t"<<fPtIdxUp<<"\t"<<fDelta<<"\t"<<fX<<"\t"<<fY<<endl;
}
//________________________________________________________________

void KVIDQAMarker::ReadAsciiFile(ifstream & file)
{
    // Read attributes of this KVIDQAMarker in ascii file

	Int_t a;
	file>>a>>fPtIdxLow>>fPtIdxUp>>fDelta>>fX>>fY;
	SetA( a );
}
//________________________________________________________________

