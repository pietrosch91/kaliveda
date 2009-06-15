/***************************************************************************
$Id: KVMultiDetArray.cpp,v 1.91 2009/04/06 11:54:54 franklan Exp $
                          kvmultidetarray.cpp  -  description
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
#include "KVMultiDetArray.h"
#include "KVDetector.h"
#include "KVDetectorEvent.h"
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

ClassImp(KVMultiDetArray)
//////////////////////////////////////////////////////////////////////////////////////
//Begin_Html
//<img src="images/kvmultidetarray_diag.gif"><br>
//
//<h3>KVMultiDetArray: A charged particle multidetector array</h3>
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
//In KVMultiDetArray child classes corresponding to specific multidetector array descriptions, the following essential methods have to be implemented:
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
//In order to initialise the geometry of a KVMultiDetArray object, call the Build() method.
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
//<img src="images/kvmultidetarray_detectparticle.gif"><br>
//End_Html
//A very useful method for calibrating detectors is DetectParticleIn(const Char_t* detname, KVNucleus* kvp). In this case, only the particle's energy needs to be
//defined. The direction will be drawn at random within the angular acceptance of the named detector. Then DetectParticle() is used to calculate the energy losses.
//Note that the detector named in the method defines only the direction of the particle, not necessarily the detector in which the particle will stop, nor does it mean
//that energy losses will only be calculated for the detector in question.
//
//Simulating the response of the detector array to an entire simulated event is realised using DetectEvent(KVEvent* event). In this case, the target energy losses
//of each particle are calculated first (see KVTarget::DetectEvent()). Particles which are not stopped in the target are then detected by the appropriate parts of the
//array. The method returns a pointer to a KVDetectorEvent i.e. a list of hit groups, which is the first step to reconstruction of the "filtered" event
//(see KVReconstructedEvent::ReconstructEvent()).
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
//
//When raw experimental data is read from e.g. data acquisition tapes, the DAQ parameters of each detector are updated with the values read from
//the file. E.g. for INDRA this is performed by the KVINDRADLT class. After reading an event, the values of the parameters can be obtained from each detector.
//The first step to reconstructing an event from the raw experimental data using the KVReconstructedEvent class (or its derivatives) is to obtain the list of groups
//which were hit in the event. This is contained in a KVDetectorEvent object, a pointer to which is given by GetDetectorEvent(). See KVReconstructedEvent for more
//details on event reconstruction.


KVMultiDetArray *gMultiDetArray;

KVMultiDetArray::KVMultiDetArray()
{
   init();
   gMultiDetArray = this;
}

void KVMultiDetArray::init()
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
   fDetectors = new THashList(20,3);
   fGroups = new KVList;
   fIDTelescopes = new THashList(20,3);
   fIDTelescopes->SetOwner(kTRUE); // owns its objects
   //cleanups
   gROOT->GetListOfCleanups()->Add(fDetectors);
   gROOT->GetListOfCleanups()->Add(fLayers);
   gROOT->GetListOfCleanups()->Add(fGroups);
   gROOT->GetListOfCleanups()->Add(fIDTelescopes);

   fDetectorTypes = new KVList;
   fTelescopes = new KVList;
   fCurrentLayerNumber = 0;
   fBrowser = 0;
   fACQParams = 0;
   fTarget = 0;
   fScatter = 0;
   fCurrentRun = 0;
   //initalise ID grid manager
   if (!gIDGridManager)
      new KVIDGridManager;

   fStatusIDTelescopes = 0;
   fCalibStatusDets = 0;
}

//______________________________________________________________________________
void KVMultiDetArray::Streamer(TBuffer & R__b)
{
   // Custom Streamer
   //For versions <3, we need to set the kMustCleanup bit of every detector, group, ID telescope,
   //ACQparam and layer so that garbage collection functions
   //Otherwise, this is a standard automatic streamer.

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      KVMultiDetArray::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
      if (R__v < 3) {
         {
            fIDTelescopes->R__FOR_EACH(TObject, SetBit) (kMustCleanup);
         }
         {
            fGroups->R__FOR_EACH(TObject, SetBit) (kMustCleanup);
         }
         {
            fACQParams->R__FOR_EACH(TObject, SetBit) (kMustCleanup);
         }
         {
            fDetectors->R__FOR_EACH(TObject, SetBit) (kMustCleanup);
         }
         {
            fLayers->R__FOR_EACH(TObject, SetBit) (kMustCleanup);
         }
      }
      if (R__v < 5) {
         //set kIDParamsSet and kIDCalParamsSet if kParamsSet=1
         if(TestBit(kParamsSet)){
            SetBit(kIDParamsSet); SetBit(kCalParamsSet);
         }
      }
   } else {
      KVMultiDetArray::Class()->WriteBuffer(R__b, this);
   }
}

//___________________________________________________________________________________
KVMultiDetArray::~KVMultiDetArray()
{
   //destroy (delete) the MDA and all the associated structure, detectors etc.

   //close browser if it is open
   if (fBrowser) {
      delete fBrowser;
      fBrowser = 0;
   }
   //delete telescope prototypes
   fTelescopes->Delete();
   delete fTelescopes;
   fTelescopes = 0;

   //delete detector prototypes
   fDetectorTypes->Delete();
   delete fDetectorTypes;
   fDetectorTypes = 0;

   //destroy all identification telescopes
   if (fIDTelescopes && fIDTelescopes->TestBit(kNotDeleted)) {
      fIDTelescopes->Delete();
      while (gROOT->GetListOfCleanups()->Remove(fIDTelescopes));
      delete fIDTelescopes;
   }
   fIDTelescopes = 0;

   //destroy all groups
   if (fGroups && fGroups->TestBit(kNotDeleted)) {
      fGroups->Delete();
      while (gROOT->GetListOfCleanups()->Remove(fGroups));
      delete fGroups;
   }
   fGroups = 0;

   //clear list of acquisition parameters
   if (fACQParams && fACQParams->TestBit(kNotDeleted)) {
      fACQParams->Clear();
      while (gROOT->GetListOfCleanups()->Remove(fACQParams));
      delete fACQParams;
   }
   fACQParams = 0;

   //clear list of detectors
   if (fDetectors && fDetectors->TestBit(kNotDeleted)) {
      fDetectors->Clear();
      while (gROOT->GetListOfCleanups()->Remove(fDetectors));
      delete fDetectors;
   }
   fDetectors = 0;

   //delete all layers
   //this will in turn delete all rings, telescopes and detectors
   if (fLayers && fLayers->TestBit(kNotDeleted)) {
      fLayers->Delete();
      while (gROOT->GetListOfCleanups()->Remove(fLayers));
      delete fLayers;
   }
   fLayers = 0;

   if (fTarget) {
      delete fTarget;
      fTarget = 0;
   }
   if (fScatter) {
      delete fScatter;
      fScatter = 0;
   }
   if (gMultiDetArray == this)
      gMultiDetArray = 0;

   if(fStatusIDTelescopes){
      fStatusIDTelescopes->Delete();
      delete fStatusIDTelescopes;
      fStatusIDTelescopes=0;
   }
   if(fCalibStatusDets){
      fCalibStatusDets->Delete();
      delete fCalibStatusDets;
      fCalibStatusDets=0;
   }
}

//_______________________________________________________________________________________

void KVMultiDetArray::Build()
{
   //Set up the multidetector array and calculate the associated groups and ID telescopes,
   //associate acquisition parameters with detectors etc.

   MakeListOfDetectorTypes();

   PrototypeTelescopes();

   BuildGeometry();

   MakeListOfDetectors();

   SetGroupsAndIDTelescopes();

   SetACQParams();

   SetCalibrators();

   SetIdentifications();

   //set flag to say Build() was called
   SetBit(kIsBuilt);
}

//_______________________________________________________________________________________
void KVMultiDetArray::UpdateArray()
{
   //must be called if modifications are made to array structure after initial Build or BuildGeometry...
   //can be redefined by specific (derived) detector array classes

   MakeListOfDetectors();
   SetGroupsAndIDTelescopes();
   SetACQParams();
   SetCalibrators();
   // if graphical interface is being used, need to update display
   if (fBrowser)
      fBrowser->UpdateArray();
}

//_______________________________________________________________________________________

void KVMultiDetArray::GetIDTelescopes(KVDetector * de, KVDetector * e,
                                      TList * idtels)
{
   //Create a KVIDTelescope from the two detectors and add it to the list.
   //
   //The different ID telescopes are defined as 'Plugin' objects in the file $KVROOT/KVFiles/.kvrootrc :
// # The KVMultiDetArray::GetIDTelescopes(KVDetector*de, KVDetector*e) method uses these plugins to
// # create KVIDTelescope instances adapted to the specific array geometry and detector types.
// # For each pair of detectors we look for a plugin with one of the following names:
// #    [name_of_dataset].de_detector_type[de detector thickness]-e_detector_type[de detector thickness]
// # Each characteristic in [] brackets may or may not be present in the name; first we test for names
// # with these characteristics, then all combinations where one or other of the characteristics is not present.
// # In addition, we first test all combinations which begin with [name_of_dataset].
// # The first plugin found in this order will be used.
// # In addition, if for one of the two detectors there is a plugin called
// #    [name_of_dataset].de_detector_type[de detector thickness]
// #    [name_of_dataset].e_detector_type[e detector thickness]
// # then we add also an instance of this 1-detector identification telescope.
   //
   //This method is called by KVGroup in order to set up all ID telescopes
   //of the array.

   KVIDTelescope *idt = 0;
   if( fDataSet == "" && gDataSet ) fDataSet = gDataSet->GetName();

   //first we look for ID telescopes specific to current dataset
   TString uri;
   uri.Form("%s.%s%d-%s%d", fDataSet.Data(), de->GetType(),
            (int) de->GetThickness(), e->GetType(),
            (int) e->GetThickness());
   if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
      set_up_telescope(de,e,idtels,idt,uri);
   }
   else
   {
      uri.Form("%s.%s%d-%s", fDataSet.Data(), de->GetType(),
            (int) de->GetThickness(), e->GetType());
      if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
         set_up_telescope(de,e,idtels,idt,uri);
      }
      else
      {
         uri.Form("%s.%s-%s%d", fDataSet.Data(), de->GetType(), e->GetType(),
            (int) e->GetThickness());
         if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
            set_up_telescope(de,e,idtels,idt,uri);
         }
         else
         {
            uri.Form("%s.%s-%s", fDataSet.Data(), de->GetType(), e->GetType());
            if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
               set_up_telescope(de,e,idtels,idt,uri);
            }
            else
            {
               //now we look for generic ID telescopes
               uri.Form("%s%d-%s%d", de->GetType(), (int) de->GetThickness(),
                  e->GetType(), (int) e->GetThickness());
               if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
                  set_up_telescope(de,e,idtels,idt,uri);
               }
               else
               {
                  uri.Form("%s%d-%s", de->GetType(), (int) de->GetThickness(),
                     e->GetType());
                  if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
                     set_up_telescope(de,e,idtels,idt,uri);
                  }
                  else
                  {
                     uri.Form("%s-%s%d", de->GetType(), e->GetType(),
                        (int) e->GetThickness());
                     if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
                        set_up_telescope(de,e,idtels,idt,uri);
                     }
                     else
                     {
                        uri.Form("%s-%s", de->GetType(), e->GetType());
                        if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
                           set_up_telescope(de,e,idtels,idt,uri);
                        }
                     }
                  }
               }
            }
         }
      }
   }

   //if no telescope found, try single-stage telescope
   idt = 0;
   //look for ID telescopes with only one of the two detectors
   uri.Form("%s.%s%d", fDataSet.Data(), de->GetType(),
            (int) de->GetThickness());
   if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
      set_up_single_stage_telescope(de,idtels,idt,uri);
   }
   else
   {
      uri.Form("%s.%s", fDataSet.Data(), de->GetType());
      if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
         set_up_single_stage_telescope(de,idtels,idt,uri);
      }
      else
      {
         uri.Form("%s.%s%d", fDataSet.Data(), e->GetType(),
            (int) e->GetThickness());
         if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
            set_up_single_stage_telescope(e,idtels,idt,uri);
         }
         else
         {
            uri.Form("%s.%s", fDataSet.Data(), e->GetType());
            if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
               set_up_single_stage_telescope(e,idtels,idt,uri);
            }
            else
            {
               uri.Form("%s%d", de->GetType(), (int) de->GetThickness());
               if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
                  set_up_single_stage_telescope(de,idtels,idt,uri);
               }
               else
               {
                  uri.Form("%s", de->GetType());
                  if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
                     set_up_single_stage_telescope(de,idtels,idt,uri);
                  }
                  else
                  {
                     uri.Form("%s%d", e->GetType(), (int) e->GetThickness());
                     if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
                        set_up_single_stage_telescope(e,idtels,idt,uri);
                     }
                     else
                     {
                        uri.Form("%s", e->GetType());
                        if ((idt = KVIDTelescope::MakeIDTelescope(uri.Data()))){
                           set_up_single_stage_telescope(e,idtels,idt,uri);
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

void KVMultiDetArray::set_up_telescope(KVDetector * de, KVDetector * e, KVList * idtels, KVIDTelescope *idt, TString& uri)
{
   idt->AddDetector(de);
   idt->AddDetector(e);
   if (de->GetGroup()) {
      idt->SetGroup(de->GetGroup());
   } else {
      idt->SetGroup(e->GetGroup());
   }
   if (idtels->FindObjectByName(idt->GetName())) {
      delete idt;
   } else {
      idtels->Add(idt);
   }
}

void KVMultiDetArray::set_up_single_stage_telescope(KVDetector * det, KVList * idtels, KVIDTelescope *idt, TString& uri)
{
   idt->AddDetector(det);
   idt->SetGroup(det->GetGroup());
   if (idtels->FindObjectByName(idt->GetName())) {
      delete idt;
   } else {
      idtels->Add(idt);
   }
}

//_______________________________________________________________________________________
void KVMultiDetArray::AddLayer()
{
   //Create a new layer in the array. The properties of the layer will be set later.

   KVLayer *kvl = new KVLayer();
   kvl->SetNumber(++fCurrentLayerNumber);
   kvl->SetArray(this);         // set reference in layer to the array
   kvl->SetBit(kMustCleanup);   //cleanup
   fLayers->Add(kvl);
}

//______________________________________________________________________________________
void KVMultiDetArray::AddLayer(KVLayer * kvl)
{
   //Add a previously defined layer to the array.
   kvl->SetNumber(++fCurrentLayerNumber);
   kvl->SetArray(this);         // set pointer in layer to the array
   kvl->SetBit(kMustCleanup);   //cleanup
   fLayers->Add(kvl);
}

//______________________________________________________________________________________
void KVMultiDetArray::SetGroupsAndIDTelescopes()
{
   //Find groups of telescopes in angular alignment placed on different layers.
   //List is in fGroups.
   //Also creates all ID telescopes in array and stores them in fIDTelescopes.
   //Any previous groups/idtelescopes are deleted beforehand.

   fGroups->Delete();           // clear out (delete) old groups
   fIDTelescopes->Delete();     // clear out (delete) old identification telescopes

   if (fLayers->GetSize() > 1) {
      fGr = 0;                  // for numbering groups
      TIter nxtlay1(fLayers), nxtlay2(fLayers);
      KVLayer *l1, *l2;

      while ((l1 = (KVLayer *) nxtlay1())) {    // loop over layers
         while ((l2 = (KVLayer *) nxtlay2())) { // loop over layers
            if ((l1->GetNumber() < l2->GetNumber())
                && ((UInt_t) l1->GetNumber() <
                    (UInt_t) fLayers->GetSize()))
               SetGroups(l1, l2);       // compare all telescopes in different layers
         }
         nxtlay2.Reset();
      }
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
                     kvg->SetBit(kMustCleanup); // cleanup
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
   }                            //if(fLayers->GetSize()>1)

   //now read list of groups and create list of ID telescopes
	KVGroup *grp;
   TIter ngrp(fGroups);
   while ((grp = (KVGroup *) ngrp())) {
      grp->GetIDTelescopes(fIDTelescopes);
   }
}

//_______________________________________________________________________________________
void KVMultiDetArray::SetGroups(KVLayer * l1, KVLayer * l2)
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
void KVMultiDetArray::UpdateGroupsInRings(KVRing * r1, KVRing * r2)
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

//___________________________________________________________________________________
void KVMultiDetArray::AddToGroups(KVTelescope * kt1, KVTelescope * kt2)
{
// The two telescopes are in angular overlap.
// a) if neither are in groups already, create a new group to which they both belong
// b) if one of them is in a group already, add the orphan telescope to it
// c) if both are in groups already, merge the two groups
   KVGroup *kvg;


   if (!kt1->GetGroup() && !kt2->GetGroup()) {  // case a)
#ifdef KV_DEBUG
      cout << "Making new Group from " << kt1->
          GetName() << " and " << kt2->GetName() << endl;
#endif
      kvg = new KVGroup();
      kvg->SetNumber(++fGr);
      kvg->SetBit(kMustCleanup);        //cleanup
      kvg->Add(kt1);
      kvg->Add(kt2);
      kvg->Sort();              // sort telescopes in list
      kvg->SetDimensions(kt1, kt2);     // set group dimensions from telescopes
      //add to list
      fGroups->Add(kvg);
   } else if ((kvg = (kt1->GetGroup())) && !kt2->GetGroup()) {  // case b) - kt1 is already in a group
#ifdef KV_DEBUG
      cout << "Adding " << kt2->GetName() << " to group " << kvg->
          GetNumber() << endl;
#endif
      //add kt2 to group
      kvg->Add(kt2);
      kvg->Sort();              // sort telescopes
      kvg->SetDimensions(kvg, kt2);     //adjust dimensions depending on kt2
   } else if ((kvg = (kt2->GetGroup())) && !kt1->GetGroup()) {  // case b) - kt2 is already in a group
#ifdef KV_DEBUG
      cout << "Adding " << kt1->GetName() << " to group " << kvg->
          GetNumber() << endl;
#endif
      //add kt1 to group
      kvg->Add(kt1);
      kvg->Sort();              // sort telescopes
      kvg->SetDimensions(kvg, kt1);     //adjust dimensions depending on kt1
   } else if (kt1->GetGroup() != kt2->GetGroup()) {     //both telescopes already in different groups
#ifdef KV_DEBUG
      cout << "Merging " << kt1->GetGroup()->
          GetNumber() << " and " << kt2->GetGroup()->GetNumber() << endl;
      cout << "because of " << kt1->GetName() << " and " << kt2->
          GetName() << endl;
#endif
      MergeGroups(kt1->GetGroup(), kt2->GetGroup());
   }
}

//_______________________________________________________________________________________
void KVMultiDetArray::MergeGroups(KVGroup * kg1, KVGroup * kg2)
{
   //Merge two existing groups into a new single group.

   //look through kg1 telescopes.
   //set their "group" to kg2.
   //if they are not already in kg2, add them to kg2.
   KVTelescope *tel = 0;
   TIter next(kg1->GetTelescopes());
   while ((tel = (KVTelescope *) next())) {
      if (kg2->Contains(tel)) {
         tel->SetGroup(kg2);    //make sure telescope has right group pointer
      } else {
         kg2->Add(tel);
      }
   }
   //remove kg1 from list and destroy it
   fGroups->Remove(kg1);
   delete kg1;
   //sort merged group and calculate dimensions
   kg2->Sort();
   kg2->SetDimensions();
   //renumber groups and reset group counter
   RenumberGroups();
}

//_______________________________________________________________________________________

void KVMultiDetArray::RenumberGroups()
{
   //Number groups according to position in list fGroups and set fGr counter to the number
   //of groups in the list
   fGr = 0;
   KVGroup *g = 0;
   TIter next(fGroups);
   while ((g = (KVGroup *) next())) {
      g->SetNumber(++fGr);
   }
}

//_______________________________________________________________________________________
void KVMultiDetArray::Print(Option_t * opt) const
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
      cout << "Structure of KVMultiDetArray object: ";
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
      cout << "Structure of KVMultiDetArray object: ";
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

//__________________________________________________________________________________

KVDetectorEvent *KVMultiDetArray::DetectEvent(KVEvent * event)
{
   //Simulate detection of event by multidetector array.
   //
   //For each particle in the event we calculate first its energy loss in the target (if the target has been defined, see KVMultiDetArray::SetTarget).
   //By default these energy losses are calculated from a point half-way along the beam-direction through the target (taking into account the orientation
   //of the target), if you want random depths for each event call GetTarget()->SetRandomized() before using DetectEvent().
   //
   //If the particle escapes the target then we look for the group in the array that it will hit. If there is one, then the detection of this particle by the
   //different members of the group is simulated. The detectors concerned have their fEloss members set to the energy lost by the particle when it crosses them.
   //
   //The method returns a KVDetectorEvent i.e.  a list of all groups hit by particles in the event. Just as with experimental data (see GetDetectorEvent())
   //this is the first step to event reconstruction.
   //The next step in "filtering" the simulated event is to use a KVReconstructedEvent (or derived object):
   //    KVReconstructedEvent::ReconstructEvent(KVDetectorEvent*) will rebuild particles by associating the different energy losses
   //    KVReconstructedEvent::IdentifyEvent() will attempt to identify them
   //etc. etc.

   //create new KVDetectorEvent
   KVDetectorEvent *tmp = new KVDetectorEvent();

   //temporary pointer to simulated event
   tmp->SetSimEvent(event);

   if (fTarget) {
      //simulate passage through target material
      fTarget->DetectEvent(event);
   }
   // iterate through list of particles
   KVNucleus *part;
   while ((part = event->GetNextParticle())) {  // loop over particles
#ifdef KV_DEBUG
      cout << "DetectEvent(): looking at particle---->" << endl;
      part->Print();
#endif


      //find group in array hit by particle
      KVGroup *grp_tch = GetGroup(part->GetTheta(), part->GetPhi());
      if (grp_tch) {
#ifdef KV_DEBUG
         cout << "DetectEvent(): found hit group---->" << endl;
         grp_tch->Print("angles");
#endif
         //simulate detection of particle by this group
         if (part->GetEnergy() > 0) {
            grp_tch->DetectParticle(part);

            //add group to list of hit groups
            tmp->AddGroup(grp_tch);
         }
      }
   }

   return tmp;
}

//__________________________________________________________________________________
Bool_t KVMultiDetArray::DetectParticle(KVNucleus * part)
{
// Simulate detection of a single particle (nucleus) by multidetector array.
// We look for the group in the array that the particle will hit, after exiting
// the target (if one is defined).
// If a group is found the detection of this particle
// by the different members of the group is simulated.
// The detectors concerned have their fEloss members set to the energy lost by the
// particle when it crosses them.
//
// Returns kTRUE if particle hits detector in array, kFALSE if not (i.e. particle
// in beam pipe etc.)

   //find group in array hit by particle
   KVGroup *grp_tch = GetGroup(part->GetTheta(), part->GetPhi());
   if (grp_tch) {
#ifdef KV_DEBUG
      cout << "DetectParticle(): found hit group---->" << endl;
      grp_tch->Print("angles");
#endif
      //simulate detection of particle by this group
      grp_tch->DetectParticle(part);
      return kTRUE;
   }
   return kFALSE;
}

//____________________________________________________________________________________________
void KVMultiDetArray::ReplaceTelescope(const Char_t * name,
                                       KVTelescope * new_kvt)
{
   //Replace (and destroy) the named telescope in the array with a telescope cloned
   //from the prototype given by the pointer new_kvt.

   KVTelescope *kvt = GetTelescope(name);
   if (!kvt) {
      Warning("ReplaceTelescope", KVMDA_REPL_TEL_NOT_FOUND, name);
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
void KVMultiDetArray::ReplaceDetector(const Char_t * name,
                                      KVDetector * new_kvd)
{
   //Replace (and destroy) the named detector in the array with a detector based on the prototype
   //given by the pointer new_kvd.

   KVDetector *kvd = GetDetector(name);
   if (!kvd) {
      Warning("ReplaceDetector", KVMDA_REPL_DET_NOT_FOUND, name);
      return;
   }
   //get telescope of detector, so we know where to put the replacement
   KVTelescope *tel = kvd->GetTelescope();
   if (!tel) {
      Error("ReplaceDetector", "Detector %s is not in a telescope", name);
      return;
   }
   tel->ReplaceDetector(kvd, new_kvd);
}

//____________________________________________________________________________________________

KVTelescope *KVMultiDetArray::GetTelescope(const Char_t * name) const
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

//____________________________________________________________________________________________
KVDetector *KVMultiDetArray::GetDetector(const Char_t * name) const
{
   // return pointer to array detector with "name"
   if (!fDetectors)
      return 0;
   return (KVDetector *) fDetectors->FindObjectByName(name);
}

//____________________________________________________________________________________________
KVIDTelescope *KVMultiDetArray::GetIDTelescope(const Char_t * name) const
{
//Return pointer to DeltaE-E ID Telescope with "name"

   if (!fIDTelescopes)
      return 0;
   return (KVIDTelescope *) fIDTelescopes->FindObjectByName(name);
}

//_______________________________________________________________________________________

KVGroup *KVMultiDetArray::GetGroup(Float_t theta, Float_t phi)
{
// return pointer to group in array according to given polar coordinates
   if (!fGroups)
      return 0;
   TIter next(fGroups);
   KVGroup *obj;
   while ((obj = (KVGroup *) next())) { // loop over group list
      if (obj->IsInPolarRange(theta) && obj->IsInPhiRange(phi))
         return obj;
   }
   return 0;                    // no group found with these coordinates
}

//_______________________________________________________________________________________

KVGroup *KVMultiDetArray::GetGroup(const Char_t * name)
{
   //return pointer to group in array which contains detector or telescope
   //with name "name"

   KVTelescope *tel = 0;
   KVDetector *det = 0;
   KVGroup *grp = 0;
   if ((tel = GetTelescope(name))) {
      grp = GetGroup(tel->GetTheta(), tel->GetPhi());
      return grp;
   }
   if ((det = GetDetector(name))) {
      grp = GetGroup(det->GetTheta(), det->GetPhi());
   }
   return grp;
}

//_______________________________________________________________________________________

TList *KVMultiDetArray::GetTelescopes(Float_t theta, Float_t phi)
{
   //Create and fill list of telescopes at position (theta, phi) sorted
   //according to distance from target (closest first).
   //User must delete list after use.
   KVGroup *gr = GetGroup(theta, phi);
   TList *tmp = 0;
   if (gr)
      tmp = gr->GetTelescopes(theta, phi);
   return tmp;
}

//_______________________________________________________________________________________

void KVMultiDetArray::RemoveLayer(KVLayer * lay, Bool_t kDeleteLay)
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

void KVMultiDetArray::RemoveLayer(UInt_t num)
{

   //remove from the array and destroy the numbered layer given as argument

   KVLayer *lay = GetLayer(num);
   if (lay)
      RemoveLayer(lay);
}

//_______________________________________________________________________________________

void KVMultiDetArray::RemoveLayer(const Char_t * name)
{

   //remove from the array and destroy the named layer given as argument

   KVLayer *lay = GetLayer(name);
   if (lay)
      RemoveLayer(lay);
}

//________________________________________________________________________________________
void KVMultiDetArray::MakeListOfDetectors()
{
   //set up list of all detectors in array

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
                  k4->SetBit(kMustCleanup);     // cleanup
               fDetectors->Add(k4);
            }
         }
      }
   }
#ifdef KV_DEBUG
   Info("MakeListOfDetectors", "Success");
#endif
}

//________________________________________________________________________________________
KVList *KVMultiDetArray::GetListOfDetectors() const
{
   return fDetectors;
}

//________________________________________________________________________________________
TGraph *KVMultiDetArray::GetPedestals(const Char_t * det_signal,const Char_t * det_type, Int_t ring_number,Int_t
run_number)
{

	//Renvoie sous forme de TGraph (en fonction du numero de module)
	//les piedestaux du signal (det_signal) asssocies aux detecteurs de type (det_type)
	//qui sont presents dans la couronne ring_number pour un numero de run donne (si run_number==-1)
	//on suppose que gMultiDetArray->SetParameters(xxx) a ete fait en amont
	//L'utilisateur doit effacer ce TGraph tout seul comme un grand apres usage
	//Une recherche sur l existence ou non du graph permet d eviter des boucles inutiles
	//Si l appel est reitere

	if (run_number!=-1 || run_number!=Int_t(GetCurrentRunNumber()))
		SetParameters(run_number);

	KVList* sltype = 0;
	KVList* slring = 0;
	TGraph* gr_ped=0;

	KVString sgraph; sgraph.Form("KVPed_%s_%s_%d_%d",det_signal,det_type,ring_number,GetCurrentRunNumber());
	if ( ( gr_ped=(TGraph* )gROOT->FindObject(sgraph.Data()) ) ) return gr_ped;

	if ( (sltype = GetListOfDetectors()->GetSubListWithMethod(det_type,"GetType")) ){
		KVString sring; sring.Form("%d",ring_number);
		if ( (slring = sltype->GetSubListWithMethod(sring,"GetRingNumber")) ){
				gr_ped = new TGraph(); gr_ped->SetName(sgraph.Data());
				for (Int_t mm=0;mm<slring->GetEntries();mm+=1){
					gr_ped->SetPoint(gr_ped->GetN(),
						((KVDetector*)slring->At(mm))->GetModuleNumber(),
						((KVDetector*)slring->At(mm))->GetPedestal(det_signal));

				}
			delete slring;
			return gr_ped;
		}
		delete sltype;
	}
	return 0;

}
//________________________________________________________________________________________
void KVMultiDetArray::StartBrowser()
{
//Opens a graphical browser showing the layer/ring/telescope structure of the array.
//In time this could also be used to configure an array.

   if (gROOT->IsBatch()) {
      fprintf(stderr, "Browser cannot run in batch mode\n");
   }
   if (!fBrowser)
      fBrowser = new KVMultiDetBrowser(this, gClient->GetRoot(), 10, 10);
}

//________________________________________________________________________________________
void KVMultiDetArray::CloseBrowser()
{
//close graphical configuration tool for the array

   if (fBrowser) {
      cout << "Closing KVMultiDetBrowser" << endl;
      delete fBrowser;
      fBrowser = 0;
   }
}

//_________________________________________________________________________________
const KVMultiDetBrowser *KVMultiDetArray::GetBrowser()
{
//get pointer to GUI
   return fBrowser;
}

//_________________________________________________________________________________
void KVMultiDetArray::Clear(Option_t * opt)
{
   //Reset all groups (lists of detected particles etc.)
   //and detectors in groups (energy losses, ACQparams etc. etc.)
   //and the target if there is one
   if (fGroups) {
      TIter next(fGroups);
      KVGroup *grp;
      while ((grp = (KVGroup *) next())) {
         grp->Reset();
      }
   }
   if (GetTarget())
      GetTarget()->Clear();
}

//_________________________________________________________________________________________
void KVMultiDetArray::AddACQParam(KVACQParam * par)
{
   //Add an acquisition parameter corresponding to a detector of the array.
   //The fACQParams list is added to the list of cleanups (gROOT->GetListOfCleanups).
   //Each acq-param has its kMustCleanup bit set.
   //Thus, if the acq-param is deleted (e.g. by the detector which owns it), it is
   //automatically removed from the fACQParams list by ROOT.

   if (!fACQParams) {
      fACQParams = new KVList;
      fACQParams->SetOwner(kFALSE);
      gROOT->GetListOfCleanups()->Add(fACQParams);
   }
   if (par) {
      par->SetBit(kMustCleanup);
      fACQParams->Add(par);
   } else
      Warning("AddACQParam", "Null pointer passed as argument");
}

//_________________________________________________________________________________________

void KVMultiDetArray::SetACQParams()
{
   // Set up acquisition parameters in all detectors of the array
   // Any detectors whose parameters are already setup will just be read
   // (otherwise UpdateArray will keep on adding ACQParams...)

   if (fACQParams) {
      fACQParams->Clear();
   }

   TIter next(GetListOfDetectors());
   KVDetector *det;
   while ((det = (KVDetector *) next())) {
      if (!det->GetACQParamList()) {
         //set up acqparams in detector
         det->SetACQParams();
      }
      //loop over acqparams and add them to fACQParams list, checking
      //their status (working or not working ?)
      TIter next_par(det->GetACQParamList());
      KVACQParam *par;
      while ((par = (KVACQParam *) next_par())) {
         AddACQParam(par);
         par->SetWorking( gDataSet->GetDataSetEnv( Form("KVACQParam.%s.Working", par->GetName()), kTRUE ) );
      }
   }
}

//_________________________________________________________________________________

void KVMultiDetArray::SetCalibrators()
{
   //Set up calibrators in all detectors of the array
   //Note that this only initialises the calibrator objects associated to each
   //detector (defined in each detector class's SetCalibrators method),
   //it does not set the parameters of the calibrations: this is done by
   //SetParameters or SetRunCalibrationParameters

   GetListOfDetectors()->R__FOR_EACH(KVDetector, SetCalibrators)();
}

//_________________________________________________________________________________

void KVMultiDetArray::UpdateCalibrators()
{
   //This method can be used to update the calibrations of the detectors of a KVMultiDetArray
   //object which has been read back from a ROOT file, and was possibly written with an old
   //version of the class(es) which is now obsolete.
   //The existing calibrator objects (read in from the file) are first removed from the array's
   //detectors, and then SetCalibrators() is called in order to set new calibrators as defined
   //in the current version of the class(es).
   //In order to set the parameters of the new calibrators for a given run,
   //SetParameters or SetRunCalibrationParameters must be called after this method.

   GetListOfDetectors()->R__FOR_EACH(KVDetector, RemoveCalibrators)();
   SetCalibrators();
}

//_________________________________________________________________________________

void KVMultiDetArray::GetDetectorEvent(KVDetectorEvent* detev)
{
   //Clears and then fills 'detev' with list of "hit" groups.
   //We loop over all groups of the array and test whether KVGroup::Fired() returns true or false.
   //We add each hit group to the list (if the group is not already in the list).

	//clear old event
	detev->Clear();
   //loop over groups
   TIter next_grp(fGroups);
   KVGroup *grp;
   while ((grp = (KVGroup *) next_grp())) {

      if (grp->Fired()) {

         //add new group to list of hit groups
         detev->AddGroup(grp);

      }
   }
}


//_________________________________________________________________________________


void KVMultiDetArray::SetTarget(const Char_t * material,
                                const Float_t thickness)
{
   //Define the target used for a given experimental set-up. For material names, see KVMaterial.
   //The thickness is in mg/cm2.
   //Use SetTarget(0) to remove the existing target.

   if (!material) {
      if (fTarget) {
         delete fTarget;
         fTarget = 0;
      }
      if (fScatter) {
         delete fScatter;
         fScatter = 0;
      }
      return;
   }
   if (!GetTarget())
      fTarget = new KVTarget(material, thickness);

   if (!fScatter) {
      fScatter = new KV2Body();
      fScatter->SetTarget((Int_t) fTarget->GetZ(),
                          (Int_t) fTarget->GetMass());
   }
}

void KVMultiDetArray::SetTarget(KVTarget * targ)
{
   //Adopt KVTarget object for use as experimental target i.e. we make a clone of the object pointed to by 'targ'.
   //Therefore, any subsequent modifications to the target should be made to the object whose pointer is returned by GetTarget().
   //This object will be deleted with the detector array, or when the target is changed.
   //
   //Calling SetTarget(0) will remove any existing target.

   if (fTarget) {
      delete fTarget;
      fTarget = 0;
   }
   if (!targ)
      return;
   fTarget = (KVTarget *) targ->Clone();
}

void KVMultiDetArray::SetTargetMaterial(const Char_t * material)
{
   //Define the target material used for a given experimental set-up.
   //For material names, see KVDetector.

   if (!GetTarget())
      fTarget = new KVTarget(material, 0.0);
   if (!fScatter) {
      fScatter = new KV2Body();
      fScatter->SetTarget((Int_t) GetTarget()->GetZ(),
                          (Int_t) GetTarget()->GetMass());
   }
}

void KVMultiDetArray::SetTargetThickness(const Float_t thickness)
{
   //Define the target thickness (mg/cm2) used for a given experimental set-up.
   //Need to define material first
   if (GetTarget()) {
      if (GetTarget()->GetLayerByIndex(1))
         GetTarget()->GetLayerByIndex(1)->SetThickness(thickness);
   }
}

void KVMultiDetArray::RemoveGroup(KVGroup * grp)
{
   //Remove (i.e. destroy) all the telescopes belonging to a given group
   if (grp) {
      grp->Destroy();
   }
}

void KVMultiDetArray::RemoveGroup(const Char_t * name)
{
   //Remove (i.e. destroy) all the telescopes belonging to the group in array
   //which contains detector or telescope with name "name"
   KVGroup *grp = GetGroup(name);
   if (grp) {
      RemoveGroup(grp);
   }
}

KVRing *KVMultiDetArray::GetRing(const Char_t * layer,
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

KVRing *KVMultiDetArray::GetRing(const Char_t * layer, UInt_t ring_number) const
{
   //find numbered ring in named layer
   KVLayer *tmp = GetLayer(layer);
   KVRing *ring = 0;
   if (tmp) {
      ring = tmp->GetRing(ring_number);
   }
   return ring;
}

KVRing *KVMultiDetArray::GetRing(UInt_t layer, const Char_t * ring_name) const
{
   //find named ring in numbered layer
   KVLayer *tmp = GetLayer(layer);
   KVRing *ring = 0;
   if (tmp) {
      ring = tmp->GetRing(ring_name);
   }
   return ring;
}

KVRing *KVMultiDetArray::GetRing(UInt_t layer, UInt_t ring_number) const
{
   //find numbered ring in numbered layer
   KVLayer *tmp = GetLayer(layer);
   KVRing *ring = 0;
   if (tmp) {
      ring = tmp->GetRing(ring_number);
   }
   return ring;
}

void KVMultiDetArray::RemoveRing(const Char_t * layer,
                                 const Char_t * ring_name)
{
   //destroy named ring in named layer
   if (GetLayer(layer))
      GetLayer(layer)->RemoveRing(GetRing(layer, ring_name));
}

void KVMultiDetArray::RemoveRing(const Char_t * layer, UInt_t ring_name)
{
   //destroy named ring in named layer
   if (GetLayer(layer))
      GetLayer(layer)->RemoveRing(GetRing(layer, ring_name));
}

void KVMultiDetArray::RemoveRing(UInt_t layer, const Char_t * ring_name)
{
   //destroy named ring in named layer
   if (GetLayer(layer))
      GetLayer(layer)->RemoveRing(GetRing(layer, ring_name));
}

void KVMultiDetArray::RemoveRing(UInt_t layer, UInt_t ring_name)
{
   //destroy named ring in named layer
   if (GetLayer(layer))
      GetLayer(layer)->RemoveRing(GetRing(layer, ring_name));
}

void KVMultiDetArray::RemoveRing(KVRing * ring)
{
   //search for ring in MDA and remove it
   if (ring->GetLayer()) {
      if (GetLayer(ring->GetLayer()->GetName())) {
         ring->GetLayer()->RemoveRing(ring);
      }
   }
}

//_________________________________________________________________________________

void KVMultiDetArray::DetectParticleIn(const Char_t * detname,
                                       KVNucleus * kvp)
{
   //Given the name of a detector, simulate detection of a given particle
   //by the complete corresponding group. The particle's theta and phi are set
   //at random within the limits of the telescope to which the detector belongs

   KVDetector *kvd = GetDetector(detname);
   if (kvd) {
      KVTelescope *tele = kvd->GetTelescope();
      kvp->SetRandomMomentum(kvp->GetEnergy(), tele->GetThetaMin(),
                             tele->GetThetaMax(), tele->GetPhiMin(),
                             tele->GetPhiMax(), "random");
      DetectParticle(kvp);
   } else {
      Error("DetectParticleIn", "Detector %s not found", detname);
   }
}

//_________________________________________________________________________________

void KVMultiDetArray::SetPedestals(const Char_t * filename)
{
   //Set pedestals for all acquisition parameters whose name appears in the file 'filename'
   //The file should contain a line for each parameter beginning with:
   //parameter_name   pedestal_value
   //(anything else on the line will not be read)
   //Begin comment lines with '#'

   ifstream pedfile(filename);
   if (!pedfile.good()) {
      Error("SetPedestals", "File %s cannot be opened", filename);
      return;
   }

   KVString s;

   while (pedfile.good()) {

      s.ReadLine(pedfile);

      if (pedfile.good() && !s.BeginsWith("#")) {       //skip comments

         //break line into parts
         TObjArray *toks = s.Tokenize(" ");
         if (toks->GetSize() > 0) {
            TString name =
                ((TObjString *) toks->At(0))->GetString().
                Strip(TString::kBoth);
            KVString value(((TObjString *) toks->At(1))->GetString());

            KVACQParam *par = GetACQParam(name.Data());
            if (par)
               par->SetPedestal(value.Atof());
         }
         delete toks;
      }
   }
   pedfile.close();
}

//_________________________________________________________________________________

KVMultiDetArray *KVMultiDetArray::MakeMultiDetector(const Char_t * name)
{
   //Static function which will create and 'Build' the multidetector object corresponding to 'name'
   //These are defined as 'Plugin' objects in the file $KVROOT/KVFiles/.kvrootrc :
   //
   //Plugin.KVMultiDet:    INDRA_camp1    KVINDRA     KVIndra    "KVINDRA()"
   //+Plugin.KVMultiDet:    INDRA_camp2    KVINDRA2     KVIndra    "KVINDRA2()"
   //+Plugin.KVMultiDet:    INDRA_camp4    KVINDRA4     KVIndra    "KVINDRA4()"
   //+Plugin.KVMultiDet:    INDRA_camp5    KVINDRA5     KVIndra5    "KVINDRA5()"
   //
   //The 'name' ("INDRA_camp1" etc.) corresponds to the name of a dataset in $KVROOT/KVFiles/manip.list
   //The constructors/macros are always without arguments
   //
   //This name is stored in fDataSet

   //check and load plugin library
   TPluginHandler *ph;
   if (!(ph = LoadPlugin("KVMultiDetArray", name)))
      return 0;

   //execute constructor/macro for multidetector - assumed without arguments
   KVMultiDetArray *mda = (KVMultiDetArray *) ph->ExecPlugin(0);

   mda->fDataSet = name;
   //call Build() method
   mda->Build();
   return mda;
}

//_________________________________________________________________________________

void KVMultiDetArray::SetParameters(UShort_t run)
{
   //Set identification and calibration parameters for run.
   //This can only be done if gDataSet has been set i.e. a dataset has been chosen
   //Otherwise this just has the effect of setting the current run number

   fCurrentRun = run;
   KVDataSet *ds = gDataSet;
   if (!ds) {
      if (gDataSetManager)
         ds = gDataSetManager->GetDataSet(fDataSet.Data());
   }
   if (ds) {
      ds->GetUpDater()->SetParameters(run);
      SetBit(kParamsSet);
   }
}

//_________________________________________________________________________________

void KVMultiDetArray::SetRunIdentificationParameters(UShort_t run)
{
   //Set identification parameters for run.
   //This can only be done if gDataSet has been set i.e. a dataset has been chosen
   //Otherwise this just has the effect of setting the current run number

   fCurrentRun = run;
   KVDataSet *ds = gDataSet;
   if (!ds) {
      if (gDataSetManager)
         ds = gDataSetManager->GetDataSet(fDataSet.Data());
   }
   if (ds) {
      ds->GetUpDater()->SetIdentificationParameters(run);
      SetBit(kIDParamsSet);
   }
}

//_________________________________________________________________________________

void KVMultiDetArray::SetRunCalibrationParameters(UShort_t run)
{
   //Set calibration parameters for run.
   //This can only be done if gDataSet has been set i.e. a dataset has been chosen
   //Otherwise this just has the effect of setting the current run number

   fCurrentRun = run;
   KVDataSet *ds = gDataSet;
   if (!ds) {
      if (gDataSetManager)
         ds = gDataSetManager->GetDataSet(fDataSet.Data());
   }
   if (ds) {
      ds->GetUpDater()->SetCalibrationParameters(run);
      SetBit(kCalParamsSet);
   }
}

//_________________________________________________________________________________

void KVMultiDetArray::SetIdentifications()
{
   //Initialisation of all ACTIVE identification telescopes in the array, i.e. those appearing in a line
   //in the .kvrootrc file such as this:
   //
   //# [dataset name].ActiveIdentifications:    [type1]  [type2] ...
   //
   //The 'types' given correspond to the value given by KVIDTelescope::GetLabel(), these are the
   //identifiers used to retrieve the different plugin classes in GetIDTelescopes(KVDetector*,KVDetector*,KVList*).
   //
   //For each type of identification in the list, we retrieve the first identification telescope with this
   //label from the list of all ID telescopes, in order to call its KVIDTelescope::SetIdentificationParameters() method.
   //This method (when rederived in child classes of KVIDTelescope) initialises the identification objects
   //for ALL of the ID telescopes of the same type (class) in the array.
   //
   //Note that, in general, the parameters of the identifications for a given run are not
   //set until SetParameters or SetRunIdentificationParameters is called.

   TString id_labels = gDataSet->GetDataSetEnv("ActiveIdentifications");
   if(id_labels=="") {
      Info("SetIdentifications", "No active identifications");
      return;
   }
   //split list of labels
   TObjArray *toks = id_labels.Tokenize(' ');
   TIter next_lab(toks); TObjString *lab;
   //loop over labels/identification 'types'
   while( (lab = (TObjString*)next_lab()) ){

      //get first telescope in list with right label
      KVIDTelescope* idt = (KVIDTelescope*)GetListOfIDTelescopes()->FindObjectByLabel( lab->String().Data() );
      //set ID parameters for all telescopes of this 'type'
      if(idt) {
         Info("SetIdentifications", "Initialising %s identifications...", idt->GetLabel());
         if( idt->SetIdentificationParameters( this ) )
            Info("SetIdentifications", "OK");
      }

   }
   delete toks;
}

//_________________________________________________________________________________

void KVMultiDetArray::UpdateIdentifications()
{
   //This method can be used to update the identifications of a KVMultiDetArray
   //object which has been read back from a ROOT file, and was possibly written with an old
   //version of the identification parameters which are now obsolete.
   //We remove/destroy the existing identification parameters and replace them with the current versions.
   //In order to set the parameters of the new identifications for a given run,
   //SetParameters or SetRunIdentificationParameters must be called after this method.

   //remove existing identification objects/parameters
   fIDTelescopes->R__FOR_EACH(KVIDTelescope, RemoveIdentificationParameters) ();

   //reset identifications
   SetIdentifications();
}

//_________________________________________________________________________________

void KVMultiDetArray::UpdateIDTelescopes()
{
   //This method can be used to update the identification telescopes of a KVMultiDetArray
   //object which has been read back from a ROOT file, and was possibly written with an old
   //version of the classes which are now obsolete, or for which new KVIDTelescope classes
   //are now available.
   //In order to set the parameters of the new identifications for a given run,
   //SetParameters or SetRunIdentificationParameters must be called after this method.

   //destroy old ID telescopes
   fIDTelescopes->Delete();
   //now read list of groups and create list of ID telescopes
	KVGroup *grp;
   TIter ngrp(fGroups);
   while ((grp = (KVGroup *) ngrp())) {
      grp->GetIDTelescopes(fIDTelescopes);
   }
   //reset identifications
   SetIdentifications();
}

//_________________________________________________________________________________

void KVMultiDetArray::InitializeIDTelescopes()
{
   // Calls Initialize() method of each identification telescope (see KVIDTelescope
   // and derived classes). This is essential before identification of particles is attempted.

   fIDTelescopes->R__FOR_EACH(KVIDTelescope, Initialize) ();
}

//_________________________________________________________________________________

Double_t KVMultiDetArray::GetTotalSolidAngle(void) {
		// compute & return the total solid angle (msr) of the array
		// it is the sum of solid angles of all existing KVGroups
      Double_t ftotal_solid_angle=0.0;
	   KVGroup *grp;
      TIter ngrp(fGroups);
      while ((grp = (KVGroup *) ngrp())) {
		   ftotal_solid_angle+=grp->GetSolidAngle(); // use the KVPosition::GetSolidAngle()
      }
      return ftotal_solid_angle;
   }

//_________________________________________________________________________________

void KVMultiDetArray::PrintStatusOfIDTelescopes()
{
   // Print full status report on ID telescopes in array, using informations stored in
   // fStatusIDTelescopes (see GetStatusOfIDTelescopes).

   if( !GetCurrentRunNumber() ){
      Info("PrintStatusOfIDTelescopes", "Cannot know status without knowing RUN NUMBER");
      return;
   }

   cout << endl << "-----STATUS OF IDENTIFICATION TELESCOPES FOR RUN "
         << GetCurrentRunNumber() << "------" << endl << endl;
   //get list of active telescopes
   TString id_labels = gDataSet->GetDataSetEnv("ActiveIdentifications");
   if(id_labels=="") {
      cout << " *** No active identifications *** " << endl;
      return;
   }
   //split list of labels
   TObjArray *toks = id_labels.Tokenize(' ');

   //update status infos
   GetStatusOfIDTelescopes();

   TIter next_type( fStatusIDTelescopes );
   TList* id_type_list = 0;
   while( (id_type_list = (TList*)next_type()) ){

      cout << " *** " << id_type_list->GetName() << " Identifications -------------------" << endl;
      if( !toks->FindObject( id_type_list->GetName() ) ){
         cout << "      [NOT ACTIVE]" << endl;
      }
      TList* ok_list = (TList*)id_type_list->FindObject("OK");
      TList* notok_list = (TList*)id_type_list->FindObject("NOT OK");
      TList *print_list = 0;
      Int_t Nok = ok_list->GetEntries();
      Int_t Notok = notok_list->GetEntries();
      if( Nok && Notok ){
         if(Nok < Notok) print_list = ok_list;
         else print_list = notok_list;
      }
      if( Nok && (!Notok) ) cout << "    ALL telescopes are OK" << endl;
      else if( Notok && (!Nok) ) cout << "    NO telescopes are OK" << endl;
      else
      {
         cout << "    " << ok_list->GetEntries() << " telescopes are OK, "
               << notok_list->GetEntries() << " telescopes are NOT OK" << endl;
         cout << "    " << print_list->GetName() << " :" << endl;
         print_list->ls();
      }
      cout << endl;

   }
   delete toks;
}

//_________________________________________________________________________________

TList* KVMultiDetArray::GetStatusOfIDTelescopes()
{
   // Fill and return pointer to list fStatusIDTelescopes which contains
   // a list for each type of ID telescope in the array, each list contains a list
   // "OK" with the ID telescopes which have IsReadyForID()=kTRUE, and
   // a list "NOT OK" with the others.
   //
   // The returned TList object must not be deleted (it belongs to the KVMultiDetArray).

   if(!fStatusIDTelescopes){
      fStatusIDTelescopes = new TList; fStatusIDTelescopes->SetOwner(kTRUE);
   }
   else
   {
      fStatusIDTelescopes->Delete();
   }
   if( !fIDTelescopes || !fIDTelescopes->GetEntries() ) return fStatusIDTelescopes;
   TIter next(fIDTelescopes);
   KVIDTelescope* idt = 0;
   while( (idt = (KVIDTelescope*)next()) ){

      TString id_type = idt->GetLabel();
      TList* id_type_list = (TList*)fStatusIDTelescopes->FindObject( id_type.Data() );
      if( !id_type_list ){
         id_type_list = new TList; id_type_list->SetOwner(kTRUE); id_type_list->SetName( id_type.Data() );
         fStatusIDTelescopes->Add( id_type_list );
         id_type_list->Add( new TList ); ((TList*)id_type_list->At(0))->SetName("OK");
         id_type_list->Add( new TList ); ((TList*)id_type_list->At(1))->SetName("NOT OK");
      }
      if( idt->IsReadyForID() )
         ((TList*)id_type_list->FindObject("OK"))->Add( idt );
      else
         ((TList*)id_type_list->FindObject("NOT OK"))->Add( idt );
   }
   return fStatusIDTelescopes;
}

//_________________________________________________________________________________

KVList* KVMultiDetArray::GetIDTelescopeTypes()
{
   // Create, fill and return pointer to a list of TObjString containing the name of each type
   // of ID telescope in the array.
   //
   // Delete the KVList after use (it owns the TObjString objects)

	KVList *type_list = new KVList;
   if( !fIDTelescopes || !fIDTelescopes->GetEntries() ) return type_list;
   TIter next(fIDTelescopes);
   KVIDTelescope* idt = 0;
   while( (idt = (KVIDTelescope*)next()) ){
      if( !type_list->FindObject( idt->GetLabel() ) ){
			type_list->Add( new TObjString( idt->GetLabel() ) );
      }
   }
   return type_list;
}

//_________________________________________________________________________________

KVList* KVMultiDetArray::GetIDTelescopesWithType(const Char_t* type)
{
   // Create, fill and return pointer to a list of KVIDTelescopes with
	// the given type in the array.
   // WARNING! - check pointer is not zero (we return NULL if ID telescopes
	// list is not defined or empty)
	//
   // Delete the KVList after use (it does not own the KVIDTelescopes).

   if( !fIDTelescopes || !fIDTelescopes->GetEntries() ) return NULL;
	return fIDTelescopes->GetSubListWithLabel( type );
}

//_________________________________________________________________________________

TList* KVMultiDetArray::GetCalibrationStatusOfDetectors()
{
   // Fill and return pointer to list fCalibStatusDets which contains
   // a list for each type of detector in the array, each list contains a list
   // "OK" with the detectors which are calibrated, and
   // a list "NOT OK" with the others.
   //
   // The returned TList object must not be deleted (it belongs to the KVMultiDetArray).

   if(!fCalibStatusDets){
      fCalibStatusDets = new TList; fCalibStatusDets->SetOwner(kTRUE);
   }
   else
   {
      fCalibStatusDets->Delete();
   }
   if( !fDetectors || !fDetectors->GetEntries() ) return fCalibStatusDets;
   TIter next(fDetectors);
   KVDetector* det = 0;
   while( (det = (KVDetector*)next()) ){

      TString type = det->GetType();
      TList* type_list = (TList*)fCalibStatusDets->FindObject( type.Data() );
      if( !type_list ){
         type_list = new TList; type_list->SetOwner(kTRUE); type_list->SetName( type.Data() );
         fCalibStatusDets->Add( type_list );
         type_list->Add( new TList ); ((TList*)type_list->At(0))->SetName("OK");
         type_list->Add( new TList ); ((TList*)type_list->At(1))->SetName("NOT OK");
      }
      if( det->IsCalibrated() )
         ((TList*)type_list->FindObject("OK"))->Add( det );
      else
         ((TList*)type_list->FindObject("NOT OK"))->Add( det );
   }
   return fCalibStatusDets;
}

//_________________________________________________________________________________

void KVMultiDetArray::PrintCalibStatusOfDetectors()
{
   // Print full status report on calibration of detectors in array, using informations stored in
   // fCalibStatusDets (see GetCalibrationStatusOfDetectors).

   if( !GetCurrentRunNumber() ){
      Info("PrintCalibStatusOfDetectors", "Cannot know status without knowing RUN NUMBER");
      return;
   }

   cout << endl << "-----------STATUS OF CALIBRATIONS FOR RUN "
         << GetCurrentRunNumber() << "------------" << endl << endl;

   //update status infos
   GetCalibrationStatusOfDetectors();

   TIter next_type( fCalibStatusDets );
   TList* id_type_list = 0;
   while( (id_type_list = (TList*)next_type()) ){

      cout << " *** " << id_type_list->GetName() << " Detectors -------------------" << endl;
      TList* ok_list = (TList*)id_type_list->FindObject("OK");
      TList* notok_list = (TList*)id_type_list->FindObject("NOT OK");
      TList *print_list = 0;
      Int_t Nok = ok_list->GetEntries();
      Int_t Notok = notok_list->GetEntries();
      if( Nok && Notok ){
         if(Nok < Notok) print_list = ok_list;
         else print_list = notok_list;
      }
      if( Nok && (!Notok) ) cout << "    ALL calibrations are OK" << endl;
      else if( Notok && (!Nok) ) cout << "    NO calibrations are OK" << endl;
      else
      {
         cout << "    " << ok_list->GetEntries() << " calibrations are OK, "
               << notok_list->GetEntries() << " calibrations are NOT OK" << endl;
         cout << "    " << print_list->GetName() << " :" << endl;
         print_list->ls();
      }
      cout << endl;

   }
}

//_________________________________________________________________________________

Double_t KVMultiDetArray::GetTargetEnergyLossCorrection(KVReconstructedNucleus* ion)
{
   // Calculate the energy loss in the current target of the multidetector
   // for the reconstructed charged particle 'ion', assuming that the current
   // energy and momentum of this particle correspond to its state on
   // leaving the target.
	//
	// WARNING: for this correction to work, the target must be in the right 'state':
	//
	//      gMultiDetArray->GetTarget()->SetIncoming(kFALSE);
	//      gMultiDetArray->GetTarget()->SetOutgoing(kTRUE);
	//
	// (see KVTarget::GetParticleEIncFromERes).
	//
   // The returned value is the energy lost in the target in MeV.
   // The energy/momentum of 'ion' are not affected.

   if(fTarget&&ion) return (fTarget->GetParticleEIncFromERes(ion) - ion->GetEnergy());
   return 0;
}

//_________________________________________________________________________________

TGeoManager* KVMultiDetArray::CreateGeoManager(Double_t dx, Double_t dy, Double_t dz)
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

	if(!IsBuilt()) return NULL;

 	TGeoManager *geom = new TGeoManager(GetName(), GetTitle());
 	TGeoMaterial*matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
	TGeoMedium*Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
	TGeoVolume *top = geom->MakeBox("WORLD", Vacuum,  dx, dy, dz);
   geom->SetTopVolume(top);
	TIter next( GetListOfDetectors() ); KVDetector*det;
	while((det=(KVDetector*)next())) det->AddToGeometry();
	geom->CloseGeometry();
	return geom;
}
