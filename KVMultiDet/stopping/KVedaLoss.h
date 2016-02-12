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
   Bool_t CheckMaterialsList() const
   {
      if (!fMaterials) return init_materials();
      return kTRUE;
   };
   KVIonRangeTableMaterial* GetMaterialWithNameOrType(const Char_t* material);
   static Bool_t fgNewRangeInversion;// static flag for using new KVedaLossInverseRangeFunction

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

   ClassDef(KVedaLoss, 1) //C++ implementation of VEDALOSS stopping power calculation
};

#endif
