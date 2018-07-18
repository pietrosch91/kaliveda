#ifndef KVFAZIAGROUPRECONSTRUCTOR_H
#define KVFAZIAGROUPRECONSTRUCTOR_H

#include "KVGroupReconstructor.h"

class KVFAZIAGroupReconstructor : public KVGroupReconstructor {
protected:
   //KVReconstructedNucleus*ReconstructTrajectory(const KVGeoDNTrajectory* traj, const KVGeoDetectorNode* node);
//   void IdentifyParticle(KVReconstructedNucleus& PART){cout << "KVFAZIAGroupReconstructor::IdentifyParticle() called" << endl;}

   void CalibrateParticle(KVReconstructedNucleus* PART);
   void PostReconstructionProcessing();

public:
   KVFAZIAGroupReconstructor() {}
   virtual ~KVFAZIAGroupReconstructor() {}

   //void Identify();

   ClassDef(KVFAZIAGroupReconstructor, 1) // Reconstruct particles in FAZIA telescopes
};

#endif // KVFAZIAGROUPRECONSTRUCTOR_H
