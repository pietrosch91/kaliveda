//Created by KVClassFactory on Thu Sep 13 10:51:51 2012
//Author: Guilain ADEMARD

#include "KVVAMOSReconNuc.h"
#include "KVVAMOSDetector.h"
#include "KVVAMOSTransferMatrix.h"

ClassImp(KVVAMOSReconNuc)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSReconNuc</h2>
<h4>Nucleus identified by VAMOS spectrometer</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVVAMOSReconNuc::KVVAMOSReconNuc() : fCodes()
{
   // Default constructor
   init();
}

//________________________________________________________________

KVVAMOSReconNuc::KVVAMOSReconNuc (const KVVAMOSReconNuc& obj)  : KVReconstructedNucleus(), fCodes()
{
   // Copy constructor
   init();
   obj.Copy(*this);
}

KVVAMOSReconNuc::~KVVAMOSReconNuc()
{
   // Destructor
}
//________________________________________________________________

void KVVAMOSReconNuc::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVVAMOSReconNuc::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVReconstructedNucleus::Copy(obj);
   KVVAMOSReconNuc& CastedObj = (KVVAMOSReconNuc&)obj;
   CastedObj.fCodes = fCodes;
   CastedObj.fRT    = fRT;
}
//________________________________________________________________

void KVVAMOSReconNuc::init()
{
	//default initialisations
	if (gDataSet)
		SetMassFormula(UChar_t(gDataSet->GetDataSetEnv("KVVAMOSReconNuc.MassFormula",Double_t(kEALMass))));
}
//________________________________________________________________

void KVVAMOSReconNuc::Calibrate(){
 //Calculate and set the energy of a (previously identified) reconstructed particle,
    //including an estimate of the energy loss in the target.
    //
    //Starting from the detector in which the particle stopped, we add up the
    //'corrected' energy losses in all of the detectors through which it passed.
    //Whenever possible, for detectors which are not calibrated or not working,
    //we calculate the energy loss. Measured & calculated energy losses are also
    //compared for each detector, and may lead to new particles being seeded for
    //subsequent identification. This is done by KVIDTelescope::CalculateParticleEnergy().
    //
    //For particles whose energy before hitting the first detector in their path has been
    //calculated after this step we then add the calculated energy loss in the target,
    //using gMultiDetArray->GetTargetEnergyLossCorrection().
//	Info("Calibrate","IN");

	if( 1 ) CalibrateFromDetList();
	else    CalibrateFromTracking();

    if ( IsCalibrated() && GetEnergy()>0 ){
        if(GetZ()) {

 			Double_t E_tot = GetEnergy();
			Double_t E_sfoil = 0.;
        	Double_t E_targ  = 0.;

			//add correction for strip foil energy loss - moving charged particles only
//			E_sfoil = gVamos->GetStripFoilEnergyLossCorrection(this);
//        	SetStripFoilEnergyLoss( E_sfoil );
//			SetEnergy( E_tot += E_sfoil );

        	//add correction for target energy loss - moving charged particles only
        	E_targ = gVamos->GetTargetEnergyLossCorrection(this);
        	SetTargetEnergyLoss( E_targ );
			SetEnergy( E_tot += E_targ );
        }

        // set angles of momentum from trajectory reconstruction
        SetTheta( GetThetaL() );
        SetPhi  ( GetPhiL() - 90 );
    }

//	Info("Calibrate","OUT: E= %f, theta= %f, phi= %f",GetEnergy(), GetTheta(), GetPhi());
//cout<<endl;
}
//________________________________________________________________
		
void KVVAMOSReconNuc::CalibrateFromDetList(){
   	// The energy of each particle is calculated as follows:
   	//
   	//      E = dE_1 + dE_2 + ... + dE_N
   	//
   	// dE_1, dE_2, ... = energy losses measured in each detector through which
   	//                          the particle has passed (or stopped, in the case of dE_N).
   	//                         These energy losses are corrected for (Z,A)-dependent effects
   	//                          such as pulse-heigth defect in silicon detectors, losses in
   	//                          windows of gas detectors, etc.
   	//
   	// If none of the detectors of the detector list is calibrated, the particle's energy cannot be calculated &
   	// the code will be kECode0.
	// Whenever possible, the energy loss for fired detectors which are uncalibrated
   	// or not functioning is calculated.
   	// If the detectors used to measured energy are not calibrated or not fired, or multihit, the
   	// energy code will be kECode2.
   	// Otherwise, the code will be kECode1.
   	// The flag returned by IsCalibrated will be true is the energy code is different from kECode0.

   	//status code

	SetECode( kECode0 );

   	//uncharged particles
   	if(GetZ()==0) return;

 	SetECode( kECode1 );
	SetIsCalibrated();

	Double_t Etot = 0;
	KVVAMOSDetector *stopdet = (KVVAMOSDetector *)GetStoppingDetector();
	KVVAMOSDetector *det     = NULL;
	TIter next( GetDetectorList() );
	while( (det = (KVVAMOSDetector *)next()) ){

		// transmission=kFALSE if particle stop in det
		Bool_t transmission = ( det != stopdet );
		Double_t Edet = (det->IsECalibrated() ? det->GetEnergy() : -1);

		// The stopping detector has to be calibrated, hit by only one
		// particle and its energy has to be positive
		if( !transmission && ((det->GetNHits() != 1) || (Edet <= 0)) ){
 			SetECode( kECode0 );
			SetIsUncalibrated();
//			Info("CalibrateFromDetList","Stopping detector not calibrated");
			return;
		}

		// Detector is calibrated and hit by only one particle
		if( Edet > 0 &&  det->GetNHits() == 1  ){
			det->SetEResAfterDetector( Etot );
			Edet  = det->GetCorrectedEnergy( this, -1, transmission );
			Etot += Edet;
//			Info("CalibrateFromDetList","Corrected DeltaE= %f in %s", Edet, det->GetName());
			continue;
		}

		// Detector is uncalibrated/unfired/multihit. Calculate energy loss.
        // calculate energy of particle before detector from energy after 
        // detector

		Edet = det->GetDeltaEFromERes( GetZ(), GetA(), Etot);
        if( Edet< 0.0 ) Edet = 0.0;

		if( det->GetNHits() > 1 ){
            if(!( det->Fired() && det->IsECalibrated()) )
                det->SetEnergyLoss(Edet + det->GetEnergy());// sum up calculated energy losses in uncalibrated detector
//			Info("CalibrateFromDetList","MultiHit in %s", det->GetName());
        }
        else if( !det->Fired() || !det->IsECalibrated() )
            det->SetEnergyLoss( Edet );

        det->SetEResAfterDetector( Etot );
        Edet  = det->GetCorrectedEnergy( this, Edet, transmission);
        Etot += Edet;
		if( det->IsUsedToMeasure("E") ) SetECode( kECode2 );
//		Info("CalibrateFromDetList","Calculated DeltaE= %f in %s", Edet, det->GetName());
	}
	SetEnergy( Etot );
}
//________________________________________________________________
		
void KVVAMOSReconNuc::CalibrateFromTracking(){
	Warning("CalibrateFromTracking","TO BE IMPLEMENTED");
}
//________________________________________________________________

void KVVAMOSReconNuc::Clear(Option_t * t){
	//Reset nucleus' properties
	
	KVReconstructedNucleus::Clear(t);
   	init();
   	fCodes.Clear();
	fRT.Reset();
}
//________________________________________________________________

void KVVAMOSReconNuc::Identify()
{
   	// VAMOS-specific Z identification.
   	// Here we attribute the general identification codes depending on the
   	// result of KVReconstructedNucleus::Identify and the subcodes from the different
   	// identification algorithms:
   	// If the particle's mass A was NOT measured, we make sure that it is calculated
   	// from the measured Z using the mass formula defined by default
   	//
   	//IDENTIFIED NUCLEI
   	//Identified nuclei with ID code = 2 with subcodes 4 & 5
   	//(masse hors limite superieure/inferieure) are relabelled
   	//with kIDCode10 (identification entre les lignes CsI)
   	//
   	//UNIDENTIFIED NUCLEI
   	//Unidentified nuclei receive the general ID code for non-identified particles (kIDCode14)

   	KVReconstructedNucleus::Identify();

   	KVIdentificationResult partID;
   	Bool_t ok = kFALSE;

   	// for all nuclei we take the first identification which gives IDOK==kTRUE
   	Int_t id_no = 1;
   	KVIdentificationResult *pid = GetIdentificationResult(id_no);
   	while( pid ){
	   	if( pid->IDattempted && pid->IDOK ){
		   	ok = kTRUE;
		   	partID = *pid;
		   	break;
	   	}
	   	++id_no;
	   	pid = GetIdentificationResult(id_no);
   	}

   	if(ok){
       	SetIsIdentified();
       	KVIDTelescope* idt = (KVIDTelescope*)GetIDTelescopes()->FindObjectByType( partID.GetType() );
        if( !idt ){
        	Warning("Identify", "cannot find ID telescope with type %s", partID.GetType());
        	GetIDTelescopes()->ls();
        	partID.Print();
        }
        SetIdentifyingTelescope(  idt );
        SetIdentification( &partID );
   	}
	else{
      	/******* UNIDENTIFIED PARTICLES *******/

      	/*** general ID code for non-identified particles ***/
      	SetIDCode( kIDCode14 );
   	}
}
//________________________________________________________________

void KVVAMOSReconNuc::MakeDetectorList()
{
    // Protected method, called when required to fill fDetList with pointers to
    // the detectors whose names are stored in fDetNames.
    // If gVamos=0x0, fDetList list will be empty.

	fDetList.Clear();
    if ( gVamos ){
    	fDetNames.Begin("/");
    	while ( !fDetNames.End() ) {
    	    KVDetector* det = gVamos->GetDetector( fDetNames.Next(kTRUE) );
    	    if ( det ) fDetList.Add(det);
    	} 
    }
}
//________________________________________________________________

void KVVAMOSReconNuc::ReconstructTrajectory(){
	//Reconsturction of the trajectory at the focal plane and then at
	//the target point.

	ReconstructFPtraj();
	ReconstructLabTraj();
}
//________________________________________________________________

void KVVAMOSReconNuc::ReconstructFPtraj(){




	UChar_t res = 0;
	Double_t xyzf[3];          // [3] => [X, Y, Z]
	Double_t XYZf[4][3];       // [4] => [Complete Det1, Complete Det2, Incomplete Det1, Incomplete Det2]
	// [3] => [X, Y, Z]
	Short_t Idx[4] = {-1, -1, -1, -1};// [4] => [Complete Det1, Complete Det2, Incomplete Det1, Incomplete Det2]
	UChar_t IncDetBitmask = 3;        // bits 11 => 3, first bit means missing X, second bit means missing Y
	Bool_t inc1IsX = kTRUE;           // true if the coordinate given by the first incomplete position is X ( false if Y)
	UShort_t Ncomp = 0, Ninc = 0;     // Number of Complete and Incomplete
	// position measurments 

	fCodes.SetFPCode( kFPCode0 ); // Initialize FP codes to code 0 "no FP position recon."
	fRT.dirFP.SetXYZ(0, 0, 1);

	const Char_t *FPdetName = NULL;
	KVVAMOSDetector *d = NULL;
	// Loop over detector name to be used for the Focal plane Position
	// reconstruction
	for( Short_t i=0; (FPdetName = fCodes.GetFPdetName(i)); i++ ){

		// Look at only the detectors in 'fDetList' with the name or
		// type 'FPdetName' measuring a position.
		if(
				((d = (KVVAMOSDetector *)GetDetectorList()->FindObject( FPdetName )) ||
 		 		 (d = (KVVAMOSDetector *)GetDetectorList()->FindObjectByType( FPdetName )))
				&& (res = d->GetPosition(xyzf)) > 4 
		  ){


			if( res == 7 ){ 
				// X, Y and Z measured bits 111 => 7
				// Complete position (XYZ)

				Idx[ Ncomp ] = i;
				for(Int_t j=0; j<3; j++ ) XYZf[ Ncomp ][ j ] = xyzf[ j ];
				Ncomp++;
			}
			else if ( IncDetBitmask&res ){
				// look at only incomplete position measurment (XZ or YZ)
				// not found yet

				Idx[ 2+Ninc ] = i;
				for(Int_t j=0; j<3; j++ ) XYZf[ 2+Ninc ][ j ] = xyzf[ j ];
				if( Ninc == 0 ) inc1IsX = res&1;
				Ninc++;
				IncDetBitmask = (IncDetBitmask | 4)^res;
			}


			if( Ncomp == 2 ){
				//Case where the Focal plane Position is reconstructed from 2 complete position measurment.

				fRT.dirFP.SetX( (XYZf[1][0] - XYZf[0][0])/( XYZf[1][2] - XYZf[0][2]) );
				fRT.dirFP.SetY( (XYZf[1][1] - XYZf[0][1])/( XYZf[1][2] - XYZf[0][2]) );
				fCodes.SetFPCode( Idx[0], Idx[1], Idx[2], Idx[3], inc1IsX );
				break;
			}
			else if( (Ncomp == 1) && !IncDetBitmask ){
				//Case where the Focal plane Position is reconstructed from 1 complete position measurment and 2 incomplete position measurment.

				fRT.dirFP.SetX( (XYZf[3-inc1IsX][0] - XYZf[0][0])/( XYZf[3-inc1IsX][2] - XYZf[0][2]) );
				fRT.dirFP.SetY( (XYZf[2+inc1IsX][1] - XYZf[0][1])/( XYZf[2+inc1IsX][2] - XYZf[0][2]) );
				fCodes.SetFPCode( Idx[0], Idx[1], Idx[2], Idx[3], inc1IsX );
				break;
			}

		}
	}

	if( fCodes.TestFPCode( kFPCode0 ) ) return;

	// normalize the direction vector dirFP
	fRT.dirFP *= 1./fRT.dirFP.Mag();
	
	// Xf = Xc1      - Zc1*tan( Thetaf )
	fRT.pointFP[0] = XYZf[0][0] - XYZf[0][2]*fRT.dirFP.X()/fRT.dirFP.Z();
	
	// Yf = Yc1      - Zc1*tan( Phif   )
	fRT.pointFP[1] = XYZf[0][1] - XYZf[0][2]*fRT.dirFP.Y()/fRT.dirFP.Z();
	
	RunTrackingAtFocalPlane();
//	if( CheckTrackingCoherence() ) fRT.SetFPparamsReady();
// 	else ReconstructFPtrajByFitting( GetDetectorList() );
	fRT.SetFPparamsReady();
}
//________________________________________________________________

//void KVVAMOSReconNuc::ReconstructFPtrajByFitting(){
//
//	Info("ReconstructFPtrajByFitting","IN");
//	UChar_t res = 0;
//	Double_t xyzf [3];          // [3] => [X, Y, Z]
//	Double_t dxyzf[3];          // [3] => [X, Y, Z]
//	fCodes.SetFPCode( kFPCode0 ); // Initialize FP codes to code 0 "no FP position recon."
//
//	static TGraphErrors graphX(4); // 4 is the max number of FPdetectors
//	static TGraphErrors graphY(4); // 4 is the max number of FPdetectors
//	Int_t NptX=0, NptY=0;         // Number of Points in X-Z and Y-Z plans
//	const Char_t *FPdetName = NULL;
//	KVVAMOSDetector *d = NULL;
//	// Loop over detector name to be used for the Focal plane Position
//	// reconstruction
//	for( Short_t i=0; (FPdetName = fCodes.GetFPdetName(i)); i++ ){
//
//		// Look at only the detectors in 'fDetList' with the name or
//		// type 'FPdetName' measuring a position.
//		if(
//				((d = (KVVAMOSDetector *)GetDetectorList()->FindObject( FPdetName )) ||
// 		 		 (d = (KVVAMOSDetector *)GetDetectorList()->FindObjectByType( FPdetName )))
//				&& (res = d->GetPosition(xyzf)) > 4 
//		  ){
//
//			d->GetDeltaXYZf( dxyzf );
//
//			if( res & 1 ){ // X position is OK
//
//				graphX.SetPoint     ( NptX, xyzf[2], xyzf [0]);
//				graphX.SetPointError( NptX,      0 , dxyzf[0]>0.01 ? dxyzf[0] : 0.01);
//				NptX++;
//			}
//			if( res & 2 ){ // Y position is OK
//				graphY.SetPoint     ( NptY, xyzf[2], xyzf [1]);
//				graphY.SetPointError( NptY,      0 , dxyzf[1]>0.01 ? dxyzf[1] : 0.01 );
//				NptY++;
//			}
//
//		}
//	}
//
//	if( NptX>1 && NptY>1  ){
//
//		graphX.Set( NptX );
//		graphY.Set( NptY );
//
//
//		fRT.dirFP.SetXYZ(0, 0, 1);
//		TF1 *ff = (TF1 *)gROOT->GetFunction("pol1");
//
//		//	graphX.Fit(ff,"QNC");
//		graphX.Fit(ff,"QC");
//		fRT.dirFP.SetX( ff->GetParameter(1) );
//		fRT.pointFP[0] = ff->GetParameter(0);
//
//		//	graphY.Fit(ff,"QNC");
//		graphY.Fit(ff,"QC");
//		fRT.dirFP.SetY( ff->GetParameter(1) );
//		fRT.pointFP[1] = ff->GetParameter(0);
//
//		// normalize the direction vector dirFP
//		fRT.dirFP *= 1./fRT.dirFP.Mag();
//
//		// Set FPcode 31 for the reconstruction by fitting
//		fCodes.SetFPCode( kFPCode31 );
//      RunTrackingAtFocalPlane();
//		if( CheckTrackingCoherence() ) {
//			fRT.SetFPparamsReady();
//			return;
//		}
//	}
//
// 	fTrackRes.Clear();
//}
//________________________________________________________________

void KVVAMOSReconNuc::ReconstructLabTraj(){
	// Reconstruction of the trajectory at the target point, in the reference
	// frame of the laboratory, from the trajectory at the focal plane.
	// The method ReconstructFPtraj() has to be call first.

	// No trajectory reconstruction in the laboratory if the reconstruction
	// in the focal plane is not OK.
	if( fCodes.TestFPCode( kFPCode0 ) ) return;
	KVVAMOSTransferMatrix *tm = gVamos->GetTransferMatrix();
	tm->ReconstructFPtoLab( &fRT );
	RunTrackingAtTargetPoint();
}
//________________________________________________________________

void KVVAMOSReconNuc::RunTrackingAtFocalPlane(){
	// Run the tracking of this reconstructed trajectory in each volume (detectors)  punched through at the focal plane.

	// Tracking is impossible if the trajectory reconstruction
	// in the focal plane is not OK.
	fTrackRes.Clear();
	if( fCodes.TestFPCode( kFPCode0 ) ) return;


	KVVAMOSDetector *stopdet = (KVVAMOSDetector *)GetStoppingDetector();
	TGeoVolume *stopVol = (TGeoVolume *)stopdet->GetActiveVolumes()->Last();

	// For gGeoManager the origin is the target point.
	// Starting point has to be set from this origin.

	// intersection point between reconstructed trajectory and the Focal
	// Plane in the focal plane frame of reference
	Double_t XYZ_FP[3] = { fRT.pointFP[0], fRT.pointFP[1], 0 };
	// same intersection point in the frame of reference
	// centered at the target point
	gVamos->FocalToTarget( XYZ_FP, XYZ_FP );

	//tracking direction
   	Double_t dir[3];

	TGeoVolume *FPvol    = gVamos->GetFocalPlaneVolume();
//	//--------------------------------------------------
//	Info("RunTracking","Runnig the Focal Plane backward Traking");
//	//--------------------------------------------------
   	//  direction of the tracking = direction of the trajectory at the focal plane
   	fRT.dirFP.GetXYZ( dir );
	gVamos->FocalToTargetVect( dir, dir );

   	// Initializing tracking (i.e. setting both initial point and direction
   	// and finding the state). Start from the FP intersection point
   	gGeoManager->InitTrack( XYZ_FP, dir );

	TGeoVolume *topVol   = gGeoManager->GetTopVolume();
//   	TGeoVolume* VAMOSvol = gVamos->GetGeoVolume();
   	TGeoVolume* curVol   = gGeoManager->GetCurrentVolume();
   	TGeoVolume* prevVol  = NULL;


   	// move along trajectory until a new volume is hit
   	// Stop when the point is outside the Focal Plane volume or when it is
   	// inside the stopping detector
   	do{

   	   	gGeoManager->FindNextBoundaryAndStep();

	   	if( curVol != FPvol ){
   	   	   	Double_t step = gGeoManager->GetStep();
		   	fTrackRes.SetValue( curVol->GetName(), step );
//	   	   	cout<<"Step = "<<setw(15)<< step <<" cm in "<<curVol->GetName()<<"( "<<curVol->GetTitle()<<" )"<<endl;
	   	}

 		if(curVol == stopVol) break;

	   	prevVol = curVol;
   	   	curVol  = gGeoManager->GetCurrentVolume();

   	}
   	while( (curVol != topVol) && !gGeoManager->IsOutside() );
//   	while( !gGeoManager->IsOutside() );

//	//--------------------------------------------------
//	Info("RunTracking","Runnig the Focal Plane forward Traking");
//	//--------------------------------------------------
   	//  direction of the tracking = inverse direction of the trajectory at the focal plane
   	TVector3( -fRT.dirFP ).GetXYZ( dir );
	gVamos->FocalToTargetVect( dir, dir );

   	// Initializing tracking (i.e. setting both initial point and direction
   	// and finding the state). Start from the FP intersection point
   	gGeoManager->InitTrack( XYZ_FP, dir );

   	curVol   = gGeoManager->GetCurrentVolume();
   	prevVol  = NULL;

   	// move along trajectory until we hit a new volume
   	// Stop when the point is outside the Focal Plane volume
   	do{

   	   	gGeoManager->FindNextBoundaryAndStep();

	   	if( curVol != FPvol ){
   	   	   	Double_t step = gGeoManager->GetStep();
		   	fTrackRes.SetFirstValue( curVol->GetName(), step );
//	   	   	cout<<"Step = "<<setw(15)<< step <<" cm in "<<curVol->GetName()<<"( "<<curVol->GetTitle()<<" )"<<endl;
	   	}

	   	prevVol = curVol;
   	   	curVol  = gGeoManager->GetCurrentVolume();

   	}
   	while( (curVol != topVol) && !gGeoManager->IsOutside() );
}
//________________________________________________________________

void KVVAMOSReconNuc::RunTrackingAtTargetPoint(){

	if( !fRT.FPtoLabWasAttempted() ) return;
	
	Double_t XYZ_target[3] = { 0., 0., 0. };

	//tracking direction
   	Double_t dir[3];

	TGeoVolume *FPvol = gVamos->GetFocalPlaneVolume();

   	//  direction of the tracking = direction of the trajectory at the target point (lab) 
   	fRT.dirLab.GetXYZ( dir );

   	// Initializing tracking (i.e. setting both initial point and direction
   	// and finding the state). Start from the FP intersection point
   	gGeoManager->InitTrack( XYZ_target, dir );

	TGeoVolume *topVol   = gGeoManager->GetTopVolume();
   	TGeoVolume* VAMOSvol = gVamos->GetGeoVolume();
   	TGeoVolume *curVol   = gGeoManager->GetCurrentVolume();
   	TGeoVolume *prevVol  = NULL;

   	// move along trajectory until we hit a new volume
   	// Stop when the point is outside the top volume or
   	// inside the Focal Plane volume
   	Int_t idx = 0;
   	do{

   	   	gGeoManager->FindNextBoundaryAndStep();

	   	if( (curVol != topVol) && (curVol != VAMOSvol) ){
   	   	   	Double_t step = gGeoManager->GetStep();
		   	fTrackRes.SetValueAt( curVol->GetName(), step, idx++ );
//	   	   	cout<<"Step = "<<setw(15)<< step <<" cm in "<<curVol->GetName()<<"( "<<curVol->GetTitle()<<" )"<<endl;
	   	}

	   	prevVol = curVol;
   	   	curVol  = gGeoManager->GetCurrentVolume();

   	}
   	while( !gGeoManager->IsOutside() && (curVol != FPvol) );

}
//________________________________________________________________

Bool_t KVVAMOSReconNuc::CheckTrackingCoherence(){
	// Verifies the coherence between the tracking result and the list of
	// fired detectors.
	// If at least one active volum of each fired detector is
	// inside the tracking result fTrackRes. Return kTRUE if this
	// is OK.
	
	TIter nextdet( GetDetectorList() );
	KVVAMOSDetector *det = NULL;
	while( (det = (KVVAMOSDetector *)nextdet()) ){

		Bool_t ok = kFALSE;

		TIter nextvol( det->GetActiveVolumes() );
		TGeoVolume *vol = NULL;
		while( (vol = (TGeoVolume *)nextvol()) && !ok ){

			TIter next_tr( fTrackRes.GetList());
			TObject *tr = NULL;
			while( (tr = next_tr()) && !ok ){
				if( !strcmp( tr->GetName(), vol->GetName() ) ) ok = kTRUE; 
			}
		}

		if( !ok ) return kFALSE;
	}
	return kTRUE;
}
