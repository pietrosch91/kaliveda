//Created by KVClassFactory on Mon Apr 22 14:53:13 2013
//Author: John Frankland,,,

#include "KVGeoNavigator.h"
#include "KVEvent.h"
#include <TGeoManager.h>

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

void KVGeoNavigator::PropagateParticle(KVNucleus*part, TVector3 *TheOrigin)
{
    // Propagate a particle through the geometry in the direction of its momentum
    // Propagation will stop as soon as we encounter a volume whose name begins with "DEADZONE"

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
    // move along trajectory until we hit a new volume
    fGeometry->FindNextBoundaryAndStep();
    fStepSize = fGeometry->GetStep();
    TGeoVolume* newVol = fGeometry->GetCurrentVolume();
    TGeoNode* newNod = fGeometry->GetCurrentNode();

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

        // move on to next volume crossed by trajectory
        fGeometry->FindNextBoundaryAndStep();
        fStepSize = fGeometry->GetStep();
        newVol = fGeometry->GetCurrentVolume();
        newNod = fGeometry->GetCurrentNode();
    }
}
