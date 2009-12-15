//Created by KVClassFactory on Sat Oct  3 14:18:09 2009
//Author: John Frankland,,,

#ifndef __KVINDRADETECTOR_H
#define __KVINDRADETECTOR_H

#include "KVDetector.h"
#include "KVTelescope.h"

class KVINDRADetector : public KVDetector
{

public:
    KVINDRADetector();
    virtual ~KVINDRADetector();
    KVINDRADetector(const Char_t * type, const Float_t thick = 0.0)
            : KVDetector(type,thick)
    {
    };

    const Char_t *GetArrayName();
   virtual UInt_t GetRingNumber() const {
   	return (GetTelescope() ? GetTelescope()->GetRingNumber() : 0);
   	};
   virtual UInt_t GetModuleNumber() const {
      return GetTelescopeNumber();
   };

    virtual void AddACQParamType(const Char_t * type);
    virtual KVACQParam *GetACQParam(const Char_t*/*type*/);

    ClassDef(KVINDRADetector,1)//Detectors of INDRA array
};

#endif
