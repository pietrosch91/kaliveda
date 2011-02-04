//Created by KVClassFactory on Wed Feb  2 15:49:27 2011
//Author: frankland,,,,

#ifndef __KVEDALOSS_H
#define __KVEDALOSS_H

#include "KVIonRangeTable.h"
#include "KVHashList.h"

class KVedaLossMaterial;

class KVedaLoss : public KVIonRangeTable {
   static KVHashList* fMaterials;// static list of all known materials

   Bool_t init_materials() const;
   Bool_t CheckMaterialsList() const {
      if (!fMaterials) return init_materials();
      return kTRUE;
   };

public:
   KVedaLoss();
   virtual ~KVedaLoss();

   Bool_t IsMaterialKnown(const Char_t* material);
   Bool_t IsMaterialGas(const Char_t* material);

   KVedaLossMaterial* GetMaterial(const Char_t* material);
   const Char_t* GetMaterialName(const Char_t* material);

   Double_t GetDensity(const Char_t* material);
   void SetTemperatureAndPressure(const Char_t*material, Double_t temperature, Double_t pressure);
   Double_t GetZ(const Char_t* material);
   Double_t GetAtomicMass(const Char_t* material);

   void Print(Option_t* = "") const;

   virtual Double_t GetRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);

   virtual Double_t GetDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);

   ClassDef(KVedaLoss, 1) //C++ implementation of VEDALOSS stopping power calculation
};

#endif
