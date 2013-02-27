//Created by KVClassFactory on Thu Jul 19 20:50:06 2012
//Author: Guilain ADEMARD

#ifndef __KVSPECTRODETECTOR_H
#define __KVSPECTRODETECTOR_H

#include "KVDetector.h"
#include "TGeoVolume.h"
#include "TGeoManager.h"

class KVSpectroDetector : public KVDetector//, public TGeoVolume
{
	protected:
		TList *fActiveVolumes;
		Int_t  fNabsorbers;      // Number of absobers
		Double_t fTotThick;      // Total thickness of the detector
		static Int_t fNumVol;    // used to set number to each new volume

   void AddAbsorber(TGeoVolume*, TGeoMatrix* matrix = 0, Bool_t active = kFALSE);
   public:
   KVSpectroDetector();
   KVSpectroDetector(const Char_t* type);
   KVSpectroDetector (const KVSpectroDetector&) ;
   void init();
   virtual ~KVSpectroDetector();

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy (TObject & obj) const;
#else
   virtual void Copy (TObject & obj);
#endif

   void AddAbsorber(KVMaterial*);
   void AddAbsorber(const Char_t* material, TGeoShape* shape, TGeoMatrix* matrix= 0, Bool_t active= kFALSE);
   virtual void AddToTelescope(KVTelescope * T, const int =
                               KVD_RECPRC_CNXN);
   virtual Bool_t BuildGeoVolume(TEnv *infos, TGeoVolume *ref_vol = 0);
   virtual void DetectParticle(KVNucleus *, TVector3 * norm = 0);
   virtual Double_t GetELostByParticle(KVNucleus *, TVector3 * norm = 0);
   TGeoMedium* GetGeoMedium(const Char_t* ="");
   virtual TGeoVolume* GetGeoVolume();
   virtual TGeoVolume* GetGeoVolume(const Char_t*, const Char_t*, TGeoShape*);
   virtual TGeoVolume* GetGeoVolume(const Char_t* name, const Char_t* material, const Char_t* shape_name, const Char_t* params);
   virtual Double_t GetParticleEIncFromERes(KVNucleus * , TVector3 * norm = 0);
   virtual UInt_t GetTelescopeNumber() const;
	virtual void GetVerticesInOwnFrame(TVector3* /*corners[8]*/, Double_t /*depth*/, Double_t /*layer_thickness*/);
   void SetActiveVolume(TGeoVolume*);


   const Char_t *GetDetectorEnv(const Char_t * type, const Char_t* defval="", TEnv *env=NULL) const;
   Double_t GetDetectorEnv(const Char_t * type, Double_t defval, TEnv *env=NULL) const;
   Int_t GetDetectorEnv(const Char_t * type, Int_t defval, TEnv *env=NULL) const;
   Bool_t GetDetectorEnv(const Char_t * type, Bool_t defval, TEnv *env=NULL) const;


   virtual void SetMaterial(const Char_t * type);
   inline TList *GetActiveVolumes() const{ return fActiveVolumes;}
   inline Double_t GetTotalThickness() const{ return fTotThick;}


//   virtual Double_t GetCorrectedEnergy(const KVNucleus*, Double_t e =
//                                       -1., Bool_t transmission=kTRUE);
//   virtual Int_t FindZmin(Double_t ELOSS = -1., Char_t mass_formula = -1);
//
//
//   virtual void Print(Option_t * option = "") const;
//
//   TList *GetTelescopesForIdentification();
//   void GetAlignedIDTelescopes(TCollection * list);
//
//	static KVDetector *MakeDetector(const Char_t * name, Float_t thick);
//	virtual void AddToGeometry();


//   virtual Double_t GetEIncOfMaxDeltaE(Int_t Z, Int_t A);
//   virtual Double_t GetDeltaE(Int_t Z, Int_t A, Double_t Einc);
//   virtual Double_t GetTotalDeltaE(Int_t Z, Int_t A, Double_t Einc);
//   virtual Double_t GetERes(Int_t Z, Int_t A, Double_t Einc);
//   virtual Double_t GetIncidentEnergy(Int_t Z, Int_t A, Double_t delta_e =
//                              -1.0, enum KVIonRangeTable::SolType type = KVIonRangeTable::kEmax);
//   /*virtual Double_t GetEResFromDeltaE(...)  - DON'T IMPLEMENT, CALLS GETINCIDENTENERGY*/
//   virtual Double_t GetDeltaEFromERes(Int_t Z, Int_t A, Double_t Eres);
//   virtual Double_t GetIncidentEnergyFromERes(Int_t Z, Int_t A, Double_t Eres);
//   virtual Double_t GetRange(Int_t Z, Int_t A, Double_t Einc);
//   virtual Double_t GetLinearRange(Int_t Z, Int_t A, Double_t Einc);   
//   virtual Double_t GetPunchThroughEnergy(Int_t Z, Int_t A);
//	virtual TGraph* DrawPunchThroughEnergyVsZ(Int_t massform=KVNucleus::kBetaMass);
//	virtual TGraph* DrawPunchThroughEsurAVsZ(Int_t massform=KVNucleus::kBetaMass);
//   
//   virtual TF1* GetEResFunction(Int_t Z, Int_t A);
//   virtual TF1* GetELossFunction(Int_t Z, Int_t A);
//	virtual TF1* GetRangeFunction(Int_t Z, Int_t A);
//   
//   virtual Double_t GetSmallestEmaxValid(Int_t Z, Int_t A);
//   
//   
//   virtual void ReadDefinitionFromFile(const Char_t*);
//   
//   virtual TList* GetAlignedDetectors(UInt_t direction = /*KVGroup::kBackwards*/ 1);
//   void ResetAlignedDetectors(UInt_t direction = /*KVGroup::kBackwards*/ 1);
//   
//	virtual void DeduceACQParameters(Int_t zz=-1,Int_t aa=-1){};










   ClassDef(KVSpectroDetector,1)//Base class for the description of detectors in spectrometer
};

#endif
