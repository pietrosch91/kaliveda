//Created by KVClassFactory on Thu Jan 19 15:37:43 2017
//Author: John Frankland,,,

#ifndef __KVFRAMETRANSFORM_H
#define __KVFRAMETRANSFORM_H

#include "TLorentzRotation.h"

class KVFrameTransform : public TLorentzRotation {
public:
   KVFrameTransform() {}
   KVFrameTransform(const TVector3& boost, Bool_t beta = kFALSE);
   KVFrameTransform(const TRotation& r);
   KVFrameTransform(const TLorentzRotation& r);
   KVFrameTransform(const KVFrameTransform& r);
   KVFrameTransform(const TVector3& boost, const TRotation& r, Bool_t beta = kFALSE);
   KVFrameTransform(const TRotation& r, const TVector3& boost, Bool_t beta = kFALSE);

   virtual ~KVFrameTransform() {}

   ClassDef(KVFrameTransform, 1) //Utility class for kinematical transformations of KVParticle class
};

#endif
