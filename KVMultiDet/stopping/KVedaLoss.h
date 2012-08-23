//Created by KVClassFactory on Wed Feb  2 15:49:27 2011
//Author: frankland,,,,

#ifndef __KVEDALOSS_H
#define __KVEDALOSS_H

#include "KVIonRangeTable.h"
#include "KVHashList.h"

class KVedaLossMaterial;
class TGeoMaterial;

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
   TGeoMaterial* GetTGeoMaterial(const Char_t* material);
   const Char_t* GetMaterialName(const Char_t* material);

   virtual Double_t GetEmaxValid(const Char_t* material, Int_t Z, Int_t A);
   
   Double_t GetDensity(const Char_t* material);
   void SetTemperatureAndPressure(const Char_t*material, Double_t temperature, Double_t pressure);
   Double_t GetZ(const Char_t* material);
   Double_t GetAtomicMass(const Char_t* material);

   void Print(Option_t* = "") const;
   TObjArray* GetListOfMaterials();

   virtual Double_t GetRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);

   virtual Double_t GetDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);

   virtual Double_t GetEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.);

   virtual Double_t GetEIncFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.);
   virtual Double_t GetLinearEIncFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.);

   virtual Double_t GetEIncFromDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t DeltaE, Double_t e, enum KVIonRangeTable::SolType type = KVIonRangeTable::kEmax,  Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.);
   virtual Double_t GetLinearEIncFromDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t DeltaE, Double_t e, enum KVIonRangeTable::SolType type = KVIonRangeTable::kEmax, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.);

   virtual Double_t GetDeltaEFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t ERes, Double_t e, Double_t isoAmat = 0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearDeltaEFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t ERes, Double_t e, Double_t isoAmat = 0., Double_t T=-1., Double_t P=-1.);
         
   virtual Double_t GetPunchThroughEnergy(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) ;
   virtual Double_t GetLinearPunchThroughEnergy(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) ;
   
   virtual Double_t GetMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetEIncOfMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearEIncOfMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.);
      
   static void SetIgnoreEnergyLimits(Bool_t yes=kTRUE);
   
   ClassDef(KVedaLoss, 1) //C++ implementation of VEDALOSS stopping power calculation
};

#endif
