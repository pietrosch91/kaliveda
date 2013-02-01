//Created by KVClassFactory on Wed Jul 25 09:43:37 2012
//Author: Guilain ADEMARD

#include "KVVAMOSDetector.h"
#include "KVFunctionCal.h"

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
}
//________________________________________________________________

KVVAMOSDetector::KVVAMOSDetector(UInt_t number, const Char_t* type) : KVSpectroDetector(type){
	// create a new VAMOS detector of a given material.

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

   obj.Copy(*this);
}

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
	KVFunctionCal *c     = NULL;
	TF1           *func  = NULL;
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


		func = new TF1(par->GetName(),"pol1",0.,maxch);
		c = new KVFunctionCal(this, func);
		c->SetType(calibtype.Data());
		c->SetNumber( par->GetNumber() );
		c->SetACQParam( par );
		c->SetStatus( kFALSE );
		if(!AddCalibrator(c)) delete c;
	}
}
