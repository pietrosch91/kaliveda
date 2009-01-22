/*
$Id: KVFunctionCal.cpp,v 1.5 2008/10/07 15:55:20 franklan Exp $
$Revision: 1.5 $
$Date: 2008/10/07 15:55:20 $
*/

//Created by KVClassFactory on Wed Sep 19 14:01:23 2007
//Author: Eric Bonnet

#include "KVFunctionCal.h"
#include "KVDBParameterSet.h"
#include "KVMultiDetArray.h"

#include "TF1.h"

ClassImp(KVFunctionCal)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFunctionCal</h2>
<h4>analytic function calibration E = f(channel)</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////


//------------------------------
KVFunctionCal::KVFunctionCal(TF1 *ff):KVCalibrator(1)
//------------------------------
{
   // Constructor of KVCalibrator in used
	// Initialisation of the calibration function fcalibfunction
	fPedCorr=kTRUE;
	fcalibfunction=ff;
	
}

//------------------------------
KVFunctionCal::KVFunctionCal(KVDetector * kvd,TF1 *ff):KVCalibrator(1)
//------------------------------
{
   // Constructor of KVCalibrator in used
	// Initialisation of the calibration function fcalibfunction
	// and link with the KVDetector is done
	fPedCorr=kTRUE;
   SetDetector(kvd);
   fcalibfunction=ff;

}

//------------------------------
KVFunctionCal::KVFunctionCal(KVDBParameterSet* kvdbps):KVCalibrator(1)
//------------------------------
{
   // Specific constructor using a KVDBParameterSet*
	// From where all information on the calibration function, the detector and type of signal
	// are specified (for more details see KVINDRAUpDater_e475s::SetCalibParameters(KVDBRun* kvrun))
	fPedCorr=kTRUE;
   fcalibfunction=new TF1(kvdbps->GetName(),kvdbps->GetParamName(0));
	for (Int_t pp=0;pp<fcalibfunction->GetNpar();pp+=1) {
		fcalibfunction->SetParameter(pp,kvdbps->GetParameter(pp+1));
	}
	fcalibfunction->SetRange(kvdbps->GetParameter(kvdbps->GetParamIndex("xmin")),kvdbps->GetParameter(kvdbps->GetParamIndex("xmax")));
	SetType(kvdbps->GetTitle());
	SetDetector(gMultiDetArray->GetDetector(kvdbps->GetName()));

}

//------------------------------
void KVFunctionCal::ChangeCalibParameters(KVDBParameterSet *kvdbps)
//------------------------------
{
   // Update of calibration parameters
	fPedCorr=kTRUE;
   if (fcalibfunction) delete fcalibfunction;
	fcalibfunction=new TF1(kvdbps->GetName(),kvdbps->GetParamName(0));
	for (Int_t pp=0;pp<fParamNumber-1;pp+=1) {
		fcalibfunction->SetParameter(pp,kvdbps->GetParameter(pp+1));
	}
	fcalibfunction->SetRange(kvdbps->GetParameter(kvdbps->GetParamIndex("xmin")),kvdbps->GetParameter(kvdbps->GetParamIndex("xmax")));
	
}

//------------------------------
void KVFunctionCal::SetConversionType(TString from,TString to,TString signal)
//------------------------------
{
  
	// Define the calibration scheme for example from channel to mev concerning PG signal
	// It gives Channel->MeV(PG)
	SetType(Form("%s->%s(%s)",from.Data(),to.Data(),signal.Data()));

}

//------------------------------
Double_t KVFunctionCal::Compute(Double_t from) const
//------------------------------
{
	// Give the calibration result for a giving value 
	if (fcalibfunction) return fcalibfunction->Eval(from);
	else return -666;
}

//------------------------------
Double_t KVFunctionCal::Invert(Double_t to)
//------------------------------
{
	// Give the original value from a calibrated value 
	if (fcalibfunction){
		Double_t xmin, xmax; fcalibfunction->GetRange(xmin,xmax);
		return fcalibfunction->GetX(to, xmin, xmax);
	}
	else return -666;

}
//------------------------------
Double_t KVFunctionCal::operator() (Double_t chan) {
//------------------------------
   //Overloading of "()" to allow syntax such as:
   //
   //        KVLinCal calibrator;
   //           ....
   //        Float_t calibrated_volts = calibrator(channel);
   //
   //equivalently to:
   //
   //        Float_t calibrated_volts = calibrator.Compute(channel);
   return Compute(chan);
}
