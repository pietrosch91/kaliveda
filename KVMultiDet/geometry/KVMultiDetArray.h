/***************************************************************************
                          kvmultidetarray.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVMultiDetArray.h,v 1.55 2009/03/03 14:27:15 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVMULTIDETARRAY_H
#define KVMULTIDETARRAY_H

#define KVGROUP_MERGE_NOT_IMPLEMENTED "Group merging not implemented yet. Your groups will not be correctly described"
#define KVMDA_REPL_TEL_NOT_FOUND "Telescope %s not found among array telescopes"
#define KVMDA_REPL_DET_NOT_FOUND "Detector %s not found among array detectors"

#include "KVBase.h"
#include "KVSeqCollection.h"
#include "KVLayer.h"
#include "TGraph.h"

class KVEvent;
class KVRing;
class KVTelescope;
class KVGroup;
class KVDetector;
class KVDetectorEvent;
class KVACQParam;
class KVReconstructedEvent;
class KVMultiDetBrowser;
class KVRList;
class KVMaterial;
class KVNucleus;
class KVNameValueList;
class KVTarget;
class KVIDTelescope;
class KV2Body;
class KVReconstructedNucleus;
class KVReconstructedEvent;
class TGeoManager;
class KVUniqueNameList;

class KVMultiDetArray:public KVBase {

   friend class KVMultiDetBrowser;

 private:

    KVTarget * fTarget;         //target used in experiment
   enum {
      kIsRemoving = BIT(14),     //flag set during call to RemoveLayer etc.
      kParamsSet = BIT(15),      //flag set when SetParameters called
      kIsBuilt = BIT(16),        //flag set when Build() is called
      kIsBeingDeleted = BIT(17), //flag set when dtor is called
      kIDParamsSet = BIT(18),    //flag set when SetRunIdentificationParameters called
      kCalParamsSet = BIT(19)    //flag set when SetRunCalibrationParameters called
   };

   TList* fStatusIDTelescopes;//! used by GetStatusIDTelescopes
   TList* fCalibStatusDets;//! used by GetStatusIDTelescopes

 protected:
    UInt_t fCurrentLayerNumber; //! used to number layers
   UInt_t fGr;                  //! used to number groups
   KVMultiDetBrowser *fBrowser; //! graphical visualisation and configuration tool

   KVSeqCollection *fLayers;             //-> list of layers in array sorted by layer number
   KVSeqCollection *fDetectorTypes;      //-> list of detector types used to construct telescopes of array
   KVSeqCollection *fTelescopes;         //-> list of telescope prototypes used to construct array
   KVSeqCollection *fGroups;             //->list of groups of telescopes in array
   KVDetectorEvent *fHitGroups;          //!   list of hit groups in simulation
   KVSeqCollection *fIDTelescopes;       //->deltaE-E telescopes in groups
   KVSeqCollection *fDetectors;          //->list of references to all detectors in array
   KVSeqCollection *fACQParams;          //references to data acquisition parameters associated to detectors

   KV2Body *fScatter;           //!for elastic scattering kinematics

   TString fDataSet;            //!name of associated dataset, used with MakeMultiDetector()
   UInt_t fCurrentRun;          //Number of the current run used to call SetParameters
   
   Bool_t fSimMode;             //!=kTRUE in "simulation mode" (use for calculating response to simulated events)

   Bool_t fROOTGeometry;//!=kTRUE if ROOT TGeo geometry and algorithms used for tracking/filtering
   Int_t fFilterType;//! type of filtering (used by DetectEvent)
   
   void SetGroups(KVLayer *, KVLayer *);
   void UpdateGroupsInRings(KVRing * r1, KVRing * r2);
   void AddToGroups(KVTelescope * kt1, KVTelescope * kt2);
   void MergeGroups(KVGroup * kg1, KVGroup * kg2);
   void RenumberGroups();
   virtual void SetGroupsAndIDTelescopes();
   virtual void MakeListOfDetectorTypes() {
      AbstractMethod("MakeListOfDetectorTypes");
   };
   virtual void PrototypeTelescopes() {
      AbstractMethod("PrototypeTelescopes");
   };
   virtual void BuildGeometry() {
      AbstractMethod("BuildGeometry");
   };
   virtual void MakeListOfDetectors();
   void SetACQParams();

	void set_up_telescope(KVDetector * de, KVDetector * e, TCollection * idtels, KVIDTelescope *idt, TString& uri);
	void set_up_single_stage_telescope(KVDetector * det, TCollection * idtels, KVIDTelescope *idt, TString& uri);
   
   virtual void SetCalibrators();
	virtual void SetDetectorThicknesses();

 public:
       // filter types. values of fFilterType
   enum EFilterType
   {
      kFilterType_Geo,
      kFilterType_GeoThresh,
      kFilterType_Full
   };
   void SetFilterType(Int_t t){fFilterType=t;};
   KVMultiDetArray();
   virtual ~ KVMultiDetArray();
   void init();

   virtual void Build();

   virtual void UpdateArray();

   void AddLayer();
   void AddLayer(KVLayer * kvl);

   virtual void GetIDTelescopes(KVDetector *, KVDetector *, TCollection *);

   virtual void Clear(Option_t * opt = "");
   virtual void Print(Option_t * opt = "") const;

   KVTelescope *GetTelescope(const Char_t * name) const;
   virtual KVDetector *GetDetector(const Char_t * name) const;
   KVGroup *GetGroup(Float_t theta, Float_t phi);
   KVGroup *GetGroup(const Char_t *);
   KVSeqCollection *GetGroups() const {
      return fGroups;
   }
   TList *GetTelescopes(Float_t theta, Float_t phi);
   KVSeqCollection *GetDetectorTypes() {
      return fDetectorTypes;
   };
   KVSeqCollection *GetListOfDetectors() const;
   KVSeqCollection *GetLayers() const {
      return fLayers;
   };
   KVLayer *GetLayer(const Char_t * name) const {
      //find layer with name
      return (KVLayer *) fLayers->FindObject(name);
   }
   KVLayer *GetLayer(UInt_t num) const {
      //find layer with number
      return (KVLayer *) fLayers->FindObjectByNumber(num);
   }
   KVRing *GetRing(const Char_t * layer, const Char_t * ring_name) const;
   KVRing *GetRing(const Char_t * layer, UInt_t ring_number) const;
   KVRing *GetRing(UInt_t layer, const Char_t * ring_name) const;
   KVRing *GetRing(UInt_t layer, UInt_t ring_number) const;
   void RemoveRing(const Char_t * layer, const Char_t * ring_name);
   void RemoveRing(const Char_t * layer, UInt_t ring_number);
   void RemoveRing(UInt_t layer, const Char_t * ring_name);
   void RemoveRing(UInt_t layer, UInt_t ring_number);
   void RemoveRing(KVRing * ring);
   void RemoveLayer(KVLayer *, Bool_t kDeleteLay = kTRUE);
   void RemoveLayer(const Char_t *);
   void RemoveLayer(UInt_t);
   void RemoveGroup(KVGroup *);
   void RemoveGroup(const Char_t *);
   void ReplaceTelescope(const Char_t * name, KVTelescope * new_kvt);
   void ReplaceDetector(const Char_t * name, KVDetector * new_kvd);

   void StartBrowser();
   void CloseBrowser();
   const KVMultiDetBrowser *GetBrowser();

   void AddACQParam(KVACQParam *);
   KVSeqCollection *GetACQParams() {
      return fACQParams;
   };
   KVACQParam *GetACQParam(const Char_t * name) {
      return (KVACQParam *) fACQParams->FindObject(name);
   };
   virtual void SetArrayACQParams();

	virtual void DetectEvent(KVEvent * event,KVReconstructedEvent* rec_event,const Char_t* detection_frame="");
   virtual void GetDetectorEvent(KVDetectorEvent* detev, KVSeqCollection* fired_params = 0);
   KVNameValueList* DetectParticle_KV(KVNucleus * part);
   KVNameValueList* DetectParticle_TGEO(KVNucleus * part);
   KVNameValueList* DetectParticle(KVNucleus * part)
   {
      // Simulate detection of a charged particle by the array.
      // The actual method called depends on the value of fROOTGeometry:
      //   fROOTGeometry=kTRUE:  calls DetectParticle_TGEO, particle propagation performed using
      //               TGeo description of array and algorithms from ROOT TGeo package
      //   fROOTGeometry=kFALSE:  calls DetectParticle_KV, uses simple KaliVeda geometry
      //                to simulate propagation of particle
      //
      // The default value is given in .kvrootrc by variable KVMultiDetArray.FilterUsesROOTGeometry
      return (fROOTGeometry?DetectParticle_TGEO(part):DetectParticle_KV(part));
   };
   void DetectParticleIn(const Char_t * detname, KVNucleus * kvp);

   KVIDTelescope *GetIDTelescope(const Char_t * name) const;
   KVSeqCollection *GetListOfIDTelescopes() const {
      return fIDTelescopes;
   };
	KVList* GetIDTelescopeTypes();
	KVSeqCollection* GetIDTelescopesWithType(const Char_t* type);

   virtual void SetTarget(const Char_t * material,
                          const Float_t thickness);
   void SetTarget(KVTarget * target);
   void SetTargetMaterial(const Char_t * material);
   void SetTargetThickness(const Float_t thickness);
   KVTarget *GetTarget() {
      return fTarget;
   };

   virtual Double_t GetTargetEnergyLossCorrection(KVReconstructedNucleus*);

   Bool_t IsRemoving() {
      return TestBit(kIsRemoving);
   }

   virtual void SetPedestals(const Char_t *);

   virtual Bool_t IsBuilt() const {
      return TestBit(kIsBuilt);
   }
   static KVMultiDetArray *MakeMultiDetector(const Char_t * name);

   Bool_t IsBeingDeleted() {
      return TestBit(kIsBeingDeleted);
   };
   virtual void SetParameters(UShort_t n);
   virtual void SetRunIdentificationParameters(UShort_t n);
   virtual void SetRunCalibrationParameters(UShort_t n);

   Bool_t ParamsSet() {
      return TestBit(kParamsSet);
   };
   Bool_t IDParamsSet() {
      return (TestBit(kIDParamsSet)||ParamsSet());
   };
   Bool_t CalParamsSet() {
      return (TestBit(kCalParamsSet)||ParamsSet());
   };
   UInt_t GetCurrentRunNumber() const {
      return fCurrentRun;
   };

   virtual void SetIdentifications();
   virtual void InitializeIDTelescopes();
   virtual void UpdateIDTelescopes();
   virtual void UpdateIdentifications();
   virtual void UpdateCalibrators();

	virtual Double_t GetTotalSolidAngle(void);

	virtual Double_t GetSolidAngleByLayerAndRing(const Char_t* layer,UInt_t ring_number) {
		// return the solid angle (msr) for a given KVRing conditioned by the chosen KVLayer
		return GetRing(layer,ring_number)->GetSolidAngle();
	}
	virtual Double_t GetSolidAngleByLayer(const Char_t* layer) {
		// return the solid angle (msr) of all the KVRing conditioned by the chosen KVLayer
		Double_t sol_ang=0;
		KVRing *cou; TIter nxtcou(GetLayer(layer)->GetRings()); while ((cou = (KVRing *) nxtcou())) sol_ang+=cou->GetSolidAngle();
		return sol_ang;
	}
   TList* GetStatusOfIDTelescopes();
   TList* GetCalibrationStatusOfDetectors();
   void PrintStatusOfIDTelescopes();
   void PrintCalibStatusOfDetectors();

	virtual TGeoManager* CreateGeoManager(Double_t /*dx*/ = 500, Double_t /*dy*/ = 500, Double_t /*dz*/ = 500);
   virtual void SetSimMode(Bool_t on = kTRUE)
   {
   	// Set simulation mode of array (and of all detectors in array)
   	// If on=kTRUE (default), we are in simulation mode (calculation of energy losses etc.)
   	// If on=kFALSE, we are analysing/reconstruction experimental data
   	fSimMode = on;
   	GetListOfDetectors()->Execute("SetSimMode", Form("%d", (Int_t)on));
   };
   virtual Bool_t IsSimMode() const
   {
   	// Returns simulation mode of array:
   	//   IsSimMode()=kTRUE : we are in simulation mode (calculation of energy losses etc.)
   	//   IsSimMode()=kFALSE: we are analysing/reconstruction experimental data
   	return fSimMode;
   };

	virtual Double_t GetPunchThroughEnergy(const Char_t* detector, Int_t Z, Int_t A);
	virtual TGraph* DrawPunchThroughEnergyVsZ(const Char_t* detector, Int_t massform=KVNucleus::kBetaMass);
	virtual TGraph* DrawPunchThroughEsurAVsZ(const Char_t* detector, Int_t massform=KVNucleus::kBetaMass);
   
   void SetROOTGeometry(Bool_t on=kTRUE) { fROOTGeometry=on; };
   Bool_t IsROOTGeometry() const { return fROOTGeometry; };
   
   ClassDef(KVMultiDetArray, 6) //Base class for describing multidetector arrays.
   Int_t FilteredEventCoherencyAnalysis(Int_t round, KVReconstructedEvent* rec_event);
};

//................  global variable
R__EXTERN KVMultiDetArray *gMultiDetArray;

#endif
