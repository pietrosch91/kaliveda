//Created by KVClassFactory on Wed Jul 25 09:43:37 2012
//Author: Guilain ADEMARD

#include "Riostream.h"
#include "KVVAMOSDetector.h"
#include "KVVAMOS.h"
using namespace std;

ClassImp(KVVAMOSDetector)

	////////////////////////////////////////////////////////////////////////////////
	// BEGIN_HTML <!--
	/* -->
	   <h2>KVVAMOSDetector</h2>
	   <h4>Detectors of VAMOS spectrometer</h4>
	   <!-- */
	// --> END_HTML
	////////////////////////////////////////////////////////////////////////////////

KVVAMOSDetector::KVVAMOSDetector()
{
   	// Default constructor
   	init();
}
//________________________________________________________________

KVVAMOSDetector::KVVAMOSDetector(UInt_t number, const Char_t* type) : KVSpectroDetector(type){
	// create a new VAMOS detector of a given material.

   	init();
	SetNumber(number);
}
//________________________________________________________________

KVVAMOSDetector::KVVAMOSDetector (const KVVAMOSDetector& obj)  : KVSpectroDetector()
{
   	// Copy constructor
   	// This ctor is used to make a copy of an existing object (for example
   	// when a method returns an object), and it is always a good idea to
   	// implement it.
   	// If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   	init();
   	obj.Copy(*this);
}
//________________________________________________________________

KVVAMOSDetector::~KVVAMOSDetector()
{
   	// Destructor
}
//________________________________________________________________

void KVVAMOSDetector::Copy (TObject& obj) const
{
   	// This method copies the current state of 'this' object into 'obj'
   	// You should add here any member variables, for example:
   	//    (supposing a member variable KVVAMOSDetector::fToto)
   	//    CastedObj.fToto = fToto;
   	// or
   	//    CastedObj.SetToto( GetToto() );

   	KVSpectroDetector::Copy(obj);
   	//KVVAMOSDetector& CastedObj = (KVVAMOSDetector&)obj;
}
//________________________________________________________________

void KVVAMOSDetector::init(){
	fTimeParam = NULL;
	fCh_ns     = NULL;
}
//________________________________________________________________

Double_t KVVAMOSDetector::GetCalibTimeHF() const{
	// Caculate time of flight in ns from coder value.
	// Return 0 if calibration not ready or detector not fired
	
	if( IsTimeCalibrated() && fCh_ns->GetACQParam()->Fired("P"))
		return fCh_ns->Compute();
	else return 0;
}
//________________________________________________________________

Float_t KVVAMOSDetector::GetTimeHF() const{
	// Access acquisition data value associated to the time of flight 
	// recorded between the beam pulse and timing from this detector.
	// Returns value as a floating-point number which is the raw channel number
	// read from the coder plus a random number in the range [-0.5,+0.5].
   	// If the detector has no ACQ parameter for time of flight,
	// or if the raw channel number = 0, the value returned is -1

	return ( fTimeParam ? fTimeParam->GetData() : -1 );
}
//________________________________________________________________

void KVVAMOSDetector::Initialize(){
	// Initialize the data members. Called by KVVAMOS::Initialize().
	fTimeParam = (KVACQParam *)GetACQParamList()->FindObjectByType("T");
	if( fTimeParam ){
		TString acqname;
		acqname.Form("channel->ns %s", fTimeParam->GetName());
 		fCh_ns =(KVFunctionCal *)GetCalibrator( acqname.Data() );
	}
}
//________________________________________________________________

void KVVAMOSDetector::SetCalibrators(){
	// Setup the calibrators for this detector. Call once name
	// has been set.
	// The calibrators are KVFunctionCal.
	// By default the all the calibration functions are first-degree
	// polynomial function and the range [Xmin,Xmax]=[0,4096].
	// Here the calibrator are not ready (KVFunctionCal::GetStatus()).
	// You have to give the parameters and change the status
	// (see KVFunctionCal::SetParameter(...) and KVFunctionCal::SetStatus(...))
	

	TIter nextpar(GetACQParamList());

	KVACQParam    *par   = NULL;
	Double_t       maxch = 16384.;       // 14 bits

	TString  calibtype("ERROR");

	while((par = (KVACQParam *)nextpar())){

		if(!strcmp(par->GetType(),"E")){
 			calibtype = "channel->MeV";
		}
		else if(!strcmp(par->GetType(),"Q")){
 			calibtype = "channel->Volt";
			maxch     = 4096.;           // 12 bits
		}
		else if(!strcmp(par->GetType(),"T")){
 			calibtype = "channel->ns";
		}
		else continue;

		calibtype.Append(" ");
		calibtype.Append(par->GetName());

		TF1 *func        = new TF1(par->GetName(),"pol1",0.,maxch);
		KVFunctionCal *c = new KVFunctionCal(this, func);
		c->SetType(calibtype.Data());
		c->SetNumber( par->GetNumber() );
		c->SetACQParam( par );
		c->SetStatus( kFALSE );
		if(!AddCalibrator(c)) delete c;
	}
}
