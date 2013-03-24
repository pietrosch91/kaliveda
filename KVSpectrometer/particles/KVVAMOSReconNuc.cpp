//Created by KVClassFactory on Thu Sep 13 10:51:51 2012
//Author: Guilain ADEMARD

#include "KVVAMOSReconNuc.h"
#include "KVVAMOSDetector.h"

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
}

//________________________________________________________________

KVVAMOSReconNuc::KVVAMOSReconNuc (const KVVAMOSReconNuc& obj)  : KVReconstructedNucleus(), fCodes()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVVAMOSReconNuc::~KVVAMOSReconNuc()
{
   // Destructor
   init();
   fCode.Clear();
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
   //KVVAMOSReconNuc& CastedObj = (KVVAMOSReconNuc&)obj;
}
//________________________________________________________________

void KVVAMOSReconNuc::init()
{
	//default initialisations
	if (gDataSet)
		SetMassFormula(UChar_t(gDataSet->GetDataSetEnv("KVVAMOSReconNuc.MassFormula",Double_t(kEALMass))));
	fXf = fYf = fThetaf = fPhif = 0.;
}
//________________________________________________________________

void KVVAMOSReconNuc::Calibrate(){

	/////////////////////////////////
	// set code here ////////////////
	/////////////////////////////////
	
//	SetIsCalibrated();
		//add correction for target energy loss - charged particles only
//		Double_t E_targ = 0.;
//		if(GetZ()) {
//			E_targ = gVAMOS->GetTargetEnergyLossCorrection(this);
//			SetTargetEnergyLoss( E_targ );
//		}
//		Double_t E_tot = GetEnergy() + E_targ;
//		SetEnergy( E_tot );

	Warning("Calibrate","TO BE IMPLEMENTED");
}
//________________________________________________________________

void KVVAMOSReconNuc::Clear(Option_t * t){
	//Reset nucleus' properties
	
	KVReconstructedNucleus::Clear(t);
   	init();
   	fCodes.Clear();
}
//________________________________________________________________

void KVVAMOSReconNuc::ConstructFocalPlanTrajectory(KVList *detlist){

	TIter next_det( detlist );
	KVVAMOSDetector *d = NULL;
	while( (d = (KVVAMOSDetector *)next_det()) ){
   		AddDetector(d);
        d->AddHit(this);  // add particle to list of particles hitting detector
        d->SetAnalysed(kTRUE);   //cannot be used to seed another particle
	}


	UChar_t res = 0;
	Double_t xyzf[3];          // [3] => [X, Y, Z]
	Double_t XYZf[4][3];       // [4] => [Complete Det1, Complete Det2, Incomplete Det1, Incomplete Det2]
	// [3] => [X, Y, Z]
	Short_t Idx[4] = {-1, -1, -1, -1};// [4] => [Complete Det1, Complete Det2, Incomplete Det1, Incomplete Det2]
	UChar_t IncDetBitmask = 3;       // bits 11 => 3, first bit means missing X, second bit means missing Y
	Bool_t inc1IsX;                  // true if the coordinate given by the first incomplete position is X ( false if Y)
	UShort_t Ncomp = 0, Ninc = 0; // Number of Complete and Incomplete
	// position measurments 

	fCodes.SetFPCode( kFPCode0 ); // Initialize FP codes to code 0 "no FP position recon."

	const Char_t *FPdetName = NULL;
	// Loop over detector name to be used for the Focal plan Position
	// reconstruction
	for( Short_t i=0; (FPdetName = fCodes.GetFPdetName(i)); i++ ){

		// Look at only the detectors in 'detlist' with the name or
		// type 'FPdetName' measuring a position.
		if(
				((d = (KVVAMOSDetector *)detlist->FindObject( FPdetName )) ||
 		 		 (d = (KVVAMOSDetector *)detlist->FindObjectByType( FPdetName )))
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
				//Case where the Focal plan Position is reconstructed from 2 complete position measurment.

				// tan( Thetaf )   = (Xc2-Xc1)/(Zc2-Zc1)
				Double_t TanThetaf = (XYZf[1][0] - XYZf[0][0])/( XYZf[1][2] - XYZf[0][2]);

				// tan( Phif   )   = (Yc2-Yc1)/(Zc2-Zc1)
				Double_t TanPhif   = (XYZf[1][1] - XYZf[0][1])/( XYZf[1][2] - XYZf[0][2]);

				// Xf = Xc1      - Zc1*tan( Thetaf )
				fXf = XYZf[0][0] - XYZf[0][2]*TanThetaf;

				// Yf = Yc1      - Zc1*tan( Phif   )
				fYf = XYZf[0][1] - XYZf[0][2]*TanPhif;

				fThetaf = TMath::ATan( TanThetaf  )*TMath::RadToDeg();
				fPhif   = TMath::ATan( TanPhif    )*TMath::RadToDeg();

				fCodes.SetFPCode( Idx[0], Idx[1], Idx[2], Idx[3], inc1IsX );
				break;
			}
			else if( (Ncomp == 1) && !IncDetBitmask ){
				//Case where the Focal plan Position is reconstructed from 1 complete position measurment and 2 incomplete position measurment.

				// tan( Thetaf )   = (Xi1-Xc1)/(Zi1-Zc1)
				Double_t TanThetaf = (XYZf[3-inc1IsX][0] - XYZf[0][0])/( XYZf[3-inc1IsX][2] - XYZf[0][2]);

				// tan( Phif   )   = (Xi2-Xc1)/(Zi2-Zc1)
				Double_t TanPhif   = (XYZf[2+inc1IsX][1] - XYZf[0][1])/( XYZf[2+inc1IsX][2] - XYZf[0][2]);

				// Xf = Xc1      - Zc1*tan( Thetaf )
				fXf = XYZf[0][0] - XYZf[0][2]*TanThetaf;

				// Yf = Yc1      - Zc1*tan( Phif   )
				fYf = XYZf[0][1] - XYZf[0][2]*TanPhif;

				fThetaf = TMath::ATan( TanThetaf  )*TMath::RadToDeg();
				fPhif   = TMath::ATan( TanPhif    )*TMath::RadToDeg();

				fCodes.SetFPCode( Idx[0], Idx[1], Idx[2], Idx[3], inc1IsX );
				break;
			}

		}
	}
}
//________________________________________________________________

void KVVAMOSReconNuc::ConstructLabTrajectory(){
	Warning("ConstructLabTrajectory","TO BE IMPLEMENTED");
}
