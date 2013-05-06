//Created by KVClassFactory on Mon Apr 22 14:53:13 2013
//Author: John Frankland,,,

#ifndef __KVGEONAVIGATOR_H
#define __KVGEONAVIGATOR_H

#include "KVBase.h"
#include "TVector3.h"

class KVNucleus;
class KVEvent;
class TGeoManager;
class TGeoVolume;
class TGeoNode;
class TGeoHMatrix;

class KVGeoNavigator : public KVBase
{
private:
    TGeoManager* fGeometry;//geometry to navigate
    TGeoVolume* fCurrentVolume;//current volume
    TGeoNode* fCurrentNode;//current node
    TGeoNode* fCurrentDetectorNode;//node for current detector
    TGeoHMatrix* fCurrentMatrix;//current global transformation matrix
    TString fCurrentPath;//current full path to physical node
    TGeoNode* fMotherNode;//mother node of current node
    Double_t fStepSize;//distance to travel in volume
    TVector3 fEntryPoint;//position of particle on entering volume
    TVector3 fExitPoint;//position of particle on exiting volume
    Bool_t fStopPropagation;//flag set by user when particle propagation should stop

public:
    KVGeoNavigator(TGeoManager*);
    virtual ~KVGeoNavigator();

    void PropagateEvent(KVEvent*, TVector3 *TheOrigin=0);
    void PropagateParticle(KVNucleus*, TVector3 *TheOrigin=0);
    virtual void ParticleEntersNewVolume(KVNucleus *);

    TGeoManager* GetGeometry() const { return fGeometry; }
    TGeoVolume* GetCurrentVolume() const { return fCurrentVolume; }
    TGeoNode* GetCurrentNode() const { return fCurrentNode; }
    TGeoHMatrix* GetCurrentMatrix() const;
    Double_t GetStepSize() const { return fStepSize; }
    const TVector3& GetEntryPoint() const { return fEntryPoint; }
    const TVector3& GetExitPoint() const { return fExitPoint; }
    TGeoVolume* GetCurrentDetectorNameAndVolume(KVString &, Bool_t&);
    TGeoNode* GetCurrentDetectorNode() const;
    TString GetCurrentPath() const { return fCurrentPath; }

    Bool_t StopPropagation() const { return fStopPropagation; }
    void SetStopPropagation(Bool_t stop = kTRUE) { fStopPropagation = stop; }

    void ExtractDetectorNameFromPath(KVString&);

    ClassDef(KVGeoNavigator,0)//Propagate particles of an event through a TGeo geometry
};

#endif
