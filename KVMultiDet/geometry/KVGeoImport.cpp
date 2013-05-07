//Created by KVClassFactory on Tue Apr 23 13:04:21 2013
//Author: John Frankland,,,

#include "KVGeoImport.h"
#include <KVMultiDetArray.h>
#include <KVIonRangeTableMaterial.h>
#include <TGeoBBox.h>
#include <KVEvent.h>
#include <KVGroup.h>
#include <TGeoPhysicalNode.h>

ClassImp(KVGeoImport)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGeoImport</h2>
<h4>Import a ROOT geometry into a KVMultiDetArray object</h4>
<!-- */
// --> END_HTML
//
// To use:
//      KVGeoImport geo(gGeoManager, new KVedaLoss, new KVMultiDetArray);
//      gImp.ImportGeometry();
//
// RULES FOR VALID DETECTOR GEOMETRY
//
// 1.) All detector volumes & nodes must have names which begin with "DET_"
//
// 2.) They must be made of materials which are known by the range table fRangeTable.
//
// 3.) For multi-layer detectors, the "active" layer volume/node must have a name beginning with "ACTIVE_"
//
// 4.) The "thickness" of the detector or any layer inside a multilayer detector
//     will be taken as the size of the volume's shape along its Z-axis
//     (so make sure that you define your detector volumes in this way).
//
// 5.) It is assumed that the natural length units of the geometry are centimetres.
//
// 6.) The name of the KVDetector object created and added to the array will be taken
//     from the unique full path of the node corresponding to the geometrical positioning
//     of the detector, see KVGeoNavigator::ExtractDetectorNameFromPath
//
// STRUCTURE ELEMENTS
// A structure element  is any association of detectors which may occur several times
// in a geometry in different places, with the same internal structure.
// For example: a wall of detectors in a rectangular array.
// The detectors in the wall may all be represented by a single volume (if they
// are identical) called "DET_SI", for example, which is placed N times in N
// different positions on nodes called, by default, "DET_SI_1", "DET_SI_2", etc.
// Once you have defined the wall volume, call it "STRUCT_SIWALL" (or whatever you
// want: only the "STRUCT_" part is mandatory).
// Now you can place the same wall structure in different positions of your
// geometry on nodes called "STRUCT_SIWALL_1", "STRUCT_SIWALL_2", etc.
// This will give a full path to the node of DET_SI_1 in SIWALL_2 like this:
//    "/TOP_1/STRUCT_SIWALL_2/DET_SI_1"
// which will be interpreted by KVGeoNavigator::ExtractDetectorNameFromPath as a
// detector with unique name:
//   "SIWALL_2_SI_1"
// (the "STRUCT_" and "DET_" parts are stripped off).
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

    KVDetector* detector = GetCurrentDetector();
    if(!detector) return;
    if(!fCurrentGroup){
        if(detector->GetGroup()) {
            fCurrentGroup=detector->GetGroup();
        }
        else {
            fCurrentGroup = new KVGroup;
            fCurrentGroup->SetNumber(++fGroupNumber);
            fCurrentGroup->Add(detector);
            fArray->Add(fCurrentGroup);
        }
    }
    else
    {
        KVGroup* det_group = detector->GetGroup();
        if(!det_group) {
            fCurrentGroup->Add(detector);
        }
        else {
            if(det_group!=fCurrentGroup)
                Warning("ParticleEntersNewVolume",
                        "Detector %s : already belongs to %s, now seems to be in %s",
                        detector->GetName(), det_group->GetName(),
                        fCurrentGroup->GetName());
        }
    }
    detector->GetNode()->SetName(detector->GetName());
    if(fLastDetector && detector!=fLastDetector) {
        fLastDetector->GetNode()->AddBehind(detector);
        detector->GetNode()->AddInFront(fLastDetector);
    }
    fLastDetector = detector;
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
    fGroupNumber=0;
    for(theta=ThetaMin; theta<=ThetaMax; theta+=dTheta){
        for(phi=PhiMin; phi<=PhiMax; phi+=dPhi){
                nuc->SetTheta(theta);
                nuc->SetPhi(phi);
                fCurrentGroup = 0;
                fLastDetector = 0;
                PropagateEvent(evt);
                count++;
        }
    }
    fArray->SetGeometry(GetGeometry());
    fArray->CalculateDetectorSegmentationIndex();
    Info("ImportGeometry",
         "Tested %d directions - Theta=[%f,%f:%f] Phi=[%f,%f:%f]",count,ThetaMin,ThetaMax,dTheta,PhiMin,PhiMax,dPhi);
    Info("ImportGeometry",
         "Imported %d detectors into array", fArray->GetDetectors()->GetEntries());
    fArray->CreateIDTelescopesInGroups();
    Info("ImportGeometry",
         "Created %d identification telescopes in array", fArray->GetListOfIDTelescopes()->GetEntries());
}

KVDetector* KVGeoImport::GetCurrentDetector()
{
    // Returns pointer to KVDetector corresponding to current location
    // in geometry. Detector is created and added to array if needed.
    // We also set up any geometry structure elements (from nodes beginning with "STRUCT_")

    KVString detector_name;
    Bool_t multilay;
    TGeoVolume* detector_volume = GetCurrentDetectorNameAndVolume(detector_name,multilay);
    // failed to identify current volume as part of a detector
    if(!detector_volume) return 0;

    // has detector already been built ? if not, do it now
    KVDetector* det = fArray->GetDetector(detector_name);
    if(!det) {
        det = BuildDetector(detector_name, detector_volume);
        if(det) {
            det->SetMatrix(GetCurrentMatrix());
            det->SetShape((TGeoBBox*)detector_volume->GetShape());
            fArray->Add(det);
            Int_t nstruc = CurrentStructures().GetEntries();
            if(nstruc){
                // Build and add geometry structure elements
                KVGeoStrucElement* ELEM = fArray;
                for(register int i=0;i<nstruc;i++){
                    KVGeoStrucElement* elem = (KVGeoStrucElement*)CurrentStructures()[i];
                    KVGeoStrucElement* nextELEM = ELEM->GetStructure(elem->GetName());
                    if(!nextELEM){
                        // make new structure
                        nextELEM = new KVGeoStrucElement(elem->GetName(), elem->GetType());
                        nextELEM->SetNumber(elem->GetNumber());
                        ELEM->Add(nextELEM);
                    }
                    ELEM=nextELEM;
                }
                // add detector to last structure
                ELEM->Add(det);
            }
        }
    }
    return det;
}

KVDetector *KVGeoImport::BuildDetector(TString det_name, TGeoVolume* det_vol)
{
    // Create a KVDetector with given name for the given volume
    //
    // Detector definition in geometry
    // ===============================
    // 1.) All detector volumes & nodes must have names which begin with "DET_"
    //
    // 2.) They must be made of materials which are known by the range table fRangeTable.
    //
    // 3.) For multi-layer detectors, the "active" layer volume/node must have a name beginning with "ACTIVE_"
    //
    // 4.) The "thickness" of the detector or any layer inside a multilayer detector
    //     will be taken as the size of the volume's shape along its Z-axis
    //     (so make sure that you define your detector volumes in this way).
    //
    // 5.) It is assumed that the natural length units of the geometry are centimetres.
    //
    // 6.) The name of the KVDetector object created and added to the array will be taken
    //     from the unique full path of the node corresponding to the geometrical positioning
    //     of the detector, see KVGeoNavigator::ExtractDetectorNameFromPath


    KVDetector* d = new KVDetector;
    d->SetName(det_name);

    Int_t nlayer = det_vol->GetNdaughters();
    if(nlayer){
        for(int i=0;i<nlayer;i++){
            AddLayer(d, det_vol->GetNode(i)->GetVolume());
        }
    }
    else
        AddLayer(d, det_vol);
    return d;
}

void KVGeoImport::AddLayer(KVDetector *det, TGeoVolume *vol)
{
    // Add an absorber layer to the detector
    // Volumes representing 'active' layers in detectors must have names
    // which begin with "ACTIVE_"

    TGeoMaterial* material = vol->GetMaterial();
    KVIonRangeTableMaterial* irmat = fRangeTable->GetMaterial(material);
    if(!irmat){
        Warning("AddLayer", "Unknown material %s/%s used in layer %s of detector %s",
                material->GetName(), material->GetTitle(), vol->GetName(), det->GetName());
        return;
    }
    TGeoBBox* sh = dynamic_cast<TGeoBBox*>(vol->GetShape());
    if(!sh) {
        Warning("AddLayer", "Unknown shape class %s used in layer %s of detector %s",
                vol->GetShape()->ClassName(), vol->GetName(), det->GetName());
        return; // just in case - for now, all shapes derive from TGeoBBox...
    }
    Double_t width = 2.*sh->GetDZ(); // thickness in centimetres
    KVMaterial* absorber;
    if( irmat->IsGas() ){
        Double_t p = material->GetPressure();
        Double_t T = material->GetTemperature();
        absorber = new KVMaterial(irmat->GetType(), width, p, T);
    }
    else
        absorber = new KVMaterial(irmat->GetType(), width);
    det->AddAbsorber(absorber);
    TString vnom = vol->GetName();
    if(vnom.BeginsWith("ACTIVE_")) det->SetActiveLayer( det->GetListOfAbsorbers()->GetEntries()-1 );
}

