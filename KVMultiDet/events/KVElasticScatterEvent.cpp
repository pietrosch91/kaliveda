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
#include "KVGroup.h"
#include "KVDetector.h"
#include "KVTelescope.h"
#include "TH2F.h"
#include "TH1F.h"

ClassImp(KVElasticScatterEvent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVElasticScatterEvent</h2>
<h4>simulate ElasticScatterEvent and answer of a given (multi-)detector : A + B -> A + B</h4>
<!-- */
// --> END_HTML
/*
Define the entrance channel
	The object needs at least :
	- a projectile nucleus (SetProjNucleus() method)
	- a target nucleus (SetTargNucleus() method and/or SetTargetMaterial)
Simulate the elastic scatter 	
	- Use the Process(Int_t ntimes) method, it performs ntimes diffusion
	- The direction of diffused projectile defined with theta (polar angle) and phi (azimulthal angle) are randomized
	by default between theta 0->180 and phi 0->360
	- One can restrict these ranges using DefineDefineAngularRange() methods
	- the kinematics of the two body reaction is performed using the KV2Body class methods
Simulate Energy loss in target (OPTIONAL)
	- the target material has to be set using the SetTargetMaterial() method
	- the incoming projectile goes until the interaction point (energy loss calculation)
	- the outgoing projectile and target leave the targe (energy loss calculation)
Simulate Multi Detector answer (OPTIONAL)
	- the answer of the multidetector array of the 2 nuclei is then simulated Filter() method		
*/
////////////////////////////////////////////////////////////////////////////////

//_______________________________________________________________________
KVElasticScatterEvent::KVElasticScatterEvent()
{
   // Default constructor
	init();
	
}

//_______________________________________________________________________
KVElasticScatterEvent::~KVElasticScatterEvent()
{
   // Destructor
	delete proj;
	delete targ;
	if (kb2) delete kb2;
	
	if (sim_evt) delete sim_evt;
	if (rec_evt) delete rec_evt;
	
	if (gMultiDetArray) 
		gMultiDetArray->SetSimMode(kFALSE);

	ClearHistos();
	ClearTrees();
}

//_______________________________________________________________________
void KVElasticScatterEvent::init()
{

	kIPPVector.SetXYZ(0,0,0);

	kchoix_layer=-1;
	kXruth_evt=0;
	kTreatedNevts=0;

	th_min=0;
	th_max=180;
	phi_min=0;
	phi_max=360;

	lhisto = 0;
	ltree = 0;

	targ = new KVNucleus();
	proj = new KVNucleus();
	
	rec_evt = 0;
	sim_evt = 0;
	ktarget=0;
	kb2 = 0;
 
 	ResetBit( kProjIsSet );
	ResetBit( kTargIsSet );
	ResetBit( kHasTarget );
	ResetBit( kIsUpdated );
	
	SetDiffNucleus("PROJ");
	SetRandomOption("isotropic");
	
}

//_______________________________________________________________________
void KVElasticScatterEvent::Reset()
{
	//Set contents/entries to zero
	kTreatedNevts=0;
	ResetHistos();
	ResetTrees();

}

//_______________________________________________________________________
void KVElasticScatterEvent::SetSystem(KVDBSystem* sys)
{
	//Define the entrance channel using KVDBSystem object
	//Get projectile and target via KVDBSystem::GetKinematics()
	//Get target material using KVDBSystem::GetTarget()
	if (sys->GetKinematics()){
		SetProjNucleus(sys->GetKinematics()->GetNucleus(1));
		SetTargNucleus(sys->GetKinematics()->GetNucleus(2));
	}
	else {
		Error("SetSystem","KVDBSystem pointer is not valid");
		return;
	}
	SetTargetMaterial(sys->GetTarget());
}

//_______________________________________________________________________
void KVElasticScatterEvent::SetSystem(Int_t zp,Int_t ap,Double_t ekin,Int_t zt,Int_t at)
{
	//Define the entrance channel 
	//zp, ap, ekin, atomic number, mass number and kinetic energy (MeV) of the projectile
	//zt, at, atomic number, mass number of the target
	KVNucleus nn(zp,ap,ekin);
	SetProjNucleus(&nn);
	nn.SetZAandE(zt,at,0.0);
	SetTargNucleus(&nn);

}

//_______________________________________________________________________
void KVElasticScatterEvent::SetTargNucleus(KVNucleus *nuc)
{ 
	//Define new target nucleus
	//Call ValidateEntrance method when change on the target, projectile
	//has been performed
	if (!nuc) return;
	nuc->Copy(*targ);
	targ->SetName("TARG");
	SetBit(kTargIsSet);
	ResetBit(kIsUpdated);

}

//_______________________________________________________________________
void KVElasticScatterEvent::SetDiffNucleus(KVString name)
{ 
	//Defini le noyau qui est concerné par le domaine angulaire pour le tirage aleatoire
	//name="PROJ" (default), on etudie la diffusion du projectile
	//name="TARG" on etudie la diffusion de la cible
	
	if (name=="TARG"){ kDiffNuc=4; }
	else {
		kDiffNuc=3;
		if (name!="PROJ") { 
			Warning("SetDiffNucleus","%s Le nom en argument doit etre PROJ ou TARG, par defaut on choisit le projectile",name.Data());
		}
	}
}

//_______________________________________________________________________
void KVElasticScatterEvent::SetRandomOption(Option_t* opt)
{
	//Defini le mode de tirage aleatoire pour l'angle polaire
	//opt="isotropic" ou "" (defaut) ou "random"
	//voir KVPosition::GetRandomDirection()
	kRandomOpt = opt;
	
}

//_______________________________________________________________________
Bool_t KVElasticScatterEvent::IsIsotropic(){

	return strcmp(kRandomOpt,"random");

}

//_______________________________________________________________________
void KVElasticScatterEvent::SetProjNucleus(KVNucleus *nuc)
{
	//Define new projectile nucleus
	//Call ValidateEntrance method when change on the target, projectile
	//has been performed
	if (!nuc) return;
	nuc->Copy(*proj);
	proj->SetName("PROJ");
	SetBit(kProjIsSet);
	ResetBit(kIsUpdated);

}

//_______________________________________________________________________
KVNucleus* KVElasticScatterEvent::GetNucleus(const Char_t* name) const
{
	//return the current projectile ("PROJ") or the target ("TARG") nucleus
	KVString sname(name);
	if (sname=="PROJ")		return proj;
	else if (sname=="TARG") return targ;
	else return 0;

}

//_______________________________________________________________________
KVNucleus* KVElasticScatterEvent::GetNucleus(Int_t ii) const
{
	//return the current projectile (ii=1) or the target (ii==2) nucleus
	if (ii==1) 			return proj;
	else if (ii==2) 	return targ;
	else return 0;

}

//_______________________________________________________________________
void KVElasticScatterEvent::SetTargetMaterial(KVTarget *targ,Bool_t IsRandomized)
{
	//Define a new target material where the nuclei will be propagated
	// if IsRandomized=kTRUE, the interaction point are randomly determined
	if (!targ) return;
	if (ktarget) delete ktarget;
	
	ktarget = new KVTarget(*targ);
	ktarget->SetRandomized(IsRandomized);
	SetBit(kHasTarget);
	ResetBit(kIsUpdated);
}

//_______________________________________________________________________
KVTarget* KVElasticScatterEvent::GetTarget() const
{ 
	//return the current target material 
	return ktarget;
	
}
	
//_______________________________________________________________________
void KVElasticScatterEvent::GenereKV2Body()
{
	//Protected Method called by ValidateEntrance() method
	//Genere the KV2Body object which manage the 2 body kinematics
	//for the elastik scatter
	//
	//Store the original momentum of the projectile nuclei
	//
	//
	//Define the KVSimEvent and KVReconstructedEvent pointer
	//where are stored the projectile/target nuclei couple after diffusion / detection
	//StartEvents() methods
	//
	//Make a copy of projectile and target nuclei for the KVSimEvent
	//
	if (kb2) delete kb2;
	kb2 = new KV2Body(new KVNucleus(*proj),new KVNucleus(*targ));
	kb2->CalculateKinematics();
	
	GetNucleus("PROJ")->SetE0();

	//Creer ou clear les deux pointeurs associes aux evts simules et reconstruits
	StartEvents();
	
	GetNucleus("PROJ")->Copy(*(KVSimNucleus* )sim_evt->AddParticle());
	GetNucleus("TARG")->Copy(*(KVSimNucleus* )sim_evt->AddParticle());
}

//_______________________________________________________________________
void KVElasticScatterEvent::StartEvents()
{
	//Define the KVSimEvent and KVReconstructedEvent pointer
	//where are stored the projectile/target nuclei couple after diffusion / detection
	if (!sim_evt) 	sim_evt = new KVSimEvent();
	else 				sim_evt->Clear();

	if (!rec_evt)	rec_evt = new KVReconstructedEvent();
	else 				rec_evt->Clear();

}

//_______________________________________________________________________
Bool_t KVElasticScatterEvent::DefineTargetNucleusFromLayer(KVString layer_name) {
	
	//Define the nucleus target from the type
	//of the given layer which belongs to the predefined target
	//layer_name has to be the chimical symbol of the material
	
	if ( !IsTargMatSet() )
		return kFALSE;
	
	if (GetTarget()->GetLayers()->GetEntries()==0) return kFALSE;
	KVMaterial* mat=0;
	if (layer_name=="") {
		kchoix_layer=1;
		mat = GetTarget()->GetLayerByIndex(kchoix_layer);
	}
	else {
		if ( !(mat = GetTarget()->GetLayer(layer_name)) ){
			printf("le nom du layer %s ne correspond a aucun present dans le cible\n",layer_name.Data());
			printf("Attention le layer doit etre appele par son symbol (ie Calcium -> Ca)");
			ktarget->GetLayers()->Print();
			return kFALSE;
		}
		kchoix_layer=GetTarget()->GetLayerIndex(layer_name);
	}
	
	KVNucleus* nuc = new KVNucleus();
	nuc->SetZandA(TMath::Nint(mat->GetZ()),TMath::Nint(mat->GetMass()));
	
	SetTargNucleus(nuc);
	
	return kTRUE;

}

//_______________________________________________________________________
Bool_t KVElasticScatterEvent::ValidateEntrance()
{
	//Check if there is :
	//	- one define projectile nuclei : SetProjNucleus()
	//	- one define target nuclei : SetTargNucleus()
	//	- one define material target : SetTargetMaterial()
	//If no target nucleus has been set via the SetTargNucleus() method
	//A target nucleus correspond to the first layer of the target is choosen
	//calling the DefineTargetNucleusFromLayer() method
	//
	//
	//Check if the gMultiDetArray is valid, affiliate the target material gMultiDetArray->SetTarget(KVTarget*)
	//and put the multi det array in simulation mode gMultiDetArray->SetSimMode(kTRUE)
	//gMultiDetArray->SetSimMode(kFALSE) is done by the destructor of the class
	//
	//Generate the KV2Body object to calculate kinematics of the elastic scatter
	//
	//if histograms and trees is defined do nothing for this objects
	//if not DefineTrees() and DefineHistos() are called. 
	//if you want to regenerate histograms and/or trees
	//call ClearHistos() and/or ClearTrees() before using ValidateEntrance()
	//
	//Return kTRUE if everything is ready
	//
	
	if ( !IsProjNucSet() ){
		Error("ValidateEntrance","Il n'y a pas de noyau projectile -> use SetProjNucleus()");
		return kFALSE;
	}
	
	if ( !IsTargNucSet() ){
		Info("ValidateEntrance","Il n'y a pas de noyau cible");
		if ( !IsTargMatSet() ){
			Error("ValidateEntrance","Il n'y a pas de noyau cible -> use SetTargNucleus() ou SetTargetMaterial");
			return kFALSE;
		}
		else{
			if ( DefineTargetNucleusFromLayer() )
				Info("ValidateEntrance","Definition du noyau cible via DefineTargetNucleusFromLayer()");
		}
	}
	
	if (gMultiDetArray){
		gMultiDetArray->SetSimMode(kTRUE);
		if (GetTarget())
			gMultiDetArray->SetTarget(GetTarget());
		else 
			Warning("ValidateEntrance","Les pertes dans la cible ne seront pas pris en compte");
		DefineAngularRange(gMultiDetArray);
	}
	else {
		Warning("ValidateEntrance","gMultiDetArray does not refer to a valid object\n");
		printf("\t -> The elastic scatter events will not be detected\n");
		//return kFALSE;
	}
	
	GenereKV2Body();
	
	//Define histograms/trees only at the first call
	//of validate entrance
	
	if (!ltree) DefineTrees();
	if (!lhisto) DefineHistos();
	
	//Print();
	
	SetBit(kIsUpdated);
	
	return kTRUE;

}


//_______________________________________________________________________
void KVElasticScatterEvent::Process(Int_t ntimes,Bool_t reset)
{
	//process ntimes elastic scatter
	//if reset=kTRUE, reset histograms, trees and counter before
	if ( !IsUpdated() )
		if (!ValidateEntrance()) return;
		
	if (reset) Reset();
	Int_t nn=0;
	while (nn<ntimes){ 
		MakeDiffusion();
		nn+=1;
		if ((nn%1000)==0)
			Info("Process","%d/%d diffusions treated",nn,ntimes);
	}
	Info("Process","End of process : %d diffusions performed",kTreatedNevts);
}

//_______________________________________________________________________
void KVElasticScatterEvent::MakeDiffusion()
{
	//
	//Propagation dans la cible du projectile jusqu'au point d interaction 
	//			PropagateInTargetLayer();
	//Tirage aleatoire d'un couple theta phi pour la direction de diffusion du projectile
	//Determination de la cinematique de la voie de sortie
	//		SetAnglesForDiffusion(the,phi);
	//Filtre
	//		Filter
	//Traite (eventuellement )l evenement
	//		TreateEvent();
	
	KVSimNucleus* knuc = 0;
	while ( (knuc = (KVSimNucleus* )sim_evt->GetNextParticle()) ){
		knuc->Clear_NVL();
		knuc->RemoveAllGroups();
	}
	//-------------------------
	if (IsTargMatSet()){
		NewInteractionPointInTargetLayer();
		PropagateInTargetLayer();
	}
	//-------------------------
	

	Double_t tmin = GetTheta("min");
	if (tmin >= kb2->GetMaxAngleLab(kDiffNuc)) {
		GetNucleus("PROJ")->SetMomentum(*GetNucleus("PROJ")->GetPInitial());
		return;
	}
	Double_t tmax = GetTheta("max");
	
	if (tmax <= kb2->GetMinAngleLab(kDiffNuc)) {
		GetNucleus("PROJ")->SetMomentum(*GetNucleus("PROJ")->GetPInitial());
		return;
	}
	
	if (tmin<kb2->GetMinAngleLab(kDiffNuc)) tmin = kb2->GetMinAngleLab(kDiffNuc);
	if (tmax>kb2->GetMaxAngleLab(kDiffNuc)) tmax = kb2->GetMaxAngleLab(kDiffNuc);
	
	Double_t pmin=GetPhi("min");
	Double_t pmax=GetPhi("max");
	
	kposalea.SetPolarMinMax(tmin,tmax);
	kposalea.SetAzimuthalMinMax(pmin,pmax);
	
	//kposalea.GetRandomParticle(kpartalea,kRandomOpt);
	
	Double_t th_deg,ph_deg;
	kposalea.GetRandomAngles(th_deg,ph_deg,kRandomOpt);
	/*
	 = kpartalea.GetTheta();
	Double_t ph_deg = kpartalea.GetPhi();
	*/
	((TH2F* )lhisto->FindObject("phi_theta"))->Fill(th_deg,ph_deg);
	((TH1F* )lhisto->FindObject("costheta"))->Fill(TMath::Cos(TMath::DegToRad()*th_deg));
	
	SetAnglesForDiffusion(th_deg,ph_deg);
	
	if (gMultiDetArray) 
		Filter();
	
	TreateEvent();

	kTreatedNevts+=1;
}

//_______________________________________________________________________
void KVElasticScatterEvent::NewInteractionPointInTargetLayer()
{
	//Choose a new interaction point in the current target layer
	//This position can be read via the GetInteractionPointInTargetLayer()
	//method
	if (kchoix_layer!=-1){
		TVector3 dir = GetNucleus("PROJ")->GetMomentum();
		ktarget->SetInteractionLayer(kchoix_layer,dir);
		//kIPPVector = ktarget->GetInteractionPoint();
	}
	kIPPVector = ktarget->GetInteractionPoint(GetNucleus("PROJ"));
	((TH1F* )lhisto->FindObject("target_layer_depth"))->Fill(kIPPVector.Z());
}

//_______________________________________________________________________
void KVElasticScatterEvent::PropagateInTargetLayer()
{
	//Apply Energy loss calculation to the entering projectile
	//along its path in the target layer to the interation point
	//
	//The outgoing (after diffusion) pathes are not treated here but
	//in the Filter() method
	//
	//Double_t entree = GetNucleus("PROJ")->GetKE();
	Double_t eLostInTarget = GetNucleus("PROJ")->GetKE();
	ktarget->SetIncoming(kTRUE);
	ktarget->DetectParticle(GetNucleus("PROJ"),0);
	eLostInTarget -= GetNucleus("PROJ")->GetKE();
	
	((KVSimNucleus* )sim_evt->GetParticleWithName("PROJ"))->SetValue("Before Int",eLostInTarget);
	//On modifie l'energie du projectile dans KV2Body
	//pour prendre en compte l energie deposee dans la cible
	//avant de faire le calcul de la cinematique
	if (GetNucleus("PROJ")->GetKE()==0){
		GetNucleus("PROJ")->Print();
		printf("%lf / %lf\n",eLostInTarget,proj->GetKE());
	}
	kb2->GetNucleus(1)->SetKE(GetNucleus("PROJ")->GetKE());
	kb2->CalculateKinematics();
		
	ktarget->SetIncoming(kFALSE);

}

//_______________________________________________________________________
TVector3& KVElasticScatterEvent::GetInteractionPointInTargetLayer()
{
	//return the last interaction point in the target
	return kIPPVector;

}

//_______________________________________________________________________
void KVElasticScatterEvent::SetAnglesForDiffusion(Double_t theta,Double_t phi)
{
	//Determination a partir du theta choisi
	//de l'energie cinetique du projectile diffuse
	//All kinematics properties calculated in the KV2Body class
	//are accessible via the KV2Body& GetKinematics() method
	//
	// WARNING: in inverse kinematics, there are two projectile energies
	// for each lab angle.
	
	//Int_t kb2->GetELab(Int_t OfNucleus, Double_t ThetaLab, Int_t AngleNucleus, Double_t& e1, Double_t& e2) const
	Double_t ediff1,ediff2;
	Int_t nsol_kin = kb2->GetELab(kDiffNuc, theta, kDiffNuc, ediff1,ediff2);
	
	kXruth_evt = kb2->GetXSecRuthLab(theta,kDiffNuc);
	
	//On modifie l energie et les angles du projectile ou cible diffusé(e)
	//puis par conservation, on deduit ceux du noyau cible
	KVSimNucleus* knuc = 0;
	
	if (kDiffNuc==3)
		knuc = (KVSimNucleus* )sim_evt->GetParticleWithName("PROJ");
	else 
		knuc = (KVSimNucleus* )sim_evt->GetParticleWithName("TARG");
			
	knuc->SetKE(ediff1);
	knuc->SetTheta(theta);
	knuc->SetPhi(phi);
	((TH2F* )lhisto->FindObject("ek_theta"))->Fill(knuc->GetTheta(),knuc->GetKE());
	
	//Conservation de l impulsion
	//Conservation de l energie tot
	TVector3 ptot = proj->Vect()+targ->Vect();
	Double_t etot = proj->E()+targ->E();
	//on retire la contribution du noyau diffusé
	ptot -= knuc->Vect();
	etot -= knuc->E();
	//on met a jour les pptés la cible ou projectile diffusé(e)
	
	if (kDiffNuc==3)
		knuc = (KVSimNucleus* )sim_evt->GetParticleWithName("TARG");
	else 
		knuc = (KVSimNucleus* )sim_evt->GetParticleWithName("PROJ");
	
	knuc->SetPxPyPzE(ptot.X(),ptot.Y(),ptot.Z(),etot);
	
	((TH2F* )lhisto->FindObject("ek_theta"))->Fill(knuc->GetTheta(),knuc->GetKE());
	
	sim_evt->SetNumber(kTreatedNevts);
	
	sim_evt->SetValue("XRuth",kXruth_evt);
	sim_evt->SetValue("ThDiff",theta);
	sim_evt->SetValue("EkDiff",ediff1);
	sim_evt->SetValue("IPz",kIPPVector.Z());
	
	if (nsol_kin==2)
		sim_evt->SetValue("Sol2",ediff2);
	
	//L' energie cinetique du projectile est reinitialisee
	//pour la prochaine diffusion
	GetNucleus("PROJ")->SetMomentum(*GetNucleus("PROJ")->GetPInitial());
	
}

//_______________________________________________________________________
void KVElasticScatterEvent::Filter()
{
	//Simulate passage of the projectile/target couple
	//through the multidetector refered by the gMultiDetArray pointer
	//if it is not valid do nothing
	
	if (IsTargMatSet()){
		ktarget->SetOutgoing(kTRUE);
		gMultiDetArray->DetectEvent(sim_evt,rec_evt);
		ktarget->SetOutgoing(kFALSE);
	}
	else {
		gMultiDetArray->DetectEvent(sim_evt,rec_evt);
	}
}

//_______________________________________________________________________
void KVElasticScatterEvent::TreateEvent()
{

	((TTree* )ltree->FindObject("ElasticScatter"))->Fill();
	
	KVSimNucleus* knuc = 0;
	while ( (knuc = (KVSimNucleus* )sim_evt->GetNextParticle("DETECTED")) ){
		
		((TH2F* )lhisto->FindObject("ek_theta_DETECTED"))->Fill(knuc->GetTheta(),knuc->GetKE());
	
	}

}

//_______________________________________________________________________
void KVElasticScatterEvent::Print()
{
	
	kb2->Print();
	
	printf("#####################\n");
	printf("## KVElasticScatterEvent::Print() ##\n");
	printf("# Diffusion elastique traitee :\n");
	printf("# %s+%s@%1.2lf MeV/A\n",
		GetNucleus(1)->GetSymbol(),
		GetNucleus(2)->GetSymbol(),
		GetNucleus(1)->GetKE()/GetNucleus(1)->GetA()
	);
	if ( IsTargMatSet() ){
		printf("-------------------------\n");
		printf("# Propagation dans une cible de:\n");
		for (Int_t nn=0;nn<GetTarget()->GetLayers()->GetEntries();nn+=1){
			Double_t epaiss = GetTarget()->GetLayerByIndex(nn+1)->GetAreaDensity()/( KVUnits::mg / pow(KVUnits::cm, 2.) );
			printf("#\ttype:%s epaisseur:%lf (mg/cm**2) / %lf\n",
				GetTarget()->GetLayerByIndex(nn+1)->GetType(),
				epaiss,
				GetTarget()->GetLayerByIndex(nn+1)->GetThickness()
			);
		}
	}
	printf("-------------------------\n");
	if (!gMultiDetArray){
		printf("Pointer gMultiDetArray does not refer to a valid object\n");
	}
	else {
		printf("# Detection par %s\n",gMultiDetArray->GetName());
	}
	printf("#####################\n");

}
//_______________________________________________________________________
void KVElasticScatterEvent::DefineHistos()
{
	//Definition of control histograms
	//- phi_theta : filled with angles choosen to determine the direction of the diffused projectile
	//- target_layer_depth : interaction point position in the target 
	//- ek_theta : filled with energies and polar angles of projectile and target nuclei after diffusion
	//- ek_theta_DETECTED : filled with energies and polar angles of projectile and target nuclei if 
	//they are detected by the multidetarray

	lhisto = new KVHashList(); 
	lhisto->SetOwner(kTRUE);

	lhisto->Add(new TH2F("phi_theta","phi_theta",180,0,180,360,0,360));
	lhisto->Add(new TH1F("costheta","costheta",200,-1,1));
	if ( IsTargMatSet() ){
		Float_t thickness = GetTarget()->GetThickness();
		lhisto->Add(new TH1F("target_layer_depth","target_layer_depth",TMath::Nint(thickness*110),0,thickness*1.1));
	}
	Float_t totalE = GetNucleus(1)->GetKE();
	lhisto->Add(new TH2F("ek_theta","ek_theta",180,0,180,TMath::Nint(totalE*11),0,totalE*1.1));
	lhisto->Add(new TH2F("ek_theta_DETECTED","ek_theta",180,0,180,TMath::Nint(totalE*11),0,totalE*1.1));

}

//_______________________________________________________________________
KVHashList* KVElasticScatterEvent::GetHistos() const
{
	//return the list where histo are stored
	return lhisto;

}
//_______________________________________________________________________
void KVElasticScatterEvent::ResetHistos()
{

	//Reset histo in the list
	lhisto->Execute("Reset","");

}
//_______________________________________________________________________
void KVElasticScatterEvent::ClearHistos()
{
	//Efface la liste des histo et leur contenu et met le pointeur a zero
	if (lhisto) delete lhisto; lhisto=0;

}

//_______________________________________________________________________
void KVElasticScatterEvent::DefineTrees()
{
	//Definition of tree where simulated events are stored
	ltree = new KVHashList(); 
	ltree->SetOwner(kTRUE);
	TTree* tt = 0;
	
	tt = new TTree("ElasticScatter",IsA()->GetName());
	tt->Branch("Simulated_evts", "KVSimEvent", &sim_evt, 10000000, 0)->SetAutoDelete(kFALSE);
	ltree->Add(tt);
}

//_______________________________________________________________________
KVHashList* KVElasticScatterEvent::GetTrees() const
{
	//return the list where histo are stored
	return ltree;

}

//_______________________________________________________________________
void KVElasticScatterEvent::ClearTrees()
{
	//Efface la liste des arbres et leur contenu et met le pointeur a zero
	if (ltree) delete ltree; ltree=0;

}

//_______________________________________________________________________
void KVElasticScatterEvent::ResetTrees()
{
	//Reset the tree contents
	ltree->Execute("Reset","");

}

//_______________________________________________________________________
void KVElasticScatterEvent::DefineAngularRange(Double_t tmin, Double_t tmax, Double_t pmin,Double_t pmax)
{
	//Define in which angular (polar and azimuthal) range
	//The projectile diffusion direction will be randomized
	//If this method is not used
	//Default range is \theta [0,180] and \phi [0,360]
	if (tmin!=-1) th_min=tmin;
	if (tmax!=-1) th_max=tmax;
	if (pmin!=-1) phi_min=pmin;
	if (pmax!=-1) phi_max=pmax;

}

//_______________________________________________________________________
void KVElasticScatterEvent::DefineAngularRange(TObject* obj)
{
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
	
//_______________________________________________________________________
Double_t KVElasticScatterEvent::GetTheta(KVString opt) const
{
	//Return the limite in theta range (polar angle)
	//opt has to be "min" or "max"
	if (opt=="min") return th_min;
	else if (opt=="max") return th_max;
	else return -1;

}

//_______________________________________________________________________
Double_t KVElasticScatterEvent::GetPhi(KVString opt) const 
{
	
	//Return the limite in phi range (azimuthal angle)
	//opt has to be "min" or "max"
	if (opt=="min") return phi_min;
	else if (opt=="max") return phi_max;
	else return -1;

}
