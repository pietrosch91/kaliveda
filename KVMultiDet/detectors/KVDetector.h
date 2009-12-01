/***************************************************************************
                          kvdetector.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVDetector.h,v 1.71 2009/05/22 14:45:40 ebonnet Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVDETECTOR_H
#define KVDETECTOR_H

#ifndef KVD_RECPRC_CNXN
#define KVD_RECPRC_CNXN 1
#endif
#ifndef KVD_NORECPRC_CNXN
#define KVD_NORECPRC_CNXN 0
#endif


#define KVDETECTOR_UNKNOWN_MATERIAL "Constructor called with unknown material"
#define KVDETECTOR_UNKNOWN_DETECTOR "Constructor called with unknown detector prototype"
#define KVDETECTOR_NOT_IN_TELESCOPE "Detector not associated to any telescope: position undefined"
#define KVDETECTOR_ADD_TO_UNKNOWN_TELESCOPE "Pointer to telescope is null."

//for energies less than this (MeV) particles are considered to be stopped
#define KVDETECTOR_MINIMUM_E 0.001

#include "KVMaterial.h"
#include "TRef.h"
#include "KVList.h"
#include "KVRList.h"
#include "KVNucleus.h"
#include "KVACQParam.h"
#include "Binary_t.h"

class KVDetectorBrowser;
class KVModule;
class KVTelescope;
class KVCalibrator;
class KVGroup;
class KVIDTelescope;
class TGeoVolume;

Double_t ELossActive(Double_t * x, Double_t * par);
Double_t EResDet(Double_t * x, Double_t * par);

class KVDetector:public KVMaterial {

   friend class KVDetectorBrowser;

 private:
    KVDetectorBrowser * fBrowser;       //! GUI for viewing and modifying characteristics
   Short_t fActiveLayer;        //The active absorber in the detector
   KVTelescope *fTelescope;     //reference to telescope to which detector belongs
   KVList *fIDTelescopes;       //->list of ID telescopes to which detector belongs
   KVList *fIDTelAlign;         //->list of ID telescopes made of this detector and all aligned detectors placed in front of it
   TList *fIDTele4Ident;  //!list of ID telescopes used for particle ID

   Bool_t fReanalyse;           //!     flag indicating that this detector should be reanalysed for particle reconstruction
   Double_t fECalc;             //! calculated energy loss used in reanalysis (after subtraction of other particles' contributions)

   enum {
      kIsAnalysed = BIT(14),    //for reconstruction of particles
      kActiveSet = BIT(15),     //internal - flag set true when SetActiveLayer called
      kUnidentifiedParticle = BIT(16),  //set if detector is in an unidentified particle's list
      kIdentifiedParticle = BIT(17),    //set if detector is in an identified particle's list
      kIsBeingDeleted = BIT(18) //set when clearing ID telescopes list in destructor
   };

   Int_t fIdentP;               //! temporary counters, determine state of identified/unidentified particle flags
   Int_t fUnidentP;             //! temporary counters, determine state of identified/unidentified particle flags

 protected:
Int_t npar_loss;//!number of params for eloss function
Int_t npar_res;//!number of params for eres function
Double_t *par_loss;//!array of params for eloss function
Double_t *par_res;//!array of params for eres function

   TString fFName;              //!dynamically generated full name of detector
   KVList *fModules;            //references to connected electronic modules (not implemented yet)
   KVList *fCalibrators;        //list of associated calibrator objects
   KVList *fACQParams;          //list of raw data parameters read from coders
   KVRList *fParticles;         //list of particles hitting detector in an event
   KVList *fAbsorbers;          //->list of absorbers making up the detector
   UShort_t fSegment;           //used in particle reconstruction
   Float_t fGain;               //gain of amplifier
   Int_t fCalWarning;           //!just a counter so that missing calibrator warning is given only once

	Double_t fTotThickness; //! used to store value calculated by GetTotalThicknessInCM
	Double_t fDepthInTelescope; //! used to store depth of detector in parent telescope

	Binary8_t  fFiredMask;//bitmask used by Fired to determine which parameters to take into account

 public:
    KVDetector();
    KVDetector(const Char_t * type, const Float_t thick = 0.0);
    KVDetector(const KVDetector &);
   void init();
    virtual ~ KVDetector();

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif

   virtual void SetMaterial(const Char_t * type);
   void AddAbsorber(KVMaterial *);
   void SetActiveLayer(KVMaterial *);
   void SetActiveLayer(Short_t actif) {
      fActiveLayer = actif;
      SetBit(kActiveSet);
   };
   KVMaterial *GetActiveLayer() const;
   KVMaterial *GetAbsorber(Int_t i) const;
   KVMaterial *GetAbsorber(const Char_t*) const;
   KVList* GetListOfAbsorbers() const
   {
   	return fAbsorbers;
	};
   virtual const Char_t *GetArrayName();

	Double_t GetTotalThicknessInCM()
	{
		// Calculate and return the total thickness of ALL absorbers making up the detector,
		// not just the active layer (as for GetThickness()).
		//
		// As different materials can have different default units for thickness, we convert
		// everything into centimetres and return the total thickness with these units.
		fTotThickness=0;
		TIter next(fAbsorbers); KVMaterial* mat;
		while( (mat = (KVMaterial*)next()) ) fTotThickness += mat->GetThicknessInCM();
		return fTotThickness;
	};

   const Char_t *GetMaterialName() const {
      if (GetActiveLayer())
         return GetActiveLayer()->GetName();
      return KVMaterial::GetName();
   };
   virtual void DetectParticle(KVNucleus *, TVector3 * norm = 0);
   virtual Double_t GetELostByParticle(KVNucleus *, TVector3 * norm = 0);
   virtual Double_t GetParticleEIncFromERes(KVNucleus * , TVector3 * norm = 0);

   virtual void AddToTelescope(KVTelescope * T, const int =
                               KVD_RECPRC_CNXN);
   KVTelescope *GetTelescope() const;
   void SetTelescope(KVTelescope * kvt);
   Float_t GetTheta() const;
   Float_t GetPhi() const;

   void ConnectModule(KVModule *, const int fcon = KVD_RECPRC_CNXN);

   void SetGain(Float_t gain);
   Float_t GetGain() const;

   virtual Double_t GetEnergy();
   virtual void SetEnergy(Double_t e);
   virtual Double_t GetEnergyLoss() {
      return GetEnergy();
   };
   virtual void SetEnergyLoss(Double_t e) {
      SetEnergy(e);
   };
   virtual Double_t GetCorrectedEnergy(UInt_t z, UInt_t a, Double_t e =
                                       -1., Bool_t transmission=kTRUE);
   virtual UInt_t FindZmin(Double_t ELOSS = -1., Char_t mass_formula = -1);

   void AddACQParam(KVACQParam*);
   virtual KVACQParam *GetACQParam(const Char_t*/*name*/);
   KVList *GetACQParamList() {
      return fACQParams;
   };
   virtual Float_t GetACQData(const Char_t*/*name*/);
   virtual Float_t GetPedestal(const Char_t*/*name*/);
   virtual void SetPedestal(const Char_t*/*name*/, Float_t);

   virtual UInt_t GetRingNumber() const;
   virtual UInt_t GetTelescopeNumber() const;
   virtual UInt_t GetModuleNumber() const {
      return GetTelescopeNumber();
   };

   Bool_t AddCalibrator(KVCalibrator * cal);
   KVCalibrator *GetCalibrator(const Char_t * name,
                               const Char_t * type) const;
   KVCalibrator *GetCalibrator(const Char_t * type) const;
   KVList *GetListOfCalibrators() const {
      return fCalibrators;
   };
   KVGroup *GetGroup() const;
   virtual Bool_t IsCalibrated() const;

   virtual void Clear(Option_t * opt = "");
   virtual void Reset() {
      Clear();
   };
   virtual void Print(Option_t * option = "") const;

   virtual void StartBrowser();
   virtual void CloseBrowser();

   // Add to the list of particles hitting this detector in an event
   void AddHit(KVNucleus * part)
   {
      if (!fParticles) fParticles = new KVRList();
      fParticles->Add(part);
   };

   // Return the list of particles hitting this detector in an event
   KVRList *GetHits() const
   {
      return fParticles;
   };

   // Return the number of particles hitting this detector in an event
   Int_t GetNHits() const
   {
      return (fParticles ? fParticles->GetSize() : 0);
   };

   inline UShort_t GetSegment() const;
   inline void SetSegment(UShort_t s);
   Bool_t IsAnalysed() {
      return TestBit(kIsAnalysed);
   };
   void SetAnalysed(Bool_t b = kTRUE) {
      SetBit(kIsAnalysed, b);
   };
   inline virtual Bool_t Fired(Option_t * opt = "any");
   inline virtual Bool_t FiredP(Option_t * opt = "any");

   virtual void SetACQParams();
   virtual void SetCalibrators();
   virtual void RemoveCalibrators();

   virtual void AddIDTelescope(KVIDTelescope * idt);
   KVList *GetIDTelescopes();
   KVList *GetAlignedIDTelescopes();
   TList *GetTelescopesForIdentification();
   void GetAlignedIDTelescopes(TCollection * list);

   Bool_t Reanalyse() const {
      return fReanalyse;
   };
   void SetECalc(Double_t);
   Double_t GetECalc() const
   {
      //Returns calculated energy loss in detector after subtraction of the
      //contributions from already-identified particles (see KVDetector::SetReanalyse)
      return fECalc;
   };
   virtual Short_t GetCalcACQParam(KVACQParam*) const
   {
      // Calculates & returns value of given acquisition parameter corresponding to the
      // current value of fEcalc, i.e. the calculated residual energy loss in the detector
      // after subtraction of calculated energy losses corresponding to each identified
      // particle crossing this detector.
      //
      // This needs to be implemented in child classes. Basically it requires inversion
      // of thecalibration of the detector.
      return 0;
   };

   void SetReanalyse(Bool_t flag=kTRUE)
   {
      //Used in particle reconstruction.
      //If the primary identification of a particle passing through this detector
      //reveals an inconsistency between the measured energy loss and the calculated
      //energy loss of the particle, then this may mean that another particle
      //also passed through or stopped in this detector.
      //In this case the fECalc member should be set to the calculated energy loss
      //in the detector after subtraction of the contributions from already-identified
      //particles (see KVDetector::SetECalc).
      //The "reanalysis" flag is set to true by calling SetECalc (SetReanalyse(kTRUE)).
      //A secondary identification procedure should then be used to identify particles
      //based on the calculated residual energy losses.

      fReanalyse = flag;
   };

   inline void IncrementUnidentifiedParticles(Int_t n = 1) {
      fUnidentP += n;
      fUnidentP = (fUnidentP > 0) * fUnidentP;
      SetBit(kUnidentifiedParticle, (Bool_t) (fUnidentP > 0));
   };
   inline void IncrementIdentifiedParticles(Int_t n = 1) {
      fIdentP += n;
      fIdentP = (fIdentP > 0) * fIdentP;
      SetBit(kIdentifiedParticle, (Bool_t) (fIdentP > 0));
   };
   Bool_t BelongsToUnidentifiedParticle() const {
      return TestBit(kUnidentifiedParticle);
   };
   Bool_t BelongsToIdentifiedParticle() const {
      return TestBit(kIdentifiedParticle);
   };

   Bool_t IsBeingDeleted() {
      return TestBit(kIsBeingDeleted);
   };

   virtual void SetEResParams(Int_t Z, Int_t A);
   virtual void SetELossParams(Int_t Z, Int_t A);

	static KVDetector *MakeDetector(const Char_t * name, Float_t thick);
   const TVector3& GetNormal();

	virtual TGeoVolume* GetGeoVolume();
	virtual void AddToGeometry();
	virtual void GetVerticesInOwnFrame(TVector3 */*corners[8]*/, Double_t /*depth*/, Double_t /*layer_thickness*/);
	virtual Double_t GetEntranceWindowSurfaceArea();

	virtual void SetFiredBitmask();
	Binary8_t GetFiredBitmask() const { return fFiredMask; };

	ClassDef(KVDetector, 7)      //Base class for the description of detectors in multidetector arrays
};

inline KVCalibrator *KVDetector::GetCalibrator(const Char_t * name,
                                               const Char_t * type) const
{
   if (fCalibrators)
      return (KVCalibrator *) fCalibrators->FindObjectWithNameAndType(name, type);
   return 0;
}

inline KVCalibrator *KVDetector::GetCalibrator(const Char_t * type) const
{
   if (fCalibrators)
      return (KVCalibrator *) fCalibrators->FindObjectByType(type);
   return 0;
}

inline UShort_t KVDetector::GetSegment() const
{
   //used in reconstruction of particles
   return fSegment;
}

inline void KVDetector::SetSegment(UShort_t s)
{
   //set segmentation level - used in reconstruction of particles
   fSegment = s;
}

inline void KVDetector::SetGain(Float_t gain)
{
   fGain = gain;
}

inline Float_t KVDetector::GetGain() const
{
   return fGain;
}

//_________________________________________________________________________________

Bool_t KVDetector::Fired(Option_t * opt)
{
   //opt="any" (default):
   //Returns true if ANY* of the working acquisition parameters associated with the detector were fired in an event
   //opt="all" :
   //Returns true if ALL* of the working acquisition parameters associated with the detector were fired in an event
   //opt="Pany" :
   //Returns true if ANY* of the working acquisition parameters associated with the detector were fired in an event
   //and have a value greater than their pedestal value
   //opt="Pall" :
   //Returns true if ALL* of the working acquisition parameters associated with the detector were fired in an event
   //and have a value greater than their pedestal value
   //
   // *the actual parameters taken into account can be fine tuned using environment variables such as
   //          KVDetector.Fired.ACQParameterList.[type]: PG,GG,T
   // See KVDetector::SetFiredBitmask() for more details.

   if(opt[0]=='P') return FiredP(opt+1);

	Binary8_t event; // bitmask for event
	TIter next(fACQParams);
	KVACQParam* par; Int_t id = 0;
	while( (par = (KVACQParam*)next()) ){
	    if( par->Fired() ) event.SetBit(id);
	    else event.ResetBit(id);
	    id++;
	}
	Binary8_t ok = fFiredMask&event;
	// "all" considered parameters fired if ok == mask
	// "any" considered parameters fired if ok != 0
	if (!strcmp(opt, "all")) 	return (ok == fFiredMask);
	Binary8_t zero="0";
	return (ok != zero);
}
//_________________________________________________________________________________

Bool_t KVDetector::FiredP(Option_t * opt)
{
   //opt="any" :
   //Returns true if ANY* of the working acquisition parameters associated with the detector were fired in an event
   //and have a value greater than their pedestal value
   //opt="all" :
   //Returns true if ALL* of the working acquisition parameters associated with the detector were fired in an event
   //and have a value greater than their pedestal value
   //
   // *the actual parameters taken into account can be fine tuned using environment variables such as
   //          KVDetector.Fired.ACQParameterList.[type]: PG,GG,T
   // See KVDetector::SetFiredBitmask() for more details.

	Binary8_t event; // bitmask for event
	TIter next(fACQParams);
	KVACQParam* par; Int_t id = 0;
	while( (par = (KVACQParam*)next()) ){
	    if( par->Fired("P") ) event.SetBit(id);
	    else event.ResetBit(id);
	    id++;
	}
	Binary8_t ok = fFiredMask&event;
	// "all" considered parameters fired if ok == mask
	// "any" considered parameters fired if ok != 0
	if (!strcmp(opt, "all")) 	return (ok == fFiredMask);
	Binary8_t zero = "0";
	return (ok != zero);
}

#endif
