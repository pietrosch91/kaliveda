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
#include "KVIonRangeTable.h"

class KVNucleus;
class KVTelescope;
class TGeoMedium;

Double_t ELossSaclay(Double_t * x, Double_t * par);
Double_t EResSaclay(Double_t * x, Double_t * par);

class KVMaterial:public KVBase {

 protected:
   static KVIonRangeTable* fIonRangeTable; //  pointer to class used to calculate charged particle ranges & energy losses
   
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
   Int_t fAmasr;                // isotopic mass of element
   UInt_t fUnits;               //used to determine how to calculate effective thickness
   Double_t fThick;              // thickness of absorber
   Double_t fPressure;        // pressure of gas, if this is a gas
   Double_t fTemp;  // temperature of gas, if this is a gas
   Double_t fELoss;             //total of energy lost by all particles traversing absorber
   static Char_t kUnits[5][10]; //!strings for names of units for thickness
      
 public:
   KVMaterial();
   KVMaterial(const Char_t * type, const Double_t thick = 0.0);
   KVMaterial(const KVMaterial &);
   void init();
   virtual ~ KVMaterial();
   void SetMass(Double_t a);
   virtual void SetMaterial(const Char_t * type);
   Double_t GetMass() const;
   Double_t GetZ() const;
   Double_t GetDensity() const;
   void SetThickness(Double_t thick);
   Double_t GetThickness() const;
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

   virtual void SetPressure(Double_t);
   virtual void SetTemperature(Double_t);

   Double_t GetPressure() const;
   Double_t GetTemperature() const;

   Bool_t IsIsotopic() const;
   Bool_t IsNat() const;
   
   virtual const TVector3& GetNormal() {
      // Return vector normal to surface of absorber. For a KVMaterial, this is (0,0,1) as
      // a basic absorber has no orientation. Rederived in child classes KVTarget and KVDetector.
      return fNormToMat;
   };
	
	virtual TGeoMedium* GetGeoMedium(const Char_t* /*med_name*/="");
   
   ClassDef(KVMaterial, 6)      // Class describing physical materials used to construct detectors & targets
};

#endif
