//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#include "KVINDRA_VAMOS.h"
#include "TPluginManager.h"
#include "KVSpectroDetector.h"

ClassImp(KVINDRA_VAMOS)

	////////////////////////////////////////////////////////////////////////////////
	// BEGIN_HTML <!--
	/* -->
	   <h2>KVINDRA_VAMOS</h2>
	   <h4>INDRA + VAMOS experimental setup for the e494s and the e503 experiments performed at GANIL</h4>
	   <!-- */
	// --> END_HTML
	////////////////////////////////////////////////////////////////////////////////

KVINDRA_VAMOS::KVINDRA_VAMOS()
{
   	// Default constructor
   	init();
}
//________________________________________________________________
void KVINDRA_VAMOS::init()
{
    //Basic initialisation called by constructor.

	Info("init","To be implemented");

	fVamos = NULL;
}
//________________________________________________________________

KVINDRA_VAMOS::~KVINDRA_VAMOS()
{
   	// Destructor
   	SafeDelete(fVamos);
}
//________________________________________________________________

void KVINDRA_VAMOS::Build(){
	// Overrides KVMultiDetArray::Build in order to set the name of the
	// couple INDRA+VAMOS. Set up the geometry of INDRA and VAMOS,
	// associate the acquistion parameters with detectors, etc...
	
	Info("Build","Building INDRA ...");
	KVINDRAe503::Build();
	SetName("INDRA_VAMOS");
	SetTitle("INDRA+VAMOS  experimental setup");

	// Build VAMOS spectrometer
	Info("Build","Building VAMOS ...");
	fVamos = KVVAMOS::MakeVAMOS(fDataSet.Data());

	fACQParams->AddAll(fVamos->GetACQParams());
	fDetectors.AddAll(fVamos->GetListOfDetectors());
//	fIDTelescopes->AddAll(fVamos->GetListOfIDTelescopes());
}
//________________________________________________________________

void KVINDRA_VAMOS::Clear(Option_t *opt ){
	//Reset all groups (lists of detected particles etc.)
    //and detectors in groups (energy losses, ACQparams etc. etc.)
    //and the target if there is one
    //and the VAMOS detectors

	KVINDRAe503::Clear( opt );
	if( fVamos ) fVamos->Clear( opt );
}
