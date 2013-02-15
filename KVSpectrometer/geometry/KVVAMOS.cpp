//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#include "KVVAMOS.h"
#include "TPluginManager.h"
#include "KVVAMOSDetector.h"
#include "TClass.h"
#include "KVDataSetManager.h"
#include "KVUpDater.h"
#include "KVFunctionCal.h"

using namespace std;

ClassImp(KVVAMOS)

	////////////////////////////////////////////////////////////////////////////////
	// BEGIN_HTML <!--
	/* -->
	   <h2>KVVAMOS</h2>
	   <h4>VAMOS: variable mode spectrometer at GANIL</h4>
	   <!-- */
	// --> END_HTML
	////////////////////////////////////////////////////////////////////////////////

KVVAMOS *gVamos;

KVVAMOS::KVVAMOS()
{
   	// Default constructor
   	init();
   	gVamos = this;
}
//________________________________________________________________

void KVVAMOS::init()
{
    //Basic initialisation called by constructor.
    //
    //Cleanups
    //The fDetectors list contains references to objects owned to VAMOS 
    //spectrometer, but which may be deleted by other objects. Then we use
    //the ROOT automatic garbage collection to make sure that any object
    //deleted eslsewhere is removed automatically from this list.

	fDetectors = new KVList;
	fDetectors->SetCleanup(kTRUE);

	fVACQParams   = NULL;
	fVCalibrators = NULL;

	Info("init","To be implemented");
}
//________________________________________________________________

KVVAMOS::KVVAMOS (const KVVAMOS& obj)  : KVDetector() //KVBase()
{
   	// Copy constructor
   	// This ctor is used to make a copy of an existing object (for example
   	// when a method returns an object), and it is always a good idea to
   	// implement it.
   	// If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   	obj.Copy(*this);
}
//________________________________________________________________

KVVAMOS::~KVVAMOS(){
   	// Destructor
   	
	// Clear list of acquisition parameters
   	if(fDetectors && fDetectors->TestBit(kNotDeleted)){
		fDetectors->Clear();
		delete fDetectors;
	}
	fDetectors = NULL;

	// Clear list of acquisition parameters belonging to VAMOS
	if(fVACQParams && fVACQParams->TestBit(kNotDeleted)){
		fVACQParams->Clear();
		delete fVACQParams;
	}
	fVACQParams = NULL;
	SafeDelete( fVCalibrators );

	if(gVamos == this) gVamos = NULL;
}
//________________________________________________________________

void KVVAMOS::BuildGeometry(){
// Construction of the detector geometry at the focal plan of VAMOS for the
// e494s experiment. All subsequent realistations derive from this
// class and make modifications to this basic structure.
// 
//  WARNING: here only the geometry with TGeoManager is built.
//  The assignment of detector for each volume will be done
//  in MakeListOfDetectors.
	
	Warning("BuildGeometry","To be changed (see documentation)");
}
//________________________________________________________________

void KVVAMOS::Initialize(){
	// Initialize data members of the VAMOS detectors and of VAMOS 
	// itself.
	fDetectors->R__FOR_EACH(KVVAMOSDetector,Initialize)();	
}
//________________________________________________________________

void KVVAMOS::AddACQParam(KVACQParam* par, Bool_t owner){
	// Add an acquisition parameter corresponding to a detector
	// at the focal plan of the spectrometer. The fACQParams and fVACQParams
	// lists are added to the list of cleanups (gROOT->GetListOfCleanups).
	// Each acqisition parameter has its kMustCleanup bit set.
	// Thus, if this acq. parameter is deleted, it is automatically
	// removed from the lists by ROOT.

	if(!par){
	Warning("AddACQParam","Null pointer passed as argument");
	return;
	}

	// Add ACQ param. in global list
	if(!fACQParams){
		fACQParams = new KVList;
		fACQParams->SetName(Form("List of ACQ param. for detectors of %s",GetName()));
		fACQParams->SetOwner(kFALSE);
		fACQParams->SetCleanup(kTRUE);
	}
	fACQParams->Add(par);

	// Add ACQ param. in list of VAMOS if it is owner
	if(!owner) return;
   	if(!fVACQParams){
		fVACQParams = new KVList;
		fVACQParams->SetName(Form("List of ACQ param. belonging to %s",GetName()));
		fVACQParams->SetCleanup(kTRUE);
	}
	par->SetDetector( this );
	fVACQParams->Add(par);
}
//________________________________________________________________

Bool_t KVVAMOS::AddCalibrator(KVCalibrator *cal, Bool_t owner){
	//Associate a calibration with  VAMOS. owner = true means that
	//the calibrator is associated to an ACQ Parameter belonging to VAMOS
	//( for example all the calibrators of times of flight).
	//owner = false means the calibrator is associate to an ACQ parameter
	//belonging to a detector at the focal plan (Energies, charges, ...). 
   //If the calibrator object has the same class and type
   //as an existing object in the list (see KVCalibrator::IsEqual),
   //it will not be added to the list
   //(avoids duplicate calibrators) and the method returns kFALSE.
   //
   if (!fCalibrators){
       fCalibrators = new KVList();
	   fCalibrators->SetOwner(kFALSE);
   }
   if(fCalibrators->FindObject(cal)) return kFALSE;
   fCalibrators->Add(cal);

   if( !owner ) return kTRUE;
   if (!fVCalibrators){
       fVCalibrators = new KVList();
   }
   fVCalibrators->Add(cal);

   return kTRUE;
}
//________________________________________________________________

void KVVAMOS::Build(){
	// Build the VAMOS spectrometer. 
	
	SetName("VAMOS");
	SetTitle("VAMOS spectrometer for the e494s experiment");
	BuildGeometry();
	MakeListOfDetectors();
	SetIDTelescopes();
	SetACQParams();
	SetCalibrators();
	Initialize();
}
//________________________________________________________________

void KVVAMOS::Clear(Option_t *opt ){
	// Call "Clear" method of each and every detector in VAMOS,
	// to reset energy loss and KVDetector::IsAnalysed() state
	// plus ACQ parameters set to zero
	
	KVDetector::Clear();
	fDetectors->R__FOR_EACH(KVDetector,Clear)();	
	
}
//________________________________________________________________

void KVVAMOS::Copy (TObject& obj) const
{
   	// This method copies the current state of 'this' object into 'obj'
   	// You should add here any member variables, for example:
   	//    (supposing a member variable KVVAMOS::fToto)
   	//    CastedObj.fToto = fToto;
   	// or
   	//    CastedObj.SetToto( GetToto() );

   	KVDetector::Copy(obj);
//   	KVBase::Copy(obj);
   	//KVVAMOS& CastedObj = (KVVAMOS&)obj;
}
//________________________________________________________________

void KVVAMOS::MakeListOfDetectors(){
	// Build detectors of VAMOS.
	// The detectors are defined and associated to their TGeoVolume's
	// which compose it. 
	
	TString envname = "KVSpectrometer.DetectorList";
	 //TString envname = Form("%s.DetectorList",ClassName());

	cout<<envname<<endl;	
	TString list = gDataSet->GetDataSetEnv(envname.Data());
	TObjArray *tok = list.Tokenize(" ");
	TIter nextdet(tok);
	TObject* obj = NULL;
	KVNumberList numlist;
	// Loop over each detector of the spectrometer
	while(  (obj = nextdet()) ){
		const Char_t* detname = obj->GetName();
		cout<<detname<<endl;
		TClass* detcl = TClass::GetClass(detname);
		if(!detcl){
 			cout<<Form("ERROR: class %s not found in the dictionary",detname)<<endl;
			continue;
		}

		// envname.Form("%s.%s.Number",ClassName(),detname);
		envname.Form("KVSpectrometer.%s.Number",detname);
		cout<<envname<<endl;	
		list = gDataSet->GetDataSetEnv(envname.Data());
		numlist.SetList(list.Data());
		numlist.Begin();
		// Loop over detectors with same type.
		// Different by their number
		while(!numlist.End()){
			Int_t num = numlist.Next();

			cout<<"Building "<<detcl->GetName()<<", number "<<num<<endl;

			// Making the detector
			KVSpectroDetector *det = (KVSpectroDetector*) detcl->New();
			det->SetNumber(num);
			det->SetName(det->GetArrayName());
			det->BuildFromFile();
			Warning("MakeListOfDetectors","The detector %s have to be assigned to TGeoVolume",det->GetName());
			fDetectors->Add(det);
		}
	}
	delete tok;
}
//________________________________________________________________

KVVAMOS *KVVAMOS::MakeVAMOS(const Char_t* name){
	// Static function which will create and 'Build' the VAMOS spectrometer
	// object corresponding to 'name'.
	// These are defined as 'Plugin' objects in the file $KVROOT/KVFiles/.kvrootrc :
    //
    //Plugin.KVVAMOS:    INDRA_e494s    KVVAMOS     KVVamos    "KVVAMOS()"
    //+Plugin.KVVAMOS:    INDRA_e503    KVVAMOS_e503     KVVamos_e503    "KVVAMOS_e503()"
    //
    //The 'name' ("INDRA_e464s" etc.) corresponds to the name of a dataset in $KVROOT/KVFiles/manip.list
    //The constructors/macros are always without arguments
    //
    //This name is stored in fDataSet

    //check and load plugin library
    TPluginHandler *ph;
    if (!(ph = LoadPlugin("KVSpectrometer", name)))
        return 0;

    //execute constructor/macro for multidetector - assumed without arguments
    KVVAMOS *vamos = (KVVAMOS *) ph->ExecPlugin(0);

    vamos->fDataSet = name;
    //call Build() method
    vamos->Build();
    return vamos;
}
//________________________________________________________________

void KVVAMOS::SetACQParams(){
	// Set up acquisition parameters in all detectors at the focal
	// plan + any acquisition parameters which are not directly associated
	// to a detector and we associate to the spectrometer.

	if(fACQParams)  fACQParams->Clear();
	if(fVACQParams) fVACQParams->Clear();

	SetArrayACQParams();

	TIter next(GetListOfDetectors());
	KVSpectroDetector *det;
	while((det = (KVSpectroDetector*)next())){
		KVSeqCollection *l= det->GetACQParamList();
		if(!l){
			//detector has no acq params
			//set up acq params in detector
			det->SetACQParams();
			l= det->GetACQParamList();
		}
		// loop over acq params and add them to fACQParams list,
		TIter next_par(l);
		KVACQParam *par  = NULL;
		TList lvpar, ldpar;
		while((par = (KVACQParam*)next_par())){
				AddACQParam(par);
				par->SetWorking(gDataSet->GetDataSetEnv(Form("KVACQParam.%s.Working", par->GetName()), kTRUE));
		}

		//Set bitmask
		det->SetFiredBitmask();
	}
}
//________________________________________________________________

void KVVAMOS::SetArrayACQParams(){
	// Add acquisition parameters which are not
	// associated to a detector. The list of ACQ parameters is 
	// defined in environment variables such as
	// [dataset name].KVSpectrometer.ACQParameterList: TSI_HF TSED1_SED2 ...
	// in the .kvrootrc file.

	TString envname = "KVSpectrometer.ACQParameterList";
	//TString envname = Form("%s.DetectorList",ClassName());

	cout<<envname<<endl;	
	TString list = gDataSet->GetDataSetEnv(envname.Data());
	TObjArray *tok = list.Tokenize(" ");
	TIter nextparam(tok);
	TObject* obj = NULL;
	Info("SetArrayACQParams","List of ACQ Parameters belonging to %s:",GetName());

	KVACQParam *par = NULL;
	Short_t num = 1;
	while(  (obj = nextparam()) ){
		TString param( obj->GetName() );
		cout<<param.Data()<<" ";
		// VAMOS is the owner of the acq param. (kTRUE)
		AddACQParam( par = new KVACQParam(param.Data()),kTRUE);
		if( param.BeginsWith("T") ) par->SetType("T");
		par->SetNumber( 7000 + num++);
		par->SetWorking(gDataSet->GetDataSetEnv(Form("KVACQParam.%s.Working", par->GetName()), kTRUE));
	}
	cout<<endl;
	delete tok;
}
//________________________________________________________________

void KVVAMOS::SetCalibrators(){
	// Note that this only initialises the calibrators objects associated
	// to each detector (defined in each detector class's SetCalibrators method) and to VAMOS.
	// It does not set the parameters of the calibrations: this is done
	// by SetParameters.

	TIter nextdet( fDetectors );
	KVDetector *det = NULL;
	while( (det = (KVDetector *)nextdet()) ){
		det->SetCalibrators();

		KVSeqCollection *l= det->GetListOfCalibrators();
		TIter nextcal( l );
		KVCalibrator *cal  = NULL;
		while( (cal = (KVCalibrator *)nextcal()) ){
			if(	!AddCalibrator(cal, kFALSE) )
				Error("SetCalibrators","Duplicated calibrator %s of detector %s in VAMOS's list",cal->GetTitle(),det->GetName());
		}
	}

	// For ACQ parameters belonging to VAMOS.
	// We set only calibrators for time of flight ACQ parameters,
	// we assume that their name begin with the character 'T'.
	// Their calibrator will have 'channel->ns' type.

	TString calibtype;
	KVACQParam    *par  = NULL;
	KVFunctionCal *c    = NULL;
	TF1           *func = NULL;

	TIter nextpar( fVACQParams );
	while((par = (KVACQParam *)nextpar())){
		if(strcmp(par->GetType(),"T")) continue;
		calibtype = "channel->ns";
		calibtype.Append(" ");
		calibtype.Append( par->GetName() );

		func = new TF1(par->GetName(),"pol1",0., 16384.);
		c = new KVFunctionCal(this, func);
		c->SetType( calibtype.Data() );
		c->SetNumber( par->GetNumber() );
		c->SetACQParam( par );
		c->SetStatus( kFALSE );
		if( !AddCalibrator(c,kTRUE) ) delete c;
	}
}
//________________________________________________________________

void KVVAMOS::SetIDTelescopes(){
	// Create all ID telescopes and stores them in fIDTelescopes.
	
	Warning("SetGIDTelescopes","To be implemented");
}
//________________________________________________________________

void KVVAMOS::SetParameters(UShort_t run){
	// Set identification and calibration parameters for run;
	// This can only be done if gDataSet has been set i.e. a
	// dataset has been chosen.

	KVDataSet *ds = gDataSet;
	if(!ds){
		if(!gDataSetManager) return;
		ds = gDataSetManager->GetDataSet(fDataSet.Data());
	}
	if(!ds) return;
	ds->cd();
	ds->GetUpDater()->SetParameters(run);
}
