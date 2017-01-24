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
//
// This class handles transformations between different reference frames for
// KVParticle kinematics. It is a utility class servicing the relevant methods in
// KVParticle.
////////////////////////////////////////////////////////////////////////////////

KVKinematicalFrame::KVKinematicalFrame(const Char_t* name, const KVParticle* original, const KVFrameTransform& trans)
   : TNamed(name, "Kinematical frame"), fTransform(trans), fParticle((KVParticle*)original->IsA()->New())
{
   // Create representation of original particle in transformed frame
   // This frame has a name which can be used to retrieve it from a list

   ReapplyTransform(original);
}

KVKinematicalFrame::KVKinematicalFrame(const KVFrameTransform& trans, const KVParticle* original)
   : TNamed(), fTransform(trans), fParticle((KVParticle*)original->IsA()->New())
{
   // Create representation of original particle in transformed frame

   ReapplyTransform(original);
}

KVKinematicalFrame::KVKinematicalFrame(KVParticle* p, const KVFrameTransform& t)
   : TNamed(), fTransform(t), fParticle(nullptr)
{
   // Modify the kinematics of the particle according to the given transformation
   // Recursively update the kinematics in all frames defined for this particle
   p->Transform(fTransform.Inverse());
   p->UpdateAllFrames();
}

KVKinematicalFrame::KVKinematicalFrame(const KVKinematicalFrame& o)
   : TNamed((const TNamed&)o), fTransform(o.fTransform),
     fParticle(o.GetParticle() ? (KVParticle*)o.GetParticle()->IsA()->New() : nullptr)
{
   // Copy constructor required for rootcint (not rootcling)
}

void KVKinematicalFrame::ReapplyTransform(const KVParticle* original)
{
   // Apply stored kinematical transformation to the particle

   original->Copy(*(fParticle.get()));   //copy all information on particle
   fParticle->Transform(fTransform.Inverse());
}

void KVKinematicalFrame::ApplyTransform(const KVParticle* original, const KVFrameTransform& trans)
{
   // Apply new kinematical transformation to the particle and update all subframes

   fTransform = trans;
   ReapplyTransform(original);
   // recursively update all subframes
   fParticle->UpdateAllFrames();
}
