//Created by KVClassFactory on Wed Feb 13 10:18:35 2013
//Author: Guilain ADEMARD

#include "KVSeDPositionCal.h"

ClassImp(KVSeDPositionCal)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSeDPositionCal</h2>
<h4>Position calibration for SeD detectors of VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSeDPositionCal::KVSeDPositionCal() : KVCalibrator(NPAR_SEDPOSCAL)
{
   // Default constructor
   SetType("position->cm");
}
//________________________________________________________________

KVSeDPositionCal::KVSeDPositionCal(KVDetector *det) : KVCalibrator(NPAR_SEDPOSCAL)
{
    //Create a position calibration object for a specific detector (*det)
    //The calibrator takes the name of the detector.
   SetDetector( det );
   SetName( det->GetName() );
   SetType(Form("position->cm %s",GetName()));
}
//________________________________________________________________

KVSeDPositionCal::~KVSeDPositionCal()
{
   // Destructor
}
//________________________________________________________________
   
Double_t KVSeDPositionCal::Compute(Double_t chan) const{
	// Obsolete method, use Compute(Double_t X, Double_t Y, Double_t &Xcal, Double_t &Ycal).
	chan = chan;
	Warning("Compute(Double_t chan)","Obsolete method, use Compute(Double_t X, Double_t Y, Double_t &Xcal, Double_t &Ycal)");
	return -1;
}
//________________________________________________________________

Double_t KVSeDPositionCal::operator() (Double_t chan){
	// Obsolete method, use operator()(Double_t X, Double_t Y, Double_t &Xcal, Double_t &Ycal).
	chan = chan;
	Warning("operator()(Double_t chan)","Obsolete method, use operator()(Double_t X, Double_t Y, Double_t &Xcal, Double_t &Ycal)");
	return -1;
}
//________________________________________________________________

Double_t KVSeDPositionCal::Invert(Double_t volts){
	//Obsolete method, use Invert(Double_t Xcal, Double_t Ycal, Double_t &X, Double_t &Y).
	volts = volts;
	Warning("Invert(Double_t volts)","Obsolete method, to much difficult to invert this calibrator!");
	return -1;
}
//________________________________________________________________

Bool_t KVSeDPositionCal::Compute(Double_t X, Double_t Y, Double_t &Xcal, Double_t &Ycal){

	Double_t Xtmp[2];
	Xtmp[0]= X+1.; Xtmp[1]= Y+1.; // To have wires starting at 1

	Xcal = Ycal = -666;
	if(Xtmp[0]<=0 || Xtmp[1]<=0) return kFALSE;

	UShort_t num = 0;
	for(Int_t h=0; h<2; h++){ // calibration process performed two times 
		for(Int_t i=0;i<2;i++){ // Correction in X and in Y

			// calibration on the SeD# plane in cm
			Double_t cor   = Xtmp[i];
			Double_t other = Xtmp[ i ? 0 : 1 ];
			Xtmp[i] = 0.;
			for(Int_t j=0;j<=SEDPOSCAL_FIT_ORDER;j++){
 				for(Int_t k=0;k<=SEDPOSCAL_FIT_ORDER;k++){
					if( GetParameter(num) != 0 )
						Xtmp[i] += GetParameter(num) * TMath::Power(cor,j)*TMath::Power(other,k);
					num++;
				}
			}
		}
	}

	Xcal = Xtmp[0]; Ycal = Xtmp[1]; 

	return kTRUE;
}
//________________________________________________________________

Bool_t KVSeDPositionCal::operator() (Double_t X, Double_t Y, Double_t &Xcal, Double_t &Ycal){
	return Compute(X,Y,Xcal,Ycal);
}
