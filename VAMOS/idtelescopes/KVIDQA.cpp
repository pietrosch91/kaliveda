//Created by KVClassFactory on Fri Jan 23 15:43:30 2015
//Author: ademard

#include "KVIDQA.h"
#include "KVIDQAGrid.h"

ClassImp(KVIDQA)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDQA</h2>
<h4>ID telescope used to identify Q and A with VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDQA::KVIDQA() : fQAGrid(NULL)
{
   // Default constructor

	// fIDCode = kIDCode???;
}
//________________________________________________________________

KVIDQA::~KVIDQA()
{
   // Destructor
}
//________________________________________________________________
   
const Char_t *KVIDQA::GetArrayName(){
 	// Name of identification telescope given in the form IDV_Det1_Det2.
 	// Prefix VID for Vamos IDentification.
   	// The detectors are signified by their names i.e. KVDetector::GetName

	
	KVDetector *det  = (KVDetector *)GetDetectors()->Last();
	if( det ){
		SetName( Form("VID_QA_%s", det->GetName()) );
		SetType( Form("%s"    , det->GetType()) );
	}
	else SetName( "VID_QA_EMPTY" );

	return fName.Data();
}
//________________________________________________________________
   
Bool_t KVIDQA::Identify(KVIdentificationResult *IDR, Double_t x, Double_t y){
	// Obsolete method for this class.

	Error("Identify(KVIdentificationResult *, Double_t , Double_t )","Obsolete method. Please use Identify(KVVAMOSReconNuc *, Double_t , Double_t )");
    return kFALSE;
}
//________________________________________________________________
   
Bool_t KVIDQA::Identify(KVVAMOSReconNuc *nuc, Double_t x, Double_t y){
 	//Particle identification and code setting using identification grid KVIDQAGrid

	Warning("Identify(KVVAMOSReconNuc *, Double_t , Double_t )","TO BE IMPLEMENTED");
    return kFALSE;
}
//________________________________________________________________

void KVIDQA::Initialize(){
	// Initialisation of telescope before identification.
    // This method MUST be called once before any identification is attempted.
    // Initialisation of grid is performed here.
    // IsReadyForID() will return kTRUE if a grid is associated to this telescope for the current run.

	fGrid  = (KVIDZAGrid *)GetIDGrid(1);
    if ( fGrid && fGrid->InheritsFrom(KVIDQAGrid::Class()) ){
		fQAGrid = (KVIDQAGrid *)fGrid;
        fGrid->Initialize();
        SetBit(kReadyForID);
    }
    else
        ResetBit(kReadyForID);
}
