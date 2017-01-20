//Created by KVClassFactory on Thu Jan 19 17:29:54 2017
//Author: John Frankland,,,

#ifndef __KVKINEMATICALFRAME_H
#define __KVKINEMATICALFRAME_H

#include "TNamed.h"
#include "KVParticle.h"
#include "KVFrameTransform.h"

class KVKinematicalFrame : public TNamed {
   KVFrameTransform       fTransform;    //! kinematical transform wrt 'parent' frame
   unique_ptr<KVParticle> fParticle;     //! kinematically transformed particle

public:
   KVKinematicalFrame(const Char_t* name, const KVParticle* original, const KVFrameTransform& trans);
   virtual ~KVKinematicalFrame() {}

   KVParticle* GetParticle() const
   {
      return fParticle.get();
   }
   void ReapplyTransform(const KVParticle* original);
   void ApplyTransform(const KVParticle* original, const KVFrameTransform& trans);

   ClassDef(KVKinematicalFrame, 0) //Kinematical representation of a particle in a different frame
};

#endif
