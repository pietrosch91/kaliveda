/***************************************************************************
$Id: KVASMultiDetArray.cpp,v 1.91 2009/04/06 11:54:54 franklan Exp $
                          KVASMultiDetArray.cpp  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "Riostream.h"
#include "TROOT.h"
#include "KVASMultiDetArray.h"
#include "KVDetector.h"
#include "KVDetectorEvent.h"
#include "KVReconstructedEvent.h"
#include "KVACQParam.h"
#include "KVRList.h"
#include "KVLayer.h"
#include "KVEvent.h"
#include "KVNucleus.h"
#include "KVGroup.h"
#include "KVRing.h"
#include "KVTelescope.h"
#include "KVMaterial.h"
#include "KVMultiDetBrowser.h"
#include "KVTarget.h"
#include "KVIDTelescope.h"
#include "KV2Body.h"
#include <KVString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <KVIDGridManager.h>
#include <KVDataSetManager.h>
#include <KVUpDater.h>
#include "TPluginManager.h"
#include "KVDataSet.h"
#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "KVHashList.h"
#include "KVNameValueList.h"
#include "KVUniqueNameList.h"
#include "KVIonRangeTable.h"

using namespace std;

ClassImp(KVASMultiDetArray)
//////////////////////////////////////////////////////////////////////////////////////
//Begin_Html
//<img src="images/KVASMultiDetArray_diag.gif"><br>
//
//<h3>KVASMultiDetArray: A charged particle multidetector array</h3>
//End_Html
//Base class for describing multidetector array geometries and manipulating their associated data.
//
//It plays a fundamental role in: reading raw data; calibrating data; analysing reduced data; simulating the response of the array to generated events (filtering).
//
//A multidetector array is basically a large collection of detectors arranged in a specific geometry. The KVDetector class describes all aspects related
//to individual detectors (stopping power, data acquisition parameters), but not their spacial positioning. In order to place a detector in a multidetector geometry,
//it must belong to a KVTelescope, which is a stack of detectors placed one behind the other with a common angular acceptance and position. The next step up in the
//geometrical hierarchy is the KVRing axially-symmetric ring of telescopes all having the same type and angular dimensions, covering a given range of polar angles.
//Finally, the different rings are assembled in to layers using the KVLayer class. These are used to define the hierarchy of detectors with respect to their distance
//from the target, i.e. the order in which a particle leaving the target will pass through them (the target for a given experiment is described by an object of the KVTarget class).
//
//The geometry of the array is defined in the BuildGeometry() method. From this geometry are then deduced two other types of objects decribing relationships between
//telescopes and detectors.
//First, the notion of a "group" of telescopes is used to associate detectors in different layers (in principle with different geometries) through which
//particles leaving the target may pass. This is essential for particle reconstruction in the case where the detectors in one layer of the array have a larger angular
//acceptance than the others: the KVGroup contains all the detectors/telescopes which are either in front of or behind this "widest" detector.
//Second, all possible ways of identifying particles detected by the array are deduced from the geometry and for each one an object derived from KVIDTelescope
//is created, depending on the definitions given in GetIDTelescopes() for each possible couple of detectors. All aspects of particle identification are handled
//by classes derived from KVIDTelescope.
//
//In KVASMultiDetArray child classes corresponding to specific multidetector array descriptions, the following essential methods have to be implemented:
//Begin_Html
//<ul>
//<tt><li>MakeListOfDetectorTypes()
//<br>creates prototype KVDetector objects for all the different detector types needed
//  to build the telescopes of the array. The list of prototypes is in fDetectorTypes.</tt>
//<tt><li>PrototypeTelescopes()
// <br>creates prototype KVTelescope objects for all the different telescopes needed
//  to build the array. The list of prototypes is in fTelescopes.</tt>
//<tt><li>void BuildGeometry()
//<br>actually assembles the different telescopes, rings and layers of the array in the desired geometry.</tt>
//</ul>
//End_Html
//In order to initialise the geometry of a KVASMultiDetArray object, call the Build() method.
//
//The current multidetector array can be accessed through the gMultiDetArray global pointer.
//
//Begin_Html
//<h3>Calculating energy losses &amp; filtering simulated events</h3>
//End_html
//To calculate the energy losses of a charged particle in the array, use DetectParticle(KVNucleus* part). The appropriate detectors in the array are first found
//according to the particle's direction (it is assumed that the particle's momentum has been defined beforehand), and then its passage through them (in the order
//defined by the KVLayer - KVTelescope structures : see figure below) is simulated. Afterwards the particle's energy will have been reduced by the total energy lost in all absorbers,
//while the energy lost in the active part of each detector is available via the KVDetector::GetEnergy() method of each detector.
//Note that we do not take account of energy losses in the target in this method.
//
//Begin_Html
//<img src="images/KVASMultiDetArray_detectparticle.gif"><br>
//End_Html
//A very useful method for calibrating detectors is DetectParticleIn(const Char_t* detname, KVNucleus* kvp). In this case, only the particle's energy needs to be
//defined. The direction will be drawn at random within the angular acceptance of the named detector. Then DetectParticle() is used to calculate the energy losses.
//Note that the detector named in the method defines only the direction of the particle, not necessarily the detector in which the particle will stop, nor does it mean
//that energy losses will only be calculated for the detector in question.
//
//Simulating the response of the detector array to an entire simulated event is realised using the method
//DetectEvent(KVEvent* event,KVReconstructedEvent* rev_evt). 
//In this case and if the a target is defined, the energy losses in it
//of each particle are calculated first (see KVTarget::DetectEvent()). Particles which are not stopped in the target are then detected by the appropriate parts of the
//array. 
//The method needs :
//	-	a valid pointer for the simulated event which will be filtered 
//	-  KVReconstructedEvent pointer where user obtain, at the end, a list of KVReconstructedNucleus after the first step to reconstruction of the "filtered" event.
//For each particles of the KVEvent input pointer a list of energy loss in each detector are associated.
//the energy of these particles are the same as before the filter process.
//Different tags using the KVNucleus::AddGroup method are set depending on the status of the particles.
//The multi detector is cleared at the beginning of the method, to remove all traces of the precedent event
//Begin_Html
//<h3>Data acquisition &amp; reading raw data</h3>
//End_html
//The link between the detectors in the array and data acquisition parameters is realised by the KVACQParam class. Each detector has a list of the DAQ parameters
//associated to it, i.e. a list of KVACQParam objects. The DAQ parameters for all the detectors in the array are set up when the Build() method is called, via the
//method SetACQParams(). This tells each detector in turn to initialize its list of parameters. Additional DAQ parameters not directly associated to a detector may
//be added by the user with AddACQParam().
//The entire list of DAQ parameters associated with the array can be obtained using GetACQParams().
//A specific parameter can be retrieved using its name and the GetACQParam(const Char_t* name) method (see specific KVDetector::SetACQParams() method
//for how each detector names its DAQ parameters).
//Of course, you can also obtain the DAQ parameters and their associated values directly from the detector: see KVDetector::GetACQParam(const Char_t* type),
//KVDetector::GetACQParamList(), and KVDetector::GetACQData(const Char_t* type).


KVASMultiDetArray::KVASMultiDetArray()
{
    init();
}

void KVASMultiDetArray::init()
{
    //Basic initialisation called by constructor.
    //Creates layers list fLayers, detector types list fDetectorTypes, detectors list fDetectors,
    //telescope prototypes list fTelescopes, groups list fGroups, identification telescopes list
    //fIDTelescopes, and creates (if not already existing) identification grid manager object, gIDGridManager.
    //
    //Cleanups
    //The fDetectors and fACQParams lists contain references to objects which are
    //referenced & owned by other objects. We use the ROOT automatic garbage collection
    //to make sure that any object deleted elsewhere is removed automatically from these lists.
    //
    //The fLayers, fGroups & fIDTelescopes lists contain objects owned by the multidetector array,
    //but which may be deleted by other objects (or as a result of the deletion of other
    //objects: i.e. if all the detectors in a group are deleted, the group itself is destroyed).
    //We use the ROOT automatic garbage collection to make sure that any object deleted
    //elsewhere is removed automatically from these lists.

    fLayers = new KVList;
    fLayers->SetCleanup(kTRUE);
 
    fCurrentLayerNumber = 0;
}

//___________________________________________________________________________________

KVASMultiDetArray::~KVASMultiDetArray()
{
    //destroy (delete) the MDA and all the associated structure, detectors etc.

    //delete all layers
    //this will in turn delete all rings, telescopes and detectors
    if (fLayers && fLayers->TestBit(kNotDeleted)) {
        fLayers->Delete();
        delete fLayers;
    }
    fLayers = 0;

}

//_______________________________________________________________________________________
void KVASMultiDetArray::AddLayer()
{
    //Create a new layer in the array. The properties of the layer will be set later.

    KVLayer *kvl = new KVLayer();
    kvl->SetNumber(++fCurrentLayerNumber);
    kvl->SetArray(this);         // set reference in layer to the array
    fLayers->Add(kvl);
}

//______________________________________________________________________________________
void KVASMultiDetArray::AddLayer(KVLayer * kvl)
{
    //Add a previously defined layer to the array.
    kvl->SetNumber(++fCurrentLayerNumber);
    kvl->SetArray(this);         // set pointer in layer to the array
    fLayers->Add(kvl);
}

//_______________________________________________________________________________________
void KVASMultiDetArray::SetGroups(KVLayer * l1, KVLayer * l2)
{
//Update the list of groups in the detector array by comparing all telescopes in two layers.

    TIter lay1_nextring(l1->GetRings());
    TIter lay2_nextring(l2->GetRings());
    KVRing *kring1, *kring2;
    // loop over all pairs of rings in the two layers
    while ((kring1 = (KVRing *) lay1_nextring())) {
        while ((kring2 = (KVRing *) lay2_nextring())) {
#ifdef KV_DEBUG
            cout <<
            "SetGroups(KVLayer * l1, KVLayer * l2) - Looking at rings:" <<
            endl;
            cout << "   kring1 - " << kring1->GetName() << ", " << kring1->
            GetType() << ", " << kring1->GetNumber() << endl;
            cout << "   kring2 - " << kring2->GetName() << ", " << kring2->
            GetType() << ", " << kring2->GetNumber() << endl;
#endif
            if ((kring1->IsOverlappingWith(kring2)))       // overlap between rings ?
#ifdef KV_DEBUG
                cout << "   +++ Calling UpdateGroupsInRings" << endl;
#endif
            UpdateGroupsInRings(kring1, kring2);
        }
        lay2_nextring.Reset();
    }
}

//____________________________________________________________________________
void KVASMultiDetArray::UpdateGroupsInRings(KVRing * r1, KVRing * r2)
{
//Calculate groups by comparing two rings in different layers.
    TIter r1nxtele(r1->GetTelescopes());
    TIter r2nxtele(r2->GetTelescopes());
    KVTelescope *kvt1, *kvt2;
    // loop over all pairs of telescopes in the two rings
    while ((kvt1 = (KVTelescope *) r1nxtele())) {
        while ((kvt2 = (KVTelescope *) r2nxtele())) {
            if (kvt1->IsOverlappingWith(kvt2)) {
                AddToGroups(kvt1, kvt2);
            }
        }
        r2nxtele.Reset();
    }
}

//_______________________________________________________________________________________
void KVASMultiDetArray::Print(Option_t * opt) const
{
//Print out the structure of the multidetector array.
//If the option string "groups" is given, information is structured by KVGroup objects -
//i.e. the details of all the telescopes of the array are given, grouped according to their
//angular alignment.
//Otherwise the structure is represented as follows:
// - for each KVLayer, all KVRing objects are described
// - for each KVRing object, all KVTelescope objects are described
// - for each KVTelescope object, all KVDetector objects are described

    if (!strcmp(opt, "groups")) {
        TIter next(fGroups);
        KVGroup *obj;
        cout << "Structure of KVASMultiDetArray object: ";
        if (GetName())
            cout << GetName();
        cout << endl;
        cout << "--------------------------------------------------------" <<
        endl;
        cout << fGroups->GetSize() << " Groups" << endl;
        while ((obj = (KVGroup *) next())) {
            obj->Print("    ");
        }
    } else {
// print out detailed structure of array
        TIter next(fLayers);
        KVLayer *obj;
        cout << "Structure of KVASMultiDetArray object: ";
        if (GetName())
            cout << GetName();
        cout << endl;
        cout << "--------------------------------------------------------" <<
        endl;
        cout << fLayers->GetSize() << " Layers" << endl;
        while ((obj = (KVLayer *) next())) {
            obj->Print();
        }
    }
}

void KVASMultiDetArray::ReplaceTelescope(const Char_t * name,
                                       KVTelescope * new_kvt)
{
    //Replace (and destroy) the named telescope in the array with a telescope cloned
    //from the prototype given by the pointer new_kvt.

    KVTelescope *kvt = GetTelescope(name);
    if (!kvt) {
        Warning("ReplaceTelescope", "Telescope %s not found", name);
        return;
    }

    KVRing *ring = kvt->GetRing();
    if (!ring) {
        Error("ReplaceTelescope", "Telescope %s does not belong to a Ring",
              name);
        return;
    }
    ring->ReplaceTelescope(kvt, new_kvt);
}

//____________________________________________________________________________________________

KVTelescope *KVASMultiDetArray::GetTelescope(const Char_t * name) const
{
    // Return pointer to telescope in array with name given by "name"
    if (!fLayers)
        return 0;
    TIter nextL(fLayers);
    KVLayer *k1;
    KVRing *k2;
    KVTelescope *k3;
    while ((k1 = (KVLayer *) nextL())) { // loop over layers
        TIter nextR(k1->GetRings());
        while ((k2 = (KVRing *) nextR())) {       // loop over rings
            TIter nextT(k2->GetTelescopes());
            while ((k3 = (KVTelescope *) nextT())) {
                if (!strcmp(k3->GetName(), name))
                    return k3;
                // search among telescopes of ring
            }
        }
    }
    return 0;
}


//_______________________________________________________________________________________

void KVASMultiDetArray::RemoveLayer(KVLayer * lay, Bool_t kDeleteLay)
{
    //Remove layer from the MDA's list and delete the layer
    //(if kDeleteLay=kTRUE : default).
    //A check is made if the lay belongs to the MDA.
    //If kDeleteLay=kFALSE layer is not deleted.

    if (fLayers) {
        if (fLayers->FindObject(lay)) {
            fLayers->Remove(lay);
            SetBit(kIsRemoving);   //////set flag tested in KVLayer dtor
            if (kDeleteLay)
                delete lay;
            ResetBit(kIsRemoving); //unset flag tested in KVLayer dtor
        }
    }
}


//_______________________________________________________________________________________

void KVASMultiDetArray::RemoveLayer(UInt_t num)
{

    //remove from the array and destroy the numbered layer given as argument

    KVLayer *lay = GetLayer(num);
    if (lay)
        RemoveLayer(lay);
}

//_______________________________________________________________________________________

void KVASMultiDetArray::RemoveLayer(const Char_t * name)
{

    //remove from the array and destroy the named layer given as argument

    KVLayer *lay = GetLayer(name);
    if (lay)
        RemoveLayer(lay);
}

//________________________________________________________________________________________
void KVASMultiDetArray::MakeListOfDetectors()
{
    // Set up list of all detectors in array
    // This method is called after array geometry has been defined.
    // We set the name of each detector by calling its KVDetector::GetArrayName()
    // method (or overrides in child classes), as the name may depend on the
    // position in the final array geometry.

    fDetectors->Clear();

    TIter nextL(fLayers);
    KVLayer *k1;
    KVRing *k2;
    KVTelescope *k3;
    KVDetector *k4;
    while ((k1 = (KVLayer *) nextL())) { // loop over layers
        TIter nextR(k1->GetRings());
        while ((k2 = (KVRing *) nextR())) {       // loop over rings
            TIter nextT(k2->GetTelescopes());
            while ((k3 = (KVTelescope *) nextT())) {       // loop over telescopes
                TIter nextD(k3->GetDetectors());
                while ((k4 = (KVDetector *) nextD())) {     // loop over detectors
                    if (k4->InheritsFrom("KVDetector"))
                        k4->SetName( k4->GetArrayName() );//set name of detector
                    fDetectors->Add(k4);
                }
            }
        }
    }
#ifdef KV_DEBUG
    Info("MakeListOfDetectors", "Success");
#endif
}

KVRing *KVASMultiDetArray::GetRing(const Char_t * layer,
                                 const Char_t * ring_name) const
{
    //find named ring in named layer
    KVLayer *tmp = GetLayer(layer);
    KVRing *ring = 0;
    if (tmp) {
        ring = tmp->GetRing(ring_name);
    }
    return ring;
}

KVRing *KVASMultiDetArray::GetRing(const Char_t * layer, UInt_t ring_number) const
{
    //find numbered ring in named layer
    KVLayer *tmp = GetLayer(layer);
    KVRing *ring = 0;
    if (tmp) {
        ring = tmp->GetRing(ring_number);
    }
    return ring;
}

KVRing *KVASMultiDetArray::GetRing(UInt_t layer, const Char_t * ring_name) const
{
    //find named ring in numbered layer
    KVLayer *tmp = GetLayer(layer);
    KVRing *ring = 0;
    if (tmp) {
        ring = tmp->GetRing(ring_name);
    }
    return ring;
}

KVRing *KVASMultiDetArray::GetRing(UInt_t layer, UInt_t ring_number) const
{
    //find numbered ring in numbered layer
    KVLayer *tmp = GetLayer(layer);
    KVRing *ring = 0;
    if (tmp) {
        ring = tmp->GetRing(ring_number);
    }
    return ring;
}

void KVASMultiDetArray::RemoveRing(const Char_t * layer,
                                 const Char_t * ring_name)
{
    //destroy named ring in named layer
    if (GetLayer(layer))
        GetLayer(layer)->RemoveRing(GetRing(layer, ring_name));
}

void KVASMultiDetArray::RemoveRing(const Char_t * layer, UInt_t ring_name)
{
    //destroy named ring in named layer
    if (GetLayer(layer))
        GetLayer(layer)->RemoveRing(GetRing(layer, ring_name));
}

void KVASMultiDetArray::RemoveRing(UInt_t layer, const Char_t * ring_name)
{
    //destroy named ring in named layer
    if (GetLayer(layer))
        GetLayer(layer)->RemoveRing(GetRing(layer, ring_name));
}

void KVASMultiDetArray::RemoveRing(UInt_t layer, UInt_t ring_name)
{
    //destroy named ring in named layer
    if (GetLayer(layer))
        GetLayer(layer)->RemoveRing(GetRing(layer, ring_name));
}

void KVASMultiDetArray::RemoveRing(KVRing * ring)
{
    //search for ring in MDA and remove it
    if (ring->GetLayer()) {
        if (GetLayer(ring->GetLayer()->GetName())) {
            ring->GetLayer()->RemoveRing(ring);
        }
    }
}

//_________________________________________________________________________________

TGeoManager* KVASMultiDetArray::CreateGeoManager(Double_t dx, Double_t dy, Double_t dz)
{
    // This will create an instance of TGeoManager (any previous existing geometry gGeoManager
    // will be automatically deleted) and initialise it with the full geometry of the current multidetector
    // array. Every detector of the array will be represented in the resulting geometry.
    //
    // In order to build and view the geometry of a multidetector array, do:
    //
    //   TGeoManager* gm = gMultiDetArray->CreateGeoManager()
    //   gm->GetTopVolume()->Draw()
    //
    // For information on using the ROOT geometry package, see TGeoManager and related classes,
    // as well as the chapter "The Geometry Package" in the ROOT Users' Guide.
    //
    // The optional arguments (dx,dy,dz) are the half-lengths in centimetres of the "world"/"top" volume
    // into which all the detectors of the array are placed. This should be big enough so that all detectors
    // fit in. The default values of 500 give a "world" which is a cube 1000cmx1000cmx1000cm (with sides
    // going from -500cm to +500cm on each axis).

    if (!IsBuilt()) return NULL;
    SafeDelete(fGeoManager);
    fGeoManager = new TGeoManager(GetName(), GetTitle());
    TGeoMaterial*matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    TGeoMedium*Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
    TGeoVolume *top = fGeoManager->MakeBox("WORLD", Vacuum,  dx, dy, dz);
    fGeoManager->SetTopVolume(top);
    TIter nxt_lay(fLayers); KVLayer* L;
    while( (L = (KVLayer*)nxt_lay()) ){
    	L->AddToGeometry();
    }
    fGeoManager->CloseGeometry();
    return fGeoManager;
}

void KVASMultiDetArray::CalculateGroupsFromGeometry()
{
    fGroups->Delete();           // clear out (delete) old groups

   TIter nxtlay1(fLayers);
   KVLayer *l1;
    if (fLayers->GetSize() > 1) {
        fGr = 0;                  // for numbering groups
        TIter nxtlay2(fLayers);
        KVLayer *l2;

        while ((l1 = (KVLayer *) nxtlay1())) {    // loop over layers
            while ((l2 = (KVLayer *) nxtlay2())) { // loop over layers
                if ((l1->GetNumber() < l2->GetNumber())
                        && ((UInt_t) l1->GetNumber() <
                            (UInt_t) fLayers->GetSize()))
                    SetGroups(l1, l2);       // compare all telescopes in different layers
            }
            nxtlay2.Reset();
        }
     }  //if(fLayers->GetSize()>1)

        // Finally, create groups for all orphan telescopes which are not in any existing
        // group

        nxtlay1.Reset();          // reset loop over layers
        while ((l1 = (KVLayer *) nxtlay1())) {    // loop over layers
            if (l1->GetRings()) {
                TIter nxtrng(l1->GetRings());
                KVRing *robj;
                while ((robj = (KVRing *) nxtrng())) {      // loop over rings
                    TIter nxtscp(robj->GetTelescopes());
                    KVTelescope *tobj;
                    while ((tobj = (KVTelescope *) nxtscp())) {      // loop over telescopes
                        if (!tobj->GetGroup()) {      // orphan telescope

                            KVGroup *kvg = new KVGroup();
                            kvg->SetNumber(++fGr);
                            kvg->Add(tobj);
                            //group dimensions determined by detector dimensions
                            kvg->SetPolarMinMax(tobj->GetThetaMin(),
                                                tobj->GetThetaMax());
                            kvg->SetAzimuthalMinMax(tobj->GetPhiMin(),
                                                    tobj->GetPhiMax());
                            fGroups->Add(kvg);
                        }
                            }
                }
            }
        }
}
