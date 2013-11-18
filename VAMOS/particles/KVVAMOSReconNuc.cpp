//Created by KVClassFactory on Thu Sep 13 10:51:51 2012
//Author: Guilain ADEMARD

#include "KVVAMOSReconNuc.h"
#include "KVVAMOSDetector.h"
#include "KVVAMOSTransferMatrix.h"
#include "KVVAMOSReconGeoNavigator.h"
#include "KVNamedParameter.h"
#include "KVTarget.h"

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

void KVVAMOSReconNuc::Streamer(TBuffer &R__b)
{
   	// Stream an object of class KVVAMOSReconNuc. The array (fDetE) of the contributions
   	// of each detectors to the nucleus' energy is written/read if the nucleus
   	// is calibrated.

   	if (R__b.IsReading()) {
      	R__b.ReadClassBuffer(KVVAMOSReconNuc::Class(),this);
	  	if( IsCalibrated() ){
		  	Int_t N = GetDetectorList()->GetEntries();
		  	fDetE = new Float_t[ N ];
		  	R__b.ReadFastArray( fDetE, N);
		}
	}
    else {
      	R__b.WriteClassBuffer(KVVAMOSReconNuc::Class(),this);
	  	if( IsCalibrated() ) R__b.WriteFastArray( fDetE, GetDetectorList()->GetEntries() );
   	}
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

	fStripFoilEloss = 0;
	fToF = fFlightDist = 0;
	fDetE = NULL;
}
//________________________________________________________________

void KVVAMOSReconNuc::Calibrate(){
 	//Calculate and set the energy of a (previously identified) reconstructed nucleus,
    //including an estimate of the energy loss in the stripping foil and in the target.
    //
    //Starting from the detector in which the nucleus stopped, we add up the
    //'corrected' energy losses in all of the detectors through which it passed.
    //Whenever possible, for detectors which are not calibrated or not working,
    //we calculate the energy loss. 
    //
    //For nuclei whose energy before hitting the first detector in their path has been
    //calculated after this step we then add the calculated energy loss in stripping foil and in the target,
    //using:
    //   gVamos->GetStripFoilEnergyLossCorrection();
    //   gMultiDetArray->GetTargetEnergyLossCorrection().


	//	Info("Calibrate","IN");

	if( 1 ) CalibrateFromDetList();
	else    CalibrateFromTracking();

    if ( IsCalibrated() && GetEnergy()>0 ){

 	   	// set angles of momentum from trajectory reconstruction

    	if( fRT.FPtoLabWasAttempted() ){
        	SetTheta( GetThetaL() );
        	SetPhi  ( GetPhiL() - 90 );
		}

        if(GetZ()) {

 			Double_t E_tot   = GetEnergy();
			Double_t E_sfoil = 0.;
        	Double_t E_targ  = 0.;

			//add correction for stripping foil energy loss - moving charged particles only
			E_sfoil = gVamos->GetStripFoilEnergyLossCorrection(this);
        	SetStripFoilEnergyLoss( E_sfoil );
			SetEnergy( E_tot += E_sfoil );

        	//add correction for target energy loss - moving charged particles only
        	gMultiDetArray->GetTarget()->SetIncoming(kFALSE);
          	gMultiDetArray->GetTarget()->SetOutgoing(kTRUE);
        	E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this);
        	SetTargetEnergyLoss( E_targ );
			SetEnergy( E_tot += E_targ );
        }
    }

//	Info("Calibrate","OUT: E= %f, theta= %f, phi= %f, momentum= %f",GetEnergy(), GetTheta(), GetPhi(), GetMomentum().Mag());
//    cout<<endl;
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
	Bool_t stopdetOK = kTRUE;
	KVVAMOSDetector *stopdet = (KVVAMOSDetector *)GetStoppingDetector();
	KVVAMOSDetector *det     = NULL;
	TIter next( GetDetectorList() );
	if( !fDetE ) fDetE = new Float_t[ GetDetectorList()->GetEntries() ];
	Int_t idx = -1;
	while( (det = (KVVAMOSDetector *)next()) ){
		fDetE[++idx] = 0.;
		if( !stopdetOK ) continue;
		// transmission=kFALSE if particle stop in det
		Bool_t transmission = ( det != stopdet );
		Double_t Edet = (det->IsECalibrated() ? det->GetEnergy() : -1);

		// The stopping detector has to be calibrated, hit by only one
		// particle and its energy has to be positive
		if( !transmission && ((det->GetNHits() != 1) || (Edet <= 0)) ){
 			SetECode( kECode0 );
			SetIsUncalibrated();
			stopdetOK = kFALSE;
//			Info("CalibrateFromDetList","Stopping detector not calibrated");
			return;
		}

		// Detector is calibrated and hit by only one particle
		if( Edet > 0 &&  det->GetNHits() == 1  ){
			det->SetEResAfterDetector( Etot );

			// if Edet is greater than the maximal possible theoretical value
			// this may be because Z or A are underestimated and one of both
			// has to be increased (see KVDetector::GetCorrectedEnergy)
			if( Edet > det->GetMaxDeltaE( GetZ(), GetA() ) ){
				Int_t z0 = GetZ();
				Int_t a0 = GetA();
				Int_t z=z0, a=a0;
				Bool_t isOK = kFALSE;

				Warning("CalibrateFromDetList","MeasuredDE>MaxDE in %s, Z= %d, A=%d, MeasuredDE= %f MeV, MaxDE %f MeV",det->GetName(), z0, a0, Edet,det->GetMaxDeltaE( z0, a0 ));

				while( !isOK && (z-GetZ()<=10) ){
					//increase Z
					++z;
					//modify A only if it is not measured
					if( !IsAMeasured() ) a = GetAFromZ( z, (Char_t)GetMassFormula() );
					isOK  = ( Edet <= det->GetMaxDeltaE( z, a ) );
					Info("CalibrateFromDetList","changing Z= %d, A=%d, MeasuredDE= %f MeV, MaxDE %f MeV", z, a, Edet,det->GetMaxDeltaE( z, a ));
				}
				Info("CalibrateFromDetList","MeasuredDE<MaxDE in %s by changing Z: %d->%d, A: %d->%d \n",det->GetName(), z0 ,z, a0 ,a);
				SetZandA( z, a );
				Edet  = det->GetCorrectedEnergy( this, -1, transmission );
				// we back to initial values of Z and A
				SetZandA( z0, a0 );
			}
			else Edet  = det->GetCorrectedEnergy( this, -1, transmission );
			Etot += Edet;
			fDetE[idx] = Edet;
//			Info("CalibrateFromDetList","Corrected DeltaE= %f in %s, idx= %d", Edet, det->GetName(), idx);
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
		fDetE[idx] = Edet;
		if( det->IsUsedToMeasure("E") ) SetECode( kECode2 );
//		Info("CalibrateFromDetList","Calculated DeltaE= %f in %s, idx= %d", Edet, det->GetName(), idx);
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

	if( fDetE ) delete[] fDetE;

   	init();
   	fCodes.Clear();
	fRT.Reset();
}
//________________________________________________________________
 		
void KVVAMOSReconNuc::GetAnglesFromStoppingDetector(Option_t *){
	// Overwrites the same method of the mother class. This method is obsolete
	// for a nucleus reconstructed in VAMOS because angles are set by the 
	// reconstruction.
	// Overwriting is important because this method is called in the Streamer
	// method of KVReconstructedEvent to calculate again the angles.
	return;
}
//________________________________________________________________

Double_t KVVAMOSReconNuc::GetRealA() const{
	// Returns the real value of the mass number calculated for the 
	// measured energy and the measured time of flight.
	// Begin_Latex 
	// A = #frac{E}{(#gamma-1)u}
	// End_Latex
	// where 
	//   E     : corrected kinetic energy in (MeV)
	//   u     : atomic mass unit in MeV/c^2
	//   gamma : Lorentz factor calculated from the velocity
	//           deduced from the time of flight measurment
	//
	//This method overrides the same method in the mother class.

	Double_t gamma = GetGammaFromToF();
	return gamma==1 ? 0 : GetEnergyBeforeVAMOS()/((gamma-1)*u());

}
//________________________________________________________________

Double_t KVVAMOSReconNuc::GetRealAoverQ() const{
	// returns the ratio between the mass number A and the charge state Q
	// calculated from the measurment of the Time of Flight of the nucleus.
	// The returned value is real. Returns zero if the time of flight is
	// not correct.
	// Begin_Latex 
	// #frac{A}{Q} = #frac{10 C}{u} #frac{B_{#rho}}{ #gamma #beta}
	// End_Latex
	// where
	//   u             : atomic mass unit in MeV/c^2
	//   C             : speed of light in vacuum in cm/ns 
	//   beta and gamma: relativistic quantities calculated from the velocity
	//                   deduced from the time of flight measurment
	
	Double_t tmp = ( GetBetaFromToF()*GetGammaFromToF() );
	if( tmp == 0 ) return 0;

	return GetBrho()*C()*10./( u()*tmp );
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
	// Reconstruct the Focal-Plane trajectory and set the appropriate 
	// FPCode. If the reconstruction is well carried out (i.e. FPCode>kFPCode0)
	// then the tracking along this trajectory is runned.

	UChar_t res = 0;
	Double_t xyzf[3];          // [3] => [X, Y, Z]
	Double_t XYZf[4][3];       // [4] => [Complete Det1, Complete Det2, Incomplete Det1, Incomplete Det2]
	// [3] => [X, Y, Z]
	Short_t Idx[4] = {-1, -1, -1, -1};// [4] => [Complete Det1, Complete Det2, Incomplete Det1, Incomplete Det2]
	UChar_t IncDetBitmask = 3;        // bits 11 => 3, first bit means missing X, second bit means missing Y
	Bool_t inc1IsX = kTRUE;           // true if the coordinate given by the first incomplete position is X ( false if Y)
	UShort_t Ncomp = 0, Ninc = 0;     // Number of Complete and Incomplete
	// position measurments 

	SetFPCode( kFPCode0 ); // Initialize FP codes to code 0 "no FP position recon."
	fRT.dirFP.SetXYZ(0, 0, 1);

	const Char_t *FPdetName = NULL;
	KVVAMOSDetector *d = NULL;
	// Loop over detector name to be used for the Focal plane Position
	// reconstruction
	for( Short_t i=0; (FPdetName = GetCodes().GetFPdetName(i)); i++ ){

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
				GetCodes().SetFPCode( Idx[0], Idx[1], Idx[2], Idx[3], inc1IsX );
				break;
			}
			else if( (Ncomp == 1) && !IncDetBitmask ){
				//Case where the Focal plane Position is reconstructed from 1 complete position measurment and 2 incomplete position measurment.

				fRT.dirFP.SetX( (XYZf[3-inc1IsX][0] - XYZf[0][0])/( XYZf[3-inc1IsX][2] - XYZf[0][2]) );
				fRT.dirFP.SetY( (XYZf[2+inc1IsX][1] - XYZf[0][1])/( XYZf[2+inc1IsX][2] - XYZf[0][2]) );
				GetCodes().SetFPCode( Idx[0], Idx[1], Idx[2], Idx[3], inc1IsX );
				break;
			}

		}
	}

	if( GetCodes().TestFPCode( kFPCode0 ) ) return;

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
//	SetFPCode( kFPCode0 ); // Initialize FP codes to code 0 "no FP position recon."
//
//	static TGraphErrors graphX(4); // 4 is the max number of FPdetectors
//	static TGraphErrors graphY(4); // 4 is the max number of FPdetectors
//	Int_t NptX=0, NptY=0;         // Number of Points in X-Z and Y-Z plans
//	const Char_t *FPdetName = NULL;
//	KVVAMOSDetector *d = NULL;
//	// Loop over detector name to be used for the Focal plane Position
//	// reconstruction
//	for( Short_t i=0; (FPdetName = GetCodes().GetFPdetName(i)); i++ ){
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
//		SetFPCode( kFPCode31 );
//      RunTrackingAtFocalPlane();
//		if( CheckTrackingCoherence() ) {
//			fRT.SetFPparamsReady();
//			return;
//		}
//	}
//
//}
//________________________________________________________________

void KVVAMOSReconNuc::ReconstructLabTraj(){
	// Reconstruction of the trajectory at the target point, in the reference
	// frame of the laboratory, from the trajectory at the focal plane.
	// The method ReconstructFPtraj() has to be call first.

	// No trajectory reconstruction in the laboratory if the reconstruction
	// in the focal plane is not OK.
	if( GetCodes().TestFPCode( kFPCode0 ) ) return;
	KVVAMOSTransferMatrix *tm = gVamos->GetTransferMatrix();
	tm->ReconstructFPtoLab( &fRT );
}
//________________________________________________________________

void KVVAMOSReconNuc::CalculateCalibration(){
	// Calculate the calibration for a well know calibration
	// nucleus (Z, A, E) i.e. energy losses in each crossed detector, in the 
	// stripping foil and in the target (if it exists) as well as  
	// times of flight are calculated. We assume that you have set Z, A 
	// and energy of the nucleus before calling this method (see SetZandA(...) 
	// and SetEnergy(...) ). 
	//
	// The calculated quantites are stored  in the nucleus's list KVParticle::fParameters in the form
	//    "[name of time of flight]" = [value in ns]
	//    
	//    for example:
	//    TSED1_HF = 200

	static KVNucleus nuc;
	nuc.SetZandA( GetZ(), GetA() );
	nuc.SetMomentum( GetMomentum() );
	Double_t E  = GetEnergy();
	Double_t DE = 0.;
	nuc.SetEnergy( E );

	//target 
	if( (E>0) && gMultiDetArray->GetTarget() ){
		gMultiDetArray->GetTarget()->SetIncoming(kFALSE);
        gMultiDetArray->GetTarget()->SetOutgoing(kTRUE);
		DE = gMultiDetArray->GetTarget()->GetELostByParticle( &nuc );
		E -= DE;
		nuc.SetEnergy( E );
		GetParameters()->SetValue(Form("DE:TARGET_%s",gMultiDetArray->GetTarget()->GetName()),DE);
	}

	//target to stripping foil
		
	//stripping foil
	if( (E>0) && gVamos->GetStripFoil() ){
		DE = gVamos->GetStripFoil()->GetELostByParticle( &nuc );
		E -= DE;
		nuc.SetEnergy( E );
		GetParameters()->SetValue(Form("DE:STRIPFOIL_%s",gVamos->GetStripFoil()->GetName()),DE);
	}

	//detectors at the focal plane
	TString tmp;
	KVNamedParameter *par = NULL;
	TIter next( GetParameters()->GetList() );
	while( (par = (KVNamedParameter *)next()) ){
		tmp = par->GetName();
		if( tmp.BeginsWith("STEP:") ){
			//calculate DE
			// TO BE IMPLEMENTED
			// TO BE IMPLEMENTED
			// TO BE IMPLEMENTED
		}
		else if( tmp.BeginsWith("DPATH:") ){
			//calculate TOF from target to active layer
			// TO BE IMPLEMENTED
			// TO BE IMPLEMENTED
			// TO BE IMPLEMENTED
		}
	}
}
//________________________________________________________________

void KVVAMOSReconNuc::RunTrackingAtFocalPlane(){
	// Run the tracking of this reconstructed trajectory in each volume (detectors)  punched through at the focal plane.

	// Tracking is impossible if the trajectory reconstruction
	// in the focal plane is not OK.
	//
	// The tracking is done by KVVAMOSReconGeoNavigator::PropagateNucleus();
	
	GetNavigator()->PropagateNucleus( this );

//	if( !CheckTrackingCoherence() ){
//		Info("ReconstructTrajectory","NO tracking coherence");
//		GetDetectorList()->ls();
//		GetParameters()->Print();
//		cout<<endl;
//	}
}
//________________________________________________________________

Bool_t KVVAMOSReconNuc::CheckTrackingCoherence(){
	// Verifies the coherence between the tracking result and the list of
	// fired detectors.
	// If at least one active volum of each fired detector is
	// inside the tracking result (saved in fParameters list). Return kTRUE if this
	// is OK.
	
	TString str;
	TIter nextdet( GetDetectorList() );
	KVVAMOSDetector *det = NULL;
	while( (det = (KVVAMOSDetector *)nextdet()) ){

		Bool_t ok = kFALSE;

		TIter next_tr( GetParameters()->GetList());
		TObject *tr = NULL;
		while( (tr = next_tr()) && !ok ){
			str = tr->GetName();
			if( str.BeginsWith( Form("DPATH:%s",det->GetName()) ) ) ok = kTRUE; 
		}

		if( !ok ) return kFALSE;
	}
	return kTRUE;
}
//________________________________________________________________

void KVVAMOSReconNuc::SetFlightDistanceAndTime(){
	// Set the best calibrated time of flight (ToF) and correct the path to 
	// set the distance associated to this ToF. The best ToF is found from
	// the list of time acquisition parameters set in the environment variable
	// KVVAMOSCodes.ACQParamListForToF
	// and from the list (fDetList) of detector punched through by the nucleus.
	// The first calibrated and fired acq. parameter belonging both to
	// the start detector and to the stop detector (only to the start detector 
	// for an HF-time)  of the list fDetList will be chosen, and the total flight distance
	// will be equal to:
	//  - the path (from target point to focal plan) corrected on the distance
	//    covered between the focal plan and the start detector for HF-time. 
	//  - the distance between the two detectors.

	TIter next_det( GetDetectorList() );
	KVVAMOSDetector *det   = NULL;
	KVVAMOSDetector *stop  = NULL;
	const Char_t *t_type   = NULL;
	const Char_t *par_name = NULL;
	KVACQParam *par        = NULL;
	Bool_t ok              = kFALSE;
	Double_t calibT        = 0; 

	// loop over the time acquisition parameters
	for( Short_t i=0; !ok && (par_name = GetCodes().GetToFName(i)); i++ ){

		par = gVamos->GetVACQParam( par_name );
		if( !par ) continue;

		t_type = par_name+1;
		Bool_t isT_HF = !strcmp("HF",par->GetLabel());
		
		next_det.Reset();
		while( (det = (KVVAMOSDetector *)next_det()) ){

			// for HF time we only need the stast detector
			if( isT_HF ){
			 	if( det->IsStartForT( t_type ) && (calibT = det->GetCalibT( t_type ))>0 ){
					ok = kTRUE;
					break;
 				}
			}
			// otherwise we need start and stop detectors
			else{
 				if( !stop && det->IsStopForT( t_type ) ){
					stop = det;
				}
				else if( stop && det->IsStartForT( t_type )  && (calibT = det->GetCalibT( t_type ))>0 ){
					ok = kTRUE;
					break;
				}
			}
		}
	}

	if( !ok ){ 
		SetTCode( kTCode0 );
		return;
	}

	// FIRST METHODE
	fToF = calibT;
	ok &= SetFlightDistance( det, stop );

	// SECOND METHODE
//	ok &= SetCorrectedFlightDistanceAndTime( calibT, det, stop );

	SetTCode(( ok ? par->GetName() : "") );
}
//________________________________________________________________

Bool_t KVVAMOSReconNuc::SetCorrectedFlightDistanceAndTime( Double_t tof,  KVVAMOSDetector *start, KVVAMOSDetector *stop){
	// Set the corrected flight distance/time  which will give the real velocity
	// of the nucleus prior to entering VAMOS. It is the distance/time between the nucleus's
	// exit in the stripping foil, if this nucleus loses energy inside this foil,
	// otherwise in the target, and the entry in the first crossed detector.
	//
	// WARNING: this method has to be called after the energy calibration.

	fFlightDist = 0.;
	fToF        = 0.;

	if( GetCodes().TestFPCode( kFPCode0 ) ) return kFALSE;
	if( GetPath()<=0. ) return kFALSE;
	

	//Find the first crossed detector and its DeltaPath
	Double_t DeltaPath = 0.;	
	KVVAMOSDetector *cros_det = NULL;
	Int_t Ndet = GetDetectorList()->GetEntries() ;
	for( Int_t i=Ndet-1; i>=0; i-- ){
		cros_det = (KVVAMOSDetector *)GetDetectorList()->At(i);
		DeltaPath = GetDeltaPath( cros_det );
		if( DeltaPath != 0. ) break;
		cros_det = NULL;
	}

	if( !cros_det ){ 
		Error("SetCorrectedFlightDistance","First crossed detector not found");
		return kFALSE;
	}


	fFlightDist = GetPath();
	fToF        = tof;

	cout<<endl;
	Info("SetCorrectedFlightDistance","Initial Path= %f cm, and ToF= %f ns", fFlightDist, fToF);
	
	static KVNucleus nuc;
	nuc.SetZandA( GetZ(), GetA() );

	Double_t Zlab     = TMath::Abs( GetLabDirection().Z() );
	Bool_t   isT_HF   = ( stop ? kFALSE : kTRUE );

	if( Zlab ){

		//Calculate the effective thickness of the target in cm
		Double_t targ_thick = 0.; //in cm
		TIter next( gMultiDetArray->GetTarget()->GetLayers() );
		KVMaterial *mat = NULL;
   		while ((mat = (KVMaterial *) next())){
      		targ_thick += mat->GetThickness();
   		}
		targ_thick = targ_thick/Zlab/2.;

		//Calculate the distance between target point-stripping foil 
		Double_t strip_foil_dist  = gVamos->GetStripFoilPosition()/Zlab;

		//Calculate the effective thickness of the stripping foil 
		Double_t strip_foil_thick = gVamos->GetStripFoil()->GetThickness()/Zlab;

		if( isT_HF ){
			//TIME: remove the DeltaT in the target
			nuc.SetEnergy( GetEnergy() );
			fToF -= targ_thick/nuc.GetV().Mag();
			Info("SetCorrectedFlightDistance","TIME: removing DeltaT(target)= %f ns", targ_thick/nuc.GetV().Mag());

			//TIME: remove the TOF between the target and the stripping foil
			nuc.SetEnergy( GetEnergy() - GetTargetEnergyLoss() );
			fToF -= ( strip_foil_dist - targ_thick )/nuc.GetV().Mag();
			Info("SetCorrectedFlightDistance","TIME: removing T(target-strip_foil)= %f ns", ( strip_foil_dist - targ_thick )/nuc.GetV().Mag());

			//TIME: remove the DeltaT in the stripping foil
			fToF -= strip_foil_thick/nuc.GetV().Mag();
			Info("SetCorrectedFlightDistance","TIME: removing DeltaT(strip_foil)= %f ns",strip_foil_thick/nuc.GetV().Mag());
		}

		if( GetStripFoilEnergyLoss()>0. ){

			//DISTANCE: remove the distance between target-stripping foil
			fFlightDist -= strip_foil_dist;
			Info("SetCorrectedFlightDistance","DISTANCE: removing StripFoilPosition= %f cm", strip_foil_dist);

			//DISTANCE: remove the stripping foil effective thickness
			fFlightDist -= strip_foil_thick;
			Info("SetCorrectedFlightDistance","DISTANCE: removing StripFoilThickness= %f cm", strip_foil_thick);
		}
		else{
			//DISTANCE: remove the half of the target effective thickness
			fFlightDist -= targ_thick;
			Info("SetCorrectedFlightDistance","DISTANCE: removing Thick_Target= %f cm", targ_thick);
		}
	}


	//DISTANCE: remove the DeltaPath of the first crossed detector
	fFlightDist += DeltaPath;
	Info("SetCorrectedFlightDistance","adding DeltaPath(%s)= %f cm: final FlightDist= %f cm", cros_det->GetName(), DeltaPath, fFlightDist);


	if( isT_HF ){
		//TIME: remove DeltaT/ToF in/between layers placed before the active
		//layer of the first crossed detector
		KVVAMOSDetector *det = NULL;
		for( Int_t i=Ndet-1; i>=0; i-- ){
			det = (KVVAMOSDetector *)GetDetectorList()->At(i);
			// I STOP HERE
			// I STOP HERE
			// I STOP HERE
		
			if( det == cros_det ) break;
		}
	}

	//TIME CORRECTION FOR NOT T_HF IS NOT IMPLEMENTED

	return kTRUE;
}
//________________________________________________________________

Bool_t KVVAMOSReconNuc::SetFlightDistance( KVVAMOSDetector *start, KVVAMOSDetector *stop){
	// Sets the flight distance from the start detector to the stop detector.
	// If stop=NULL then the corresponding time of flight 
	// is assumed to be measured from the beam HF and the distance will be
	// equal to the reconstructed path (GetPath) plus (or minus) the distance between
	// the trajectory position at the focal plane (FP) and the trajectory position
	// at the start detector if this detector is localised behinds the FP (or
	// forwards the FP).

	fFlightDist = 0;

	if( GetCodes().TestFPCode( kFPCode0 ) ) return kFALSE;
	fFlightDist = GetPath( start, stop );

	if( fFlightDist > 0. ) return kTRUE;

	TString warn;
	if( stop ) warn.Form("detectors %s and %s",start->GetName(), stop->GetName());
	else  warn.Form("target point and detector %s",start->GetName());
	Warning("SetFlightDistance","Impossible to set flight distance between %s; FPCode%d (%s)",warn.Data(),GetCodes().GetFPCodeIndex(), GetCodes().GetFPStatus());
	cout<<endl;

	return kFALSE;
}
//________________________________________________________________

Float_t KVVAMOSReconNuc::GetPath(KVVAMOSDetector *start, KVVAMOSDetector *stop) const{
	// Returns the flight distance travelled by the nucleus from the start detector to the stop detector.
	// If stop=NULL, returns the distance from the target point to the start detector, 
	// i.e. distance corresponding to a time of flight  measured from the beam HF then the distance will be
	// equal to the reconstructed path (GetPath) plus (or minus) the distance between
	// the trajectory position at the focal plane (FP) and the trajectory position
	// at the start detector if this detector is localised behinds the FP (or
	// forwards the FP).
	//
	// Returns zero if DeltaPath is not found for the detectors.
	
	Float_t dp_start = GetDeltaPath( start );
	if( dp_start ){
		// case where stop signal is given by HF i.e. 'stop' is null
		if( stop ){
			Float_t dp_stop = GetDeltaPath( stop );
			if( dp_stop ) return TMath::Abs( dp_stop - dp_start );
			else return 0.;
 		}	
		// case where stop signal is given by detector i.e. 'stop' not null
		else if( GetPath() ) return GetPath() + dp_start;
	}
	return 0.;
}
//________________________________________________________________

Float_t KVVAMOSReconNuc::GetPath(const Char_t *start_label, const Char_t *stop_label) const{
	// Returns the flight distance travelled by the nucleus from the start detector to the stop detector.
	// If stop=NULL, returns the distance from the target point to the start detector, 
	// i.e. distance corresponding to a time of flight  measured from the beam HF then the distance will be
	// equal to the reconstructed path (GetPath) plus (or minus) the distance between
	// the trajectory position at the focal plane (FP) and the trajectory position
	// at the start detector if this detector is localised behinds the FP (or
	// forwards the FP).
	//
	// Returns zero if DeltaPath is not found for the detectors.
	//
	// Inputs - labels of start and stop detectors

	KVVAMOSDetector *start = (KVVAMOSDetector *)GetDetectorList()->FindObjectByLabel( start_label );

	if( !strcmp( stop_label, "") ) return GetPath( start );
	KVVAMOSDetector *stop = (KVVAMOSDetector *)GetDetectorList()->FindObjectByLabel( stop_label );
	return GetPath( start, stop );
}
//________________________________________________________________

Float_t KVVAMOSReconNuc::GetDeltaPath( KVVAMOSDetector *det ) const{
	//returns the DeltaPath value associated to the detector 'det' used to correct
	//the flight distance.
	//Its value is given by a parameter stored in fParameters with the name
	//DPATH:<det_name>. If this parameter is not found then we take the parameter
	//with the name begining by DPATH:<det_time_base_name>. Otherwise zero is
	//returned with a warning message.
	//
	// This method has to be called once the tracking has been runned since
	// the DPATH parameter is calculated for each detector crossed by the nucleus
	// at this step (see RunTrackingAtFocalPlane).
	
	if( !det ) return 0.;
	// Find the parameter with the name DPATH:<detector_name>
	KVNamedParameter *par = GetParameters()->FindParameter( Form("DPATH:%s",det->GetName()) );
	if( par ) return par->GetDouble();

	// Find the parameter with the name begining by DPATH:<detector_time_base_name>
	TString tmp;
	TIter next( GetParameters()->GetList() );
	while( (par = (KVNamedParameter *)next()) ){
		tmp = par->GetName();
		if( tmp.BeginsWith( Form("DPATH:%s",det->GetTBaseName()) ) ){
			Info("GetDeltaPath","DeltaPath for the detector %s is given by %s",det->GetName(), par->GetName() );
 		   	return par->GetDouble(); 
		}
	}

	// No parameter found
	Warning("GetDeltaPath","DeltaPath not found for the detector %s",det->GetName());

	return 0.;
}
//________________________________________________________________

Float_t KVVAMOSReconNuc::GetEnergy( const Char_t *det_label ) const{
	// Returns the calculated contribution of each detector to the 
	// nucleus' energy from their label ("CHI","SI","SED1","SED2",...). 
	// Retruns -1 if no detector is found or if yet no contribution has
	// been determined ( done by methods Calibrate or InverseCalibration ).
 
	if( !fDetE ) return -1.;
	Int_t idx = GetDetectorIndex( det_label );
	return idx < 0 ? -1. : fDetE[idx];
}
//________________________________________________________________

Float_t KVVAMOSReconNuc::GetEnergyBefore( const Char_t *det_label ) const{
 	// Returns the kinetic energy of the nucleus prior to entering in 
 	// detector with label 'det_label' ("CHI","SI","SED1","SED2",...). 
	// Retruns -1 if no detector is found or if yet no contribution has
	// been determined ( done by methods Calibrate or InverseCalibration ).

	if( !fDetE ) return -1.;
	Int_t idx = GetDetectorIndex( det_label );
	if( idx < 0 ) return -1.;
	Float_t E = 0.;
	while( idx > -1  ) E+= fDetE[idx--];
	return E;
}
//________________________________________________________________

Float_t KVVAMOSReconNuc::GetEnergyAfter( const Char_t *det_label ) const{
 	// Returns the kinetic energy of the nucleus prior to entering in 
 	// detector with label 'det_label' ("CHI","SI","SED1","SED2",...). 
	// Retruns -1 if no detector is found or if yet no contribution has
	// been calculated ( done by methods Calibrate or InverseCalibration ).

	if( !fDetE ) return -1.;
	Int_t idx = GetDetectorIndex( det_label );
	if( idx < 0 ) return -1.;
	idx--;
	Float_t E = 0.;
	while( idx > -1  ) E+= fDetE[idx--];
	return E;
}
//________________________________________________________________

Int_t KVVAMOSReconNuc::GetDetectorIndex( const Char_t *det_label ) const{
	// Returns the index of the detector in the list of detectors (fDetList)
	// through which particle passed, from its label ("CHI","SI","SED1","SED2",...).
	// Returns -1 if no detector found.
	
	TIter next( GetDetectorList() );
    KVBase *obj;
	Int_t idx = 0;
    while ( (obj = (KVBase *)next()) ){
            if ( !strcmp(obj->GetLabel(), det_label ) ) break;
			idx++;
    }
	return  idx < GetDetectorList()->GetEntries() ? idx : -1;
}
