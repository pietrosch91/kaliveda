//Created by KVClassFactory on Wed Feb  2 15:49:27 2011
//Author: frankland,,,,

#ifndef __KVEDALOSS_H
#define __KVEDALOSS_H

#include "KVBase.h"

class KVedaLoss : public KVBase
{
   static TList* fMaterials;// static list of all known materials
    
   void init_materials();
   Bool_t CheckMaterialsList() const
   {
      if(!fMaterials) init_materials();
   };
   
   public:
   KVedaLoss();
   virtual ~KVedaLoss();

   ClassDef(KVedaLoss,1)//C++ implementation of VEDALOSS stopping power calculation
};

#endif
