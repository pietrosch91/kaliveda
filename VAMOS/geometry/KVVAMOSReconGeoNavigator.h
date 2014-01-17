//Created by KVClassFactory on Mon Jul  8 15:40:35 2013
//Author: Guilain ADEMARD

#ifndef __KVVAMOSRECONGEONAVIGATOR_H
#define __KVVAMOSRECONGEONAVIGATOR_H

#include "KVGeoNavigator.h"

class KVVAMOSReconNuc;
class KVIonRangeTable;

class KVVAMOSReconGeoNavigator : public KVGeoNavigator
{
	public:

		enum ECalib{ 
			kNoCalib  = 0,
 		   	kECalib   = BIT(0),
			kTCalib   = BIT(1),
 		   	kFullCalib = kECalib | kTCalib
		};

	protected:

		Bool_t fDoNothing;// true if nothing has to be done when nucleus enters in new volume
		ECalib fCalib; // 0: No calib., 1: E calib., 2: T calib., 3: full calib.
		TVector3 fOrigine;// Origine of the initial propagation i.e. coord. intersection point of the trajectory and the focal plane
		TVector3 fFOrigine;// Origine of the forward propagation i.e. coord. of a point before the first detector at the focal plane
		Double_t fE; // Energy of the propagated nucleus
 		KVIonRangeTable* fRangeTable;

	public:

      	KVVAMOSReconGeoNavigator(TGeoManager*, KVIonRangeTable*);
   		virtual ~KVVAMOSReconGeoNavigator();

   		void ParticleEntersNewVolume(KVNucleus *nuc);

   		void PropagateNucleus(KVVAMOSReconNuc*, ECalib cal = kNoCalib);

   		ClassDef(KVVAMOSReconGeoNavigator,1)//Propagate nuclei through the geometry of VAMOS for their reconstruction/calibrate/identification
};

#endif
