//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#include "KVVAMOS.h"
#include "KVVAMOSDetector.h"
#include "KVVAMOSTransferMatrix.h"

#include "KVGroup.h"
#include "KVDataSetManager.h"
#include "KVUpDater.h"
#include "KVFunctionCal.h"
#include "KVIDGridManager.h"

#include "TSystemDirectory.h"
#include "TPluginManager.h"
#include "TClass.h"

#include "TGeoBBox.h"

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


KVString KVVAMOS::fACQParamTypes("0:E, 1:Q, 2:T_HF, 3:T, 9:NO_TYPE");
KVString KVVAMOS::fPositionTypes("0:X, 1:Y, 2:Z, 3:XY, 4:XZ, 5:YZ, 6:XYZ, 9:NO_TYPE");


KVVAMOS::KVVAMOS()
{
   	// Default constructor
   	init();
   	gVamos = this;
}
//________________________________________________________________

KVVAMOS::KVVAMOS (const KVVAMOS& obj)  : KVMultiDetArray()
{
   	// Copy constructor
   	// This ctor is used to make a copy of an existing object (for example
   	// when a method returns an object), and it is always a good idea to
   	// implement it.
   	// If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   	obj.Copy(*this);
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

	SetUniqueID( 0 );

	fFiredDets     = NULL;
	fVACQParams    = NULL;
	fCalibrators   = NULL;
	fVCalibrators  = NULL;
	fFPvolume      = NULL;
	fVAMOSvol      = NULL;
	fTransMatrix   = NULL;
	fRotation      = NULL;
	fFocalPos      = 0; 
	fAngle         = 0;
	fBrhoRef       = -1;
	fBeamHF        = -1;

	//initalise ID grid manager
    if (!gIDGridManager)
        new KVIDGridManager;


	Info("init","To be implemented");
}
//________________________________________________________________

KVVAMOS::~KVVAMOS(){
   	// Destructor
   	
	// Clear list of acquisition parameters belonging to VAMOS
	if(fVACQParams && fVACQParams->TestBit(kNotDeleted)){
		fVACQParams->Clear();
		delete fVACQParams;
	}
	fVACQParams = NULL;

	SafeDelete( fCalibrators  );
	SafeDelete( fVCalibrators );
	SafeDelete( fFiredDets    );
	SafeDelete( fTransMatrix  );

	if(gVamos == this) gVamos = NULL;
}
//________________________________________________________________

void KVVAMOS::BuildFocalPlaneGeometry(TEnv *infos){
	// Construction of the detector geometry at the focal plane of VAMOS for the.
	
	if( !fFPvolume ){
		//The FPvolume has the size of the the Focal plane detection chamber
		TGeoMedium *Vacuum = gGeoManager->GetMedium( "Vacuum" );

		if( !Vacuum ){
			TGeoMaterial*matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
   			matVacuum->SetTitle("Vacuum");
   			Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
		}

		Double_t d  = infos->GetValue("VAMOS.FOCALCHAMBER.DEPTH" , 207.5 );
		Double_t w  = infos->GetValue("VAMOS.FOCALCHAMBER.WIDTH" , d     );
		Double_t h  = infos->GetValue("VAMOS.FOCALCHAMBER.HEIGHT", d     );

		fFPvolume   = gGeoManager->MakeBox("FocalPlane", Vacuum,  w/2, h/2, d/2);
	}

	fDetectors.R__FOR_EACH(KVVAMOSDetector,BuildGeoVolume)(infos,fFPvolume);
}
//________________________________________________________________

Bool_t KVVAMOS::BuildGeoVolume(TEnv *infos){
	// Build all the other volumes wich are not associated to the focal
	// plane detectors. Informations for the construction are read in
	// the TEnv object 'infos' ( see BuildVAMOSGeometry() ).
	//
	// Example of volumes built here:
	//              -volume of the stripping foil.
	//              -vamos assembly volume
	//              -dipole and quadrupole
	//                  ....            

	Info("BuildGeoVolume","Method to be completed");

	TGeoVolume *top = gGeoManager->GetTopVolume();

	//HERE ADD TARGET IN THE TOP VOLUME ASSEMBLY ////////////////
	// For the moment target of vacuum :-)	
	TGeoMedium *Vacuum = gGeoManager->GetMedium( "Vacuum" );
	if( !Vacuum ){
		TGeoMaterial*matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
   		matVacuum->SetTitle("Vacuum");
   		Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
	}
	TGeoVolume *target = gGeoManager->MakeTube("DEF_TARGET", Vacuum, 0., 5.,0.01);
	top->AddNode( target, 1 );
	/////////////////////////////////////////



	// this volume assembly allow the rotation of VAMOS around the 
	// target
	fVAMOSvol = gGeoManager->MakeVolumeAssembly("VAMOS");
//	fVAMOSvol = gGeoManager->MakeBox("VAMOS", Vacuum,  200, 200, 500);//TO BE CHANGE

	if( fRotation ) fRotation->Clear();
	else{
   		fRotation =  new TGeoRotation( "VAMOSrotation" );
		fRotation->RotateY( GetAngle() ); 
	}
	top->AddNode( fVAMOSvol, 1, fRotation );

	TGeoMatrix *matrix = NULL;
	TGeoShape *shape   = NULL;

	// Add a volume for the stripping foil if it is present
	Double_t th    = infos->GetValue("VAMOS.STRIP.FOIL.THICK", 0.);
	Double_t adens = infos->GetValue("VAMOS.STRIP.FOIL.AREA.DENSITY", 0.);
	Double_t dis   = infos->GetValue("VAMOS.STRIP.FOIL.POS", 0.);
	if( (th || adens) && dis ){
		TString mat = infos->GetValue("VAMOS.STRIP.FOIL.MATERIAL", "C");
		Double_t w  = infos->GetValue("VAMOS.STRIP.FOIL.WIDTH", 30.);
		Double_t h  = infos->GetValue("VAMOS.STRIP.FOIL.HEIGHT", w );

		KVMaterial kvmat( mat.Data(), th );
		if( adens ) kvmat.SetAreaDensity( adens );

		shape  = new TGeoBBox( w/2, h/2, kvmat.GetThickness()/2 );
		TGeoMedium *med = kvmat.GetGeoMedium();
		mat.Form("%s_foil",med->GetName());
		TGeoVolume* vol =  new TGeoVolume(mat.Data(),shape,med);
		vol->SetLineColor(med->GetMaterial()->GetDefaultColor());
		mat += "_pos";
		matrix = new TGeoTranslation(mat.Data(), 0., 0., dis );
		fVAMOSvol->AddNode( vol, 1, matrix);
	}

	// place the focal plane detection chamber from target position.
   	fFocalPos =  infos->GetValue("VAMOS.FOCALPOS", 0.);
	// For the moment we place it juste after the strip foil
	matrix    = new TGeoTranslation("focal_pos", 0., 0., dis+th+10+((TGeoBBox *)fFPvolume->GetShape())->GetDZ() ); // TO BE CHANGED
   	fVAMOSvol->AddNode( fFPvolume, 1, matrix );
   	///////////////////////////////////////////////

	return kTRUE;
}
//________________________________________________________________

void KVVAMOS::BuildVAMOSGeometry(){

	// Construction of the geometry of VAMOS spectrometer.
	// The informations used for building the VAMOS geometry are read in
	// all the files with the .cao extension in $KVROOT/KVFiles/<DataSet>/VAMOSgeometry
	// directory.
	//
	// This method will create an instance of TGeoManager (any previous existing geometry gGeoManager
    // will be automatically deleted) and initialise it with the full geometry of VAMOS
    //Every detector at the focal plane will be represented in the resulting geometry.
    //
    // For information on using the ROOT geometry package, see TGeoManager and related classes,
    // as well as the chapter "The Geometry Package" in the ROOT Users' Guide.
    //
    // The half-lengths of the "world"/"top" volumei, into which all the detectors
    //  are placed, are equal to 500 cm. This should be big enough so that all detectors.
    // This "world" is a cube 1000cmx1000cmx1000cm (with sides going from -500cm to +500cm on each axis).

	TEnv infos;
	if( !LoadGeoInfosIn( &infos ) ){
		Error("BuildVAMOSGeometry","No information found to build VAMOS geometry");
		return;
	}


	if (gGeoManager){
		Warning("BuildVAMOSGeometry","The existing geometry gGeoManager (%s, %s) is going to be deleted!!!", gGeoManager->GetName(), gGeoManager->GetTitle());
 	   	delete gGeoManager;
	}

   	TGeoManager *geom = new TGeoManager(Form("GEO_%s", gDataSet->GetLabel()), Form("VAMOS geometry for dataset %s", gDataSet->GetName()));
   	TGeoMaterial*matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
   	matVacuum->SetTitle("Vacuum");
   	TGeoMedium*Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
   	TGeoVolume *top = geom->MakeBox("WORLD", Vacuum,  1000, 1000, 1000);
   	geom->SetTopVolume(top);


   	BuildFocalPlaneGeometry( &infos );
   	BuildGeoVolume( &infos);

    // The option "i" is important to give unique ID for each existing
   	// Node. This is done in InitGeometry()
   	gGeoManager->CloseGeometry("i");

}
//________________________________________________________________

void KVVAMOS::InitGeometry(){
	// Initialize the geometry of VAMOS and of the detectors  
	// placed at the focal plane.

   	TGeoNode   *node = NULL;
   	TGeoVolume *vol  = NULL;
   	TString     vname;

   	// we give the unique ID for each node to simplify navigation
   	// inside the geometry. We set the same ID for the volume
   	// having the same name of the node (we assume that each
   	// volume is associated to one node only)
   	for(Int_t i=0; i<gGeoManager->GetNNodes(); i++){
	   	gGeoManager->CdNode(i);
	   	node = gGeoManager->GetCurrentNode();
	   	node->SetUniqueID( gGeoManager->GetCurrentNodeId() );
	   	vname = node->GetName();
	   	vname.Remove( vname.Last('_') );
	   	vol = gGeoManager->GetVolume( vname.Data() );
	   	if( vol ) vol->SetUniqueID( gGeoManager->GetCurrentNodeId() );
   	}

	fFocalToTarget.SetName("focal_to_target");
	GeoModified();
	UpdateGeometry();

}
//________________________________________________________________

Int_t KVVAMOS::LoadGeoInfosIn(TEnv *infos){
	// Load in a TEnv object the informations concerning the geometry of VAMOS.
	// These informations  are read in all the files with the .cao extension in 
	// $KVROOT/KVFiles/<DataSet>/VAMOSgeometry directory.
	// Returns the number of files read.

	
	// Reading geometry iformations in .cao files
	const Char_t dirname[] = "VAMOSgeometry";
	TString path( GetKVFilesDir() );
	path += "/";
	if( gDataSet ){
		path += gDataSet->GetName();
		path += "/";
	}
	path += dirname;
	TSystemDirectory dir(dirname,path.Data());
	TList *lfiles = dir.GetListOfFiles();
	TIter nextfile( lfiles );
	TSystemFile *file = NULL;
	Int_t Nfiles = 0;
	while( (file = (TSystemFile *)nextfile()) ){
		path.Form("%s/%s",file->GetTitle(),file->GetName());
		if( !path.EndsWith(".cao") ) continue;
 		infos->ReadFile(path.Data(),kEnvAll); 
		Nfiles++;
		Info("LoadGeoInfosIn","Loading file %s",file->GetName());
	}
	infos->Print();
	delete lfiles;
	return Nfiles;
}
//________________________________________________________________

void KVVAMOS::MakeListOfDetectors(){
	// Build detectors of VAMOS.
	// The detectors are defined and associated to their TGeoVolume's
	// which compose it. 
	
	 TString envname = Form("%s.DetectorList",ClassName());

	KVString list = gDataSet->GetDataSetEnv(envname.Data());
	list.Begin(" ");
	KVNumberList numlist;
	Int_t det_class_num = 1;
	// Loop over each detector of the spectrometer
	while( !list.End() ){
		KVString detname = list.Next( kTRUE );
		TClass* detcl = TClass::GetClass(detname.Data());
		if(!detcl){
			Error("KVVAMOS","class %s not found in the dictionary",detname.Data());
			continue;
		}
		else if( !detcl->InheritsFrom("KVVAMOSDetector") ){
			Error("KVVAMOS","class %s does not inherits from KVVAMOSDetector",detname.Data());
		}

		envname.Form("%s.%s.Number",ClassName(),detname.Data());
		numlist.SetList( gDataSet->GetDataSetEnv(envname.Data()));
		numlist.Begin();
		// Loop over detectors with same type.
		// Different by their number
		while(!numlist.End()){
			Int_t num = numlist.Next();

			// Making the detector
			KVSpectroDetector *det = (KVSpectroDetector*) detcl->New();
			det->SetNumber(num);
			det->SetUniqueID( 10*num + (det_class_num) );
			det->SetName(det->GetArrayName());
			fDetectors.Add(det);
		}
		det_class_num++;
	}
}
//________________________________________________________________

Bool_t KVVAMOS::ReadDetectorGroupFile( ifstream &ifile ){
	//Reads the groups of detectors to build for VAMOS from file loaded
	//in ifile and stores them as a structure.
	//Before reading, all old groups are deleted. A number is set
	//to each new group.
	//
	//The comment lines begin with '#'.
	//
	//Each line correspond to a new group and contains le name of each
	//detector which compose it, separated by a space.
	
 	ClearStructures("GROUP");          // clear out (delete) old groups

	KVString sline, detname; 
	while (ifile.good()) {         //reading the file
		sline.ReadLine( ifile );

		if(sline.IsNull()) continue;
	  	// Skip comment line
	  	if(sline.BeginsWith("#")) continue;

		KVGroup           *group   = NULL;
		KVSpectroDetector *det     = NULL;
		KVSpectroDetector *lastDet = NULL;

		sline.Begin(" ");
   		while( !sline.End() ){

			detname = sline.Next(kTRUE);
			det     = (KVSpectroDetector *)GetDetector( detname.Data() );

			if( det ){

				if( !group ){
 					group = new KVGroup;
					group->SetNumber(++fGr);
				}

				group->Add( det );
 				det->GetNode()->SetName(det->GetName());

    			if(lastDet && det!=lastDet) {
        			lastDet->GetNode()->AddBehind(det);
        			det->GetNode()->AddInFront(lastDet);
    			}
    			lastDet = det;
			}
			else Error("ReadDetectorGroupFile","Detector %s not found",detname.Data());
   		}

		if( group ){
			// Sort the detector list of each group
			// from the farest one to the closest one.
			group->SortDetectors(kSortDescending);
			Add( group );
		}
	}
	return kTRUE;
}
//________________________________________________________________

void KVVAMOS::SetArrayACQParams(){
	// Add acquisition parameters which are not
	// associated to a detector. The list of ACQ parameters is 
	// defined in environment variables such as
	// [dataset name].KVVAMOS.ACQParameterList: TSI_HF TSED1_SED2 ...
	// in the .kvrootrc file.

	if(fVACQParams) fVACQParams->Clear();
	TString envname = Form("%s.ACQParameterList",ClassName());

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
		par = new KVACQParam(param.Data());
		par->SetNumber( num++);
		if( param.BeginsWith("T") ){
			if( param.EndsWith("HF") ){
				par->SetType("T_HF");
				par->SetLabel("HF");
 			}	
			else par->SetType("T");
		}
		par->SetWorking(gDataSet->GetDataSetEnv(Form("KVACQParam.%s.Working", par->GetName()), kTRUE));
		AddACQParam( par , kTRUE);
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

	TIter nextdet( GetDetectors() );
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
		if( (*par->GetType()) != 'T' ) continue;
		calibtype = "channel->ns";
		calibtype.Append(" ");
		calibtype.Append( par->GetName() );

		func = new TF1(par->GetName(),"pol1",0., 16384.);
		c = new KVFunctionCal(func);
		c->SetType( calibtype.Data() );
		c->SetLabel( par->GetLabel() );
		c->SetUniqueID( par->GetUniqueID() );
		c->SetACQParam( par );
		c->SetStatus( kFALSE );
		if( !AddCalibrator(c,kTRUE) ) delete c;
	}
}
//________________________________________________________________

void KVVAMOS::SetGroupsAndIDTelescopes(){
	//Build groups of detectors (KVGroup) from the file given by
	//the environment variable:
	//[dataset name]. KVVAMOS.DetectorGroupFile: ...
	//( See ReadDetectorGroupFile(...) method )
	//
	//In each group, the list of detectors is sorted in order to list 
	//them from the farest one to the closest one.
	//
    //Also creates all ID telescopes (DeltaE-E, Fast-Slow ) used for the Z-identification
    //in VAMOS and stores them in fIDTelescopes.
    //
    //Any previous groups/idtelescopes are deleted beforehand.
    //
   	//As any ID grids stored in gIDGridManager will have been associated to the
   	//old ID telescopes (whose addresses now become obsolete), make sure that
   	//you deleted  all grids associated to VAMOS before to call this method.
   	//You should therefore follow this with a call to SetIdentifications()
   	//in order to reinitialize all that.

	TString envname, filename;
	envname.Form("%s.DetectorGroupFile",ClassName());
	filename = gDataSet->GetDataSetEnv(envname.Data());
	ifstream ifile;
	if( !SearchAndOpenKVFile(filename.Data(), ifile, gDataSet->GetName())){
		Error("SetGroupsAndIDTelescopes","Could not open file %s",filename.Data());
		return;
	}

	Info("SetGroupsAndIDTelescopes","Reading file with the list of detector groups of VAMOS: %s",filename.Data());

	if( !ReadDetectorGroupFile(ifile) ) Error("SetGroupsAndIDTelescopes","Bad structure inside the file %s",filename.Data());	

	ifile.close();

	//now read list of groups and create list of ID telescopes
	CreateIDTelescopesInGroups();
}
//________________________________________________________________

void KVVAMOS::UpdateGeometry(){
	// Update the geometry of VAMOS.
	// This method has to be called when the geometry is modified and
	// before using of methods giving information about geometry.
	//
	// The focal-plane to target matrix is calculated again and set
	// to each detector.

	
	if( !IsGeoModified() || !IsBuilt() ) return;	
	
	Int_t prev_id = gGeoManager->GetCurrentNodeId();

	// To be sure that the matrices will be calculated again
	gGeoManager->CdTop();
	// Focal-plane to target matrix
	gGeoManager->CdNode( fFPvolume->GetUniqueID() );
	fFocalToTarget = *gGeoManager->GetCurrentMatrix();

	// Initialize the geometry of the detectors
	TIter next_det( GetDetectors() );
	KVVAMOSDetector *det = NULL;
	while( (det = (KVVAMOSDetector *)next_det()) ){
		det->SetFocalToTargetMatrix( &fFocalToTarget );
	}

	ResetBit( kGeoModified );

	// just to not create problems if this method is called during a tracking
	gGeoManager->CdNode( prev_id );
}
//________________________________________________________________

void KVVAMOS::AddACQParam(KVACQParam* par, Bool_t owner){
	// Add an acquisition parameter corresponding to a detector
	// at the focal plane of the spectrometer. The fACQParams and fVACQParams
	// lists are added to the list of cleanups (gROOT->GetListOfCleanups).
	// Each acqisition parameter has its kMustCleanup bit set.
	// Thus, if this acq. parameter is deleted, it is automatically
	// removed from the lists by ROOT.

	if(!par){
	Warning("AddACQParam","Null pointer passed as argument");
	return;
	}

	// Add ACQ param. in global list
	KVMultiDetArray::AddACQParam( par );

	// Add ACQ param. in list of VAMOS if it is owner
	if(owner){
   		if(!fVACQParams){
			fVACQParams = new KVList;
			fVACQParams->SetName(Form("List of ACQ param. belonging to %s",GetName()));
			fVACQParams->SetCleanup(kTRUE);
		}
//		par->SetDetector( this );
		fVACQParams->Add(par);
		par->SetUniqueID( CalculateUniqueID( par ) );
	}
	else par->SetUniqueID( CalculateUniqueID( par, (KVVAMOSDetector *)par->GetDetector() ) );
}
//________________________________________________________________

Bool_t KVVAMOS::AddCalibrator(KVCalibrator *cal, Bool_t owner){
	//Associate a calibration with  VAMOS. owner = true means that
	//the calibrator is associated to an ACQ Parameter belonging to VAMOS
	//( for example all the calibrators of times of flight).
	//owner = false means the calibrator is associate to an ACQ parameter
	//belonging to a detector at the focal plane (Energies, charges, ...). 
   //If the calibrator object has the same class and type
   //as an existing object in the list (see KVCalibrator::IsEqual),
   //it will not be added to the list
   //(avoids duplicate calibrators) and the method returns kFALSE.
   //
   if (!fCalibrators){
       fCalibrators = new KVHashList();
	   fCalibrators->SetOwner(kFALSE);
   }
   if(fCalibrators->FindObject(cal)) return kFALSE;
   fCalibrators->Add(cal);

   if( owner ){
   	   if (!fVCalibrators){
       	   fVCalibrators = new KVList();
   	   }
   	   fVCalibrators->Add(cal);
   }

   return kTRUE;
}
//________________________________________________________________

void KVVAMOS::Build(){
	// Build the VAMOS spectrometer: detectors, geometry, identification
	// telescopes, acquisition parameters and calibrators. 
	//
	// After geometry building, the detectors are sorted in the 
	// list fDetectors from the Z coordinate of their first active volume,
	// in the focal Plan reference frame: from the lowest Z to the highest Z.
	
	if( IsBuilt() ) return;

	SetName("VAMOS");
	SetTitle("VAMOS spectrometer");
	MakeListOfDetectors();
	BuildVAMOSGeometry();
    InitGeometry();
	fDetectors.Sort( kSortDescending );
	SetGroupsAndIDTelescopes();
	SetACQParams();
	SetCalibrators();
	Initialize();
	SetBit(kIsBuilt);
}
//________________________________________________________________

void KVVAMOS::Clear(Option_t *opt ){
	// Call "Clear" method of each and every detector in VAMOS,
	// to reset energy loss and KVDetector::IsAnalysed() state
	// plus ACQ parameters set to zero
	
//	KVDetector::Clear();
	fDetectors.R__FOR_EACH(KVDetector,Clear)();	
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

   	KVMultiDetArray::Copy(obj);
   	//KVVAMOS& CastedObj = (KVVAMOS&)obj;
}
//________________________________________________________________

void KVVAMOS::FocalToTarget(const Double_t *focal, Double_t *target){
	// Convert the point coordinates from focal plane reference to target reference system.
	GetFocalToTargetMatrix().LocalToMaster( focal, target );
}
//________________________________________________________________

void KVVAMOS::FocalToTargetVect(const Double_t *focal, Double_t *target){
	// Convert the vector coordinates from focal plane reference to target reference system.
	GetFocalToTargetMatrix().LocalToMasterVect( focal, target );
}
//________________________________________________________________

KVList *KVVAMOS::GetFiredDetectors(Option_t *opt){
	//Fills 'list' with the fired detectors. The option 'opt' is 
	//set to the method KVSpectroDetector::Fired( opt ) used to know
	//if a detector is fired. The list is not the owner of its content.
	//It is a member variable fFiredDets of the class and will be deleted
	//with this KVVAMOS object.

	if( fFiredDets ) fFiredDets->Clear();
	else fFiredDets = new KVList( kFALSE );

	TIter next( &fDetectors );
	KVSpectroDetector *det = NULL;
	while( (det = (KVSpectroDetector *)next()) ){
		if( det->Fired( opt ) ) fFiredDets->Add( det );
	}
	return fFiredDets;
}
//________________________________________________________________

KVVAMOSTransferMatrix *KVVAMOS::GetTransferMatrix(){
	//Returns the transformation matrix allowing to map the measured
	//coordinates at the focal plane back to the target. If no matrix
	//exists then a new matrix is built from coefficient files found
	//in the directory of the current dataset ( see the method
	//KVVAMOSTransferMatrix::ReadCoefInDataSet() ).
	
	if( fTransMatrix ) return fTransMatrix;
	return (fTransMatrix = new KVVAMOSTransferMatrix( kTRUE ));
}
//________________________________________________________________

void KVVAMOS::Initialize(){
	// Initialize data members of the VAMOS detectors and of VAMOS 
	// itself. This method has to be called each time you look at a
	// new event.
	fDetectors.R__FOR_EACH(KVVAMOSDetector,Initialize)();	
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
    if (!(ph = LoadPlugin("KVVAMOS", name)))
        return 0;

    //execute constructor/macro for multidetector - assumed without arguments
    KVVAMOS *vamos = (KVVAMOS *) ph->ExecPlugin(0);

    vamos->fDataSet = name;
    //call Build() method
    vamos->Build();
    return vamos;
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
//________________________________________________________________

void KVVAMOS::SetTransferMatrix( KVVAMOSTransferMatrix *mat ){
	//Set the transformation matrix allowing to map the measured
	//coordinates at the focal plane back to the target. If a matrix
	//already exists then it is deleted first to set the new one.

	if( !mat ) return;
	if( fTransMatrix ) SafeDelete( fTransMatrix );
	fTransMatrix = mat;
}
//________________________________________________________________

void KVVAMOS::TargetToFocal( const Double_t *target, Double_t *focal ){
	// Convert the point coordinates from  target reference to focal plane reference system.
	GetFocalToTargetMatrix().MasterToLocal( target, focal );
}

//________________________________________________________________

void KVVAMOS::TargetToFocalVect(const Double_t *target, Double_t *focal){
	// Convert the vector coordinates from  target reference to focal plane reference system.
	GetFocalToTargetMatrix().MasterToLocalVect( target, focal );
}
//________________________________________________________________

UInt_t KVVAMOS::CalculateUniqueID( KVBase *param, KVVAMOSDetector *det ){
	UInt_t uid = param->GetNumber();
	if( det ){
		uid += 1000   * det->GetACQParamTypeIdx( param->GetType() );
		uid += 10000  * det->GetPositionTypeIdx( param->GetLabel() );
		uid += 100000 * det->GetUniqueID();
	}
	else{
		uid += 1000   * GetACQParamTypeIdx( param->GetType() );
		uid += 10000  * GetPositionTypeIdx( param->GetLabel() );
	}
	return uid;
}
//________________________________________________________________

UChar_t KVVAMOS::GetACQParamTypeIdx( const Char_t *type, KVVAMOSDetector *det ){
	KVString *types;
	if( det ) types = &(det->GetACQParamTypes());
	else      types = &(GetACQParamTypes());

	Ssiz_t i = types->Index( Form(":%s,", type) ); 
	return (i<0 ? 9 : types->Data()[i-1] - '0' );
}
//________________________________________________________________

UChar_t KVVAMOS::GetPositionTypeIdx( const Char_t *type, KVVAMOSDetector *det ){
	KVString *types;
	if( det ) types = &(det->GetPositionTypes());
	else      types = &(GetPositionTypes());

	Ssiz_t i = types->Index( Form(":%s,", type) ); 
	return (i<0 ? 9 : types->Data()[i-1] - '0' );
}
