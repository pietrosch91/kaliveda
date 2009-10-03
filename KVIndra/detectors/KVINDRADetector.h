//Created by KVClassFactory on Sat Oct  3 14:18:09 2009
//Author: John Frankland,,,

#ifndef __KVINDRADETECTOR_H
#define __KVINDRADETECTOR_H

#include "KVDetector.h"

class KVINDRADetector : public KVDetector
{

   public:
   KVINDRADetector();
   virtual ~KVINDRADetector();

   const Char_t *GetArrayName();
	
   virtual void AddACQParamType(const Char_t * type);
   virtual KVACQParam *GetACQParam(const Char_t*/*type*/);

   ClassDef(KVINDRADetector,1)//Detectors of INDRA array
};

#endif
