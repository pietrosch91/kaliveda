//Created by KVClassFactory on Tue Apr 23 13:04:21 2013
//Author: John Frankland,,,

#ifndef __KVGEOIMPORT_H
#define __KVGEOIMPORT_H

#include "KVGeoNavigator.h"
#include "KVGeoDNTrajectory.h"
class KVMultiDetArray;
class KVIonRangeTable;
class KVIonRangeTableMaterial;
class KVDetector;
class KVGroup;

class KVGeoImport : public KVGeoNavigator {
   KVMultiDetArray* fArray;
   KVIonRangeTable* fRangeTable;
   KVDetector* fLastDetector;
   Bool_t fCreateArray;
   TString fDetectorPlugin;
   KVGeoDNTrajectory fCurrentTrajectory;
   TVector3* fOrigin;

   KVDetector* GetCurrentDetector();
   KVDetector* BuildDetector(TString det_name, TGeoVolume* det_vol);
   void AddLayer(KVDetector*, TGeoVolume*);
   KVNameValueList fAcceptedDetectorNames;
   Bool_t fCheckDetVolNames;

public:
   KVGeoImport(TGeoManager*, KVIonRangeTable*, KVMultiDetArray*, Bool_t create = kTRUE);
   virtual ~KVGeoImport();

   virtual void ParticleEntersNewVolume(KVNucleus*);

   void ImportGeometry(Double_t dTheta = 0.1/*degrees*/, Double_t dPhi = 1.0/*degrees*/,
                       Double_t ThetaMin = 0.0/*degrees*/, Double_t PhiMin = 0.0/*degrees*/,
                       Double_t ThetaMax = 180.0/*degrees*/, Double_t PhiMax = 360.0/*degrees*/);

   void SetLastDetector(KVDetector*);

   void SetDetectorPlugin(const TString& name)
   {
      fDetectorPlugin = name;
   }
   void PropagateParticle(KVNucleus*, TVector3* TheOrigin = nullptr);

   void SetOrigin(double x, double y, double z)
   {
      // Call this method if needed before calling ImportGeometry in order to shoot particles
      // from an arbitrary point (x,y,z) instead of (0,0,0).
      // This can be useful if the detector array is "misaligned" so that its telescopes are
      // not pointing towards the theoretical origin (i.e. target position).
      fOrigin = new TVector3(x, y, z);
   }

   void AddAcceptedDetectorName(const char* name);

   ClassDef(KVGeoImport, 0) //Import a ROOT geometry into a KVMultiDetArray object
};

#endif
