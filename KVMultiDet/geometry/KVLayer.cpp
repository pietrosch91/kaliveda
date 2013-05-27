/***************************************************************************
$Id: KVLayer.cpp,v 1.17 2006/10/19 14:32:43 franklan Exp $
                          kvlayer.cpp  -  description
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
#include "KVLayer.h"
#include "KVParticle.h"
#include "KVTelescope.h"
#include "KVRing.h"
#include "KVEvent.h"
#include "KVMultiDetArray.h"
#include "KVLayerBrowser.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

using namespace std;

ClassImp(KVLayer)
////////////////////////////////////////////////////////////////////////////////////////
//Begin_Html
//<img src="images/kvmultidetarray_diag.gif"><br>
//
//<h3>KVLayer: collection of rings in a multidetector array</h3>
//End_Html
//
//A layer is composed of rings of telescopes (usually, though not necessarily, of the same type) all at the "same distance" from the target in a multidetector array.
//The geometry of different layers in the same array is not necessarily the same. They are used to define the hierarchy of detectors with respect to their distance
//from the target, i.e. the order in which a particle leaving the target will pass through them. The layer number (GetNumber()), defined by the order in which layers
//are added to the array, increases with increasing distance from the target i.e. particles always pass through the telescopes in layer 0 before those in layer 1
//(unless there is a hole in layer 0 where particles can pass directly to the layer 1 telescopes!).
    KVLayer::KVLayer()
{
   //default ctor
   init();
}


//______________________________________________________________________________
KVLayer::~KVLayer()
{
   //dtor: destroys all rings in layer

   if (fRings && fRings->TestBit(kNotDeleted)) {

      fRings->Delete();
      delete fRings;
   }
   fRings = 0;
   fBrowser = 0;
   fArray = 0;
   strcpy(fDyName, "");
}

void KVLayer::init()
{
   //default initialisation
   fRings = new KVList;
   fRings->SetCleanup();
   fBrowser = 0;
   fArray = 0;
   strcpy(fDyName, "");
}

void KVLayer::AddRing(KVRing * kvr, UInt_t fcon)
{

   //Add a previously defined ring to the layer.

   fRings->Add(kvr);
   if (fcon == KVR_RCPRC_CNXN)
      kvr->AddToLayer(this, KVR_NORCPRC_CNXN);
}

KVRing *KVLayer::AddRing()
{

//Create and add a new ring to the layer.

   KVRing *kvr = new KVRing;
   fRings->Add(kvr);
   kvr->AddToLayer(this, KVR_NORCPRC_CNXN);
   return kvr;
}

//_______________________________________________________________________________________

void KVLayer::Print(Option_t * opt) const
{
   //Print out detailed structure of layer.

   TIter next(fRings);
   KVRing *obj;
   cout << "Structure of KVLayer object: " << ((KVLayer *) this)->
       GetName() << endl;
   cout << "--------------------------------------------------------" <<
       endl;
   while ((obj = (KVRing *) next())) {
      obj->Print("    ");
   }
}


//_______________________________________________________________________________________

Int_t KVLayer::Compare(const TObject * obj) const
{
//For sorting lists of layer according to layer number.
   if (this->GetNumber() < ((KVLayer *) obj)->GetNumber())
      return -1;
   else if (this->GetNumber() > ((KVLayer *) obj)->GetNumber())
      return 1;
   else
      return 0;
}

//____________________________________________________________________________________
const Char_t *KVLayer::GetName() const
{
   //If name of layer has not been explicitly set with SetName(Char_t*),
   //return name as Layer 1, Layer 2 etc.

   if (!strcmp(fName.Data(), "")) {
      sprintf(((KVLayer *) this)->fDyName, "Layer %d", GetNumber());
      return fDyName;
   } else
      return fName.Data();
}

//________________________________________________________________________________________

void KVLayer::SetArray(KVMultiDetArray * arr)
{

   //set pointer to multidetector array parent of layer

   fArray = arr;
}

//________________________________________________________________________________________

KVMultiDetArray *KVLayer::GetArray()
{

   //return pointer to multidetector array parent of layer

   return fArray;
}

//________________________________________________________________________________________

void KVLayer::RemoveRing(const Char_t * name)
{
   //Remove ring "name" from layer (and destroy the ring)
   KVRing *tmp = GetRing(name);
   if (tmp) {
      fRings->Remove(tmp);
      delete tmp;
   }
}

//________________________________________________________________________________________

void KVLayer::RemoveRing(UInt_t num)
{
   //Remove numbered ring from layer (and destroy the ring)
   KVRing *tmp = GetRing(num);
   if (tmp) {
      fRings->Remove(tmp);
      delete tmp;
   }
}

//________________________________________________________________________________________

void KVLayer::RemoveRing(KVRing * ring, Bool_t kDeleteRing,
                         Bool_t kDeleteEmptyLayer)
{
   //Remove ring from the layer's list and delete the ring
   //(if kDeleteRing=kTRUE : default).
   //A check is made if the ring belongs to the layer.
   //If kDeleteRing=kFALSE ring is not deleted.
   //If there are no more rings after this, delete the layer
   //(unless kDeleteEmptyLayer=kFALSE)

   if (fRings) {
      if (fRings->FindObject(ring)) {
         fRings->Remove(ring);
         SetBit(kIsRemoving);   //////set flag tested in KVRing dtor
         if (kDeleteRing)
            delete ring;
         ResetBit(kIsRemoving); //unset flag tested in KVRing dtor
      }
      if (fRings->GetSize() == 0 && kDeleteEmptyLayer)
         Delete();
   }
}

//___________________________________________________________________________________________

TGeoVolume* KVLayer::GetGeoVolume()
{
	// Create and return TGeoVolume representing detectors in this layer
	
	TGeoVolume *mother_vol = gGeoManager->MakeVolumeAssembly(GetName());
	//**** BUILD & ADD Rings ****
	TIter next(fRings); KVRing*det;
	while( (det = (KVRing*)next()) ){
		TGeoVolume* det_vol = det->GetGeoVolume();
		// position ring in layer
        TGeoTranslation* tr = new TGeoTranslation(0,0,det->GetDistance());//distance set in KVRing::GetGeoVolume()
		mother_vol->AddNode(det_vol, 1, tr);
	}
	return mother_vol;
}

void KVLayer::AddToGeometry()
{
	// Construct and position a TGeoVolume shape to represent this layer in the current geometry
	if(!gGeoManager) return;

	// get volume for layer
	TGeoVolume* vol = GetGeoVolume();

	// add to geometry
	TGeoTranslation* tr = new TGeoTranslation(0,0,0);	

	// add ring volume to geometry
	gGeoManager->GetTopVolume()->AddNode(vol, 1, tr);
}


