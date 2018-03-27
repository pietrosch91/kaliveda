//Created by KVClassFactory on Thu Jan 19 15:37:43 2017
//Author: John Frankland,,,

#include "KVFrameTransform.h"
#include "KVParticle.h"

ClassImp(KVFrameTransform)

////////////////////////////////////////////////////////////////////////////////
/*
<h2>KVFrameTransform</h2>
<h4>Utility class for kinematical transformations of KVParticle class</h4>

~~~~~~~~~~
 N.B.: for boosts, default units are cm/ns
       for rotations (TRotation), default units are radians
~~~~~~~~~~

 This class is used as a generic argument for the KVEvent::SetFrame and
 KVParticle::SetFrame methods (amongst others). It is not always necessary to
 explicitly use this class when calling these methods. Any class for which
 a single-argument KVFrameTransform constructor is defined can be used, even
 a temporary (unnamed) object:

  e.g. for KVParticle::SetFrame(const Char_t* frame, const KVFrameTransform& ft)
       the following calls are valid:

~~~~~~~~~~
          KVParticle p;
          TVector3 boost(0,0,3.0);// in cm/ns
          p.SetFrame("toto", boost);
          p.SetFrame("toto", TVector3(0,0,3.0)); // same effect as previous
          TRotation rot;
          rot.RotateZ(TMath::PiOver2()); // 90 deg. rotation around beam
          p.SetFrame("titi", rot);
~~~~~~~~~~

 On the other hand, when more than one argument to the KVFrameTransform constructor
 is required, you have to explicitly call the constructor in question, although
 once again it is sufficient to use a temporary object in the method call:

~~~~~~~~~~
          p.SetFrame("tata", KVFrameTransform(TVector3(0,0,0.1),kTRUE));
           --> boost to frame moving at 0.1c in beam direction

          p.SetFrame("tutu", KVFrameTransform(rot, TVector3(0,0,0.1), kTRUE));
           --> rotation around beam axis followed by boost 0.1c
~~~~~~~~~~

*/
////////////////////////////////////////////////////////////////////////////////

KVFrameTransform::KVFrameTransform(const TVector3& boost, Bool_t beta)
   : TLorentzRotation()
{
   // Construct frame transformation using velocity boost vector
   //   if beta=kTRUE, velocity given in light speed units
   //   if beta=kFALSE [default], velocity given in cm/ns units.
   if (beta) {
      Boost(boost);
   } else {
      Boost(boost.X() / KVParticle::C(), boost.Y() / KVParticle::C(),
            boost.Z() / KVParticle::C());
   }
}

//____________________________________________________________________________//

KVFrameTransform::KVFrameTransform(const TRotation& arg1)
   : TLorentzRotation(arg1)
{
   // Construct frame transformation using rotation of axes
}

//____________________________________________________________________________//

KVFrameTransform::KVFrameTransform(const TLorentzRotation& r)
   : TLorentzRotation(r)
{
   // Construct frame transformation using a TLorentzRotation
}

KVFrameTransform::KVFrameTransform(const KVFrameTransform& r)
   : TLorentzRotation((const TLorentzRotation&)r)
{
   // Copy constructor
}

//____________________________________________________________________________//

KVFrameTransform::KVFrameTransform(const TVector3& boost, const TRotation& r, Bool_t beta)
   : TLorentzRotation()
{
   // Construct frame transformation using rotation & velocity boost vector
   // in the order: BOOST then ROTATION
   //   if beta=kTRUE, velocity given in light speed units
   //   if beta=kFALSE [default], velocity given in cm/ns units.
   if (beta) {
      Boost(boost);
   } else {
      Boost(boost.X() / KVParticle::C(), boost.Y() / KVParticle::C(),
            boost.Z() / KVParticle::C());
   }
   Transform(r);
}

KVFrameTransform::KVFrameTransform(const TRotation& r, const TVector3& boost, Bool_t beta)
   : TLorentzRotation(r)
{
   // Construct frame transformation using rotation & velocity boost vector
   // in the order: ROTATION then BOOST
   //   if beta=kTRUE, velocity given in light speed units
   //   if beta=kFALSE [default], velocity given in cm/ns units.
   if (beta) {
      Boost(boost);
   } else {
      Boost(boost.X() / KVParticle::C(), boost.Y() / KVParticle::C(),
            boost.Z() / KVParticle::C());
   }
}
