//Created by KVClassFactory on Thu Jul 19 20:50:06 2012
//Author: Guilain ADEMARD

#include "KVSpectroDetector.h"
#include "KVIonRangeTable.h"
#include "TGeoBBox.h"

ClassImp(KVSpectroDetector)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSpectroDetector</h2>
<h4>Base class for the description of detectors in spectrometer</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Int_t KVSpectroDetector::fNumVol = 1;

void KVSpectroDetector::init(){
   	//default initialisations
	SetActiveLayer(-1); // Necessary to remove some warning with GetPressure and GetTemperature.   
	fActiveVolumes    = NULL;
	fActiveVolToFocal = NULL;
	fFocalToTarget    = NULL;
	fNabsorbers    = 0;
	fTotThick      = 0;
}
//________________________________________________________________

KVSpectroDetector::KVSpectroDetector()
{
   // Default constructor
   //
   // A "mother" volume will be built to group all the absorber volumes.
   // gGeoManager must point to current instance of geometry manager.

   init();
   
   if(!gGeoManager) Warning("KVSpectroDetector","gGeoManager have to be built before to continue");
}
//________________________________________________________________

KVSpectroDetector::KVSpectroDetector(const Char_t* type) : KVDetector(type){
	// Create a new detector of a given material. 
	init();
	if(!gGeoManager) Warning("KVSpectroDetector","gGeoManager have to be built before to continue");
}
//________________________________________________________________

KVSpectroDetector::KVSpectroDetector(const KVSpectroDetector& obj)  : KVDetector(((KVDetector& )obj))
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object.
   init();
   ((KVSpectroDetector&)obj).Copy(*this);
}
//________________________________________________________________

KVSpectroDetector::~KVSpectroDetector()
{
   // Destructor
   SafeDelete(fActiveVolumes);
   SafeDelete(fActiveVolToFocal);
}
//________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVSpectroDetector::Copy (TObject & obj) const
#else
void KVSpectroDetector::Copy (TObject & obj)
#endif
{
   	// This method copies the current state of 'this' object into 'obj'


   	// You should add here any member variables, for example:
   	//    (supposing a member variable KVSpectroDetector::fToto)
   	//    CastedObj.fToto = fToto;
   	// or
   	//    CastedObj.SetToto( GetToto() );

//	KVDetector::Copy(obj);
//	TGeoVolume::Copy(obj);
   	((KVSpectroDetector&)obj).SetAbsGeoVolume(GetAbsGeoVolume());
}
//________________________________________________________________

void KVSpectroDetector::AddAbsorber(KVMaterial* mat){
   //  Obsolete method.
	Warning("AddAbsorber","Obsolete method");	
}
//________________________________________________________________

void KVSpectroDetector::AddAbsorber(const Char_t* material, TGeoShape* shape, TGeoMatrix* matrix, Bool_t active){
	// Add an absorber material defined  as a daughter volume TGeoVolume.
	// This volume is defined by its material and its shape.
	// It is positioned by supplying a geometrical transformation, with
	// respect to the local reference frame of the detector.
	// By default the volume is referenced as "inactive".
	// If the volume is not the first absorber or if the matrix is not null
	// the volume is put in an assembly of volumes TGeoVolumeAssembly.
	// 
	// Input: material - material of the absorber. The list of available
	//                   materials can be found with 
	//                   det->GetRangeTable()->GetListOfMaterials()->ls()
	//                   where det is a KVSpectroDetector or another
	//                   object inheriting from KVMaterial.
	//                   If material is empty, the name of the detector
	//                   is used.
	//        shape    - shape of the volume.
	//        matrix   - matrix of the geometrical transformation.
	//        active   - kTRUE if the absorber is active, kFALSE othewise.




	TString vol_name;	
	vol_name.Form("%s_%d_%s",GetName(),fNumVol,material);
	TGeoVolume* vol = GetGeoVolume(vol_name.Data(), material, shape);
	if(!vol){
 		Error("AddAbsorber","Impossible to build the volume");
		return;
	}
	fNumVol++;
	AddAbsorber(vol,matrix,active);
}
//________________________________________________________________

void KVSpectroDetector::AddAbsorber(TGeoVolume* vol, TGeoMatrix* matrix, Bool_t active){
	// Add an absorber material defined  as a daughter volume TGeoVolume
	// and positioned by supplying a geometrical transformation matrix, with
	// respect to the local reference frame of the detector.
	// By default the volume is referenced as "inactive".
	// If the volume is not the first absorber or if the matrix is not null
	// the volume is put in an assembly of volumes TGeoVolumeAssembly.


	if(active) SetActiveVolume(vol);


	TGeoVolume* prev_vol = GetAbsGeoVolume();

	if(!prev_vol && !matrix){	
		// If there is not any absorber and any matrix
		// the volume is considered as the volume of the detector 
		Info("AddAbsorber","no previous volume and no matrix");
		SetAbsGeoVolume(vol);
		return;
	}

	TGeoVolume* vol_as = NULL;

	// Case where we want to position with the matrix the absorber which
	// is the first one of the detector
	if(!prev_vol) vol_as = gGeoManager->MakeVolumeAssembly(Form("%s_%d",GetName(), fNumVol++));
	else if(!prev_vol->InheritsFrom("TGeoVolumeAssembly")){
		// build an assembly of volumes if the volume of the
		// detector is not and put the absorbers inside

		Info("AddAbsorber","First absorber is not a TGeoVolumeAssembly");
		vol_as = gGeoManager->MakeVolumeAssembly(Form("%s_%d",GetName(),fNumVol++));
		vol_as->AddNode(prev_vol,1);
	}
	else vol_as = prev_vol;

	// Add the volume in the assembly of volumes
		Info("AddAbsorber","Adding Node");
	vol_as->AddNode(vol,1,matrix);
	SetAbsGeoVolume(vol_as);
}
//________________________________________________________________

void KVSpectroDetector::AddToTelescope(KVTelescope * T, const int){
   //  Obsolete method.
	Warning("AddToTelescope","Obsolete method");	

}
//________________________________________________________________

Bool_t KVSpectroDetector::BuildGeoVolume(TEnv *infos, TGeoVolume *ref_vol){
	// Build the detector geometry (i.e. the volumes) from informations loaded
	// in a TEnv object.
	//
	// If the detector has already a volume, i.e. the detector is
	// already built then this method does nothing.
	// The volumes will be placed inside the ref_vol if it is given in input.

	if(GetAbsGeoVolume()){
		Error("BuildGeoFromFile","Volume already existing");
 		return kFALSE; 
 	}
	Info("BuildGeoFromFile","IN");

	const Char_t *infotype[] = {"NABS","WIDTH","HEIGHT","X","Y","Z"};
	Binary8_t errorbit;
	Int_t Nbit = 0;
	TString miss;

	Int_t Nabs = GetDetectorEnv(infotype[Nbit++],-1,infos);
	if(Nabs < 0 ) errorbit.SetBit(Nbit-1);

	Double_t width = GetDetectorEnv(infotype[Nbit++],-1.,infos);
	if(width < 0 ) errorbit.SetBit(Nbit-1);

	Double_t height = GetDetectorEnv(infotype[Nbit++],-1.,infos);
	if(height < 0 ) errorbit.SetBit(Nbit-1);
	
	Double_t pos[3]; // X, Y, Z of the detector origine in the reference volume
    // This positions have to be present if ref_vol is defined
	if( ref_vol ){
		for(Int_t i=0; i<3; i++){
		pos[i] = GetDetectorEnv(infotype[Nbit++],-666.,infos);
		if( pos[i] <= -666 ) errorbit.SetBit(Nbit-1);
		}
	}

	// check if main informations are not missing
	if(  errorbit.Value() ){
		for(Int_t i=0; i<Nbit; i++){
			if( !errorbit.TestBit(i) ) continue;
			miss += ( miss.IsNull() ? "" : "; " );
			miss += infotype[i];
		}
		Error("BuildGeoVolume","Missing geometry informations (%s) for detector %s", miss.Data(), GetName());
		return kFALSE;
	}


	KVNumberList activeabs( GetDetectorEnv("ACTIVEABS","",infos) );
	Double_t thick_tot = 0;
	Double_t thick[Nabs];
	TString  mat[Nabs], type;
	miss = "";
	// look at information concerning layers (thickness, material, ...)
	for(Int_t i=0; i<Nabs; i++){ 
		type.Form("ABS.%d.MATERIAL",i+1);
		mat[i] = GetDetectorEnv(type.Data(),"",infos);
		if(mat[i].IsNull() ){
			miss += ( miss.IsNull() ? "" : "; " );
			miss += type;
 		}

		type.Form("ABS.%d.THICK",i+1);
		thick[i] = GetDetectorEnv(type.Data(),-1.,infos);
		if(thick[i] < 0 ) {
			miss += ( miss.IsNull() ? "" : "; " );
			miss += type;
 		}

		thick_tot+= thick[i];
	}
	if( !miss.IsNull() ){
		Error("BuildGeoVolume","Missing geometry informations (%s) for absorbers of detector %s", miss.Data(), GetName());
		return kFALSE;
	}

	// build volumes
	TGeoShape *shape = NULL;
	TString tmp;
	for(Int_t i=0; i<Nabs; i++){

		fTotThick+=thick[i];

		// box shape of the absorber
		shape  = new TGeoBBox( width/2, height/2, thick[i]/2 );

		// build and position absorber in mother volume.
		// Reference is the center of absorber
		Double_t ztrans = (thick[i]-thick_tot)/2;
		for(Int_t j=0; j<Nabs-1;j++) ztrans+= (j<i)*thick[j];
		tmp.Form( "%s_mat%d_tr", GetName(), i+1 );
		TGeoTranslation* tr = ( ztrans ? new TGeoTranslation(tmp.Data(), 0.,0.,ztrans) : NULL );
		AddAbsorber(mat[i].Data(),shape,tr, activeabs.Contains( i+1 ));
	}

	// incline of the detector around X axis
	Double_t Xincline = GetDetectorEnv("INCLINE.X",0.,infos);
	if( Xincline ){
		TGeoVolume  *vol_as= gGeoManager->MakeVolumeAssembly(Form("%s_%d",GetName(),fNumVol++));
		tmp.Form( "%s_rot_x", GetName() );
		TGeoRotation *rot = new TGeoRotation( tmp.Data(), 0. , Xincline, 0. );
		vol_as->AddNode( GetAbsGeoVolume(), 1, rot );
		SetAbsGeoVolume( vol_as );
	}

	if( !ref_vol ) return kTRUE;

	Double_t ref_pos[3]; // Xref, Yref, Zref
	for(Int_t i=0; i<3; i++){
		type.Form("REF.%c",'X'+i);
		ref_pos[i] = GetDetectorEnv(type.Data(),0.,infos);
		pos[i] -= ref_pos[i];
	}

	// place the detector in the reference volume 'ref_vol'
	tmp.Form( "%s_tr", GetName() );
	TGeoTranslation* ref_tr = new TGeoTranslation( tmp.Data(), pos[0], pos[1], pos[2] );
	ref_vol->AddNode( GetAbsGeoVolume(), 1, ref_tr );

	return kTRUE;
}
//________________________________________________________________

void KVSpectroDetector::DetectParticle(KVNucleus *, TVector3 * norm){
	// To be implemented. See the same method in KVDetector
	Warning("DetectParticle","To be implemented");
}
//________________________________________________________________

KVList *KVSpectroDetector::GetFiredACQParamList(Option_t *opt){
	// Returns a list with all the fired acquisiton parameters of
	// this detector. The option is set to the method KVACQParam::Fired;
	//
	// *** WARNING *** : DELETE the list returned by this method after using it !!!
	TIter next( GetACQParamList() );
	KVACQParam *par = NULL;
	KVList *list = new KVList( kFALSE );
	while( (par = (KVACQParam *)next()) ){
		if( par->Fired( opt ) ) list->Add( par ); 
	}
	return list;
}
//________________________________________________________________

TGeoHMatrix &KVSpectroDetector::GetActiveVolToFocalMatrix(Int_t i ) const{ 
	// Returns the matrix which transforms coordinates form the reference
	// frame of the active volume 'i' to the reference frame of the focal
	// plan.
	static TGeoHMatrix mm;
	mm.Clear();
	if( fActiveVolToFocal && fActiveVolToFocal->At(i) )
		mm =  (*(TGeoHMatrix *)fActiveVolToFocal->At(i));
	return mm;
}
//________________________________________________________________

Double_t KVSpectroDetector::GetELostByParticle(KVNucleus *, TVector3 * norm){
	// To be implemented. See the same method in KVDetector
	Warning("GetELostByParticle","To be implemented");

	return 0;
}
//________________________________________________________________

TGeoMedium* KVSpectroDetector::GetGeoMedium(const Char_t* mat_name){
	// By default, return pointer to TGeoMedium corresponding to this KVMaterial.
	// If argument "mat_name" is given, a pointer to a medium is return for this material.
	// mat_name = "Vacuum" is a special case: if the "Vacuum" does not exist, we create it.
	//
	// Instance of geometry manager class TGeoManager must be created before calling this
	// method, otherwise 0x0 will be returned.
	// If the required TGeoMedium is not already available in the TGeoManager, we create
	// a new TGeoMedium corresponding to the material given in argument.

	if( !gGeoManager ) return NULL;


   	TString medName, matName;
	if( !strcmp(mat_name,"") ){
   		// for gaseous materials, the TGeoMedium/Material name is of the form
   		//      gasname_pressure
   		// e.g. C3F8_37.5 for C3F8 gas at 37.5 torr
   		// each gas with different pressure has to have a separate TGeoMaterial/Medium
   		matName = GetName();
   		if(IsGas()) medName.Form("%s_%f", matName.Data(), GetPressure());
   		else medName = GetName();
  	} 
	else{
		matName = mat_name;
		medName = mat_name;
 	}

	TGeoMedium* gmed = gGeoManager->GetMedium( medName);
	if( gmed ) return gmed;

	TGeoMaterial *gmat = gGeoManager->GetMaterial( medName);
	if( !gmat ){
 		if( !strcmp(matName.Data(), "Vacuum") ){
			// create material
			gmat = new TGeoMaterial("Vacuum",0,0,0 );
		}
		else{
			// create material
			gmat = GetRangeTable()->GetTGeoMaterial(matName.Data());
			if(!gmat){
				Error("GetGeoMedium","Material %s is nowhere to be found in %s"
						,matName.Data(),GetRangeTable()->GetName());
				return NULL;
			}
			if(IsGas())	gmat->SetPressure( GetPressure() );
			gmat->SetTemperature( GetTemperature() );
			gmat->SetTransparency(0);
      	}
	}

	// For the moment the names of material and medium do not
	// depend on the temperature of the material.
	gmat->SetName(medName);
    gmat->SetTitle(matName);

	// create medium
	TGeoMedium* lastmed = (TGeoMedium*)gGeoManager->GetListOfMedia()->Last();
	Int_t numed = (lastmed ? lastmed->GetId()+1 : 0); // static counter variable used to number media
	gmed = new TGeoMedium( medName, numed, gmat );
	numed+=1;

	return gmed;
}
//________________________________________________________________

TGeoVolume* KVSpectroDetector::GetGeoVolume(const Char_t* name,const Char_t* material, TGeoShape* shape){
	// Construct a TGeoVolume shape which can be used to represent
	// a detector in the current geometry managed by gGeoManager.
	// If the argument material is empty, the name of the detector is used.
	// Input: name - name given to the volume.
	//        material - material of the volume. The list of available
	//                   materials can be found with 
	//                   det->GetRangeTable()->GetListOfMaterials()->ls()
	//                   where det is a KVSpectroDetector or another
	//                   object inheriting from KVMaterial.
	//        shape - shape of the volume.

	TGeoMedium *med = GetGeoMedium(material);
	if(!med) return NULL;
	TGeoVolume* vol =  new TGeoVolume(name,shape,med);
	if(vol) vol->SetLineColor(med->GetMaterial()->GetDefaultColor());
	return vol;
}
//________________________________________________________________

TGeoVolume* KVSpectroDetector::GetGeoVolume(const Char_t* name, const Char_t* material, const Char_t* shape_name, const Char_t* params){
	// Construct a TGeoVolume shape which can be used to represent
	// a detector in the current geometry managed by gGeoManager.
	// If the argument material is empty, the name of the detector is used.
	// Input: name - name given to the volume.
	//        material - material of the volume. The list of available
	//                   materials can be found with 
	//                   det->GetRangeTable()->GetListOfMaterials()->ls()
	//                   where det is a KVSpectroDetector or another
	//                   object inheriting from KVMaterial.
	//        shape_name - name of the shape associated to the volum
	//                     (Box, Arb8, Cone, Sphere, ...),  given
	//                     by the short name of the shape used in
	//                     the methods XXX:
	//                     TGeoManger::MakeXXX(...)


	TGeoMedium *med = GetGeoMedium(material);
	if(!med) return NULL;
	TString method = Form("Make%s",shape_name);
	TString  parameters = Form("%p,%p,%s",name,med,params);

	Info("GetGeoVolume","Trying to run the command gGeoManager->%s(%s)",method.Data(),parameters.Data());

	 gGeoManager->Execute(method.Data(),parameters.Data());
	 TGeoVolume* vol = (TGeoVolume*)gGeoManager->GetListOfVolumes()->Last(); 
	if(vol) vol->SetLineColor(med->GetMaterial()->GetDefaultColor());
	return vol;
}
//________________________________________________________________

TGeoVolume* KVSpectroDetector::GetGeoVolume(){
	// returns the TGeoVolume built when a volume is added by
	// the method AddAbsorber;
	return GetAbsGeoVolume();
}
//________________________________________________________________

Int_t KVSpectroDetector::GetMult(Option_t *opt){
	// Returns the multiplicity of fired (value above the pedestal) 
	// acquisition parameters if opt = "" (default).
	// If opt = "root" returns the multiplicity of only fired acq. 
	// parameters with GetName() containing "root". For example if
	// you want the multiplicity of fired segments B of a child class
	// KVHarpeeIC call GetMult("ECHI_B").

	Int_t mult   = 0;

	TString str( opt );
	Bool_t withroot = !str.IsNull();

	TIter next( GetACQParamList() );
	KVACQParam *par = NULL;
	while( (par = (KVACQParam *)next()) ){
		if( withroot ){
			str = par->GetName();
			if( !str.Contains( opt ) ) continue;
		}
		if( par->Fired("P") ) mult++;
	}
	return mult;
}
//________________________________________________________________

Double_t KVSpectroDetector::GetParticleEIncFromERes(KVNucleus * , TVector3 * norm){
	// To be implemented. See the same method in KVDetector
	Warning("GetParticleEIncFromERes","To be implemented");

	return 0;
}
//________________________________________________________________

Double_t KVSpectroDetector::GetXf( Int_t idx ){
	// Return the X coordinate (in cm)  by calling the
	// methode GetPosition(Double_t *Xf, Int_t idx), which can be overrided 
	// in child classes; 
	// The function returns -666 in case of an invalid request.
	Double_t X[3];
	if( !GetPosition( X, idx ) ) return -666;
	return  X[0];
}
//________________________________________________________________

Double_t KVSpectroDetector::GetYf( Int_t idx ){
	// Return the Y coordinate (in cm)  by calling the
	// methode GetPosition(Double_t *Xf, Int_t idx), which can be overrided 
	// in child classes; 
	// The function returns -666 in case of an invalid request.
	Double_t X[3];
	if( !GetPosition( X, idx ) ) return -666;
	return  X[1];
}
//________________________________________________________________

Double_t KVSpectroDetector::GetZf( Int_t idx ){
	// Return the Y coordinate (in cm)  by calling the
	// methode GetPosition(Double_t *Xf, Int_t idx), which can be overrided 
	// in child classes; 
	// The function returns -666 in case of an invalid request.
	Double_t X[3];
	if( !GetPosition( X, idx ) ) return -666;
	return  X[2];
}
//________________________________________________________________

Bool_t KVSpectroDetector::GetPosition( Double_t *XYZf, Int_t idx ){
	// Return the coordinates (in cm) of a point randomly drawn in the 
	// active volume with index 'idx'. We assume that the shape of this
	// volume is a TGeoBBox. These coordinates are given in the reference frame of the focal plan. 
	// The function returns -666 in case of an invalid request

	TGeoVolume *vol = GetActiveVolume(idx);
	if( !vol ) return kFALSE;
   	const TGeoBBox *box = (TGeoBBox *)vol->GetShape();
   	Double_t dx = box->GetDX();
   	Double_t dy = box->GetDY();
   	Double_t dz = box->GetDZ();
   	Double_t ox = (box->GetOrigin())[0];
   	Double_t oy = (box->GetOrigin())[1];
   	Double_t oz = (box->GetOrigin())[2];
   	Double_t xyz[3];
  	xyz[0] = ox-dx+2*dx*gRandom->Rndm();
   	xyz[1] = oy-dy+2*dy*gRandom->Rndm();
   	xyz[2] = oz-dz+2*dz*gRandom->Rndm();
	return ActiveVolumeToFocal( xyz , XYZf, idx );
}
//________________________________________________________________

UInt_t KVSpectroDetector::GetTelescopeNumber() const{
	//  Obsolete method.
	Warning("GetTelescopeNumber","Obsolete method");	

	return GetNumber();
}
//________________________________________________________________

void KVSpectroDetector::GetVerticesInOwnFrame(TVector3* corners, Double_t depth, Double_t layer_thickness){
	//  Obsolete method.
	Warning("GetVerticesInOwnFrame","Obsolete method");	
}
//________________________________________________________________

void KVSpectroDetector::InitGeometry(){
	// Initialize the geometry of the detector once it is placed in the
	// spectrometer (called for example in KVVAMOS::InitGeometry()).

	if( !fActiveVolToFocal ) fActiveVolToFocal = new KVList();
	else fActiveVolToFocal->Clear();

	// Matrix for each active volum is created to transform coordinates
	// from the reference frame of the volum to the reference frame of the
	// focal plan
	TIter next_vol(fActiveVolumes);	
	TGeoVolume *vol = NULL;
	TGeoHMatrix *vol_to_tgt = NULL;
	TGeoHMatrix *vol_to_fcl = NULL;
	while( (vol = (TGeoVolume *)next_vol()) ){

		gGeoManager->CdNode( vol->GetUniqueID() );

		// matrix 'volume to target'
		vol_to_tgt = gGeoManager->GetCurrentMatrix();

		// matrix 'volume to focal plan'
		vol_to_fcl = new TGeoHMatrix;

		if( fFocalToTarget ) *vol_to_fcl = fFocalToTarget->Inverse()*(*vol_to_tgt);
		else                 *vol_to_fcl = *vol_to_tgt;

		vol_to_fcl->SetName( Form("%s_to_focal",vol->GetName()) );
		fActiveVolToFocal->Add( vol_to_fcl );
	}
}
//________________________________________________________________

void KVSpectroDetector::SetActiveVolume(TGeoVolume* vol){
	// Set the volume in the list of "active" volumes, i.e. detectors
	// in which a signal is measured.
	if(!fActiveVolumes) fActiveVolumes = new KVList( kFALSE );
	fActiveVolumes->Add(vol);
}
//________________________________________________________________

void KVSpectroDetector::SetMaterial(const Char_t * type){
	// Set the same material of all the active volumes.
	Warning("SetMaterial","To be implemented");
}
//________________________________________________________________

const Char_t *KVSpectroDetector::GetDetectorEnv(const Char_t * type, const Char_t* defval, TEnv *env) const
{
   //Will look for env->GetValue "name_of_detector.type" if defined or "type_of_detector.type" if not.
   //If neither resource is defined, return the "defval" default value (="" by default).
   //If env is null then it looks for in gEnv.

	if( !env ) env = gEnv;

   TString temp;
   temp.Form("%s.%s", GetName(), type);
   if( !env->Defined(temp.Data()) )  temp.Form("%s.%s", GetType(), type);
   return env->GetValue(temp.Data(), defval);
}
//________________________________________________________________

Double_t KVSpectroDetector::GetDetectorEnv(const Char_t * type, Double_t defval, TEnv *env) const
{
    //Will look for env->GetValue "name_of_detector.type" if defined or "type_of_detector.type" if not.
   //If neither resource is defined, return the "defval" default value (="" by default).
   //If env is null then it looks for in gEnv.

	if( !env ) env = gEnv;

   TString temp;
   temp.Form("%s.%s", GetName(), type);
   if( !env->Defined(temp.Data()) )  temp.Form("%s.%s", GetType(), type);
   return env->GetValue(temp.Data(), defval);

}
//________________________________________________________________

Int_t KVSpectroDetector::GetDetectorEnv(const Char_t * type, Int_t defval, TEnv *env) const
{
    //Will look for env->GetValue "name_of_detector.type" if defined or "type_of_detector.type" if not.
   //If neither resource is defined, return the "defval" default value (="" by default).
   //If env is null then it looks for in gEnv.

	if( !env ) env = gEnv;

   TString temp;
   temp.Form("%s.%s", GetName(), type);
   if( !env->Defined(temp.Data()) )  temp.Form("%s.%s", GetType(), type);
   return env->GetValue(temp.Data(), defval);

}
//________________________________________________________________

Bool_t KVSpectroDetector::GetDetectorEnv(const Char_t * type, Bool_t defval, TEnv *env) const
{
 //Will look for env->GetValue "name_of_detector.type" if defined or "type_of_detector.type" if not.
   //If neither resource is defined, return the "defval" default value (="" by default).
   //If env is null then it looks for in gEnv.

	if( !env ) env = gEnv;

   TString temp;
   temp.Form("%s.%s", GetName(), type);
   if( !env->Defined(temp.Data()) )  temp.Form("%s.%s", GetType(), type);
   return env->GetValue(temp.Data(), defval);

  }
//________________________________________________________________

Bool_t KVSpectroDetector::ActiveVolumeToFocal(const Double_t *volume, Double_t *focal, Int_t idx){
	// Convert the point coordinates from active volume (with index 'idx') reference to focal plan reference system.

	TGeoHMatrix *mat = NULL;
	if( !fActiveVolToFocal || !(mat=(TGeoHMatrix *)fActiveVolToFocal->At(idx)) ){
		Error("ActiveVolumeToFocal","Conversion impossible!");
		return kFALSE;
	}
	mat->LocalToMaster( volume, focal );
	return kTRUE;
}
//________________________________________________________________

Bool_t KVSpectroDetector::FocalToActiveVolume(const Double_t *focal,  Double_t *volume, Int_t idx){
	// Convert the point coordinates from focal plan reference to active volume (with index 'idx') reference system.

	TGeoHMatrix *mat = NULL;
	if( !fActiveVolToFocal || !(mat=(TGeoHMatrix *)fActiveVolToFocal->At(idx)) ){
		Error("FocalToActiveVolume","Conversion impossible!");
		return kFALSE;
	}
	mat->MasterToLocal( focal, volume );
	return kTRUE;
}
//________________________________________________________________

Bool_t KVSpectroDetector::ActiveVolumeToFocalVect(const Double_t *volume, Double_t *focal, Int_t idx){
	// Convert the vector coordinates from active volume (with index 'idx') reference to focal plan reference system.

	TGeoHMatrix *mat = NULL;
	if( !fActiveVolToFocal || !(mat=(TGeoHMatrix *)fActiveVolToFocal->At(idx)) ){
		Error("ActiveVolumeToFocalVect","Conversion impossible!");
		return kFALSE;
	}
	mat->LocalToMasterVect( volume, focal );
	return kTRUE;
}
//________________________________________________________________

Bool_t KVSpectroDetector::FocalToActiveVolumeVect(const Double_t *focal,  Double_t *volume, Int_t idx){
// Convert the vector coordinates from focal plan reference to the active volume (with index 'idx') reference system.

	TGeoHMatrix *mat = NULL;
	if( !fActiveVolToFocal || !(mat=(TGeoHMatrix *)fActiveVolToFocal->At(idx)) ){
		Error("FocalToActiveVolumeVect","Conversion impossible!");
		return kFALSE;
	}
	mat->MasterToLocalVect( focal, volume );
	return kTRUE;
}
