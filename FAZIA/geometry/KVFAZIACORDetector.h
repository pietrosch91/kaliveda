//Created by KVClassFactory on Tue Aug 29 16:12:18 2017
//Author: gruyer diego

#ifndef __KVFAZIACORDETECTOR_H
#define __KVFAZIACORDETECTOR_H

#include "KVFAZIADetector.h"

class KVFAZIACORDetector : public KVFAZIADetector {
public:
   KVFAZIACORDetector();
   KVFAZIACORDetector(const Char_t* type, const Float_t thick = 0.0);

   virtual void SetCalibrators();

   virtual ~KVFAZIACORDetector();

   ClassDef(KVFAZIACORDetector, 1) //re-implementation due to special CSI calibration
};

#endif
