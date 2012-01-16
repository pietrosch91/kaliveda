/***************************************************************************
$Id: KVRing.cpp,v 1.22 2007/05/31 09:59:22 franklan Exp $
                          kvring.cpp  -  description
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
#include "TTree.h"
#include "TCollection.h"
#include "KVRing.h"
#include "KVDetector.h"
#include "KVParticle.h"
#include "KVLayer.h"
#include "TROOT.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

ClassImp(KVRing);

///////////////////////////////////////////////////////////////////////////////////////////////////
//Begin_Html
//<img src="images/kvmultidetarray_diag.gif"><br>
//
//<h3>KVRing: set of telescopes with same polar angle</h3>
//End_Html
//
//Represents one axially symmetric ring of telescopes in a multidetector array. However, by choosing theta max/min and phi max/min appropriately, a KVRing can
//have widely differing geometries. It could indeed represent only one telescope at some specific angular location etc. etc. A ring is not directly added to the multidetector
//geometry, but rather to a KVLayer object, used to define the relative "distance" from the target to the ring.
//
//Rings are not usually named; instead they are identified by their number (GetNumber()). Unless the name of the ring is explicitly set with SetName(), the
//name "Ring n" will be given by GetName(), with n=the number of the ring (usually but not always corresponds to order in which rings are added to their parent
//layer.

KVRing::KVRing()
{
   init();
}

void KVRing::init()
{
   fTelescopes = new KVList;
   fTelescopes->SetCleanup();
   fLayer = 0;
   fProto = 0;
   fStep = 1;
   strcpy(fDyName, "");
}

//_______________________________________________________________________________________

KVRing::KVRing(const UInt_t ring, const Float_t thmin,
               const Float_t thmax, const Float_t phi_0,
               const UInt_t ntel, const Float_t dist,
               KVTelescope * proto, UInt_t step, UInt_t num_first,
               Float_t dphi)
{
// create a ring with given angular specifications, number of telescopes,
// all of which are identical clones of the prototype telescope
// use TYPE of prototype telescope as TYPE of ring
   //const UInt_t ring = ring number
   //const Float_t thmin = minimum polar angle of ring
   //const Float_t thmax = maximum polar angle of ring
   //const Float_t phi_0 = azimuthal angle of centre of first telescope in ring
   //The azimuthal size of the telescopes is taken from the proto
   //const UInt_t ntel = number of telescopes in ring
   //const Float_t dist = distance from target of face d'entree of telescope
   //KVTelescope * proto = telescope prototype
   //UInt_t step = step for numbering telescopes in ring (default: 1)
   //UInt_t num_first = telescope number of first telescope (default: 1)
   //Float_t dphi = azimuthal angle between centres of adjacent telescopes (default: calculated from number of telescopes)

   init();
   SetPolarMinMax(thmin, thmax);
   Float_t phi_min = phi_0 - 0.5 * (proto->GetAzimuthalWidth());
   phi_min = (phi_min < 0. ? phi_min + 360. : phi_min);
   dphi = (dphi < 0. ? 360. / ntel : dphi);
   Float_t phi_max =
       phi_0 + ((ntel - 1.0) * dphi) + 0.5 * (proto->GetAzimuthalWidth());
   phi_max = (phi_max > 360. ? phi_max - 360. : phi_max);
   SetAzimuthalMinMax(phi_min, phi_max);
   fProto = proto;              // keep address of prototype
   fStep = step;
   SetNumber(ring);

   Float_t phi = phi_0;
   KVTelescope *kvt;
   UInt_t counter = num_first;
   for (register unsigned int i = 0; i < ntel; i++) {
      kvt = (KVTelescope *) proto->Clone();     //make new clone of prototype
      kvt->SetPolarMinMax(thmin, thmax);
      kvt->SetPhi(phi);
      phi = (phi + dphi > 360. ? (phi + dphi - 360.) : (phi + dphi));
      kvt->SetNumber(counter);
      counter += step;
      kvt->SetDistance(dist);
      AddTelescope(kvt);        //add telescope to ring
   }
   SetType(proto->GetType());
}


//_______________________________________________________________________________________
KVRing::~KVRing()
{
   //Each telescope belongs to one and only one ring, so the ring should be able to
   //delete its members.

   if (fTelescopes && fTelescopes->TestBit(kNotDeleted)) {
      fTelescopes->Delete();
      delete fTelescopes;
   }
   fTelescopes = 0;
   fLayer = 0;
   fProto = 0;
   fStep = 1;
   strcpy(fDyName, "");
}

//_______________________________________________________________________________________
void KVRing::AddTelescope(KVTelescope * tele, const int fcon)
{
// add detector telescope to current ring
   fTelescopes->Add(tele);
   if (fcon == KVD_RECPRC_CNXN)
      tele->AddToRing(this, KVD_NORECPRC_CNXN);
}

void KVRing::AddTelescope()
{
//create and add new detector telescope to current ring
   KVTelescope *tele = new KVTelescope;
   fTelescopes->Add(tele);
   tele->AddToRing(this, KVD_NORECPRC_CNXN);
}

//_______________________________________________________________________________________
void KVRing::AddToLayer(KVLayer * kvl, UInt_t fcon)
{
   fLayer = kvl;
   if (fcon == KVR_RCPRC_CNXN)
      kvl->AddRing(this, KVR_NORCPRC_CNXN);
}

//_______________________________________________________________________________________
void KVRing::Print(Option_t * opt) const
{
// print out ring structure 
   TIter next(fTelescopes);
   KVTelescope *obj;
   if (!strcmp(opt, "angles")) {
      cout << "KVRing: Thetamin=" << GetThetaMin() << " Thetamax=" <<
          GetThetaMax()
          << " Phimin=" << GetPhiMin() << " Phimax=" << GetPhiMax() <<
          endl;
      while ((obj = (KVTelescope *) next())) {
         obj->Print("angles");
      }
   } else {
      cout << "\n" << opt << "Structure of KVRing object: " << ((KVRing *)
                                                                this)->
          GetName() << endl;
      cout << opt <<
          "--------------------------------------------------------" <<
          endl;
      while ((obj = (KVTelescope *) next())) {
         obj->Print("        ");
      }
   }
}

//_______________________________________________________________________________________
KVTelescope *KVRing::GetTelescope(Float_t phi) const
{
   // give pointer to telescope in ring which covers azimuthal angle phi
   TIter next(fTelescopes);
   KVTelescope *obj;
   while ((obj = (KVTelescope *) next())) {     // loop over telescopes in ring
      if (obj->IsInPhiRange(phi))
         return obj;
   }
   return NULL;
}

//_______________________________________________________________________________________
KVTelescope *KVRing::GetTelescope(const Char_t * name) const
{
   // give pointer to telescope in ring with name
   return (KVTelescope *) fTelescopes->FindObjectByName(name);
}

//____________________________________________________________________________________________
Int_t KVRing::Compare(const TObject * obj) const
{
   // rings are sorted according to lower edge polar angle
   if (this->GetThetaMin() < ((KVRing *) obj)->GetThetaMin())
      return -1;
   else if (this->GetThetaMin() > ((KVRing *) obj)->GetThetaMin())
      return 1;
   else
      return 0;
}

//____________________________________________________________________________________

const Char_t *KVRing::GetName() const
{
   return ((KVRing *) this)->GetRingName();
}

//____________________________________________________________________________________________

const Char_t *KVRing::GetRingName()
{
   // If name of ring has not been explicitly set with SetName(Char_t*),
   // return name as Ring 1, Ring 2 etc.

   if (!strcmp(fName.Data(), "")) {
      sprintf(fDyName, "Ring %d", GetNumber());
      return fDyName;
   } else
      return fName.Data();
}

//_____________________________________________________________________________________

void KVRing::SetNumberTelescopes(UInt_t num)
{

   // change number of telescopes in ring, keeping same theta_min/max, phi_min/max,
   // and telescope prototype

   //remove all telescopes from ring
   fTelescopes->Delete();

   Float_t dphi = (GetPhiMax() - GetPhiMin()) / num;
   Float_t phi2, phi = 0.;
   KVTelescope *kvt;
   UInt_t counter = 1;

   for (register unsigned int i = 0; i < num; i++) {
      kvt = (KVTelescope *) GetPrototype()->Clone();    // make new clone of prototype
      kvt->SetPolarMinMax(GetThetaMin(), GetThetaMax());
      phi2 = (phi + dphi > 360. ? (phi + dphi - 360.) : (phi + dphi));
      kvt->SetAzimuthalMinMax(phi, phi2);
      phi = phi2;
      kvt->SetNumber(counter);
      counter += fStep;
      kvt->AddToRing(this, KVD_NORECPRC_CNXN);
      kvt->SetBit(kMustCleanup);
      fTelescopes->Add(kvt);
   }
}

void KVRing::SetLayer(KVLayer * lay)
{
   //
   //Set layer to which ring belongs
   //
   fLayer = lay;
}

void KVRing::ReplaceTelescope(KVTelescope * oldT, KVTelescope * newT)
{
   //Replace the named telescope with a clone of the new prototype "tel"

   Int_t index = fTelescopes->IndexOf(oldT);    //get index of old telescope in list
   Float_t phi = oldT->GetPhi();
   Float_t dist = oldT->GetDistance();
   Int_t number = oldT->GetNumber();

   //remove old telescope
   RemoveTelescope(oldT, kTRUE, kFALSE);

   //clone proto
   KVTelescope *new_tel = (KVTelescope *) newT->Clone();
   new_tel->SetPhi(phi);
   new_tel->SetDistance(dist);
   new_tel->SetNumber(number);
   new_tel->SetPolarMinMax(GetThetaMin(), GetThetaMax());
   //put new telescope in place of old one in ring
   fTelescopes->AddAt(new_tel, index);
   new_tel->AddToRing(this, KVD_NORECPRC_CNXN);
}

void KVRing::RemoveTelescope(const Char_t * name)
{
   //Remove named telescope from ring and destroy it
   KVTelescope *tmp = GetTelescope(name);
   //the telescope dtor removes it from the ring's list of telescopes etc.
   if (tmp)
      RemoveTelescope(tmp);
}

void KVRing::RemoveTelescope(KVTelescope * tel, Bool_t kDeleteTelescope,
                             Bool_t kDeleteEmptyRing)
{
   //Remove telescope from the ring's list and delete the telescope (if kDeleteTelescope
   //=kTRUE : default). A check is made if the telescope belongs to the ring.
   //If kDeleteTelescope=kFALSE telescope is not deleted.
   //If there are no more telescopes after this, delete the ring (unless
   //kDeleteEmptyRing=kFALSE)

   if (fTelescopes) {
      if (fTelescopes->FindObject(tel)) {
         fTelescopes->Remove(tel);
         SetBit(kIsRemoving);   //////set flag tested in KVTelescope dtor
         if (kDeleteTelescope)
            delete tel;
         ResetBit(kIsRemoving); //unset flag tested in KVTelescope dtor
      }
      if (fTelescopes->GetSize() == 0 && kDeleteEmptyRing)
         Delete();
   }
}

//______________________________________________________________________________________________

void KVRing::RemoveDetectors(const Char_t * type)
{
   //Remove all detectors of given type from telescopes of ring
//      GetTelescopes()->Execute("RemoveDetectors",Form("\"%s\"",type));
   TIter next(GetTelescopes());
   KVTelescope *tele;
   while ((tele = (KVTelescope *) next()))
      tele->RemoveDetectors(type);
}

//___________________________________________________________________________________________

TGeoVolume* KVRing::GetGeoVolume()
{
	// Create and return TGeoVolume representing detectors in this ring.
	
	TString name(GetName());
	name.ToUpper();
	name.ReplaceAll(" ","_");
	TGeoVolume *mother_vol = gGeoManager->MakeVolumeAssembly(name.Data());
	//**** BUILD & ADD TELESCOPEs ****
	TIter next(fTelescopes); KVTelescope *det;
	while( (det = (KVTelescope*)next()) ){
		TGeoVolume* det_vol = det->GetGeoVolume();
		// position telescope in ring
		// rotate telescope to orientation corresponding to (theta,phi)
		Double_t theta = det->GetTheta(); Double_t phi = det->GetPhi();
		TGeoRotation rot1, rot2;
		rot2.SetAngles(phi+90., theta, 0.);
		rot1.SetAngles(-90., 0., 0.);
		Double_t tot_len_tel = det->GetTotalLengthInCM();
		// distance to telescope centre = distance to telescope + half total length of telescope
		Double_t dist = det->GetDistance()/10. + tot_len_tel/2.;
		// translate telescope to correct distance from target (note: reference is CENTRE of telescope)
		Double_t trans_1 = dist;
		// translate telescope so that centre of ring is on origin
		Double_t trans_2 = dist*TMath::Cos(theta*TMath::DegToRad());
		// set distance for ring = distance between origin and centre of ring
		SetDistance(10.*trans_2);// distance in mm

		TGeoTranslation tran1(0,0,trans_1);
		TGeoTranslation tran2(0,0, -trans_2);
		TGeoHMatrix h = tran2 * rot2 * tran1 * rot1;
		TGeoHMatrix *ph = new TGeoHMatrix(h);
		
		mother_vol->AddNode(det_vol, 1, ph);
	}
	return mother_vol;
}

void KVRing::AddToGeometry()
{
	// Construct and position a TGeoVolume shape to represent this ring in the current geometry
	if(!gGeoManager) return;

	// get volume for ring
	TGeoVolume* vol = GetGeoVolume();

	// translate ring to correct distance
	TGeoTranslation* tr = new TGeoTranslation(0,0,GetDistance()/10.);//distance set in GetGeoVolume()	

	// add ring volume to geometry
	gGeoManager->GetTopVolume()->AddNode(vol, 1, tr);
}


