/***************************************************************************
$Id: KVMaterial.h,v 1.31 2008/12/17 13:01:26 franklan Exp $
                          kvmaterial.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr


 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVMATERIAL_H
#define KVMATERIAL_H

#include "RVersion.h"
#include "TRef.h"
#include "KVBase.h"
#include "KVList.h"
#include "TF1.h"
#include "TVector3.h"
#include "Riostream.h"

// warning messages
#define KVMATERIAL_INIT_FILE_NOT_FOUND 	"Initialisation file %s not found. Cannot create detectors"
#define KVMATERIAL_INIT_FILE_HEADER_PROBLEM 	"Problem reading initialisation file %s. Check format of header"
#define KVMATERIAL_INIT_FILE_COEFF_PROBLEM 	"Problem reading initialisation file %s. Check format of coefficients for %s"
#define KVMATERIAL_INIT_FILE_DATA_PROBLEM 	"Problem reading initialisation file %s. Check format of data"
#define KVMATERIAL_TYPE_NOT_FOUND 	"Unknown material type %s. Check file %s for known types"

#define RTT				623.61040
#define ZERO_KELVIN	273.15
#define PERC				0.02

// maximum atomic number included in range tables
#define ZMAX_VEDALOSS 100

class KVNucleus;
class KVTelescope;
class TGeoMedium;

Double_t ELossSaclay(Double_t * x, Double_t * par);
Double_t EResSaclay(Double_t * x, Double_t * par);

class KVMaterial:public KVBase {

 protected:
   TF1 * ELoss;                 //!function calculating energy loss in material from incident energy
   TF1 *ERes;                   //!function calculating residual energy from incident energy
   TVector3 fNormToMat;//!dummy vector for calculating normal to absorber

 public:
    TF1 * GetELossFunction() {
      return ELoss;
   };
   TF1 *GetEResFunction() {
      return ERes;
   };
   virtual void SetELossParams(Int_t Z, Int_t A);
   void GetELossParams(Int_t Z, Int_t A, Double_t * par);
   enum SolType {
      kEmax,
      kEmin
   };
   virtual void SetEResParams(Int_t Z, Int_t A);

   virtual Double_t GetIncidentEnergyFromERes(Int_t Z, Int_t A,
                                              Double_t Eres);

   enum {                       //constants defined for units
      kMBAR,
      kMICRONS,
      kMGCM2,
      kCM,
      kTORR
   };

 private:
   Double_t fAmat;              //atomic mass of material
   Double_t fZmat;              //atomic number of material
   Double_t fDens;              //density of material
   UInt_t fUnits;               //used to determine how to calculate effective thickness
   Double_t fGasThick;          //only used for gases
   Double_t fMoleWt;            //only used for gases
   Double_t fTemp;              //only used for gases
   Double_t fCoeff[ZMAX_VEDALOSS][15];  //[ZMAX_VEDALOSS][15] parameters for range tables
   Float_t fAmasr;              //isotopic mass for material; if not set will use default value
   static TString kDataFilePath;        //! full path to file containing energy loss parameters
   Float_t fEmax[ZMAX_VEDALOSS];        //[ZMAX_VEDALOSS] Z-dependent maximum energy/nucleon for calculation to be valid
   Float_t fEmin[ZMAX_VEDALOSS];        //[ZMAX_VEDALOSS] Z-dependent minimum energy/nucleon for calculation to be valid
   Float_t fThick;              //thickness of absorber
   Double_t fELoss;             //total of energy lost by all particles traversing absorber
   static Char_t kUnits[5][10]; //!strings for names of units for thickness
   TString fState;// state of material = "solid", "liquid", "gas"
      
 public:
   KVMaterial();
   KVMaterial(const Char_t * type, const Float_t thick = 0.0);
   KVMaterial(const KVMaterial &);
   void init();
   virtual ~ KVMaterial();
   static KVList *GetListOfMaterials();
   void SetMass(Float_t a);
   virtual void SetMaterial(const Char_t * type);
   Double_t GetMass() const;
   Double_t GetZ() const;
   Double_t GetDensity() const;
   void SetDensity(Double_t d)
	{
		// Set density of material. Units are g/cm**3
		fDens = d;
	}
   Float_t GetEmaxVedaloss(UInt_t Z) const {
      if (GetActiveLayer())
         return GetActiveLayer()->GetEmaxVedaloss(Z);
      return ((Z <= ZMAX_VEDALOSS && Z > 0) ? fEmax[Z - 1] : 0.0);
   };
   Float_t GetEminVedaloss(UInt_t Z) const {
      if (GetActiveLayer())
         return GetActiveLayer()->GetEminVedaloss(Z);
      return ((Z <= ZMAX_VEDALOSS && Z > 0) ? fEmin[Z - 1] : 0.0);
   };
   void SetThickness(Float_t thick);
   Float_t GetThickness() const;
   Double_t GetThicknessInCM() const;
   Double_t GetEffectiveThickness(TVector3 & norm, TVector3 & direction);

   virtual void DetectParticle(KVNucleus *, TVector3 * norm = 0);
   virtual Double_t GetELostByParticle(KVNucleus *, TVector3 * norm = 0);
   virtual Double_t GetParticleEIncFromERes(KVNucleus * , TVector3 * norm = 0);

   virtual void Print(Option_t * option = "") const;
   virtual Double_t GetEnergyLoss() {
      return fELoss;
   };
   virtual void SetEnergyLoss(Double_t e) {
      fELoss = e;
   };

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif
   virtual void Clear(Option_t * opt = "");

   const Char_t *GetThicknessUnits() const;
   virtual UInt_t GetUnits() const;
   void SetUnits(UInt_t);

   Double_t GetIncidentEnergy(Int_t Z, Int_t A, Double_t delta_e =
                              -1.0, enum SolType type = kEmax);
   Double_t GetDeltaE(Int_t Z, Int_t A, Double_t Einc);
   Double_t GetDeltaEFromERes(Int_t Z, Int_t A, Double_t Eres);
   Double_t GetERes(Int_t Z, Int_t A, Double_t Einc);
   Double_t GetEResFromDeltaE(Int_t Z, Int_t A, Double_t dE =
                              -1.0, enum SolType type = kEmax);
   Double_t GetBraggE(Int_t Z, Int_t A);
   Double_t GetBraggDE(Int_t Z, Int_t A);

   virtual KVMaterial *GetActiveLayer() const {
      return 0;
   };                           //to facilitate polymorphism with KVDetector class

   virtual void SetPressure(Float_t);
   virtual void SetTemperature(Float_t t);

   Float_t GetPressure() const;
   Float_t GetTemperature() const;

   Bool_t IsIsotopic() const;
   Bool_t IsNat() const;
   
   virtual const TVector3& GetNormal() {
      // Return vector normal to surface of absorber. For a KVMaterial, this is (0,0,1) as
      // a basic absorber has no orientation. Rederived in child classes KVTarget and KVDetector.
      return fNormToMat;
   };
	
	virtual TGeoMedium* GetGeoMedium(const Char_t* /*med_name*/="");
   
   ClassDef(KVMaterial, 6)      //Interface to VEDALOSS description of slowing-down of ions in absorbers
};

#endif
