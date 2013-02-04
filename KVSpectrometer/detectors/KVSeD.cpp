//Created by KVClassFactory on Mon Sep  3 11:28:25 2012
//Author: Guilain ADEMARD

#include "KVSeD.h"
#include "KVFunctionCal.h"
#include "TH1F.h"

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
	
	fQraw = new TH1F*[2]; 
	fQ    = new TH1F*[2];

	for(Int_t i=0; i<2; i++){
		fQraw[i] = NULL;
		fQ[i]    = NULL;
	}
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
//________________________________________________________________

KVSeD::~KVSeD()
{
   // Destructor
	for(Int_t i=0; i<2; i++){
		SafeDelete( fQraw[i] );
		SafeDelete( fQ[i] );
	}
	delete[] fQraw;
	delete[] fQ;
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

void  KVSeD::Clear(Option_t *option ){
	// Set the energy losses etc. to zero and reset the histograms used
	// to find the X,Y positions (Qraw and Q)

	Info("Clear","IN");
	KVVAMOSDetector::Clear( option );

	for(Int_t i=0; i<2; i++){
		if(fQraw[i]) fQraw[i]->Reset();
		if(fQ[i])    fQ[i]->Reset();
	}
	Info("Clear","OUT");
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
   
TH1F *KVSeD::GetQrawHisto(const Char_t dir){
	// Retruns the X or Y position histogram with raw charges (Q)
	
	Int_t i;
	if( dir == 'X' ) i = 0; 
	else if (dir == 'Y' ) i = 1;
	else return NULL;

	if( !fQraw[i] ) return NULL;
	fQraw[i]->Reset();
	TIter next(GetACQParamList());

	Bool_t ok = kFALSE;
	KVACQParam *par  = NULL;
	Int_t idx, num;
	while((par = (KVACQParam *)next())){

		if( !par->Fired() ) continue;
		idx = par->GetNumber()/1000-1;

		if( i != idx ) continue;
		num =  par->GetNumber() - (idx+1)*1000;
		Float_t data;
		fQraw[idx]->SetBinContent( num, data = par->GetData() );
		ok = kTRUE;
Info("GetQrawHisto","%c position: %s, num= %d, Qraw= %f",dir, par->GetName(), num, data);
	}

	if( !ok ) Warning("GetQrawHisto","No ACQ parameter fired for the %c position",dir);

 	return fQraw[i];
}
//________________________________________________________________
   
TH1F *KVSeD::GetQHisto(const Char_t dir){
	// Retruns the X or Y position histogram with calibrated charges (Q)

	Int_t i;
	if( dir == 'X' ) i = 0; 
	else if (dir == 'Y' ) i = 1;
	else return NULL;

	if( !fQ[i] ) return NULL;
	fQ[i]->Reset();
	TIter next(GetListOfCalibrators());

	Bool_t ok = kFALSE;
	KVFunctionCal *cal  = NULL;
	Int_t idx, num;
	while((cal = (KVFunctionCal *)next())){

		if( !cal->GetStatus() ) continue;
		if( !cal->GetACQParam()->Fired() ) continue;

		idx = cal->GetNumber()/1000-1;
 	   	if( i != idx ) continue;	
		num =  cal->GetNumber() - (idx+1)*1000;
		fQ[idx]->SetBinContent( num, cal->Compute() );
		ok = kTRUE;
	}

 	if( !ok ){
		Error("GetQHisto","Impossible to calibrate correctly the charges for %c position, please check the calibrator status",dir);
		return NULL;
	}
 	return fQ[i];
}
//________________________________________________________________

void KVSeD::SetACQParams(){
	// Setup the position acquistion parameters to the SeD.
	// These ACQ parameters have the type 'Q' (for charge).
	// Their name has the form: [detector type][detector number]_[X,Y]_[128,48]
	// where 128 is the number of strips for the X measurment and 48 
	// the number of 48 channels, each connecting 3 wires for the Y
	// measurment (A. Drouart et al Nuc. Instr. Meth A 579 (2007) 1090).
	//
	// Number of each ACQ parameters:
	//       SED1_X_001  --> 1001
	//       SED1_X_002  --> 1002
	//              :
	//       SED1_X_128  --> 1128
	//       SED1_Y_001  --> 2001
	//       SED1_Y_002  --> 2002
	//              :
	//       SED1_Y_128  --> 2128


	// Charge ACQ parameters	
	TString name, title;
	KVACQParam *par = NULL;
	Char_t idx[] = {'X','Y'};
	for(Int_t i=0; i<2; i++){
		for(Int_t num =1; num<=(i? _SED_NY_ : _SED_NX_); num++){
			par = new KVACQParam;
			name.Form("%s_%c_%03d",GetArrayName(),idx[i],num);
			par->SetName(name);
			par->SetType("Q");
			par->SetNumber( (i+1)*1000 + num);
			AddACQParam(par);
		}

		// build Qraw and Q histograms for X and Y position

		// Qraw histogram
		if(fQraw[i]) SafeDelete( fQraw[i] );
			name.Form("Qraw_%s_%c",GetArrayName(),idx[i]);
			title.Form("%c position for %s; Strip number; Qraw (channel)",idx[i],GetArrayName());
		fQraw[i] = new TH1F( name.Data(), title.Data(), _SED_NX_, .5, _SED_NX_ +.5);
		fQraw[i]->SetDirectory(0);

		// Q histogram
		if(fQ[i]) SafeDelete( fQ[i] );
			name.Form("Q_%s_%c",GetArrayName(),idx[i]);
			title.Form("%c position for %s; Strip number; Q (Volt)",idx[i],GetArrayName());
		fQ[i] = new TH1F( name.Data(), title.Data(), _SED_NX_, .5, _SED_NX_ +.5);
		fQ[i]->SetDirectory(0);
	}

	// time ACQ parameter
	par = new KVACQParam;
	name.Form("T%s%d_HF",GetType(),GetNumber());
	par->SetName(name);
	par->SetType("T");
	par->SetNumber( 3000 + 1);
	AddACQParam(par);
}
//________________________________________________________________

void KVSeD::ShowQrawHisto(const Char_t dir, Option_t *opt){
	opt = opt; 
	TH1F *hh = GetQrawHisto( dir );
	if( hh ) hh->Draw(opt);
}
//________________________________________________________________

void KVSeD::ShowQHisto(const Char_t dir, Option_t *opt){
	opt = opt; 
	TH1F *hh = GetQHisto( dir );
	if( hh ) hh->Draw(opt);
}
