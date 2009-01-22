/*
$Id: KVElasticScatterEvent_e475s.cpp,v 1.1 2009/01/19 14:32:14 ebonnet Exp $
$Revision: 1.1 $
$Date: 2009/01/19 14:32:14 $
*/

//Created by KVClassFactory on Wed Dec 17 17:04:49 2008
//Author: eric bonnet,,,

#include "KVElasticScatterEvent_e475s.h"
#include "KVString.h"
#include "KVRList.h"
#include "KVChIo.h"

ClassImp(KVElasticScatterEvent_e475s)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVElasticScatterEvent_e475s</h2>
<h4>derived class for e475s experiment</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVElasticScatterEvent_e475s::KVElasticScatterEvent_e475s()
{
   // Default constructor
	//Liste les types de detecteurs
	lgen.SetParameter("CI","0");
	lgen.SetParameter("SI","1");
	lgen.SetParameter("CSI","2");
	lgen.SetParameter("SI75","3");
	lgen.SetParameter("SILI","4");
	
}



KVElasticScatterEvent_e475s::~KVElasticScatterEvent_e475s()
{
   // Destructor
}

void KVElasticScatterEvent_e475s::AddHistoTree(){
	//Routine where you can create histos or trees
	//if kvlist already exists, it is delete with all objects
	//and recreate
	KVString snom;
	if (lhisto_control) delete lhisto_control;
	
	lhisto_control = new KVList();
	
	lhisto_control->Add(new TH2F("theta_phi","theta_phi",900,0,90,1800,0,360));
	Float_t thickness = GetTarget().GetThickness();
	lhisto_control->Add(new TH1F("target_layer_depth","target_layer_depth",TMath::Nint(thickness*110),0,thickness*1.1));
	
	lhisto_control->Add(new TTree("depot_det","depot_det"));
	TTree* tt = (TTree* )lhisto_control->Last();
	tt->Branch("b_noyau",&b_noyau,"b_noyau/I");
	tt->Branch("b_einc",&b_einc,"b_einc/F");
	tt->Branch("b_theta",&b_theta,"b_theta/F");
	tt->Branch("b_ruth",&b_ruth,"b_ruth/F");
	tt->Branch("b_de",&b_de,"b_de/F");
	tt->Branch("b_eres",&b_eres,"b_eres/F");
	tt->Branch("b_det",&b_det,"b_det/I");
	tt->Branch("b_evt",&b_evt,"b_evt/I");
	
}

void KVElasticScatterEvent_e475s::Process(KVMultiDetArray* mdet){
	//Exemple de ce qui peut etre fait
	//A rederiver et customiser
	b_evt = kTreatedNevts;
	
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
	k2body.SetOutgoing(3,kcurrent_evt->GetParticle("PROJ"));
	k2body.SetOutgoing(4,kcurrent_evt->GetParticle("TARG"));
	k2body.CalculateKinematics();
	
	Double_t anglemax=k2body.GetMaxAngleLab(3);
	
	Double_t the=GetTheta("min");
	if (anglemax<the) return; 
	
	Double_t phi=GetPhi("min");
	if (GetTheta("min")!=GetTheta("max"))
		the = gRandom->Uniform(GetTheta("min"),TMath::Min(GetTheta("max"),anglemax));
	if (GetPhi("min")!=GetPhi("max"))
		phi = gRandom->Uniform(GetPhi("min"),GetPhi("max"));
	
	((TH2F* )lhisto_control->FindObject("theta_phi"))->Fill(the,phi);
	
	MakeDiffusion(the,phi);
	//-------------------------
	
	DuplicateEvent("DIFF","OUT");	
	kcurrent_nuc=0;
	while ( (kcurrent_nuc = kcurrent_evt->GetNextParticle()) ){
		PropagateInTargetLayer("OUT");
	}
	
	//-------------------------
	
	DuplicateEvent("OUT","DET");
	Filter(mdet);
	
	KVReconstructedNucleus* recnuc=0;
	KVString snom;
	
	recnuc=0;
	recev.ResetGetNextParticle();
	while ( (recnuc = recev.GetNextParticle() ) ){
		
		Double_t ekbeforedet = GetNucleus(recnuc->GetName(),"OUT")->GetKE();	//Energie apres la sortie de la cible
		Double_t ekafterdet = GetNucleus(recnuc->GetName(),"DET")->GetKE();	//Energie apres detection (=0)

		const KVRList* ldet = recnuc->GetDetectorList();	//Liste des detecteurs touches par la particule consideree
		Int_t ndet=ldet->GetEntries();
		
		KVDetector* stop_det = recnuc->GetStoppingDetector();
		b_noyau=0;
		if ( !strcmp("TARG",recnuc->GetName()) ) b_noyau=1;
		b_det = lgen.GetIntValue(stop_det->GetType());
		b_einc= ekbeforedet;
		b_theta=GetNucleus(recnuc->GetName(),"OUT")->GetTheta();
		b_ruth=kXruth_evt;
		
		b_eres = stop_det->GetEnergyLoss();
		b_de=0;
		if (ndet>1)	b_de = ((KVDetector* )ldet->At(1))->GetEnergyLoss();
		
		if (ekafterdet==0)
			((TTree*) lhisto_control->FindObject("depot_det"))->Fill();
	}
}

