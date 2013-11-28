//Created by KVClassFactory on Mon Sep  3 11:29:00 2012
//Author: Guilain ADEMARD

#include "KVDriftChamber.h"
#include "TH1F.h"

ClassImp(KVDriftChamber)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVDriftChamber</h2>
<h4>Drift Chamber, used at the focal plan of VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVString KVDriftChamber::fACQParamTypes("0:Q, 1:E, 2:T, 9:NO_TYPE");
KVString KVDriftChamber::fPositionTypes("0:X, 1:Y, 2:Z, 3:XY, 9:NO_TYPE");
Int_t    KVDriftChamber::fNstripsOK = 3;
	
void KVDriftChamber::init(){
	// Initialise non-persistent pointers
	fQ  = new TH1F**[3];
	for(Int_t i=0; i<3; i++){  //[ raw, calibrated, clean ]
		fQ[i] = new TH1F*[2];
		for(Int_t j=0; j<2; j++){  //[ Chamber 1, Chamber 2 ]
			fQ[i][j]      = NULL;
		}
	}

	fRawPos [0] = fRawPos [1] = -500;
	fERawPos[0] = fERawPos[1] = -500;

	fNstripsOK = (Int_t)gDataSet->GetDataSetEnv("KVDriftChamber.NumberOfStripsOK", 3.);
	fDriftV    = gDataSet->GetDataSetEnv("KVDriftChamber.DriftVelocity", 4.84);  // in cm/us

	fTfilPar = NULL;

	//a KVDriftChamber can not be used in a ID telescope
	ResetBit( kOKforID );
}
//________________________________________________________________

KVDriftChamber::KVDriftChamber()
{
   // Default constructor
   init();
   SetType("DC");
   SetLabel("DC");
   SetName(GetArrayName());
}
//________________________________________________________________

KVDriftChamber::KVDriftChamber (const KVDriftChamber& obj)  : KVVAMOSDetector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVDriftChamber::~KVDriftChamber()
{
   // Destructor
	for(Int_t i=0; i<3; i++){ // loop over [ raw, calib, clean ]
		for(Int_t j=0; j<2; j++){ // loop over both chambers
			SafeDelete( fQ[i][j] );
		}
		delete[] fQ[i];
	}
	delete[] fQ;
}
//________________________________________________________________

void KVDriftChamber::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVDriftChamber::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVVAMOSDetector::Copy(obj);
   //KVDriftChamber& CastedObj = (KVDriftChamber&)obj;
}
//________________________________________________________________

void  KVDriftChamber::Clear(Option_t *option ){
	// Set the energy losses etc. to zero and reset the histograms used
	// to find the positions (Qraw and Q)

	KVVAMOSDetector::Clear( option );
	ResetCalculatedData();
}
//________________________________________________________________

void KVDriftChamber::Initialize(){
	// Initialize the data members. Called by KVVAMOS::Initialize().
	ResetCalculatedData();
}
//________________________________________________________________

void KVDriftChamber::ResetCalculatedData(){
	// Reset all position histograms (raw, calibrated, clean)
	// and raw positions for both chambers.
	// This method have to be called each time you read a new event (i.e.
	// when the ACQ parameter change) otherwise certain methods 
	// (such as GetPosition(), GetQrawHisto() ...) will not return 
	// what you expect.
	for(Int_t i=0; i<3; i++){
		for(Int_t j=0; j<2; j++){
			if(fQ[i][j]) fQ[i][j]->Reset();
		}
	}

	fRawPos [0] = fRawPos [1]  = -500;
	fERawPos[0] = fERawPos[1] = -500;
}
//________________________________________________________________

const Char_t* KVDriftChamber::GetArrayName(){
	// Name of detector given in the form
	// DC1 DC2 ...
	//
	// The root of the name is the detector type.
	
	if( GetNumber()>0 ){
		fFName  = Form("%s%d",GetType(),GetNumber());
		SetLabel( fFName.Data() );
	}
	else fFName = GetType();
	return fFName.Data();
}
//________________________________________________________________

const Char_t *KVDriftChamber::GetEBaseName(){
	// Base name of the energy used to be compatible 
	// GANIL acquisition parameters 
	//
	// The base name is "EFIL_<number>"

	return Form("EFIL_%d",GetNumber());
}
//________________________________________________________________

void KVDriftChamber::SetACQParams(){
	// Setup the acquistion parameters of electron drift time, energy and position  to the drift chamber.
	// Electron drif time ACQ parameters have the type 'T' (for time).
	// Energy ACQ parameters have the type 'E' (for Energy).
	// Their name has the form: [param. type]FIL_[detector number]
	//
	// Position ACQ parameters have the type 'Q' (for charge).
	// Their name has the form: STR_[detector number][chamber number]_[64]
	// where 64 is the number of strips or the X measurment in each chamber.



	// Charge ACQ parameters	
	TString name, title;
	KVACQParam *par = NULL;
	Int_t Nstrips =  64;
	const Char_t *extraname[]  = {    "raw","calib",        "clean"};
	const Char_t *extratitle[] = {       "",     "","without noise"};
	const Char_t *units[]      = {"channel", "Volt",         "Volt"};
	for(Int_t i=0; i<2; i++){ // loop over row
		Int_t c_num = i+1;
		for(Int_t num =1; num<=Nstrips; num++){  // loop over strips
			par = new KVACQParam;
			name.Form("STR_%d%d_%02d",GetNumber(),c_num,num);
			par->SetName(name);
			par->SetType("Q");
			par->SetLabel( Form("C%d", c_num) );
			par->SetNumber( num );
			par->SetUniqueID( CalculateUniqueID( par ) );
			AddACQParam(par);
		}

		// build  Q histograms for both rows
		for(Int_t j=0; j<3; j++){ // loop over {raw,calib,clean}
			// Qraw histogram
			if(fQ[j][i]) SafeDelete( fQ[j][i] );
			name.Form("Q%s_%s_C%d",extraname[j],GetArrayName(),c_num);
			title.Form("X position in chamber %d of %s %s; Strip number; Q%s (%s)",c_num,GetArrayName(),extratitle[j],extraname[j],units[j]);
			fQ[j][i] = new TH1F( name.Data(), title.Data(), Nstrips, .5, Nstrips +.5);
			fQ[j][i]->SetDirectory(0);
			fQ[j][i]->SetLineColor(j+1);
		}
	}

	// Energy ACQ Parameters
	par = new KVACQParam;
	par->SetName( GetEBaseName() );
	par->SetType("E");
	par->SetNumber( GetNumber() );
	par->SetUniqueID( CalculateUniqueID( par ) );
	AddACQParam(par);

	// Electron drift time ACQ Parameters
	par = new KVACQParam;
	par->SetName( Form("TFIL_%d",GetNumber()) );
	par->SetType("T");
	par->SetNumber( GetNumber() );
	par->SetUniqueID( CalculateUniqueID( par ) );
	AddACQParam(par);
	fTfilPar = par;
}
//________________________________________________________________
   
TH1F *KVDriftChamber::GetQrawHisto( Int_t c_num ){
	// Retruns the X position histogram with raw charges (Q) for 
	// a given chamber. c_num=[1,2] is the number of this chamber.
	// If the histogram is empty then no acquisition parameter
	// was fired.
	
	Int_t i = c_num-1;
	if( (i<0) || (i>1) ) return NULL; 
	if( !fQ[0][i] ) return NULL;
	if(fQ[0][i]->GetEntries()) return fQ[0][i];
	
	fQ[0][i]->Reset();
	TIter next(GetACQParamList());

	KVACQParam *par  = NULL;
	while((par = (KVACQParam *)next())){

 	   	if( (par->GetLabel()[1] != Char_t('1'+i) ) || !par->Fired("P") ) continue;	

		fQ[0][i]->SetBinContent( par->GetNumber(), (Double_t)par->GetCoderData() );
	}

 	return fQ[0][i];
}
//________________________________________________________________
   
TH1F *KVDriftChamber::GetQHisto( Int_t c_num ){
	// Retruns the position histogram with calibrated charges (Q)
	// for a given chamber. c_num=[1,2] is the number of this chamber.

	Int_t i = c_num-1;
	if( (i<0) || (i>1) ) return NULL; 
	if( !fQ[1][i] ) return NULL;
	if(fQ[1][i]->GetEntries()) return fQ[1][i];

	fQ[1][i]->Reset();
	TIter next(GetListOfCalibrators());

	KVCalibrator *cal  = NULL;
	KVACQParam   *par  = NULL;
	Int_t num;

	Int_t NcalOK = 0;
	while((cal = (KVCalibrator *)next())){
		

 	   	if( !cal->GetStatus() || (GetPositionTypeIdxFromID( cal->GetUniqueID() ) != i) ) continue;	
		NcalOK++;

		KVFunctionCal *calf = (KVFunctionCal *)cal;
		par = calf->GetACQParam();
		if( !par->Fired("P") ) continue;
		
		num =  calf->GetNumber();
		fQ[1][i]->SetBinContent( num, calf->Compute( (Double_t)calf->GetACQParam()->GetCoderData() ) );
	}

 	if( NcalOK < 4 ) Warning("GetQHisto","Impossible to calibrate correctly the charges for X position in the chamber %d of %s, %d calibrators are OK",c_num, GetName(), NcalOK);
	
 	return fQ[1][i];
}
//________________________________________________________________

TH1F *KVDriftChamber::GetCleanQHisto( Int_t c_num ){
	// Retruns the X position histogram with calibrated charges (Q)
	// after the substration of noise for a given chamber. c_num=[1,2] is the number of this chamber.

	Int_t i = c_num-1;
	if( (i<0) || (i>1) ) return NULL; 
	if( !fQ[2][i] ) return NULL;
	if(fQ[2][i]->GetEntries()) return fQ[2][i];

	TH1F *hQcal = GetQHisto( c_num );
	if( !hQcal ) return NULL;

	Float_t threshold = CalculateQThreshold( c_num );

	for(Int_t j=1; j<=hQcal->GetNbinsX(); j++){	//loop over strips
		Double_t QTmp = hQcal->GetBinContent(j);
		if(QTmp > threshold){
			fQ[2][i]->SetBinContent(j,QTmp-threshold);
		}
	}

	return fQ[2][i];
}
//________________________________________________________________
   
Float_t KVDriftChamber::CalculateQThreshold( Int_t c_num ){
	// Calculate the noise in order to remove it on strips for X  
	// position of a given chamber. c_num=[1,2] is the number of this
	// chamber.

	//Study the noise and the maximum value for the charge.
	//At this stage the quantities noise_*** contain also the signal!
	Float_t QTmp, noise_mean, noise_var, noise_stdev, noise_sum;
	Float_t Q_max; 
	Q_max = noise_mean = noise_var = noise_sum = noise_stdev = 0.;

	UShort_t Q_max_Nr;
	Q_max_Nr = 1000 ;	

	TH1F *hQcal = GetQHisto( c_num );
	if( !hQcal ) return 0;

	for(Int_t j=1; j<=hQcal->GetNbinsX(); j++){	//loop over strips
		QTmp = hQcal->GetBinContent(j);
		if(QTmp >0){
			noise_mean += QTmp;
			noise_var  += QTmp*QTmp;
			noise_sum  += 1.;
		}
		if(QTmp > Q_max){
			Q_max    = QTmp;
			Q_max_Nr = j;
		}
	}

	//Study the surrounding of the maximum charge
	Float_t max_mean, max_var, max_sum;
	max_mean = max_var = max_sum = 0.;
	for(Int_t j=-Border;j<=Border;j++){
		Int_t k = Q_max_Nr+j; 
		if( (0<k) && (k<=hQcal->GetNbinsX()) ){
			QTmp = hQcal->GetBinContent(k);
			if (QTmp>0){
				max_mean  += QTmp;
				max_var   += QTmp*QTmp;
				max_sum   += 1.;	
			}
		}
	}
 	//The signal of the maximum charge is removed from the quantities noise_***
	noise_mean -= max_mean;
	noise_var  -= max_var;
	noise_sum  -= max_sum;
 	//Calculate statistical quntities related to the noise and fix the threshold
 	//as the mean of the noise + 1 time its standart deviation
	if(noise_sum<2. || noise_var==0) 
	{
		noise_mean = noise_var = 0;
	}
	else 
	{
		noise_mean /= noise_sum;
		noise_var   = noise_var/noise_sum - noise_mean*noise_mean;
		noise_stdev = TMath::Sqrt(noise_var);
	}
	Float_t threshold = noise_mean + noise_stdev;

	return threshold;
}
//________________________________________________________________

void KVDriftChamber::ShowCleanQHisto(Int_t c_num, Option_t *opt){
	TH1F *hh = GetCleanQHisto( c_num );
	if( hh ) hh->Draw(opt);
}
//________________________________________________________________

void KVDriftChamber::ShowQrawHisto(Int_t c_num, Option_t *opt){
	TH1F *hh = GetQrawHisto( c_num );
	if( hh ) hh->Draw(opt);
}
//________________________________________________________________

void KVDriftChamber::ShowQHisto(Int_t c_num, Option_t *opt){
	TH1F *hh = GetQHisto( c_num );
	if( hh ) hh->Draw(opt);
}
//________________________________________________________________

Double_t KVDriftChamber::GetRawPosition(const Char_t dir){
	// Returns the position (strip for X and electron-drift-time canal for Y) deduced from the histogram representing
	// the calibrated charge versus strip number for X direction or returns the electron drift time for Y position.
	Int_t idx = IDX(dir);
	if( fRawPos[ idx ] > -500 ) return fRawPos[ idx ];
	fRawPos [ idx ] = -1;
	fERawPos[ idx ] = -1;

	if( idx == 0 ){   // for X direction

		fRawPos [ idx ] = 0.;
		fERawPos[ idx ] = 0.;
		Double_t sum    = 0.;

		for( Int_t c=1; c<3; c++ ){ //loop over both chambers
			
			TH1F *hh = GetCleanQHisto( c );
			if( !hh ) continue;

			if(hh->GetEntries()< fNstripsOK ) continue;

			///////////////////////////////////////////////////

			Int_t binMax = hh->GetMaximumBin();
			Int_t min;
			for( min = binMax-1; min>1; min--){
				if( hh->GetBinContent(min) <= 0. ){
					min++;
					break;
				}
				Double_t deltaQ = hh->GetBinContent(min)-hh->GetBinContent(min-1);
				if(deltaQ < 0 ) break;
			}
			Int_t max;
			for( max = binMax+1; max<hh->GetNbinsX(); max++){
				if( hh->GetBinContent(max) <= 0. ){
					max--;
					break;
				}
				Double_t deltaQ = hh->GetBinContent(max)-hh->GetBinContent(max+1);
				if(deltaQ < 0 ) break;
			}

			if( (max-min+1) < fNstripsOK) continue;

			hh->GetXaxis()->SetRange(min,max);

			Double_t intgl   = hh->Integral();
			sum             += intgl;
			fRawPos [ idx ] += intgl*hh->GetMean();
			fERawPos[ idx ] += intgl*hh->GetRMS()*1.2;

			hh->GetXaxis()->SetRange();
		}

		if( sum ){
			fRawPos [ idx ] /= sum;
			fERawPos[ idx ] /= sum;
		}
		else{
			fRawPos [ idx ] = -1;
			fERawPos[ idx ] = -1;
		}
	}
	else if( idx==1 ){ // for Y direction returns the acq parameter
		// of TFIL

		fRawPos [ idx ] =  ( fTfilPar ? fTfilPar->GetData() : fRawPos[idx] );
		fERawPos[ idx ] = 0.;
	}

	return fRawPos[ idx ];
}
//________________________________________________________________

Double_t KVDriftChamber::GetRawPositionError(const Char_t dir){
	// Returns the error on the position (strip for X and channel for Y) returned by GetRawPosition( dir ).

	Int_t idx = IDX(dir);
	if( fERawPos[ idx ] > -500 ) return fERawPos[ idx ];

	GetRawPosition( dir ); 
	return fERawPos[ idx ];
}

