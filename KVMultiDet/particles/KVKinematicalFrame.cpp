//Created by KVClassFactory on Thu Jan 19 17:29:54 2017
//Author: John Frankland,,,

#include "KVKinematicalFrame.h"
#include "TClass.h"

ClassImp(KVKinematicalFrame)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVKinematicalFrame</h2>
<h4>Kinematical representation of a particle in a different frame</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVKinematicalFrame::KVKinematicalFrame(const Char_t* name, const KVParticle* original, const KVFrameTransform& trans)
   : TNamed(name, "Kinematical frame"), fTransform(trans), fParticle((KVParticle*)original->IsA()->New())
{
   // Create representation of original particle in transformed frame

   ReapplyTransform(original);
}

void KVKinematicalFrame::ReapplyTransform(const KVParticle* original)
{
   // Apply stored kinematical transformation to the particle

   original->Copy(*(fParticle.get()));   //copy all information on particle
   fParticle->Transform(fTransform.Inverse());
}

void KVKinematicalFrame::ApplyTransform(const KVParticle* original, const KVFrameTransform& trans)
{
   // Apply new kinematical transformation to the particle
   fTransform = trans;
   ReapplyTransform(original);
}
