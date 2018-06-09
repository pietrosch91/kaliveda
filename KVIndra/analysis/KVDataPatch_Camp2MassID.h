//Created by KVClassFactory on Sat Jun  9 20:07:03 2018
//Author: John Frankland,,,

#ifndef __KVDATAPATCH_CAMP2MASSID_H
#define __KVDATAPATCH_CAMP2MASSID_H

#include "KVDataPatch.h"

class KVDataPatch_Camp2MassID : public KVDataPatch {
public:
   KVDataPatch_Camp2MassID();
   virtual ~KVDataPatch_Camp2MassID();
   virtual Bool_t IsRequired(TString dataset, TString datatype, Int_t,
                             TString, Int_t, const TList*)
   {
      return (dataset == "INDRA_camp2" && datatype == "root");
   }
   virtual Bool_t IsEventPatch()
   {
      return kFALSE;
   }
   virtual Bool_t IsParticlePatch()
   {
      return kTRUE;
   }
   virtual void ApplyToEvent(KVEvent*) {}
   virtual void ApplyToParticle(KVNucleus*);

   virtual void PrintPatchInfo() const;

   ClassDef(KVDataPatch_Camp2MassID, 1) //Patch to correct 2nd campaign A-identification status
};

#endif
