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
   TString fLocalMaterialsDirectory;

   Bool_t init_materials() const;
   Bool_t CheckMaterialsList() const
   {
      if (!fMaterials) return init_materials();
      return kTRUE;
   };
   KVIonRangeTableMaterial* GetMaterialWithNameOrType(const Char_t* material) const;
   static Bool_t fgNewRangeInversion;// static flag for using new KVedaLossInverseRangeFunction

   void AddMaterial(KVIonRangeTableMaterial*) const;

public:
   KVedaLoss();
   virtual ~KVedaLoss();


   void Print(Option_t* = "") const;
   TObjArray* GetListOfMaterials();

   static void SetIgnoreEnergyLimits(Bool_t yes = kTRUE);

   static void SetUseNewRangeInversion(Bool_t yes = kTRUE)
   {
      fgNewRangeInversion = yes;
   }
   static Bool_t IsUseNewRangeInversion()
   {
      return fgNewRangeInversion;
   }

   Bool_t CheckIon(Int_t Z, Int_t A) const;
   Bool_t ReadMaterials(const Char_t* path) const;

   KVIonRangeTableMaterial* AddElementalMaterial(Int_t Z, Int_t A = 0) const;
   Bool_t AddRANGEMaterial(const Char_t* name) const;
   KVIonRangeTableMaterial* AddCompoundMaterial(
      const Char_t* /*name*/, const Char_t* /* symbol */,
      Int_t /* nelem */, Int_t* /* z */, Int_t* /* a */, Int_t* /* natoms */, Double_t /* density */ = -1.0) const;
   KVIonRangeTableMaterial* AddMixedMaterial(
      const Char_t* /* name */, const Char_t* /* symbol */,
      Int_t /* nelem */, Int_t* /* z */, Int_t* /* a */, Int_t* /* natoms */, Double_t* /* proportion */,
      Double_t /* density */ = -1.0) const;
   ClassDef(KVedaLoss, 1) //C++ implementation of VEDALOSS stopping power calculation
};

#endif
