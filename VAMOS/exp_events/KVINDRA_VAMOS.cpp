//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#include "KVINDRA_VAMOS.h"
#include "KVINDRAe503.h"
#include "KVVAMOS.h"
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
	//
	// This class descibes the coupling of INDRA ( a KVINDRA object ) and 
	// VAMOS ( a KVVAMOS object ) in a same KVMultiDetArray object.
	// These two multidetectors are built in the method Build. After the call
	// of this method the following global pointers correspond to:
	//  gIndra         -->  INDRA
	//  gVamos         -->  VAMOS
	//  gMultiDetArray -->  INDRA_VAMOS
	//
	//  The method GetACQParams returns the list of ALL the acquisition parameters of 
	//  INDRA and VAMOS. Idem for the list of detectors and the list of ID telescopes
	//  with the methods GetDetectors and GetListOfIDTelescopes respectively.
	//
	//  To create a KVINDRA_VAMOS instance for a given dataset by using KVDataSet::BuildMultiDetector()
	//  method change the Plugin.KVMultiDetArray plugin in the $KVROOT/KVFiles/.kvrootrc
	//  configuration file:
	//  +Plugin.KVMultiDetArray:    INDRA_e494s    KVINDRA_VAMOS     VAMOS    "KVINDRA_VAMOS()"
	//
	//  TARGET
	//
	//  The target has to be set to the main multidetector pointed by gMultiDetArray as it is done in KVIVUpdater::SetTarget.
	//  The target is handled with gMultiDetArray then the following commands
	//    gIndra->GetTarget()
	//    gVamos->GetTarget()
	//
	//    gIndra->GetTargetEnergyLoss(...)
	//    gVamos->GetTargetEnergyLoss(...) 
	//
	//    will return zero. 
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

	fIDTelescopes->SetOwner( kFALSE );
	SetOwnsDetectors( kFALSE );
}
//________________________________________________________________

KVINDRA_VAMOS::~KVINDRA_VAMOS()
{
   	// Destructor
}
//________________________________________________________________

void KVINDRA_VAMOS::Build(Int_t run){
	// Overrides KVMultiDetArray::Build in order to set the name of the
	// couple INDRA+VAMOS. Set up the geometry of INDRA and VAMOS,
	// associate the acquistion parameters with detectors, etc...
	
	SetName("INDRA_VAMOS");
	SetTitle("INDRA+VAMOS  experimental setup");
	Info("Build","Building INDRA+VAMOS ...");

	// Build INDRA multidetector
	cout<<endl<<setw(20)<<""<<"********************************"<<endl;
	cout      <<setw(20)<<""<<"*           INDRA              *"<<endl;
	cout      <<setw(20)<<""<<"********************************"<<endl<<endl;

	KVINDRA *indra = new KVINDRAe503;
	((KVINDRAe503 *)indra)->SetDataSet( fDataSet );
	indra->Build();
	Add( indra );

	// Build VAMOS spectrometer
	cout<<endl<<setw(20)<<""<<"********************************"<<endl;
	cout      <<setw(20)<<""<<"*           VAMOS              *"<<endl;
	cout      <<setw(20)<<""<<"********************************"<<endl<<endl;

	KVVAMOS *vamos = KVVAMOS::MakeVAMOS(fDataSet.Data());
	Add( vamos );

	// Add the lists of ACQ parameters, detectors and ID telescopes of
	// INDRA and VAMOS to INDRA_VAMOS
	
   	TIter next_i( indra->GetACQParams() );
   	TIter next_v( vamos->GetACQParams() );
   	TObject *obj;
   	 while ( (obj = next_i()) || (obj = next_v()) )
      	 AddACQParam( (KVACQParam *)obj );
   
	fDetectors.AddAll ( indra->GetDetectors() );
	fDetectors.AddAll ( vamos->GetDetectors() );

	fIDTelescopes->AddAll( indra->GetListOfIDTelescopes() );
	fIDTelescopes->AddAll( vamos->GetListOfIDTelescopes() );

	// To be sure that gMultiDetArray points on this object.
	gMultiDetArray = this;	
}
//________________________________________________________________

void KVINDRA_VAMOS::Clear(Option_t *opt ){
	//Reset all groups (lists of detected particles etc.)
    //and detectors in groups (energy losses, ACQparams etc. etc.)
    //and the target if there is one
    //for INDRA and VAMOS
	
	const_cast<KVSeqCollection*>(GetStructures())->R__FOR_EACH(KVGeoStrucElement,Clear)(opt);
}
//________________________________________________________________

void KVINDRA_VAMOS::GetDetectorEvent(KVDetectorEvent* detev, KVSeqCollection* fired_params){

	// This method is obsolete. To have access to the detector events of INDRA
	// and VAMOS prefer using:
	//   gIndra->GetDetectorEvent( detev, fired_params )
	//   gVamos->GetDetectorEvent( detev, fired_params )
	// separately.
}
//________________________________________________________________

void KVINDRA_VAMOS::SetParameters(UShort_t n){
	//Set identification and calibration parameters for run.
    //This can only be done if gDataSet has been set i.e. a dataset has been chosen
    //Otherwise this just has the effect of setting the current run number

	GetINDRA()->SetParameters( n );
	GetVAMOS()->SetCurrentRunNumber( n );
	fCurrentRun = n;
}
