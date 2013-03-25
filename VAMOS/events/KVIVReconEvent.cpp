//Created by KVClassFactory on Mon Mar 18 10:44:28 2013
//Author: Guilain ADEMARD

#include "KVIVReconEvent.h"
#include "KVVAMOS.h"

using namespace std;

ClassImp(KVIVReconEvent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIVReconEvent</h2>
<h4>Event reconstructed from energy losses in INDRA array and VAMOS spectrometer</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
void KVIVReconEvent::init(){
	//Default initialisations
	fVAMOSnuc      = new KVVAMOSReconNuc;
	fNucInVAMOS    = kFALSE;
	fVAMOSCodeMask = NULL;
}
//________________________________________________________________

KVIVReconEvent::KVIVReconEvent(Int_t mult, const char *classname)
:KVINDRAReconEvent( mult, classname){
   	// Default constructor
   	init();
}
//________________________________________________________________

KVIVReconEvent::~KVIVReconEvent(){
   	// Destructor
   	SafeDelete( fVAMOSnuc );
   	SafeDelete( fVAMOSCodeMask );
}
//________________________________________________________________

void KVIVReconEvent::Streamer(TBuffer & R__b){
   	//Stream an object of class KVIVReconEvent. The KVVAMOSReconNuc member
   	//object is streamed only if a nucleus is measured in VAMOS i.e.
   	//fNucInVAMOS = true.

   	if (R__b.IsReading()) {
   		R__b.ReadClassBuffer(KVIVReconEvent::Class(), this);
		if(fNucInVAMOS){
 		   	R__b.StreamObject( fVAMOSnuc );
			if (CheckVAMOSCodes(fVAMOSnuc->GetCodes()))
            fVAMOSnuc->SetIsOK();
         else
            fVAMOSnuc->SetIsOK(kFALSE);
		}
   	} else {
   		R__b.WriteClassBuffer(KVIVReconEvent::Class(), this);
		if(fNucInVAMOS) R__b.StreamObject( fVAMOSnuc );
   	}
}
//________________________________________________________________

void KVIVReconEvent::AcceptECodesVAMOS (UChar_t code){
	//Define the calibration codes for VAMOS's detectors that you want to include 
	//in your analysis.
   	//Example: to keep only ecodes 1 and 2, use
   	//        event.AcceptECodesVAMOS( kECode2 | kECode3 );
   	//If the nucleus reconstructed in VAMOS have the correct E codes it will 
   	//have IsOK() = kTRUE.
   	//If this nucleus does not have IsOK() = kTRUE then the method GetVAMOSNuc("ok")
   	//will return a null pointer.
   	//
   	//To remove any previously defined acceptable calibration codes, use AcceptECodesVAMOS(0).
   	//
   	//N.B. : this method is preferable to using directly the KVAMOSCodes pointer 
   	//of the nucleus as the 'IsOK' status of this nucleus of the current event
   	//is automatically updated when the code mask is changed using this method.
   	
	GetVAMOSCodeMask()->SetEMask(code);
   	KVVAMOSReconNuc *nuc = GetVAMOSNuc();
   	if( nuc ){
	   	if (CheckVAMOSCodes(nuc->GetCodes()))
         	nuc->SetIsOK();
      	else
         	nuc->SetIsOK(kFALSE);
   	}
}
//________________________________________________________________

void KVIVReconEvent::AcceptFPCodesVAMOS(UInt_t code){
	//Define the VAMOS Focal plan Position reconstruction (FP) codes that you want
	//to include in your analysis.
   	//Example: to keep only codes 1 and 2, use
   	//        event.AcceptECodesVAMOS( kFPCode2 | kFPCode3 );
   	//If the nucleus reconstructed in VAMOS have the correct FP codes it will 
   	//have IsOK() = kTRUE.
   	//If this nucleus does not have IsOK() = kTRUE then the method GetVAMOSNuc("ok")
   	//will return a null pointer.
   	//
   	//To remove any previously defined acceptable FP codes, use AcceptFPCodesVAMOS(0).
   	//
   	//N.B. : this method is preferable to using directly the KVAMOSCodes pointer 
   	//of the nucleus as the 'IsOK' status of this nucleus of the current event
   	//is automatically updated when the code mask is changed using this method.
   	
	GetVAMOSCodeMask()->SetFPMask(code);
   	KVVAMOSReconNuc *nuc = GetVAMOSNuc();
   	if( nuc ){
	   	if (CheckVAMOSCodes(nuc->GetCodes()))
         	nuc->SetIsOK();
      	else
         	nuc->SetIsOK(kFALSE);
   	}

}
//________________________________________________________________

void KVIVReconEvent::AcceptIDCodesVAMOS(UShort_t code){
	//Define the identification codes for VAMOS's detectors that you want to include 
	//in your analysis.
   	//Example: to keep only ID codes 1 and 2, use
   	//        event.AcceptIDCodesVAMOS( kIDCode2 | kIDCode3 );
   	//If the nucleus reconstructed in VAMOS have the correct ID codes it will 
   	//have IsOK() = kTRUE.
   	//If this nucleus does not have IsOK() = kTRUE then the method GetVAMOSNuc("ok")
   	//will return a null pointer.
   	//
   	//To remove any previously defined acceptable identification codes, use AcceptIDCodesVAMOS(0).
   	//
   	//N.B. : this method is preferable to using directly the KVAMOSCodes pointer 
   	//of the nucleus as the 'IsOK' status of this nucleus of the current event
   	//is automatically updated when the code mask is changed using this method.
   	
	GetVAMOSCodeMask()->SetIDMask(code);
   	KVVAMOSReconNuc *nuc = GetVAMOSNuc();
   	if( nuc ){
	   	if (CheckVAMOSCodes(nuc->GetCodes()))
         	nuc->SetIsOK();
      	else
         	nuc->SetIsOK(kFALSE);
   	}
}
//________________________________________________________________

void KVIVReconEvent::Clear(Option_t * opt){
	//Reset the event to zero ready for new event.

	KVINDRAReconEvent::Clear( opt );
	if(fNucInVAMOS){
		fVAMOSnuc->Clear();
		fNucInVAMOS = kFALSE;
	}
}
//________________________________________________________________

Int_t KVIVReconEvent::GetTotalMult(Option_t * opt)
{
   //Returns total multiplicity (number of particles in INDRA + nucleus in VAMOS) of event.
   //If opt = "" (default), returns number of particles in TClonesArray* fParticles + one if nucleus is reconstructed in VAMOS
   //If opt = "ok" only particles and VAMOS's nucleus  with IsOK()==kTRUE are included.
   //
   //IN THE LATTER CASE, YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE INDRA'S EVENT USING GETNEXTPARTICLE() !!!
   //
   //BE CAREFUL: the method GetMult(Option_t *) does not return the total
   //multiplicity but only the multiplicity in INDRA.

   return GetMult( opt ) + ( GetVAMOSNuc( opt ) != NULL ) ;
}
//________________________________________________________________

KVVAMOSReconNuc *KVIVReconEvent::GetVAMOSNuc(Option_t *opt){
//Returns the nucleus reconstructed in the VAMOS event.
//Returns a NULL pointer if no nucleus is reconstructed.
//
//If opt="ok" the nucleus will be returned if FP codes, ID codes and E codes correspond to those set using AcceptFPCodesVAMOS, AcceptIDCodesVAMOS and
//AcceptECodesVAMOS.

	if( !fNucInVAMOS ) return NULL;

	TString Opt( opt );
	Opt.ToUpper();
	if( Opt == "OK" ) return ( fVAMOSnuc->IsOK() ? fVAMOSnuc : NULL );

	return fVAMOSnuc;
}
//________________________________________________________________

void KVIVReconEvent::Print(Option_t * option) const
{
   //Print out list of particles in the INDRA's event and nucleus reconstructed
   //in VAMOS.
   //If option="ok" only particles and nucleus with IsOK=kTRUE are included.

	KVINDRAReconEvent::Print( option );

	cout<<"------------- In VAMOS spectrometer --------------------"<<endl;
   KVVAMOSReconNuc *nuc = ((KVIVReconEvent *)this)->GetVAMOSNuc( option );
   if( nuc ) nuc->Print();
   else cout<<"NO NUCLEUS"<<endl;
}
//________________________________________________________________

void KVIVReconEvent::CalibrateAndIdentifyVAMOSEvent(){

	Warning("CalibrateAndIdentifyVAMOSEvent","TO BE IMPLEMENTED");
}
//________________________________________________________________

Bool_t KVIVReconEvent::ReconstructVAMOSEvent(){

	KVList *detl = gVamos->GetFiredDetectors( GetPartSeedCond() );

	//If no fired detectors then no reconstruction
	if( !detl->GetEntries() ) return kFALSE;
	//else a Nucleus can be recontructed in VAMOS
	fNucInVAMOS = kTRUE;

	fVAMOSnuc->ConstructFocalPlanTrajectory( detl );
	fVAMOSnuc->Calibrate();
	fVAMOSnuc->ConstructLabTrajectory();

	return kTRUE;
}
