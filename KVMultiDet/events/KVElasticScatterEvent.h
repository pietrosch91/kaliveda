/*
$Id: KVElasticScatterEvent.h,v 1.3 2009/01/14 15:35:50 ebonnet Exp $
$Revision: 1.3 $
$Date: 2009/01/14 15:35:50 $
*/

//Created by KVClassFactory on Thu Dec 11 14:45:29 2008
//Author: eric bonnet,,,

#ifndef __KVELASTICSCATTEREVENT_H
#define __KVELASTICSCATTEREVENT_H

#include "KVString.h"
#include "TVector3.h"
#include "KVTarget.h"
#include "KVLayer.h"
#include "KVMaterial.h"
#include "KVNucleus.h"
#include "KVEvent.h"
#include "KV2Body.h"
#include "KVMultiDetArray.h"
#include "KVReconstructedNucleus.h"
#include "KVDetectorEvent.h"
#include "KVReconstructedEvent.h"
#include "TObject.h"


class KVElasticScatterEvent
{

   protected:

	TVector3 					kIPPVector;

	KVTarget						ktarget;
	KVNucleus*					kcurrent_nuc;
	KVEvent*						kcurrent_evt;
	KVList*						kevent_list;
	KV2Body						k2body;
	KVReconstructedEvent 	recev;
	KVDetectorEvent             detevt;
	KVList*						lhisto_control;

	Double_t 					th_min,th_max,phi_min,phi_max;
	Double_t						kXruth_evt;
	Int_t 						kchoix_layer;
	Int_t 						kTreatedNevts;

   public:

	KVElasticScatterEvent();
   virtual ~KVElasticScatterEvent();
	virtual void AddHistoTree();
	virtual void ResetHistoTree();
	KVList* GetObjectList() {return lhisto_control;}

	virtual TH1* GetHisto(KVString hname) { return (TH1F* )lhisto_control->FindObject(hname); }
	virtual TTree* GetTree(KVString tname){ return (TTree* )lhisto_control->FindObject(tname); }

	virtual void SetTarget(KVTarget *targ,Bool_t IsRandomized=kTRUE) {
		targ->Copy(ktarget);
		ktarget.SetRandomized(IsRandomized);
	}
	virtual KVTarget& GetTarget() { return ktarget;}
	virtual KV2Body& GetKinematics() { return k2body;}
	virtual TVector3& GetInteractionPointInTargetLayer() { return kIPPVector; }
	virtual KVNucleus* GetNucleus(KVString nucname,KVString evtname){
		KVEvent* evt=0;
		if ( (evt = (KVEvent* )kevent_list->FindObject(evtname)) )
			return evt->GetParticle(nucname);
		else return 0;
	}
	virtual KVReconstructedNucleus* GetReconstructedNucleus(KVString nucname){
		return GetReconstructedEvent().GetParticle(nucname);
	}
	virtual KVReconstructedEvent& GetReconstructedEvent(void){
		return recev;
	}
	virtual Double_t GetTheta(KVString opt){
		if (opt=="min") return th_min;
		else if (opt=="max") return th_max;
		else return -1;
	}
	virtual Double_t GetPhi(KVString opt){
		if (opt=="min") return phi_min;
		else if (opt=="max") return phi_max;
		else return -1;
	}

	virtual void SetTargNucleus(KVNucleus *nuc) { SetEntranceNucleus(nuc,"TARG"); }
	virtual void SetProjNucleus(KVNucleus *nuc) { SetEntranceNucleus(nuc,"PROJ"); }

	virtual void DefineAngularRange(Double_t tmin, Double_t tmax, Double_t pmin,Double_t pmax){
	//Define in which angular (polar and azimuthal) range
	//The projectile diffusion direction will be randomized
	//If this method is not used
	//Default range is \theta [0,180] and \phi [0,360]
		if (tmin!=-1) th_min=tmin;
		if (tmax!=-1) th_max=tmax;
		if (pmin!=-1) phi_min=pmin;
		if (pmax!=-1) phi_max=pmax;
	}

	virtual Bool_t DefineTargetNucleusFromLayer(KVString layer_name="");
	virtual void SetEntranceNucleus(KVNucleus *nuc,KVString type="PROJ");

	virtual void DuplicateEvent(KVString from,KVString to);
	virtual Bool_t ValidateEntrance();

	virtual void Process(KVMultiDetArray* mdet=0);

	virtual void DefineAngularRange(TObject* );

	virtual void PropagateInTargetLayer(KVString opt);
	virtual void NewInteractionPointInTargetLayer();

	virtual void MakeDiffusion(Double_t theta,Double_t phi);

	virtual void Filter(KVMultiDetArray* mdet);
	virtual void CheckReconstrutedEventStatus();
	virtual void Print();

	ClassDef(KVElasticScatterEvent,1)//simulate ElasticScatterEvent and answer of a given (multi-)detector
};

#endif
