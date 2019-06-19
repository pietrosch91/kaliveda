//Created by KVClassFactory on Wed Jun 19 15:54:49 2019
//Author: John Frankland,,,

#ifndef __KVDATAPATCH_CAMP1MASSID_H
#define __KVDATAPATCH_CAMP1MASSID_H

#include "KVDataPatch.h"

class KVDataPatch_Camp1MassID : public KVDataPatch {
public:
   KVDataPatch_Camp1MassID();
   virtual ~KVDataPatch_Camp1MassID();

   virtual Bool_t IsRequired(TString dataset, TString datatype, Int_t,
                             TString, Int_t, const TList*)
   {
      return (dataset == "INDRA_camp1" && datatype == "root");
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

   ClassDef(KVDataPatch_Camp1MassID, 1) //Correct mass identification in 1st campaign data
};

#endif
