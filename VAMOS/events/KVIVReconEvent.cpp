//Created by KVClassFactory on Mon Mar 18 10:44:28 2013
//Author: Guilain ADEMARD

#include "KVIVReconEvent.h"
#include "KVVAMOSReconEvent.h"
#include "KVVAMOS.h"
#include "KVTarget.h"

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
	fVAMOSevent    = NULL;
}
//________________________________________________________________

KVIVReconEvent::KVIVReconEvent(Int_t mult_indra, const char *cl_name_indra, Int_t mult_vamos, const char *cl_name_vamos)
:KVINDRAReconEvent( mult_indra, cl_name_indra){
   	// Default constructor
   	init();
	fVAMOSevent = new KVVAMOSReconEvent( mult_vamos, cl_name_vamos );
}
//________________________________________________________________

KVIVReconEvent::~KVIVReconEvent(){
   	// Destructor
   	SafeDelete( fVAMOSevent );
}
//________________________________________________________________

void KVIVReconEvent::Streamer(TBuffer & R__b){
   	//Stream an object of class KVIVReconEvent.
   	if (R__b.IsReading()) {
   		R__b.ReadClassBuffer(KVIVReconEvent::Class(), this);
   	} else {
   		R__b.WriteClassBuffer(KVIVReconEvent::Class(), this);
   	}
	Warning("Streamer","TO BE IMPLEMENTED");
}
//________________________________________________________________

void KVIVReconEvent::Clear(Option_t * opt){
	//Reset the event to zero ready for new event.

	KVINDRAReconEvent::Clear( opt );
	fVAMOSevent->Clear( opt );
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

	Warning("GetTotalMult","TO BE IMPLEMENTED");
   return GetMult( opt );
}
//________________________________________________________________

void KVIVReconEvent::Print(Option_t * option) const{
   //Print out list of particles in the INDRA's event and list of nuclei reconstructed
   //in VAMOS.
   //If option="ok" only particles and nuclei with IsOK=kTRUE are included.

	cout<<"------------- In INDRA --------------------"<<endl;
	KVINDRAReconEvent::Print( option );

	cout<<"------------- In VAMOS --------------------"<<endl;
	fVAMOSevent->Print( option );
}
