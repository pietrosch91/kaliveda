/***************************************************************************
$Id: KVTelescope.cpp,v 1.29 2007/05/31 09:59:22 franklan Exp $
                          kvtelescope.cpp  -  description
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
#include "KVTelescope.h"
#include "KVDetector.h"
#include "KVNucleus.h"
#include "KVRing.h"
#include "KVLayer.h"
#include "KVGroup.h"
#include "TGraph.h"
#include "Riostream.h"
#include "TString.h"
#include "TROOT.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

using namespace std;

ClassImp(KVTelescope)
/////////////////////////////////////////////////////////////////////////
// KVTelescope
//
//An assembly of one or more layers of detectors, all having the same angular position and acceptance constitutes a detector telescope.
//Such a telescope can become part of a KVMultiDetArray by being placed in a KVRing inside one of the array's KVLayers.
//
//Begin_Html<H3>Inline Methods</H3>End_Html
// inline Bool_t KVTelescope::Fired(Option_t* opt) const
// {
//      //returns kTRUE if at least one detector has KVDetector::Fired(opt) = kTRUE (see KVDetector::Fired() method for options)

    KVTelescope::KVTelescope()
{
   init();
};


void KVTelescope::init()
{
   //default initialisation. a KVList is created to hold telescope detectors.
   //the telescope owns its detectors and will delete them when deleted itself.
   fDetectors = new KVList;
   fDetectors->SetCleanup();
   fGroup = 0;
   fRing = 0;
   fNdets = 0;
   fDepth = 0;
}

//____________________________________________________________________________

KVTelescope::~KVTelescope()
{
   //The detectors of a KVTelescope belong to it:
   //therefore it destroys them when the dtor is called.

   if (fDetectors && fDetectors->TestBit(kNotDeleted)) {
      fDetectors->Delete();
      delete fDetectors;
   }
   fDetectors = 0;
   fGroup = 0;
   fRing = 0;
   if (fDepth)
      delete[]fDepth;
   fDepth = 0;
   fNdets = 0;
}

//______________________________________________________________________________
void KVTelescope::AddClone(KVDetector * d, const int fcon)
{
   //Add a "clone" of the detector prototype 'd' to the telescope.
   //For backwards compatibility, this is the same as 'AddDetector'.
   //Use 'Add' if you want the 'prototype' to belong to the telescope.
   AddDetector(d, fcon);
}

//______________________________________________________________________________
void KVTelescope::AddDetector(KVDetector * d, const int fcon)
{
//Add a "clone" of the detector prototype 'd' to the telescope.
//The prototype can be used for several different telescopes and it is the
//user's responsibility to delete it.
//The order of adding the layers to the telescope is the order
//in which any incident particle will see the detectors when traversing
//the telescope.

   if (d) {
      KVDetector *tmp = (KVDetector *) d->Clone();
      if ((tmp ? tmp->IsZombie() : !tmp)) {
         Fatal("AddDetector", "Cloning of detector failed");
         d->Print();
         return;
      }
      fDetectors->Add(tmp);

      if (fcon == KVD_RECPRC_CNXN)
         tmp->AddToTelescope(this, KVD_NORECPRC_CNXN);
   } else {
      Warning("AddDetector", KVTELESCOPE_ADD_UNKNOWN_DETECTOR);
   }
}

//______________________________________________________________________________
void KVTelescope::Add(KVDetector * d, const int fcon)
{
//Add the detector 'd' to the telescope. This detector belongs to the telescope
//and will be destroyed when the telescope is destroyed.
//The order of adding the layers to the telescope is the order
//in which any incident particle will see the detectors when traversing
//the telescope.

   if (d) {
      fDetectors->Add(d);
      if (fcon == KVD_RECPRC_CNXN)
         d->AddToTelescope(this, KVD_NORECPRC_CNXN);
   } else {
      Warning("AddDetector", KVTELESCOPE_ADD_UNKNOWN_DETECTOR);
   }
}

//_____________________________________________________________________________
void KVTelescope::AddToRing(KVRing * kvr, const int fcon)
{
// add current telescope to a ring
   fRing = kvr;
   if (fcon == KVD_RECPRC_CNXN)
      kvr->AddTelescope(this, KVD_NORECPRC_CNXN);
}

//_________________________________________________________________________________

void KVTelescope::DetectParticle(KVNucleus * kvp,KVNameValueList* nvl)
{
   //Simulates the passage of a charged particle through all detectors of the telescope, in the order in which they
   //were added to it (corresponding to rank given by GetDetectorRank()).
   //Begin_Html
   //<img src="images/kvtelescope_detectparticle.gif">
   //End_Html
   //It should be noted that
   // (1) the small variations in effective detector thickness due to the particle's angle of incidence are not, for the moment, taken into account
   // (2) the simplified description of detector geometry used here does not take into account trajectories such as that marked "b" shown in the figure.
   //      All particles impinging on the first detector of the telescope are assumed to pass through all subsequent detectors as in "c" 
	//		(unless they stop in one of the detectors)
   //
	//The KVNameValueList, if it's defined, allows to store
	//the energy loss in the different detectors the particle goes through
	//exemple : for a Silicon-CsI telescope named SI_CSI_0401 , you will obtain:   
	//		{
	//			KVNucleus nn(6,12); nn.SetKE(1000);
	//			KVTelescope* tel = gMultiDetArray->GetTelescope("SI_CSI_0401");
	//			KVNameValueList* nvl = new KVNameValueList;
	//			tel->DetectParticle(&nn,nvl);
	//			nvl->Print();
	//		}
	//		Collection name='KVNameValueList', class='KVNameValueList', size=2
	//		 OBJ: TNamed	SI_0401	8.934231
	//		 OBJ: TNamed	CSI_0401	991.065769
	//The energy loss in each detector corresponds to those lost in active layer
	//
	//If an other particle went through the same telescope, we take it into account
	//an substract its contribution to energy loss	
	
	KVDetector *obj;
   
	TIter next(GetDetectors());
   while ((obj = (KVDetector *) next())) {
		Double_t ebefore = obj->GetEnergy();	//Energie dans le detecteur avant passage
		obj->DetectParticle(kvp);					//Detection de la particule
      ebefore -= obj->GetEnergy();				//la difference d energie avant et apres passage (donc l energie laissee par la particule)   
		if (nvl)
			nvl->SetValue(obj->GetName(),TMath::Abs(ebefore)); //Enregistrement de la perte d energie
		if (kvp->GetEnergy() <= 0.0)			
      	break;
   }
}

//_______________________________________________________________________________
void KVTelescope::Print(Option_t * opt) const
{
// print out telescope structure
//if opt="angles" the angular position is printed
//if opt="fired" only fired detectors are printed

   TIter next(fDetectors);
   KVDetector *obj;
   if (!strcmp(opt, "angles")) {
      cout << "--> KVTelescope: Thetamin =" << GetThetaMin() <<
          " Thetamax=" << GetThetaMax()
          << " Phimin=" << GetPhiMin() << " Phimax=" << GetPhiMax() <<
          endl;
   } else if (!strcmp(opt, "fired")) {
      while ((obj = (KVDetector *) next())) {

         if (obj->Fired())
            obj->Print("data");
      }
   } else {
      cout << "\n" << opt << "Structure of KVTelescope object: " <<
          ((KVTelescope *) this)->GetName() << " " << GetType() << endl;
      cout << opt <<
          "--------------------------------------------------------" <<
          endl;
      while ((obj = (KVDetector *) next())) {
         cout << opt << "Detector: " << obj->GetName() << endl;
      }
   }
}


//_________________________________________________________________________
UInt_t KVTelescope::GetDetectorRank(KVDetector * kvd)
{
   //returns position (1=front, 2=next, etc.) detector in the telescope structure
   if (kvd->GetTelescope() != this) {
      Warning("GetDetectorRank", KVTELESCOPE_RANK_UNKNOWN_DETECTOR);
      cout << endl;
      return 0;
   }
   TIter next(fDetectors);
   KVDetector *d;
   UInt_t i = 1;
   while ((d = (KVDetector *) next())) {
      if (d == kvd)
         return i;
      i++;
   }
   return 0;
}

//______________________________________________________________________
KVDetector *KVTelescope::GetDetector(const Char_t * name) const
{
   //Return a pointer to the detector in the telescope with the name "name".
   //
   KVDetector *tmp = (KVDetector *) fDetectors->FindObjectByName(name);
   if (!tmp)
      Warning("GetDetector(const Char_t *name)",
              "Detector %s not found in telescope %s", name, GetName());
   return tmp;
}

//__________________________________________________________________________
Int_t KVTelescope::Compare(const TObject * obj) const
{
   // telescopes are sorted
   // (i) according to layer number if they are in different layers
   // (ii) according to lower edge polar angle if they belong to the same layer
   // (iii) according to telescope number if they belong to the same ring

   if (((KVTelescope *) this)->GetLayerNumber() !=
       ((KVTelescope *) obj)->GetLayerNumber()) {
// not in the same layer - sort according to layer number, smallest first
//(closest to target)
      if (((KVTelescope *) this)->GetLayerNumber() <
          ((KVTelescope *) obj)->GetLayerNumber())
         return -1;
      else if (((KVTelescope *) this)->GetLayerNumber() >
               ((KVTelescope *) obj)->GetLayerNumber())
         return 1;
      else
         return 0;
   }
   if (((KVTelescope *) this)->GetRingNumber() !=
       ((KVTelescope *) obj)->GetRingNumber()) {
// not in the same ring - sort according to lower edge polar angle
      if (((KVTelescope *) this)->GetThetaMin() <
          ((KVTelescope *) obj)->GetThetaMin())
         return -1;
      else if (((KVTelescope *) this)->GetThetaMin() >
               ((KVTelescope *) obj)->GetThetaMin())
         return 1;
      else
         return 0;
   }
   if (((KVTelescope *) this)->GetRingNumber() ==
       ((KVTelescope *) obj)->GetRingNumber()) {
// same ring - sort according to telescope number
      if (((KVTelescope *) this)->GetNumber() <
          ((KVTelescope *) obj)->GetNumber())
         return -1;
      else if (((KVTelescope *) this)->GetNumber() >
               ((KVTelescope *) obj)->GetNumber())
         return 1;
      else
         return 0;
   }
   return 0;
}

//_______________________________________________________________________________
void KVTelescope::ResetDetectors()
{
   // Reset Energy losses to be ready for the next event
   fDetectors->Execute("Clear", "");
}

//___________________________________________________________________________________________
Bool_t KVTelescope::ContainsType(const Char_t * type) const
{
   //Returns kTRUE if the telescope contains a detector of given type (KVDetector::GetType())
   TIter nextdet(fDetectors);
   KVDetector *kvd;
   while ((kvd = (KVDetector *) nextdet())) {
      TString dtype(kvd->GetType());
      if (dtype.Contains(type))
         return kTRUE;
   }
   return kFALSE;
}

//___________________________________________________________________________________________
void KVTelescope::RemoveDetectors(const Char_t * type)
{
   //Remove all detectors of given type (KVDetector::GetType())
   TIter nextdet(fDetectors);
   KVDetector *kvd;
   while ((kvd = (KVDetector *) nextdet())) {
      TString dtype(kvd->GetType());
      if (dtype == type)
         RemoveDetector(kvd);
   }
}

//____________________________________________________________________________________
KVLayer *KVTelescope::GetLayer() const
{
   return (GetRing()? GetRing()->GetLayer() : NULL);
}

//_____________________________________________________________________________
UInt_t KVTelescope::GetLayerNumber()
{
   return (GetLayer()? GetLayer()->GetNumber() : 0);
}

//_____________________________________________________________________________
UInt_t KVTelescope::GetRingNumber()
{
   return (GetRing()? GetRing()->GetNumber() : 0);
}

//__________________________________________________________________________
const Char_t *KVTelescope::GetRingName() const
{
   return (GetRing()? GetRing()->GetName() : "");
}

//_____________________________________________________________________________
const Char_t *KVTelescope::GetLayerName() const
{
   return (GetLayer()? GetLayer()->GetName() : "");
}

//_________________________________________________________________________
KVRing *KVTelescope::GetRing() const
{
   return fRing;
}

//________________________________________________________________________
KVGroup *KVTelescope::GetGroup() const
{
   return fGroup;
}

//__________________________________________________________________________
void KVTelescope::SetGroup(KVGroup * kvg)
{
   fGroup = kvg;
}

//_________________________________________________________________________
UInt_t KVTelescope::GetGroupNumber()
{
   return (GetGroup()? GetGroup()->GetNumber() : 0);
}

//___________________________________________________________________________

void KVTelescope::ReplaceDetector(KVDetector * kvd, KVDetector * new_kvd)
{
   //Replace (and destroy) given detector with a clone of the new prototype.

   //get index of detector in telescope's list
   Int_t det_num = fDetectors->IndexOf(kvd);
   if (det_num < 0) {
      Error("ReplaceDetector", "Detector %s not found", kvd->GetName());
      return;
   }
   //now remove old detector
   RemoveDetector(kvd, kTRUE, kFALSE);
   //clone prototype
   KVDetector *new_det = (KVDetector *) new_kvd->Clone();
   //put new detector in place of old one in telescope
   fDetectors->AddAt(new_det, det_num);
   new_det->AddToTelescope(this, KVD_NORECPRC_CNXN);
}

void KVTelescope::ReplaceDetector(const Char_t * name,
                                  KVDetector * new_kvd)
{
   //Replace (and destroy) named detector with a clone of the new prototype.

   ReplaceDetector(GetDetector(name), new_kvd);
}


//____________________________________________________________________________

void KVTelescope::RemoveDetector(const Char_t * name)
{
   //Remove and destroy the named detector.

   KVDetector *det = 0;
   if ((det = GetDetector(name))) {
      RemoveDetector(det);
   }
}

//____________________________________________________________________________

void KVTelescope::RemoveDetector(KVDetector * det, Bool_t kDeleteDetector,
                                 Bool_t kDeleteEmptyTelescope)
{
   //Remove detector from the telescope's list and delete the detector
   //(if kDeleteDetector=kTRUE : default).
   //A check is made if the detector belongs to the telescope.
   //If kDeleteDetector=kFALSE detector is not deleted.
   //If there are no more detectors after this, delete the telescope
   //(unless kDeleteEmptyTelescope=kFALSE).

   if (fDetectors) {
      if (fDetectors->FindObject(det)) {
         fDetectors->Remove(det);
         SetBit(kIsRemoving);   //////set flag tested in KVDetector::~KVDetector
         if (kDeleteDetector)
            delete det;
         ResetBit(kIsRemoving); //unset flag tested in KVDetector::~KVDetector
      }
      //destroy this telescope if there are no detectors left in it
      if (fDetectors->GetSize() == 0 && kDeleteEmptyTelescope)
         Delete();
   }
}

//__________________________________________________________________________________

void KVTelescope::SetDepth(UInt_t ndet, Float_t depth)
{
   //set the depth of detector number ndet(=1,2,...) in mm.

   fNdets = GetSize();
   if (!fNdets) {
      Error("SetDepth",
            "Add detectors to telescope before setting depth.");
      return;
   }
   if ((int) ndet > fNdets) {
      Error("SetDepth",
            "Cannot set depth for detector %d in %d-member telescope.",
            ndet, fNdets);
      return;
   }
   if (!fDepth)
      fDepth = new Float_t[fNdets];
   fDepth[ndet - 1] = depth;
}

//__________________________________________________________________________________

Float_t KVTelescope::GetDepth(UInt_t ndet) const
{
   //get depth of detector ndet(=1,2,...) in mm
   if (!fDepth) {
      Error("GetDepth", "Depths have not been set.");
      return -1.0;
   }
   if ((int) ndet > fNdets) {
      Error("SetDepth",
            "Cannot get depth for detector %d in %d-member telescope.",
            ndet, fNdets);
      return -1.0;
   }
   return fDepth[ndet - 1];
}

//__________________________________________________________________________________

Float_t KVTelescope::GetDepth() const
{
   //get depth of entire telescope in mm (sum of depths of detectors)
   if (!fDepth) {
      Error("GetDepth", "Depths have not been set.");
      return -1.0;
   }
   Float_t sum = 0.0;
   for (int ndet = 0; ndet < fNdets; ndet++) {
      sum += fDepth[ndet];
   }
   return sum;
}

//__________________________________________________________________________________

void KVTelescope::SetDetectorThickness(const Char_t * detector,
                                       Double_t thickness)
{
   //Set thickness of detector belonging to this telescope

   KVDetector *det = GetDetector(detector);
   if (det)
      det->SetThickness(thickness);
}

//__________________________________________________________________________________

void KVTelescope::SetDetectorTypeThickness(const Char_t * detector_type,
                                           Double_t thickness)
{
   //Set thickness of all detectors of given type belonging to this telescope
   TIter nextdet(fDetectors);
   KVDetector *kvd;
   while ((kvd = (KVDetector *) nextdet())) {
      TString dtype(kvd->GetType());
      if (dtype == detector_type)
         kvd->SetThickness(thickness);
   }
}

//___________________________________________________________________________________________

KVDetector *KVTelescope::GetDetectorType(const Char_t * type) const
{
   //Returns first instance of a detector with given type (KVDetector::GetType()) belonging to telescope
   TIter nextdet(fDetectors);
   KVDetector *kvd;
   while ((kvd = (KVDetector *) nextdet())) {
      TString dtype(kvd->GetType());
      if (dtype == type)
         return kvd;
   }
   return 0;
}

//___________________________________________________________________________________________

TGeoVolume* KVTelescope::GetGeoVolume()
{
	// Create and return TGeoVolume representing detectors in this telescope.
	
	int no_of_dets = GetDetectors()->GetEntries();
	if(no_of_dets==1){
		// single detector "telescope": just return detector volume
		return GetDetector(1)->GetGeoVolume();
	}
	TGeoVolume *mother_vol = gGeoManager->MakeVolumeAssembly(Form("%s_TEL",GetName()));
	// total length of telescope = depth of last detector + thickness of last detector
	Double_t tot_len_tel =  GetTotalLengthInCM();
	//**** BUILD & ADD DETECTOR VOLUMES ****
	TIter next(fDetectors); KVDetector *det;
	while( (det = (KVDetector*)next()) ){
		TGeoVolume* det_vol = det->GetGeoVolume();
		// position detector in telescope	
		Double_t dist = -tot_len_tel/2. + det->GetDepthInTelescope() + det->GetTotalThicknessInCM()/2.;
		TGeoTranslation *tran = new TGeoTranslation(0., 0., dist);
		mother_vol->AddNode(det_vol, 1, tran);
	}
	return mother_vol;
}

void KVTelescope::AddToGeometry()
{
	// Construct and position a TGeoVolume shape to represent this telescope in the current geometry
	if(!gGeoManager) return;

	// get volume for telescope
	TGeoVolume* vol = GetGeoVolume();

	// rotate telescope to orientation corresponding to (theta,phi)
	Double_t theta = GetTheta(); Double_t phi = GetPhi();
	TGeoRotation rot1, rot2;
	rot2.SetAngles(phi+90., theta, 0.);
	rot1.SetAngles(-90., 0., 0.);
	Double_t tot_len_tel = GetTotalLengthInCM();
	// distance to telescope centre = distance to telescope + half total lenght of telescope
	Double_t dist = GetDistance()/10. + tot_len_tel/2.;
	// translate telescope to correct distance from target (note: reference is CENTRE of telescope)
	Double_t trans_z = dist;

	TGeoTranslation tran(0,0,trans_z);
	TGeoHMatrix h = rot2 * tran * rot1;
	TGeoHMatrix *ph = new TGeoHMatrix(h);

	// add telescope volume to geometry
	gGeoManager->GetTopVolume()->AddNode(vol, 1, ph);
}

   Double_t KVTelescope::GetTotalLengthInCM() const
   {
   	// calculate total length of telescope from entrance of first detector to
   	// backside of last detector
		int no_of_dets = GetDetectors()->GetEntries();
		Double_t tot_len_tel = GetDepthInCM(no_of_dets) + GetDetector(no_of_dets)->GetTotalThicknessInCM();
		return tot_len_tel;
   }

