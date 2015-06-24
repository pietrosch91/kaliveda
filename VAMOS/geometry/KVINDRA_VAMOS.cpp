//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#include "KVINDRA_VAMOS.h"
#include "KVINDRAe503.h"
#include "KVVAMOS.h"
#include "TPluginManager.h"
#include "KVSpectroDetector.h"
#include "KVIVReconEvent.h"
#include "KVVAMOSReconNuc.h"

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

KVNameValueList* KVINDRA_VAMOS::DetectParticle(KVNucleus * part){
    // Simulate detection of a charged particle by the array (INDRA+VAMOS).
    // The actual method called depends on the value of fROOTGeometry:
    //   fROOTGeometry=kTRUE:  calls DetectParticle_TGEO, particle propagation performed using
    //                         TGeo description of array and algorithms from ROOT TGeo package.
    //                         For the moment, this case is not implemented (TO BE DONE)
    //   fROOTGeometry=kFALSE:  only calls DetectParticle_KV of gIndra (INDRA), uses simple KaliVeda geometry
    //                          to simulate propagation of particle.
    //                          No propagation of the particle into VAMOS is simulated.
    
    if( IsROOTGeometry() ){
	   	return DetectParticle_TGEO(part);
	}
	else{
		gIndra->SetROOTGeometry( kFALSE );
		return gIndra->DetectParticle(part);
	}
};
//________________________________________________________________

void KVINDRA_VAMOS::DetectEvent(KVEvent * event,KVReconstructedEvent* rec_event,const Char_t* detection_frame){

	// the ROOT geometry can not be use for filtering
	SetROOTGeometry( kFALSE );

	// first detect event in INDRA
	GetINDRA()->DetectEvent( event, rec_event, detection_frame );

   	// iterate through list of particles
  	// and detect in VAMOS only particles UNDETECTED in INDRA
    KVNucleus *part,*_part;
    KVNameValueList* nvl = NULL;


    while ((part = event->GetNextParticle("DEAD ZONE"))) {  // loop over particles


#ifdef KV_DEBUG
        cout << "DetectEvent(): looking at particle in VAMOS---->" << endl;
        part->Print();
#endif
        _part=(KVNucleus*)part->GetFrame(detection_frame);


		if( _part->GetTheta() > 14 ) continue;

        _part->SetE0();

        if (fTarget && (fFilterType != kFilterType_Geo)){
            fTarget->SetOutgoing(kTRUE);
            //simulate passage through target material
            Double_t ebef = _part->GetKE();
            fTarget->DetectParticle(_part);
            Double_t eLostInTarget = ebef-_part->GetKE();
            if (_part->GetKE()<1.e-3) {
                part->GetParameters()->SetValue("UNDETECTED","STOPPED IN TARGET");

                part->AddGroup("UNDETECTED");
                part->AddGroup("STOPPED IN TARGET");
            }
            fTarget->SetOutgoing(kFALSE);

            part->GetParameters()->SetValue("TARGET Out",eLostInTarget);
        }


        if ( (_part->GetKE()>1.e-3) && (nvl = GetVAMOS()->DetectParticle(_part)) &&  (nvl->GetNpar()>0) ) {

			part->RemoveGroup("UNDETECTED");
            part->RemoveGroup("DEAD ZONE");
            part->GetParameters()->RemoveParameter("UNDETECTED");


            part->AddGroup("DETECTED");
            part->AddGroup("VAMOS");
            part->GetParameters()->SetValue("DETECTED","VAMOS");
		}


        SafeDelete( nvl );
        _part->SetMomentum(*_part->GetPInitial());
	}  	//fin de loop over particles


 	// EVENT RECONSTRUCTION FOR SIMPLE GEOMETRIC FILTER IN VAMOS
	//  We keep all particles belonging to groups DETECTED and VAMOS

	KVVAMOSReconEvent *rec_vamos_event = ((KVIVReconEvent *)rec_event)->GetVAMOSEvent();
 	while ((part = event->GetNextParticle())) {
        if(	part->BelongsToGroup("DETECTED") &&
        		part->BelongsToGroup("VAMOS") ){

        	_part=(KVNucleus*)part->GetFrame(detection_frame);


            KVVAMOSReconNuc *recon_nuc = (KVVAMOSReconNuc *)rec_vamos_event->AddParticle();

            // copy parameter list
            part->GetParameters()->Copy( *(recon_nuc->GetParameters()) );


            recon_nuc->SetZandA(part->GetZ(),part->GetA());

            recon_nuc->SetMomentum(_part->GetMomentum());
			recon_nuc->SetQ( _part->GetParameters()->GetIntValue("Q") );
			recon_nuc->SetBrho(_part->GetParameters()->GetDoubleValue("Brho") );
			recon_nuc->GetParameters()->SetValue("Delta", _part->GetParameters()->GetDoubleValue("Delta") );
			recon_nuc->SetThetaVandPhiV(_part->GetParameters()->GetDoubleValue("ThetaV"),
					_part->GetParameters()->GetDoubleValue("PhiV") );

			
  			recon_nuc->SetIsIdentified();
            recon_nuc->SetIsCalibrated();
            recon_nuc->SetIsOK();
            recon_nuc->SetZMeasured();
            recon_nuc->SetQMeasured();
            recon_nuc->SetAMeasured();
            recon_nuc->SetIsQandAidentified();

//			Info("DetectParticle","Detection  in VAMOS with Brho0= %.3f theta= %.1f deg: Z= %d, A= %d, Q= %d, E= %0.1f, Brho= %.3f, delta= %.4f, ThetaV= %0.1f deg, PhiV= %.1f deg, Theta= %0.1f deg, Phi= %.1f deg", gVamos->GetBrhoRef(), gVamos->GetAngle(),recon_nuc->GetZ(),recon_nuc->GetA(),recon_nuc->GetQ(),recon_nuc->GetEnergy(), recon_nuc->GetBrho(), recon_nuc->GetParameters()->GetDoubleValue("Delta"), recon_nuc->GetThetaV(), recon_nuc->GetPhiV(), recon_nuc->GetTheta(), recon_nuc->GetPhi());
		}
 	}
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

void KVINDRA_VAMOS::SetFilterType(Int_t t){
	// Set Filter type to both INDRA and VAMOS
	KVMultiDetArray::SetFilterType( t );
	GetINDRA()->SetFilterType( t );
	GetVAMOS()->SetFilterType( t );
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
//________________________________________________________________


void KVINDRA_VAMOS::SetROOTGeometry(Bool_t on)
{
    // Call with on=kTRUE if array uses ROOT geometry for tracking
    // Call SetGeometry(TGeoManager*) first with a valid geometry.

	if( on ) Error("SetROOTGeometry","Impossible for the moment to use ROOT geometry for INDRA-VAMOS");

	KVMultiDetArray::SetROOTGeometry( kFALSE );
	GetVAMOS()->SetROOTGeometry( kFALSE );
	GetINDRA()->SetROOTGeometry( kFALSE );
}


