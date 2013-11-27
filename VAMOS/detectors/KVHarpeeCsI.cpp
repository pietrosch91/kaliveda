//Created by KVClassFactory on Wed Jul 25 10:12:53 2012
//Author: Guilain ADEMARD

#include "KVHarpeeCsI.h"
#include "TGeoBBox.h"
#include "KVUnits.h"
#include "TClass.h"

ClassImp(KVHarpeeCsI)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVHarpeeSi</h2>
<h4>CsI detectors of Harpee, used at the focal plan of VAMOS for the e503 experiment</h4>
<!-- */
// --> END_HTML
//
// In order to create a detector, use the KVHarpeeCsI::KVHarpeeCsI()
// constructor.
//
// Type of detector : "CSI"
////////////////////////////////////////////////////////////////////////////////

KVList      *KVHarpeeCsI::fHarpeeCsIList  = NULL;
KVHarpeeCsI *KVHarpeeCsI::fCsIForPosition = NULL;
KVString    KVHarpeeCsI::fACQParamTypes("0:E, 9:NO_TYPE");
KVString    KVHarpeeCsI::fPositionTypes("9:NO_TYPE");

void KVHarpeeCsI::init(){
	// Initialise non-persistent pointers

	if(!fHarpeeCsIList){
 	   	fHarpeeCsIList = new KVList( kFALSE );
	}
	fHarpeeCsIList->Add( this );
	
	fCal = NULL;

	//fCanalE = NULL;

	// fSegment is set to 1 because this CsI detector is
	// an independant detector (see KVGroup::AnalyseParticles for
	// more information)
	fSegment = 1;
}
//________________________________________________________________

KVHarpeeCsI::KVHarpeeCsI(){
   	// Default constructor.
	init();
	SetType("CSI");
	SetLabel("CSI");
   	SetName(GetArrayName());
}
//________________________________________________________________


KVHarpeeCsI::KVHarpeeCsI (const KVHarpeeCsI& obj)  : KVVAMOSDetector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVHarpeeCsI::~KVHarpeeCsI()
{
   // Destructor
	fHarpeeCsIList->Remove( this );
	if( fHarpeeCsIList && !fHarpeeCsIList->GetEntries() ) SafeDelete( fHarpeeCsIList );
}
//________________________________________________________________

void KVHarpeeCsI::Copy (TObject& obj) const
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

const Char_t* KVHarpeeCsI::GetArrayName(){
	// Name of detector given in the form 
	// CSI01 CSI02 ...
	// to be compatible with GANIL acquisition parameters.
	//
	// The root of the name is the detector type + number.
	
	fFName = Form("%s%02d",GetType(),GetNumber());
	return fFName.Data();
}
//________________________________________________________________

KVHarpeeCsI *KVHarpeeCsI::GetFiredHarpeeCsI(Option_t *opt){
	// This static method returns the first fired detector found
	// in the list of all the existing silicon detectors of HARPEE.
	// See KVDetector::Fired() for more information about the option 'opt'.

	TIter next( fHarpeeCsIList );
	KVHarpeeCsI *csi = NULL;
	while( (csi =(KVHarpeeCsI *)next()) ){
		if( csi->Fired( opt ) ) return csi;
	}
	return NULL;
}
//________________________________________________________________

KVList *KVHarpeeCsI::GetHarpeeCsIList(){ 
	//Returns the global list of all KVHarpeeSi objects.
	return fHarpeeCsIList;
}
//________________________________________________________________

Int_t KVHarpeeCsI::GetMult(Option_t *opt){
	// Returns the multiplicity of fired silicon detectors of HARPEE. 
	// See KVDetector::Fired() for more information about the option 'opt'.

	Int_t mult   = 0;

	TIter next( fHarpeeCsIList );
	KVHarpeeCsI *csi = NULL;
	while( (csi = (KVHarpeeCsI *)next()) ){
		if( csi->Fired( opt ) ) mult++;
	}
	return mult;
}
//________________________________________________________________

void KVHarpeeCsI::SetACQParams(){
// Setup the energy acquisition parameter for this silicon detector.
// This parameter has the name of the detector and has the type 'E' 
// (for energy).
// 
	KVACQParam *par = new KVACQParam;
	par->SetName( GetEBaseName() );
	par->SetType("E");
	par->SetNumber( GetNumber() );
	par->SetUniqueID( CalculateUniqueID( par ) );
	AddACQParam(par);
}
//________________________________________________________________

const Char_t *KVHarpeeCsI::GetEBaseName() const{
	// Base name of the energy used to be compatible
	// GANIL acquisition parameters
	//
	// The base name is "E<type><number>".
	
	return Form("%s%.2d",GetType(),GetNumber());
}
//______________________________________________________________________________
void KVHarpeeCsI::SetCalibrators(){
	// Pulse Height Defect calibrator as well as the calibrators of
	// KVVAMOSDetector.
	fCal = new KVLightEnergyCsIVamos(this);
	fCal->SetType( Form("Light->MeV %s",GetName() ) );
	AddCalibrator(fCal);
	
}
//________________________________________________________________

void KVHarpeeCsI::Initialize(){
	// Initialize the data members. Called by KVVAMOS::Initialize().

}
//________________________________________________________________
