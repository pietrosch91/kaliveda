//Created by KVClassFactory on Tue Apr 16 09:45:50 2013
//Author: John Frankland,,,

#ifndef __KVMultiDetArray_H
#define __KVMultiDetArray_H

#include "KVBase.h"
#include "KVSeqCollection.h"
#include "TGraph.h"
#include "TGeoManager.h"
#include "KVNucleus.h"
#include "KVDetector.h"
class KVTarget;
class KVTelescope;
class KVIDTelescope;
class KVACQParam;
class KVReconstructedEvent;
class KVDetectorEvent;
class KVGroup;
class KVEvent;
class KVNameValueList;
class KVReconstructedNucleus;
class KVList;

class KVMultiDetArray : public KVBase
{

protected:

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

    UInt_t fGr;                  //! used to number groups

    KVSeqCollection *fGroups;             //->list of groups of telescopes in array
    KVDetectorEvent *fHitGroups;          //!   list of hit groups in simulation
    KVSeqCollection *fIDTelescopes;       //->deltaE-E telescopes in groups
    KVSeqCollection *fDetectors;          //->list of all detectors in array
    KVSeqCollection *fACQParams;          //list of data acquisition parameters associated to detectors

    TString fDataSet;            //!name of associated dataset, used with MakeMultiDetector()
    UInt_t fCurrentRun;          //Number of the current run used to call SetParameters

    Bool_t fSimMode;             //!=kTRUE in "simulation mode" (use for calculating response to simulated events)

    Bool_t fROOTGeometry;//!=kTRUE if ROOT TGeo geometry and algorithms used for tracking/filtering

    Int_t fFilterType;//! type of filtering (used by DetectEvent)

    TGeoManager* fGeoManager;//! array geometry

    virtual void AddToGroups(KVTelescope * kt1, KVTelescope * kt2);
    virtual void MergeGroups(KVGroup * kg1, KVGroup * kg2);
    virtual void RenumberGroups();
    virtual void BuildGeometry() {
        AbstractMethod("BuildGeometry");
    };
    virtual void MakeListOfDetectors();
    virtual void SetACQParams();

    virtual void set_up_telescope(KVDetector * de, KVDetector * e, TCollection * idtels, KVIDTelescope *idt, TString& uri);
    virtual void set_up_single_stage_telescope(KVDetector * det, TCollection * idtels, KVIDTelescope *idt, TString& uri);

    virtual void SetCalibrators();
    virtual void SetDetectorThicknesses();
    virtual TGeoManager* CreateGeoManager(Double_t /*dx*/ = 500, Double_t /*dy*/ = 500, Double_t /*dz*/ = 500){ return 0; };

public:
    void SetGeometry(TGeoManager*);
    TGeoManager* GetGeometry() const;
    // filter types. values of fFilterType
    enum EFilterType
    {
        kFilterType_Geo,
        kFilterType_GeoThresh,
        kFilterType_Full
    };
    KVMultiDetArray();
    virtual ~KVMultiDetArray();

    void SetFilterType(Int_t t){fFilterType=t;};
    void init();

    void AddDetector(KVDetector* d){
        fDetectors->Add(d);
    }

    virtual void Build();
    virtual void SetGroupsAndIDTelescopes();
    virtual void CreateIDTelescopesInGroups();
    virtual void CalculateGroupsFromGeometry();

    virtual void UpdateArray();

    virtual void GetIDTelescopes(KVDetector *, KVDetector *, TCollection *);

    virtual void Clear(Option_t * opt = "");
    virtual void Print(Option_t * opt = "") const;

    virtual KVTelescope *GetTelescope(const Char_t * name) const;
    virtual KVDetector *GetDetector(const Char_t * name) const;
    virtual KVGroup *GetGroup(Float_t theta, Float_t phi);
    virtual KVGroup *GetGroup(const Char_t *);
    KVSeqCollection *GetGroups() const {
        return fGroups;
    }
    TList *GetTelescopes(Float_t theta, Float_t phi);
    KVSeqCollection *GetListOfDetectors() const { return fDetectors; }
    void RemoveGroup(KVGroup *);
    void RemoveGroup(const Char_t *);
    void ReplaceDetector(const Char_t * name, KVDetector * new_kvd);

    void AddACQParam(KVACQParam *);
    KVSeqCollection *GetACQParams() {
        return fACQParams;
    };
    KVACQParam *GetACQParam(const Char_t * name) {
        return (KVACQParam *) fACQParams->FindObject(name);
    };
    virtual void SetArrayACQParams();

    virtual void DetectEvent(KVEvent * event,KVReconstructedEvent* rec_event,const Char_t* detection_frame="");
    virtual Int_t FilteredEventCoherencyAnalysis(Int_t round, KVReconstructedEvent* rec_event);
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
        // The default value is given in .kvrootrc by variable KVASMultiDetArray.FilterUsesROOTGeometry
        return (fROOTGeometry?DetectParticle_TGEO(part):DetectParticle_KV(part));
    };
    void DetectParticleIn(const Char_t * detname, KVNucleus * kvp);

    KVIDTelescope *GetIDTelescope(const Char_t * name) const;
    KVSeqCollection *GetListOfIDTelescopes() const {
        return fIDTelescopes;
    };
    KVList* GetIDTelescopeTypes();
    KVSeqCollection* GetIDTelescopesWithType(const Char_t* type);

    void SetTarget(const Char_t * material, const Float_t thickness);
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

    TList* GetStatusOfIDTelescopes();
    TList* GetCalibrationStatusOfDetectors();
    void PrintStatusOfIDTelescopes();
    void PrintCalibStatusOfDetectors();


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

    ClassDef(KVMultiDetArray,7)//Base class for multidetector arrays
};

//................  global variable
R__EXTERN KVMultiDetArray *gMultiDetArray;

#endif
