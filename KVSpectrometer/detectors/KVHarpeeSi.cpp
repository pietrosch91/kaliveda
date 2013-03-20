//Created by KVClassFactory on Wed Jul 25 10:12:53 2012
//Author: Guilain ADEMARD

#include "KVHarpeeSi.h"
#include "TGeoBBox.h"
#include "KVUnits.h"

ClassImp(KVHarpeeSi)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVHarpeeSi</h2>
<h4>Silicon detectors of Harpee, used at the focal plan of VAMOS</h4>
<!-- */
// --> END_HTML
//
// In order to create a detector, use the KVHarpeeSi::KVHarpeeSi()
// constructor.
//
// Type of detector : "SI"
////////////////////////////////////////////////////////////////////////////////

KVList     *KVHarpeeSi::fHarpeeSiList  = NULL;
KVHarpeeSi *KVHarpeeSi::fSiForPosition = NULL;

void KVHarpeeSi::init(){
	// Initialise non-persistent pointers

	if(!fHarpeeSiList){
 	   	fHarpeeSiList = new KVList( kFALSE );
//		fHarpeeSiList->SetCleanup();
	}
	fHarpeeSiList->Add( this );
}
//________________________________________________________________

KVHarpeeSi::KVHarpeeSi(){
   	// Default constructor.
	init();
	SetType("SI");
   	SetName(GetArrayName());
}
//________________________________________________________________

KVHarpeeSi::KVHarpeeSi(UInt_t number, Float_t thick) : KVVAMOSDetector(number, "Si")
{
   // Make a silicon detector of Harpee.
   // Type of detector: "SI"

   init();
   SetType("SI");
   SetName(GetArrayName());

	Float_t w  = 1.;                // width
	Float_t h  = 1.;                // height
	Float_t th = thick*KVUnits::um; // thickness

	
	Double_t dx = w/2;
	Double_t dy = h/2;
	Double_t dz = th/2;

	fTotThick+=th;

	// adding the absorber
	TGeoShape *shape = new TGeoBBox(dx,dy,dz);
	AddAbsorber("Si",shape,0,kTRUE);

}
//________________________________________________________________

KVHarpeeSi::KVHarpeeSi (const KVHarpeeSi& obj)  : KVVAMOSDetector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVHarpeeSi::~KVHarpeeSi()
{
   // Destructor
	fHarpeeSiList->Remove( this );
	if( fHarpeeSiList && !fHarpeeSiList->GetEntries() ) SafeDelete( fHarpeeSiList );
}
//________________________________________________________________

void KVHarpeeSi::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVHarpeeSi::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVVAMOSDetector::Copy(obj);
   //KVHarpeeSi& CastedObj = (KVHarpeeSi&)obj;
}
//________________________________________________________________

const Char_t* KVHarpeeSi::GetArrayName(){
	// Name of detector given in the form 
	// SIE_01 SIE_02 ...
	// to be compatible with GANIL acquisition parameters.
	//
	// The root of the name is the detector type + number.
	
	fFName = Form("%s_%02d",GetType(),GetNumber());
	return fFName.Data();
}
//________________________________________________________________

Double_t KVHarpeeSi::GetCalibT(const Char_t *type){
	// Calculate calibrated time in ns of type 'type' (SI_HF, SI_SED1,
	// SI_INDRA, SI_MCP, ...) for coder values.
	// Returns 0 if calibration not ready or time ACQ parameter not fired.
	// (we require that the acquisition parameter has a value
   	// greater than the current pedestal value).
   	// The reference time T0 used for the calibration is the one of the
   	// fired silicon detector of Harpee which stopped the time.


	KVHarpeeSi *firedSi = GetFiredHarpeeSi();
	if( !firedSi ) return 0;

	KVFunctionCal *cal = GetTCalibrator( type );
	if( cal && cal->GetStatus() && cal->GetACQParam()->Fired("P"))
		return cal->Compute() + firedSi->GetT0( type );
	return 0;
}
//________________________________________________________________

const Char_t *KVHarpeeSi::GetEBaseName() const{
	// Base name of the energy used to be compatible
	// GANIL acquisition parameters
	//
	// The base name is "E<type><number>".
	
	return Form("%sE_%.2d",GetType(),GetNumber());
}
//________________________________________________________________

KVHarpeeSi *KVHarpeeSi::GetFiredHarpeeSi(Option_t *opt){
	// This static method returns the first fired detector found
	// in the list of all the existing silicon detectors of HARPEE.
	// See KVDetector::Fired() for more information about the option 'opt'.

	TIter next( fHarpeeSiList );
	KVHarpeeSi *si = NULL;
	while( (si =(KVHarpeeSi *)next()) ){
		if( si->Fired( opt ) ) return si;
	}
	return NULL;
}
//________________________________________________________________

KVList *KVHarpeeSi::GetHarpeeSiList(){ 
	//Returns the global list of all KVHarpeeSi objects.
	return fHarpeeSiList;
}
//________________________________________________________________

Int_t KVHarpeeSi::GetMult(Option_t *opt){
	// Returns the multiplicity of fired silicon detectors of HARPEE. 
	// See KVDetector::Fired() for more information about the option 'opt'.

	Int_t mult   = 0;

	TIter next( fHarpeeSiList );
	KVHarpeeSi *si = NULL;
	while( (si = (KVHarpeeSi *)next()) ){
		if( si->Fired( opt ) ) mult++;
	}
	return mult;
}
//________________________________________________________________

void KVHarpeeSi::Initialize(){
	// Initialize the data members. Called by KVVAMOS::Initialize().
	fSiForPosition = NULL;
	ResetBit( kPosIsOK );
}
//________________________________________________________________

Bool_t KVHarpeeSi::PositionIsOK(){
	// Returns true if all the conditions to access to the particle position
	// are verified. In this case the position is given by the method 
	// GetPosition(...). 
	// The conditions are:
	//   -the multiplicity of fired ( "Pany" option of Fired() ) Harpee Si 
	//    detectors must be equal to one;
	//   -this detectector must be the fired detector.
	
	if( !TestBit( kPosIsOK ) ){
		Int_t mult   = 0;
		TIter next( fHarpeeSiList );
		KVHarpeeSi *si = NULL;
		while( (si = (KVHarpeeSi *)next()) ){
			if( si->Fired( "Pany" ) ){
				mult++;
				fSiForPosition = si;
 			}
		}
		if( mult != 1 ) fSiForPosition = NULL;
		SetBit( kPosIsOK );
	}
	return fSiForPosition == this;
}
//________________________________________________________________

void KVHarpeeSi::SetACQParams(){
// Setup the energy acquisition parameter for this silicon detector.
// This parameter has the name of the detector and has the type 'E' 
// (for energy).
// 
	KVACQParam *par = new KVACQParam;
	par->SetName( GetEBaseName() );
	par->SetType("E");
	par->SetUniqueID( GetNumber()*1000000 + 300000+ 90000 + 1 );
	par->SetNumber( GetNumber() );
	AddACQParam(par);
}
