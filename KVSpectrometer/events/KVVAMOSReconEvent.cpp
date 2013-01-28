//Created by KVClassFactory on Thu Sep 13 10:49:45 2012
//Author: Guilain ADEMARD

#include "KVVAMOSReconEvent.h"

ClassImp(KVVAMOSReconEvent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSReconEvent</h2>
<h4>Class for reconstructed VAMOS events</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVVAMOSReconEvent::KVVAMOSReconEvent(Int_t mult, const Char_t *classname): KVEvent(mult,classname)
{
   // Default constructor
   init();
   CustomStreamer(); //because KVVAMOSReconNuc has a customised streamer
}
//________________________________________________________________

KVVAMOSReconEvent::~KVVAMOSReconEvent()
{
   // Destructor
}
//________________________________________________________________

void KVVAMOSReconEvent::init(){
	// Default initialisations
	fNucSeedCond = "all";
}
//________________________________________________________________

void KVVAMOSReconEvent::ECalibrateEvent(){

	Warning("ECalibrateEvent","To be implemented");
}
//________________________________________________________________

void KVVAMOSReconEvent::IdentifyEvent(){

	Warning("IdentifyEvent","To be implemented");
}
//________________________________________________________________

void KVVAMOSReconEvent::Print(Option_t *opt) const{

	Warning("Print","To be implemented");
}
//________________________________________________________________

void KVVAMOSReconEvent::ReconstructEvent(){

	Warning("ReconstructEvent","To be implemented");
}
//________________________________________________________________

void KVVAMOSReconEvent::SecondaryIdentCalib(){

	Warning("SecondaryIdentCalib","To be implemented");
}
//________________________________________________________________

void KVVAMOSReconEvent::XYCalibrateEvent(){

	Warning("XYCalibrateEvent","To be implemented");
}

//________________________________________________________________



