//Created by KVClassFactory on Mon Apr 22 14:53:13 2013
//Author: John Frankland,,,

#include "KVGeoNavigator.h"
#include "KVEvent.h"
#include <TGeoManager.h>
#include <TGeoMatrix.h>

ClassImp(KVGeoNavigator)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGeoNavigator</h2>
<h4>Propagate particles of an event through a TGeo geometry</h4>
<!-- */
// --> END_HTML
// This is a base class for propagation of charged particles (KVNucleus) in events (KVEvent)
// through any TGeoManager ROOT geometry. Classes derived from this one can
// override the method
//     ParticleEntersNewVolume(KVNucleus*)
// in order to do something useful every time that a particle of the event
// enters a new volume (absorber, detector, etc.) of the geometry.
//
// Then to use your derived class do something like:
//
//  MyGeoNavigator nav( gGeoManager );
//  while( nevents-- ) {
//       nav.PropagateEvent( event );
//  }
////////////////////////////////////////////////////////////////////////////////

KVGeoNavigator::KVGeoNavigator(TGeoManager *g)
{
    // Constructor. Call with pointer to geometry.
    fGeometry = g;
}

KVGeoNavigator::~KVGeoNavigator()
{
    // Destructor
}

void KVGeoNavigator::PropagateEvent(KVEvent *TheEvent, TVector3 *TheOrigin)
{
    // Propagate a set of particles through the geometry
    // By default, propagates particles from (0,0,0) (world coordinates),
    // unless a different origin is given.
    KVNucleus* part;
    while( (part = TheEvent->GetNextParticle()) ){
        PropagateParticle(part, TheOrigin);
    }
}

void KVGeoNavigator::ParticleEntersNewVolume(KVNucleus*)
{
    // User-overridable method, redefine in child classes
    // This method is called every time that a propagated particle enters a new volume
    // in the geometry.
    // The user then has access to the following informations:
    //
    // - the TGeoVolume the particle is now entering (GetCurrentVolume());
    // - the node in the geometry this volume occupies (GetCurrentNode());
    // - the distance the particle will have to travel in this volume before
    //    leaving it (GetStepSize());
    // - the position of the particle on the boundary as it enters this volume
    //    (GetEntryPoint()).
    // - the position of the particle on the boundary as it leaves this volume
    //    (GetExitPoint()).
    //
    // If required, propagation of the particle can be stopped at any time by calling
    //      SetStopPropagation()

    AbstractMethod("ParticleEntersNewVolume");
}

TGeoHMatrix *KVGeoNavigator::GetCurrentMatrix() const
{
    // Returns pointer to current global transformation matrix
    return fCurrentMatrix;
}

TGeoVolume *KVGeoNavigator::GetCurrentDetectorNameAndVolume(TString &detector_name, Bool_t& multilayer)
{
    // Returns the name of the current detector (if we are inside a detector)
    // and whether it is a multilayer or simple detector.
    // Returns 0x0 if we are not inside a detector volume.
    //
    // N.B. the returned volume corresponds to the *whole* detector (even if it has several layers).
    // For a multilayer detector, GetCurrentVolume() returns the volume for the current layer.

    multilayer=kFALSE;
    fCurrentDetectorNode=0;
    TString volNom = GetCurrentVolume()->GetName();
    TGeoVolume* detector_volume=0;
    if(volNom.BeginsWith("DET_")){
        // simple detector
        fCurrentDetectorNode = GetCurrentNode();
        detector_name = fCurrentDetectorNode->GetName();
        detector_volume = GetCurrentVolume();
    }
    else
    {
        // have we hit 1 layer of a multilayer detector?
        TGeoVolume* mother_vol = GetCurrentNode()->GetMotherVolume();
        if(mother_vol) {
            TString mom = mother_vol->GetName();
            if(mom.BeginsWith("DET_")){
                // it *is* a multilayer detector (youpi! :-)
                if(fMotherNode) {// this is the node corresponding to the whole detector,
                                             // i.e. the one with the (unique) name of the detector
                    fCurrentDetectorNode = fMotherNode;
                    detector_name = fCurrentDetectorNode->GetName();
                    detector_volume = mother_vol;
                    multilayer=kTRUE;
                }
            }
        }
    }
    return detector_volume;
}

TGeoNode *KVGeoNavigator::GetCurrentDetectorNode() const
{
    // Returns the node corresponding to the current detector volume
    // N.B. the returned node corresponds to the *whole* detector (even if it has several layers).
    return fCurrentDetectorNode;
}

void KVGeoNavigator::PropagateParticle(KVNucleus*part, TVector3 *TheOrigin)
{
    // Propagate a particle through the geometry in the direction of its momentum,
    // until we reach the boundary of the geometry, or until fStopPropagation is set to kFALSE.
    // Propagation will also stop if we encounter a volume whose name begins with "DEADZONE"

    // Define point of origin of particles
    if(TheOrigin) fGeometry->SetCurrentPoint(TheOrigin->X(),TheOrigin->Y(),TheOrigin->Z());
    else fGeometry->SetCurrentPoint(0., 0., 0.);

    // unit vector in direction of particle's momentum
    TVector3 v = part->GetMomentum().Unit();
    // use particle's momentum direction
    fGeometry->SetCurrentDirection(v.x(), v.y(), v.z());
    fGeometry->FindNode();

    fCurrentVolume = fGeometry->GetCurrentVolume();
    fCurrentNode = fGeometry->GetCurrentNode();
    fMotherNode = fGeometry->GetMother();
    fCurrentMatrix = fGeometry->GetCurrentMatrix();
    // move along trajectory until we hit a new volume
    fGeometry->FindNextBoundaryAndStep();
    fStepSize = fGeometry->GetStep();
    TGeoVolume* newVol = fGeometry->GetCurrentVolume();
    TGeoNode* newNod = fGeometry->GetCurrentNode();
    TGeoNode* newMom = fGeometry->GetMother();
    TGeoHMatrix* newMatx = fGeometry->GetCurrentMatrix();

    Double_t XX,YY,ZZ;
    XX=YY=ZZ=0.;

    // reset user flag for stopping propagation of particle
    SetStopPropagation(kFALSE);

    // track particle until we leave the geometry or until fStopPropagation
    // becomes kTRUE
    while (!fGeometry->IsOutside()) {

        const Double_t* posi = fGeometry->GetCurrentPoint();
        fEntryPoint.SetXYZ(XX,YY,ZZ);
        XX=posi[0];YY=posi[1];ZZ=posi[2];
        fExitPoint.SetXYZ(XX,YY,ZZ);

        TString vn = GetCurrentVolume()->GetName();
        if(vn.BeginsWith("DEADZONE")) break;

        ParticleEntersNewVolume(part);

        if(StopPropagation()) break;

        fCurrentVolume = newVol;
        fCurrentNode = newNod;
        fMotherNode = newMom;
        fCurrentMatrix = newMatx;

        // move on to next volume crossed by trajectory
        fGeometry->FindNextBoundaryAndStep();
        fStepSize = fGeometry->GetStep();
        newVol = fGeometry->GetCurrentVolume();
        newNod = fGeometry->GetCurrentNode();
        newMom = fGeometry->GetMother();
        newMatx = fGeometry->GetCurrentMatrix();
    }
}

TGeoNode* KVGeoNavigator::FindPath(const Char_t* unique_name)
{

    TGeoNode* top = fGeometry->GetNode(0);

    TGeoVolume* vol = top->GetVolume();
    TObjArray* arr = 0;
    while ( (arr = vol->GetNodes()) ){

        for (Int_t ii=0;ii<arr->GetEntries();ii+=1){
            TGeoNode* node = (TGeoNode* )arr->At(ii);
            if (!strcmp(node->GetName(),unique_name)){
                printf("Mother Node %s\n",top->GetName());
                return node;
            }
            TGeoNode* nfather = node;

            node = ScanVolumes(nfather,unique_name);
            if (node){
                return node;
            }
            vol = node->GetVolume();
        }
    }

    return 0;

}


TGeoNode* KVGeoNavigator::ScanVolumes(TGeoNode* nfather,const Char_t* unique_name)
{

    TObjArray* arr = nfather->GetVolume()->GetNodes();
    if (!arr) return 0;

    for (Int_t ii=0;ii<arr->GetEntries();ii+=1){
        TGeoNode* node = (TGeoNode* )arr->At(ii);

        if (!strcmp(node->GetName(),unique_name)){
            printf("Mother Node %s %p\n",nfather->GetName(),nfather);
            return node;
        }
        node =  ScanVolumes(node , unique_name);
        if (node){
            printf("Mother Node %s %p\n",nfather->GetName(),nfather);
            return node;
        }
    }

    return 0;

}
