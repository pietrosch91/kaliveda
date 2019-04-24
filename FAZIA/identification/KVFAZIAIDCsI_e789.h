//Created by KVClassFactory on Tue Sep  8 16:00:28 2015
//Author: ,,,

#ifndef __KVFAZIAIDCsI_e789_H
#define __KVFAZIAIDCsI_e789_H

#include "KVFAZIAIDTelescope.h"
#include "KVIDGCsI.h"

class KVFAZIADetector;

class KVFAZIAIDCsI_e789 : public KVFAZIAIDTelescope {

protected:
   KVIDGCsI* CsIGrid;//! telescope's grid
   KVFAZIADetector* fCsI;//!

public:
   KVFAZIAIDCsI_e789();
   virtual ~KVFAZIAIDCsI_e789();

   virtual UShort_t GetIDCode()
   {
      return kCsI;
   };
   virtual Bool_t Identify(KVIdentificationResult*, Double_t x = -1., Double_t y = -1.);

   Double_t GetIDMapX(Option_t* opt = "");
   Double_t GetIDMapY(Option_t* opt = "");

   virtual void Initialize();
   virtual Bool_t CanIdentify(Int_t Z, Int_t /*A*/)
   {
      // Used for filtering simulations
      // Returns kTRUE if this telescope is theoretically capable of identifying a given nucleus,
      // without considering thresholds etc.
      // For CsI Rapide-Lente detectors, identification is typically possible up to Z=4
      return (Z < 5);
   }

   ClassDef(KVFAZIAIDCsI_e789, 1) //id telescope to manage FAZIA CsI identification
};

#endif
