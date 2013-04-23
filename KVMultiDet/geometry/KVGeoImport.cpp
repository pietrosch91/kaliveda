//Created by KVClassFactory on Tue Apr 23 13:04:21 2013
//Author: John Frankland,,,

#include "KVGeoImport.h"
#include <KVMultiDetArray.h>
#include <KVIonRangeTableMaterial.h>
#include <TGeoBBox.h>
#include <KVEvent.h>

ClassImp(KVGeoImport)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGeoImport</h2>
<h4>Import a ROOT geometry into a KVMultiDetArray object</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVGeoImport::KVGeoImport(TGeoManager* g, KVIonRangeTable* r, KVMultiDetArray * m) : KVGeoNavigator(g)
{
   // Import geometry described by TGeoManager into KVMultiDetArray object
    fArray = m;
    fRangeTable = r;
}

KVGeoImport::~KVGeoImport()
{
    // Destructor
}

void KVGeoImport::ParticleEntersNewVolume(KVNucleus *)
{
    // All detectors crossed by the particle's trajectory are added to the multidetector
    // and the groups (KVGroup) of aligned detectors are set up
    //
    // Detector definition in geometry
    //==============================
    // All detector volumes (TGeoVolume or daughter class objects) must have names which begin with "DET_"
    // They must be made of materials which are known by the range table fRangeTable.
    // The "thickness" of the detector will be taken as the size of the volume's shape along its Z-axis
    // (so make sure that you define your detector volumes in this way).
    // It is assumed that the natural length units of the geometry are centimetres.
    // The name of the KVDetector object created and added to the array will be taken
    // from the (unique) name of the node corresponding to the geometrical positioning of the detector.

    TString volNom = GetCurrentVolume()->GetName();
    if(!volNom.BeginsWith("DET_")) return;

    TGeoMaterial* material = GetCurrentVolume()->GetMaterial();
    KVIonRangeTableMaterial* irmat=0;
    if ( (irmat = fRangeTable->GetMaterial(material)) ) {
        // Only known materials are imported
        if(!fArray->GetDetector(GetCurrentNode()->GetName())){
            KVDetector* det = BuildDetector(irmat);
            if(det) fArray->AddDetector(det);
        }
    }
}

void KVGeoImport::ImportGeometry(Double_t dTheta, Double_t dPhi,
                                  Double_t ThetaMin, Double_t PhiMin, Double_t ThetaMax, Double_t PhiMax)
{
    // Scan the geometry in order to find all detectors and detector alignments.
    // This is done by sending out "particles" from (0,0,0) in all directions between
    // (ThetaMin,ThetaMax) - with respect to Z-axis - and (PhiMin,PhiMax) - cylindrical
    // angle in the (X,Y)-plane, over a grid of step dTheta in Theta and dPhi in Phi.

    KVEvent* evt = new KVEvent();
    KVNucleus* nuc = evt->AddParticle();
    nuc->SetZAandE(1,1,1);
    Double_t theta,phi;
    Int_t count=0;
    for(theta=ThetaMin; theta<=ThetaMax; theta+=dTheta){
        for(phi=PhiMin; phi<=PhiMax; phi+=dPhi){
                nuc->SetTheta(theta);
                nuc->SetPhi(phi);
                PropagateEvent(evt);
                count++;
        }
    }
    Info("ImportGeometry",
         "Tested %d directions - Theta=[%f,%f:%f] Phi=[%f,%f:%f]",count,ThetaMin,ThetaMax,dTheta,PhiMin,PhiMax,dPhi);
    Info("ImportGeometry",
         "Imported %d detectors into array:", fArray->GetListOfDetectors()->GetEntries());
    fArray->GetListOfDetectors()->ls();
}

KVDetector *KVGeoImport::BuildDetector(KVIonRangeTableMaterial *mat)
{
    // Create a detector for the current volume
    // We use the width given by TGeoBBox::GetDZ() to define the thickness of the detector.

    TGeoBBox* sh = dynamic_cast<TGeoBBox*>(GetCurrentVolume()->GetShape());
    if(!sh) return 0x0; // just in case - for now, all shapes derive from TGeoBBox...
    Double_t width = 2.*sh->GetDZ(); // thickness in centimetres
    KVDetector* d;
    if( mat->IsGas() ){
        TGeoMaterial* material = GetCurrentVolume()->GetMaterial();
        Double_t p = material->GetPressure();
        Double_t T = material->GetTemperature();
        d = new KVDetector();
        d->AddAbsorber(new KVMaterial(mat->GetType(), width, p, T) );
    }
    else
        d = new KVDetector(mat->GetType(), width);
    d->SetName( GetCurrentNode()->GetName() );
    return d;
}

