/*
$Id: KVElasticScatterEvent.cpp,v 1.5 2009/01/14 15:35:50 ebonnet Exp $
$Revision: 1.5 $
$Date: 2009/01/14 15:35:50 $
*/

//Created by KVClassFactory on Thu Dec 11 14:45:29 2008
//Author: eric bonnet,,,

#include "KVElasticScatterEvent.h"
#include "KVPosition.h"
#include "KVMultiDetArray.h"
#include "KVDetector.h"
#include "KVTelescope.h"
#include "TH2F.h"
#include "TH1F.h"

ClassImp(KVElasticScatterEvent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVElasticScatterEvent</h2>
<h4>simulate ElasticScatterEvent and answer of a given (multi-)detector</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------
KVElasticScatterEvent::KVElasticScatterEvent()
{
   // Default constructor
	kevent_list = new KVList();
	kIPPVector.SetXYZ(0,0,0);
	kcurrent_nuc=0;
	kcurrent_evt=0;
	kchoix_layer=-1;
	kXruth_evt=0;
	kTreatedNevts=0;

	th_min=0;
	th_max=180;
	phi_min=0;
	phi_max=360;

	lhisto_control = 0;
}

//--------------------------------------------------
KVElasticScatterEvent::~KVElasticScatterEvent()
{
   // Destructor
	delete kevent_list;
	if (lhisto_control) delete lhisto_control; lhisto_control=0;
	kcurrent_nuc=0;
	kcurrent_evt=0;
}

//--------------------------------------------------
void KVElasticScatterEvent::AddHistoTree(){
	//Routine where you can create histos or trees
	//if kvlist already exists, it is delete with all objects
	//and recreate

	if (lhisto_control) delete lhisto_control;

	lhisto_control = new KVList();

	lhisto_control->Add(new TH2F("theta_phi","theta_phi",180,0,180,360,0,360));
	Float_t thickness = GetTarget().GetThickness();
	lhisto_control->Add(new TH1F("target_layer_depth","target_layer_depth",TMath::Nint(thickness*110),0,thickness*1.1));
	Float_t totalE = GetNucleus("PROJ","START")->GetKE();
	lhisto_control->Add(new TH2F("evt_OUT","EkVSTheta",180,0,180,TMath::Nint(totalE*11),0,totalE*1.1));

}
//--------------------------------------------------
void KVElasticScatterEvent::ResetHistoTree(){
	//Reset objects in the list

	lhisto_control->Execute("Reset","");

}
//--------------------------------------------------
void KVElasticScatterEvent::DefineAngularRange(TObject* obj){
	//Define in which angular (polar and azimuthal) range
	//The projectile diffusion direction will be randomized
	//From the geometry of the given object obj
	//This method is defined for object derived from
	//	- KVPosition (ie KVTelescope KVRing KVGroup etc ...)
	// - KVDetector (in this case, the KVTelescope it belongs to is used)
	// - KVMultiDetArray
	Double_t tmin=-1,tmax=-1,pmin=-1,pmax=-1;
	if (obj->InheritsFrom("KVPosition")){
		KVPosition* pos_obj=(KVPosition* )obj;
		tmin = pos_obj->GetThetaMin();
		tmax = pos_obj->GetThetaMax();
		pmin = pos_obj->GetPhiMin();
		pmax = pos_obj->GetPhiMax();
	}
	else if (obj->InheritsFrom("KVDetector")){
		KVTelescope* pos_obj=((KVDetector* )obj)->GetTelescope();
		tmin = pos_obj->GetThetaMin();
		tmax = pos_obj->GetThetaMax();
		pmin = pos_obj->GetPhiMin();
		pmax = pos_obj->GetPhiMax();
	}
	else if (obj->InheritsFrom("KVMultiDetArray")){
		KVMultiDetArray* pos_obj=(KVMultiDetArray* )obj;
		KVSeqCollection* ll = pos_obj->GetGroups();
		KVGroup* gr=0;
		Double_t tmin2=180,tmax2=0;
		Double_t pmin2=360,pmax2=0;
		for (Int_t nl=0;nl<ll->GetEntries();nl+=1){
			gr = (KVGroup* )ll->At(nl);
			if (gr->GetThetaMin()<tmin2) 	tmin2 = gr->GetThetaMin();
			if (gr->GetThetaMax()>tmax2) 	tmax2 = gr->GetThetaMax();
			if (gr->GetPhiMin()<pmin2) 	pmin2 = gr->GetPhiMin();
			if (gr->GetPhiMax()>pmax2)		pmax2 = gr->GetPhiMax();
		}
		tmin = tmin2;
		tmax = tmax2;
		pmin = pmin2;
		pmax = pmax2;
	}
	else {
		printf("les objects de type %s ne sont pas implemente dans KVElasticScatterEvent::DefineAngularRange\n",obj->IsA()->GetName());
		return;
	}
	DefineAngularRange(tmin,tmax,pmin,pmax);

}

//--------------------------------------------------
Bool_t KVElasticScatterEvent::DefineTargetNucleusFromLayer(KVString layer_name) {
	//Define the nucleus target from the type
	//of the given layer which belongs to the predefined target
	//layer_name has to be the chimical symbol of the material
	//Available materials implemented in KaliVeda can be listed using
	//static method KVMaterial::GetListOfMaterials()->ls()

	if (GetTarget().GetLayers()->GetEntries()==0) return kFALSE;
	KVMaterial* mat=0;
	if (layer_name=="") {
		kchoix_layer=1;
		mat = GetTarget().GetLayerByIndex(kchoix_layer);
	}
	else {
		if ( !(mat = GetTarget().GetLayer(layer_name)) ){
			printf("le nom du layer %s ne correspond a aucun present dans le cible\n",layer_name.Data());
			printf("Attention le layer doit etre appele par son symbol (ie Calcium -> Ca)");
			ktarget.GetLayers()->Print();
			return kFALSE;
		}
		kchoix_layer=GetTarget().GetLayerIndex(layer_name);
	}

	KVNucleus* nuc = new KVNucleus(TMath::Nint(mat->GetZ()),TMath::Nint(mat->GetMass()));
	SetEntranceNucleus(nuc,"TARG");
	return kTRUE;

}

//--------------------------------------------------
void KVElasticScatterEvent::SetEntranceNucleus(KVNucleus *nuc,KVString type){
	//Define entrance nucleus
	// - for projectile type="PROJ"
	// - for target type="TARG"
	//This method allows us to define nucleus target with different type
	//than the target layer ones
	//For example if you want to simulate elastic scattering
	//of a projectile of Krypton on a Tantalium nucleus taking into account
	//the energy loss in Ca target

	type.ToUpper();
	if (type!="PROJ" && type!="TARG") return;

	if ( !(kcurrent_evt = (KVEvent* )kevent_list->FindObject("START")) ){
		kevent_list->Add(new KVEvent());
		kcurrent_evt = (KVEvent* )kevent_list->Last();
		kcurrent_evt->SetName("START");
	}
	if ( !(kcurrent_nuc = kcurrent_evt->GetParticle(type)) ){
		kcurrent_nuc = kcurrent_evt->AddParticle();
		nuc->Copy(*kcurrent_nuc);
		kcurrent_nuc->SetName(type);
	}
	else {
		kcurrent_nuc->Clear();
		nuc->Copy(*kcurrent_nuc);
		kcurrent_nuc->SetName(type);
	}

}

//--------------------------------------------------
void KVElasticScatterEvent::DuplicateEvent(KVString from,KVString to){
	//Create or copy event from the one called "from" to one called "to"
	//Every step of the diffusion is kept in memory
	// START	->initial configuration
	// IN		->after the entrance path in the target layer
	// DIFF	->after the diffusion
	// OUT	-> after the outgoing path in the target layer
	// DET	-> after the detection
	// the pointeur kcurrent_evt is set to the named "to" event

	if ( !(kcurrent_evt = (KVEvent* )kevent_list->FindObject(to)) ){
		kcurrent_evt = new KVEvent();
		((KVEvent* )kevent_list->FindObject(from))->Copy(*kcurrent_evt);
		kcurrent_evt->SetName(to);
		kevent_list->Add(kcurrent_evt);
	}
	else {
		kcurrent_nuc=0;
		while ((kcurrent_nuc = kcurrent_evt->GetNextParticle())){
			((KVEvent* )kevent_list->FindObject(from))->GetParticle(kcurrent_nuc->GetName())->Copy(*kcurrent_nuc);
		}
	}
}

//--------------------------------------------------
Bool_t KVElasticScatterEvent::ValidateEntrance(){
	//Check if there is one define projectile and target nuclei
	//If no target nucleus has been set via the SetEntranceNucleus() or SetTargNucleus() methods
	//A target nucleus correspond to the first layer of the target is choosen
	//calling the DefineTargetNucleusFromLayer() method

	if ( (kcurrent_evt = (KVEvent* )kevent_list->FindObject("START")) ){
		if ( !(kcurrent_evt->GetParticle("PROJ")) ){
			printf("Il n'y a pas de noyau projectile pour faire la diffusion elastique\n");
			return kFALSE;
		}
		if ( !(kcurrent_evt->GetParticle("TARG")) ){
			if ( !DefineTargetNucleusFromLayer() ){
				printf("Il n'y a pas de noyau cible pour faire la diffusion elastique\n");
				return kFALSE;
			}
		}
	}
	else {
		printf("il n y a pas d evts START\n");
		return kFALSE;
	}
	kcurrent_evt=0;

	Print();

	return kTRUE;

}

//--------------------------------------------------
void KVElasticScatterEvent::PropagateInTargetLayer(KVString opt){
	//Apply Energy loss calculation to the current nucleus
	//along its path in the target layer
	//The option IN or OUT
	//corresponds to the incoming (before diffusion)
	//ot to the outgoing (after diffusion) pathes
	if (opt=="IN"){
		ktarget.SetIncoming();
		ktarget.DetectParticle(kcurrent_nuc,0);
	}
	else if (opt=="OUT") {
		ktarget.SetOutgoing();
		ktarget.DetectParticle(kcurrent_nuc,0);
	}
	else {
		cout << "opt has to be IN or OUT" << opt << endl;
	}

}

//--------------------------------------------------
void KVElasticScatterEvent::NewInteractionPointInTargetLayer(){
	//Choose a new interaction point in the current target layer
	//This position can be read via the GetInteractionPointInTargetLayer()
	//method
	if (kchoix_layer!=-1){
		TVector3 dir = kcurrent_nuc->GetMomentum();
		ktarget.SetInteractionLayer(kchoix_layer,dir);
		kIPPVector = ktarget.GetInteractionPoint();
	}
	else {
		kIPPVector = ktarget.GetInteractionPoint(kcurrent_nuc);
	}
}

//--------------------------------------------------
void KVElasticScatterEvent::MakeDiffusion(Double_t theta,Double_t phi){
	//Determination a partir du theta choisi
	//de l'energie cinetique du projectile diffuse
	//All kinematics properties calculated in the KV2Body class
	//are accessible via the KV2Body& GetKinematics() method
	//
	// WARNING: in inverse kinematics, there are two projectile energies
	// for each lab angle. We only use the highest energy, corresponding
	// to the most forward CM angle.
	k2body.GetNucleus(3)->SetTheta(theta);
	k2body.GetNucleus(3)->SetPhi(phi);
	Int_t nsol;
	Double_t e1, e2;
	nsol = k2body.GetELab(3, theta, 3, e1, e2);
	k2body.GetNucleus(3)->SetKE( TMath::Max(e1, e2) );

	//Diffusion elastique
	//		p1+p2 = p3+p4
	//		Etot1+ Etot2 = Etot3+Etot4
	TVector3 ptot = k2body.GetNucleus(1)->Vect()+k2body.GetNucleus(2)->Vect();
	Double_t etot = k2body.GetNucleus(1)->E()+k2body.GetNucleus(2)->E();

	ptot -= k2body.GetNucleus(3)->Vect();
	etot -= k2body.GetNucleus(3)->E();

	k2body.GetNucleus(4)->SetPxPyPzE(ptot.X(),ptot.Y(),ptot.Z(),etot);
	kXruth_evt = k2body.GetXSecRuthLab(theta);

}

//--------------------------------------------------
void KVElasticScatterEvent::Filter(KVMultiDetArray* mdet){
	//Check if object mdet is defines
	//if not the default KVMultiDetArray is choosen
	//if is not defined to
	//this method do nothing

	if (!mdet) 	mdet=gMultiDetArray;
	if (!mdet)	return;

	// Reconstruction de l'evt a partir des pertes
	// d energies dans les differents etages de detection
	// Methode de KVReconstructedEvent

	mdet->Clear();

	recev.Clear();
	recev.SetPartSeedCond("any"); //indispensable pour que la reconstruction marche
											//seul les marqueurs de temps sont touches ...

	KVNucleus* nuc = 0;
	while( (nuc = kcurrent_evt->GetNextParticle()) ){
	   if(!mdet->DetectParticle(nuc)){

		}
	}
	mdet->GetDetectorEvent(&detevt);
   recev.ReconstructEvent(&detevt);

	CheckReconstrutedEventStatus();

}

//--------------------------------------------------
void KVElasticScatterEvent::CheckReconstrutedEventStatus(){
	// Algorythme assez basic pour faire le lien entre
	// les particules a la sortie de la cible et celles reconstruites
	// apres la detection en utilisant le champs fName
	//
	// Cet Algorythme "converge" si le compteur nOK est egale
	// a la multiplicite de l'evt reconstruit apres la detection
	// on met de cote d'office les possible pile-up
	// ou multi-hit dans un meme KVGroup de detection

	kcurrent_evt = ((KVEvent* )kevent_list->FindObject("OUT"));
	KVReconstructedNucleus* recnuc=0;
	Int_t nOK=0;
	//Boucle sur les particules reconstruites
	recev.ResetGetNextParticle();
	while ( (recnuc = recev.GetNextParticle()) ){

		kcurrent_nuc=0;
		kcurrent_evt->ResetGetNextParticle();
		//Boucle sur les particules a la sortie de la cible
		while ( (kcurrent_nuc=kcurrent_evt->GetNextParticle()) ){
			Bool_t OK=kTRUE;
			OK &= recnuc->GetGroup()->IsInPolarRange(kcurrent_nuc->GetTheta());
			OK &= recnuc->GetGroup()->IsInPhiRange(kcurrent_nuc->GetPhi());
			//Si l'angle solide du KVGroup considere inclu la direction initiale d une particule
			//On donne le meme nom a la particule reconstruite
			if (OK){
				recnuc->SetName(kcurrent_nuc->GetName());
				nOK+=1;
			}
		}
	}

	recev.ResetGetNextParticle();
	if (nOK!=recev.GetMult())
		printf("WARNING:KVElasticScatterEvent::CheckReconstrutedEventStatus(): miss something in reconstruction %d %d\n",recev.GetMult(),nOK);

}

//--------------------------------------------------
void KVElasticScatterEvent::Print(){

	printf("#####################\n");
	printf("## KVElasticScatterEvent::Print() ##\n");
	printf("# Diffusion elastique traitee :\n");
	printf("# %s+%s@%1.1lf MeV/A\n",
		GetNucleus("PROJ","START")->GetSymbol(),
		GetNucleus("TARG","START")->GetSymbol(),
		GetNucleus("PROJ","START")->GetKE()/GetNucleus("PROJ","START")->GetA()
	);
	printf("# Propagation dans une cible de:\n");
	for (Int_t nn=0;nn<GetTarget().GetLayers()->GetEntries();nn+=1){
		printf("# type:%s epaisseur:%1.2lf (mg/cm**2)\n",
			GetTarget().GetLayerByIndex(nn+1)->GetType(),
			GetTarget().GetLayerByIndex(nn+1)->GetAreaDensity()/(KVUnits::mg/pow(KVUnits::cm, 2.))
		);
	}
	printf("#####################\n");

}

//--------------------------------------------------
void KVElasticScatterEvent::Process(KVMultiDetArray* mdet){
	//Exemple de ce qui peut etre fait
	//A rederiver et customiser
	kTreatedNevts+=1;

	DuplicateEvent("START","IN");
	kcurrent_nuc=kcurrent_evt->GetParticle("PROJ");
	NewInteractionPointInTargetLayer();

	((TH1F* )lhisto_control->FindObject("target_layer_depth"))->Fill(GetInteractionPointInTargetLayer().Z());

	PropagateInTargetLayer("IN");
	k2body.SetProjectile(kcurrent_evt->GetParticle("PROJ"));
	k2body.SetTarget(kcurrent_evt->GetParticle("TARG"));
	//-------------------------

	DuplicateEvent("IN","DIFF");
	k2body.SetOutgoing(kcurrent_evt->GetParticle("PROJ"));
	k2body.CalculateKinematics();

	Double_t anglemax=k2body.GetMaxAngleLab(3);

	Double_t the=GetTheta("min");
	if (anglemax<the) return;

	Double_t phi=GetPhi("min");
	if (GetTheta("min")!=GetTheta("max"))
		the = gRandom->Uniform(GetTheta("min"),TMath::Min(GetTheta("max"),k2body.GetMaxAngleLab(3)));
	if (GetPhi("min")!=GetPhi("max"))
		phi = gRandom->Uniform(GetPhi("min"),GetPhi("max"));

	((TH2F* )lhisto_control->FindObject("theta_phi"))->Fill(the,phi);

	MakeDiffusion(the,phi);
	//-------------------------

	DuplicateEvent("DIFF","OUT");
	kcurrent_nuc=0;
	while ( (kcurrent_nuc = kcurrent_evt->GetNextParticle()) ){
		PropagateInTargetLayer("OUT");
		((TH2F* )lhisto_control->FindObject("evt_OUT"))->Fill(kcurrent_nuc->GetTheta(),kcurrent_nuc->GetKE());
	}

	//-------------------------

	DuplicateEvent("OUT","DET");
	Filter(mdet);
	//-------------------------

}
