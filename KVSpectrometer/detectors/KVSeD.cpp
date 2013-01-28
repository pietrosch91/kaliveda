//Created by KVClassFactory on Mon Sep  3 11:28:25 2012
//Author: Guilain ADEMARD

#include "KVSeD.h"
#include "KVFunctionCal.h"

ClassImp(KVSeD)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSeD</h2>
<h4>Secondary Electron Detector, used at the focal plan of VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
void KVSeD::init(){
	// Initialise non-persistent pointers
}
//________________________________________________________________

KVSeD::KVSeD()
{
   // Default constructor
   init();
   SetType("SED");
   SetName(GetArrayName());

}
//________________________________________________________________

KVSeD::KVSeD (const KVSeD& obj)  : KVVAMOSDetector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVSeD::~KVSeD()
{
   // Destructor
}
//________________________________________________________________

void KVSeD::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVSeD::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVVAMOSDetector::Copy(obj);
   //KVSeD& CastedObj = (KVSeD&)obj;
}
//________________________________________________________________

const Char_t* KVSeD::GetArrayName(){
	// Name of detector given in the form 
	// SED1 SED2 ...
	// to be compatible GANIL acquisition parameters
	//
	// The root of the name is the detector type.
	
	fFName = Form("%s%d",GetType(),GetNumber());
	return fFName.Data();
}
//________________________________________________________________

void KVSeD::SetACQParams(){
	// Setup the position acquistion parameters to the SeD.
	// These ACQ parameters have the type 'Q' (for charge).
	// Their name has the form: [detector type][detector number]_[X,Y]_[128,48]
	// where 128 is the number of strips for the X measurment and 48 
	// the number of 48 channels, each connecting 3 wires for the Y
	// measurment (A. Drouart et al Nuc. Instr. Meth A 579 (2007) 1090).
	TString name;
	KVACQParam *par = NULL;
	Char_t idx[] = {'X','Y'};
	for(Int_t i=0; i<2; i++){
		for(Int_t num =1; num<=(i? 48 : 128); num++){
			par = new KVACQParam;
			name.Form("%s%d_%c_%03d",GetType(),GetNumber(),idx[i],num);
			par->SetName(name);
			par->SetType("Q");
			AddACQParam(par);
		}
	}

//	// time parameter
//	par = new KVACQParam;
//	name.Form("T%s%d_HF",GetType(),GetNumber());
//	par->SetName(name);
//	par->SetType("T");
//	AddACQParam(par);
}
//________________________________________________________________

void KVSeD::SetCalibrators(){
	// Setup the calibrators for this detector. Call once name
	// has been set.
	// The calibrators are KVFunctionCal.
	// By default the all the calibration functions are first-degree
	// polynomial function and the range [Xmin,Xmax]=[0,4096].
	// Here the calibrator are not ready (KVFunctionCal::GetStatus()).
	// You have to give the parameters and change the status
	// (see KVFunctionCal::SetParameter(...) and KVFunctionCal::SetStatus(...))
	
	TIter nextpar(GetACQParamList());

	KVACQParam   *par  = NULL;
	KVCalibrator *c    = NULL;
	TF1          *func = NULL;
	Double_t      maxch= 4096;

	TString  calibtype;

	while((par = (KVACQParam *)nextpar())){

		if(!strcmp(par->GetType(),"Q")){
 			calibtype = "channel->Volt";
			maxch     = 4096.;             // 12 bits
		}
		else if(!strcmp(par->GetType(),"T")){
 			calibtype = "channel->ns";
			maxch     = 16384.;           // 14 bits

		}
		else continue;

		calibtype.Append(" ");
		calibtype.Append(par->GetName());



		func = new TF1(GetName(),"pol1",0.,maxch);
		c = new KVFunctionCal(this, func);
		c->SetType(calibtype.Data());
		if(!AddCalibrator(c)) delete c;
	}
}
//________________________________________________________________

//void KVSeD::SetCalibrators(KVDBParameterSet *kvdbps){
////	KVFunctionCal *cal = new KVFunction(
//}
