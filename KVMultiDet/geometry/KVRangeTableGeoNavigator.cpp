//Created by KVClassFactory on Tue Apr 23 10:27:56 2013
//Author: John Frankland,,,

#include "KVRangeTableGeoNavigator.h"
#include <TGeoMaterial.h>
#include <TGeoVolume.h>
#include <TGeoNode.h>
#include "KVNucleus.h"
#include <KVIonRangeTableMaterial.h>

ClassImp(KVRangeTableGeoNavigator)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVRangeTableGeoNavigator</h2>
<h4>Propagate particles through a geometry and calculate their energy losses</h4>
<!-- */
// --> END_HTML
// Given a valid ROOT geometry, we propagate the particles of an event
// and, every time a particle traverses a volume made of a TGeoMaterial
// with a name corresponding to a material
// known by this range table, we calculate the energy loss of the particle,
// and store it in the particle's list KVParticle::fParameters in the form
//   "DE_[node name]" = [energy lost in volume]
// We also store the coordinates (world-frame) of the particle's entry and exit
// in each volume:
//   "Xin_[node name]" = [entry coordinates for volume]
//   "Yin_[node name]"
//   "Zin_[node name]"
//   "Xout_[node name]" = [exit coordinates for volume]
//   "Yout_[node name]"
//   "Zout_[node name]"
////////////////////////////////////////////////////////////////////////////////

KVRangeTableGeoNavigator::KVRangeTableGeoNavigator(TGeoManager* g, KVIonRangeTable* r) : KVGeoNavigator(g)
{
   fRangeTable = r;
   fCutOffEnergy = 1.e-3;
}

KVRangeTableGeoNavigator::~KVRangeTableGeoNavigator()
{
    // Destructor
}

void KVRangeTableGeoNavigator::ParticleEntersNewVolume(KVNucleus *part)
{
    // Overrides method in KVGeoNavigator base class.
    // Every time a particle enters a new volume, we check the material to see
    // if it is known (i.e. contained in the range table fRangeTable).
    // If so, then we calculate the energy loss of the particle in this volume,
    // reduce the particle's energy accordingly, and, if the energy falls below
    // a certain cut-off (default 1.e-3 MeV; can be modified with
    // SetCutOffKEForPropagation(Double_t) ), we stop the propagation.

    Double_t de = 0;
    Double_t e = part->GetEnergy();
    // calculate energy losses in known materials
    TGeoMaterial* material = GetCurrentVolume()->GetMaterial();
    KVIonRangeTableMaterial* irmat=0;
    if ( (irmat = fRangeTable->GetMaterial(material)) ) {
        de = irmat->GetLinearDeltaEOfIon(
                    part->GetZ(), part->GetA(), e, GetStepSize(), 0.,
                    material->GetTemperature(),
                    material->GetPressure());
        e -= de;
        if(e<=fCutOffEnergy) {
            e=0.;
            SetStopPropagation();//propagation will stop after this step
        }
        //set flag to say that particle has been slowed down
        part->SetIsDetected();
        //If this is the first absorber that the particle crosses, we set a "reminder" of its
        //initial energy
        if (!part->GetPInitial()) part->SetE0();
        part->GetParameters()->SetValue(Form("DE_%s",GetCurrentNode()->GetName()), de);
        part->GetParameters()->SetValue(Form("Xin_%s",GetCurrentNode()->GetName()), GetEntryPoint().X());
        part->GetParameters()->SetValue(Form("Yin_%s",GetCurrentNode()->GetName()), GetEntryPoint().Y());
        part->GetParameters()->SetValue(Form("Zin_%s",GetCurrentNode()->GetName()), GetEntryPoint().Z());
        part->GetParameters()->SetValue(Form("Xout_%s",GetCurrentNode()->GetName()), GetExitPoint().X());
        part->GetParameters()->SetValue(Form("Yout_%s",GetCurrentNode()->GetName()), GetExitPoint().Y());
        part->GetParameters()->SetValue(Form("Zout_%s",GetCurrentNode()->GetName()), GetExitPoint().Z());
        part->SetEnergy(e);
    }
}

