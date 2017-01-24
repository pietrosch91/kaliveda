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
   KVKinematicalFrame(KVParticle*, const KVFrameTransform&);
   KVKinematicalFrame(const KVFrameTransform& trans, const KVParticle* original);
   KVKinematicalFrame(const KVKinematicalFrame&);
   virtual ~KVKinematicalFrame() {}

   KVParticle* GetParticle() const
   {
      return fParticle.get();
   }
   const KVParticle* operator->() const
   {
      return (const KVParticle*)GetParticle();
   }
   void ReapplyTransform(const KVParticle* original);
   void ApplyTransform(const KVParticle* original, const KVFrameTransform& trans);

   const KVFrameTransform& GetTransform() const
   {
      return fTransform;
   }
   void SetTransform(const KVFrameTransform& f)
   {
      fTransform = f;
   }

   ClassDef(KVKinematicalFrame, 0) //Handle representation of a particle in different kinematical frames
};

#endif
