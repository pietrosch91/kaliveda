//Created by KVClassFactory on Tue Apr 23 13:04:21 2013
//Author: John Frankland,,,

#ifndef __KVGEOIMPORT_H
#define __KVGEOIMPORT_H

#include "KVGeoNavigator.h"
class KVMultiDetArray;
class KVIonRangeTable;
class KVIonRangeTableMaterial;
class KVDetector;

class KVGeoImport : public KVGeoNavigator
{
    KVMultiDetArray* fArray;
    KVIonRangeTable* fRangeTable;

    KVDetector* BuildDetector(TString det_name, TGeoVolume *det_vol);
    void AddLayer(KVDetector*, TGeoVolume*);

   public:
   KVGeoImport(TGeoManager*, KVIonRangeTable*, KVMultiDetArray*);
   virtual ~KVGeoImport();

   virtual void ParticleEntersNewVolume(KVNucleus *);

   void ImportGeometry(Double_t dTheta=0.5/*degrees*/, Double_t dPhi=15.0/*degrees*/,
                       Double_t ThetaMin=0.0/*degrees*/, Double_t PhiMin=0.0/*degrees*/,
                       Double_t ThetaMax=180.0/*degrees*/, Double_t PhiMax=360.0/*degrees*/);

   ClassDef(KVGeoImport,0)//Import a ROOT geometry into a KVMultiDetArray object
};

#endif
