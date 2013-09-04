//Created by KVClassFactory on Mon Jul  8 15:40:35 2013
//Author: Guilain ADEMARD

#ifndef __KVVAMOSRECONGEONAVIGATOR_H
#define __KVVAMOSRECONGEONAVIGATOR_H

#include "KVGeoNavigator.h"

class KVVAMOSReconNuc;
class KVIonRangeTable;

class KVVAMOSReconGeoNavigator : public KVGeoNavigator
{

	Bool_t fForward; // 'true/false' for forward/backward propagation
	TVector3 fOrigine;// Origine of the propagation i.e. coord. intersection point of the trajectory and the focal plan
 	KVIonRangeTable* fRangeTable;

   public:
   KVVAMOSReconGeoNavigator(TGeoManager*, KVIonRangeTable*);
   virtual ~KVVAMOSReconGeoNavigator();

   void ParticleEntersNewVolume(KVNucleus *nuc);

   void PropagateNucleus(KVVAMOSReconNuc*);

   ClassDef(KVVAMOSReconGeoNavigator,1)//Propagate nuclei through the geometry of VAMOS for their reconstruction/calibrate/identification
};

#endif
