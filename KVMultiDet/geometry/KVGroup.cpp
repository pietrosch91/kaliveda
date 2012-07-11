/***************************************************************************
                          kvgroup.cpp  -  description
                             -------------------
    begin                : Fri May 24 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVGroup.cpp,v 1.35 2008/01/17 15:17:20 franklan Exp $
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
#include "KVGroup.h"
#include "KVNucleus.h"
#include "KVList.h"
#include "KVTelescope.h"
#include "KVDetector.h"
#include "KVMultiDetArray.h"
#include "KVLayer.h"
#include "KVIDTelescope.h"
#include "KVReconstructedNucleus.h"
#include "TROOT.h"
#include "KVNameValueList.h"
#include "KVIDGraph.h"

using namespace std;

ClassImp(KVGroup);
/////////////////////////////////////////////////////////////////////
//KVGroup
//
//The notion of a "group" of telescopes is used to associate detectors in different layers (in principle with different geometries) through which
//particles leaving the target may pass. This is essential for particle reconstruction in the case where the detectors in one layer of the array have a larger angular
//acceptance than the others: the KVGroup contains all the detectors/telescopes which are either in front of or behind this "widest" detector.
//The KVGroup is the smallest unit of the multidetctor array structure which can be treated independently of all the rest, especially concerning particle reconstruction.
//
//Begin_Html<H3>Inline Methods</H3>End_Html
// inline Bool_t KVGroup::Fired(Option_t* opt) const
// {
//      //returns kTRUE if at least one telescope has KVTelescope::Fired(opt) = kTRUE (see KVDetector::Fired() method for options)

//_____________________________________________________________________________________

KVGroup::KVGroup()
{
   init();
}

//_________________________________________________________________________________

void KVGroup::init()
{
   //Default initialisation

   fTelescopes = new KVList(kFALSE);
   fTelescopes->SetCleanup();
   fNumberOfLayers = 0;
   fLayNumMin = 0;
   fLayNumMax = 0;
   fReconstructedNuclei = 0;
   fDetectors = 0;
}

//_________________________________________________________________________________

KVGroup::KVGroup(const KVGroup & obj)
{
   //copy ctor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVGroup &) obj).Copy(*this);
#endif
}

//_____________________________________________________________________________________

KVGroup::~KVGroup()
{

   //Set all telescope group pointers to 0
   KVTelescope *tel = 0;
   if (fTelescopes && fTelescopes->TestBit(kNotDeleted)) {
      TIter next(fTelescopes);
      while ((tel = (KVTelescope *) next()))
         tel->SetGroup(0);
      fTelescopes->Clear();
      delete fTelescopes;
   }
   fTelescopes = 0;
   if (fReconstructedNuclei && fReconstructedNuclei->TestBit(kNotDeleted)) {
      fReconstructedNuclei->Clear();
      delete fReconstructedNuclei;
      fReconstructedNuclei = 0;
   }
   fNumberOfLayers = 0;
   fLayNumMin = 0;
   fLayNumMax = 0;
   fReconstructedNuclei = 0;
   if (fDetectors && fDetectors->TestBit(kNotDeleted)) {
      fDetectors->Clear();
      delete fDetectors;
      fDetectors = 0;
   }
}

//_____________________________________________________________________________________

void KVGroup::Add(KVTelescope * kvt)
{
//Add a telescope to the current group.
   if (kvt) {
      fTelescopes->Add(kvt);
      kvt->SetGroup(this);
   } else
      Warning("Add", KVGROUP_ADD_UNKNOWN_TELESCOPE);
}

Bool_t KVGroup::Contains(KVTelescope * tele) const
{
   //kTRUE if telescope belongs to this group

   if (fTelescopes->FindObject(tele))
      return kTRUE;
   return kFALSE;
}

//_____________________________________________________________________________________

void KVGroup::SetDimensions()
{
   //Set dimensions of group according to dimensions of all its telescopes.
   KVTelescope *tel, *tel1;
   TIter next(fTelescopes);
   tel = (KVTelescope *) next();
   if (!tel)
      return;
   tel1 = (KVTelescope *) next();
   if (!tel1)
      return;
   SetDimensions(tel, tel1);
   while ((tel = (KVTelescope *) next())) {
      SetDimensions(this, tel);
   }
}

//_____________________________________________________________________________________

void KVGroup::SetDimensions(KVPosition * p1, KVPosition * p2)
{
   //Adjust angular dimensions of group according to theta-min/max, phi-min/max
   //of p1 and p2, where p1 and p2 are either two telescopes or a group (most
   //probably this group) and a telescope.
   //For theta-min/max, it is the smallest/largest angle which is used for the group dimension.
   //For azimuthal/phi angles, all 4 combinations of "min" and "max" are tried and the one
   //which gives the greatest azimuthal width to the group is kept.

   Float_t theta_min = TMath::Min(p1->GetThetaMin(), p2->GetThetaMin());
   Float_t theta_max = TMath::Max(p1->GetThetaMax(), p2->GetThetaMax());
   SetPolarMinMax(theta_min, theta_max);

   KVPosition *pp[2];           //array of pointers
   pp[0] = p1;
   pp[1] = p2;
   Float_t max_width = 0.;
   for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
         if (GetAzimuthalWidth(pp[i]->GetPhiMin(), pp[j]->GetPhiMax()) >
             max_width) {
            SetAzimuthalMinMax(pp[i]->GetPhiMin(), pp[j]->GetPhiMax());
            max_width = GetAzimuthalWidth();
         }
      }
   }
}

//_____________________________________________________________________________________

void KVGroup::Print(Option_t * opt) const
{
//Print out the characteristics of the group and its member telescopes.
//  --> KVGroup::Print("angles") prints out the ranges in theta and phi of the group.
//  --> KVGroup::Print() just prints out all the detectors in the group.
//  --> KVGroup::Print("fired") only print fired detectors (i.E. KVDetector::Fired()==kTRUE)
   TIter next(fTelescopes);
   KVTelescope *obj;
   if (!strcmp(opt, "fired")) {
      while ((obj = (KVTelescope *) next())) {
         obj->Print(opt);
      }
   } else {
      cout << "\n";
      if (strcmp(opt, "angles"))
         cout << opt;
      cout << "Structure of KVGroup object: ";
      if (strcmp(GetName(), "")) {
         cout << GetName();
      } else {
         cout << GetNumber();
      }
      cout << endl;
      if (strcmp(opt, "angles"))
         cout << opt;
      cout << "--------------------------------------------------------" <<
          endl;
      if (!strcmp(opt, "angles")) {
         cout << "Thetamin=" << GetThetaMin() << " Thetamax=" <<
             GetThetaMax()
             << " Phimin=" << GetPhiMin() << " Phimax=" << GetPhiMax() <<
             endl;
      }
      while ((obj = (KVTelescope *) next())) {
         obj->Print("        ");
      }
   }
}

//_____________________________________________________________________________________

void KVGroup::Sort()
{
   //Make sure telescopes are ordered by increasing layer number i.e. increasing distance from target.
   //This is so that when simulating the energy losses of a charged particle passing
   //through the telescopes of the group, we get it in the right order!

   fTelescopes->Sort();
}

//_____________________________________________________________________________________

KVNameValueList*  KVGroup::DetectParticle(KVNucleus * part)
{
   //Calculate energy losses of a charged particle traversing the telescopes of the group.
	//This method return a list of TNamed where each detector which is throught in the particle
	//are written with the corrresponding energy loss
	//WARNING : this KVNameValueList has to be deleted by the user
	//				after use
	//return 0 if no telescope are on the path of the particle (DEAD zone)
   TList *d = GetTelescopes(part->GetTheta(), part->GetPhi());
   if (d) {
      TIter nextd(d);
      KVTelescope *t = 0;
      KVNameValueList* nvl = new KVNameValueList();
		while ((t = (KVTelescope *) nextd())) {
         t->DetectParticle(part,nvl);
         if (part->GetEnergy() <= 0.0){
				delete d;
				return nvl;
			}	
		}
      delete d;
   	return nvl;
	}
	return 0;
}

//_____________________________________________________________________________________

TList *KVGroup::GetTelescopes(Float_t theta, Float_t phi) const
{
   //Create and fill list of telescopes in group at position (theta,phi),
   //sorted according to distance from target (smallest layer number i.e. closest first).
   //User must delete list after use.

   TIter next(fTelescopes);
   KVTelescope *t;
   TList *list = 0;
   while ((t = (KVTelescope *) next())) {
      if (t->IsInPolarRange(theta) && t->IsInPhiRange(phi)) {
         if (!list)
            list = new TList;
         list->Add(t);
      }
   }
   if (list)
      list->Sort();
   return list;
}

//_____________________________________________________________________________________

TList *KVGroup::GetTelescopesInLayer(UInt_t nlayer)
{
   //Create and fill list of telescopes belonging to Layer number nlayer in the group.
   //User must delete list after use.

   TIter next(fTelescopes);
   KVTelescope *t;
   TList *list = 0;
   while ((t = (KVTelescope *) next())) {
      if (t->GetLayerNumber() == nlayer) {
         if (!list)
            list = new TList;
         list->Add(t);
      }
   }
   return list;
}

//________________________________________________________________________________

KVDetector *KVGroup::GetDetector(const Char_t * name)
{
   //find named detector in group

   KVDetector *det = (KVDetector *) GetDetectors()->FindObjectByName(name);
   return det;
}

//________________________________________________________________________________

KVTelescope *KVGroup::GetTelescope(const Char_t * name)
{
   //find named telescope in group
   return (KVTelescope *) (fTelescopes->FindObjectByName(name));
}

//______________________________________________________________________________

void KVGroup::Reset()
{
   //Reset the group, i.e. wipe the list of reconstructed nuclei and call "Clear" method of
   //each and every detector in the group.

   if (fReconstructedNuclei && fReconstructedNuclei->GetSize()) {
      fReconstructedNuclei->Clear();
   }
   //reset energy loss and KVDetector::IsAnalysed() state
   //plus ACQParams set to zero
   GetDetectors()->R__FOR_EACH(KVDetector, Clear)();
}

//_________________________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVGroup::Copy(TObject & obj) const
#else
void KVGroup::Copy(TObject & obj)
#endif
{
   //
   //Copy this to obj
   //
   KVPosition::Copy(obj);
   ((KVGroup &) obj).SetTelescopes(GetTelescopes());
}

//_________________________________________________________________________________

void KVGroup::SetTelescopes(KVList * list)
{
   //
   //Set list of telescopes by copying list
   //
   list->Copy(*fTelescopes);
}

//_________________________________________________________________________________

void KVGroup::AddHit(KVReconstructedNucleus * kvd)
{
   if (!fReconstructedNuclei) {
      fReconstructedNuclei = new KVList(kFALSE);
      fReconstructedNuclei->SetCleanup();
   }
   fReconstructedNuclei->Add(kvd);
}

//_________________________________________________________________________________

void KVGroup::RemoveHit(KVReconstructedNucleus * kvd)
{
   //Remove reconstructed nucleus from group's list of reconstructed
   //particles.
   if (fReconstructedNuclei) {
      fReconstructedNuclei->Remove(kvd);
      if (fReconstructedNuclei->GetSize() == 0) {
         delete fReconstructedNuclei;
         fReconstructedNuclei = 0;
      }
   } else {
      Warning("RemoveHit", "No reconstructed nuclei in this group");
   }
}

//_________________________________________________________________________________

void KVGroup::CountLayers()
{
   //Count the number of different layers to which telescopes in the group belong.
   //This is based on different layers having different numbers.
   //The layer closest to the target is assumed to have the smallest layer number,
   //the layer furthest from the target is assumed to have the largest layer number.
   fNumberOfLayers = 0;
   fLayNumMin = 99;
   fLayNumMax = 0;
   if (fTelescopes) {
      TIter ntel(fTelescopes);
      KVTelescope *tel;
      UInt_t laynums[10];
      while ((tel = (KVTelescope *) ntel())) {
         if (fNumberOfLayers) {
            Bool_t found = kFALSE;
            //Check to make sure layer number not already in array
            for (UInt_t i = 0; i < fNumberOfLayers; i++) {
               if (tel->GetLayerNumber() == laynums[i])
                  found = kTRUE;
            }
            if (!found) {
               laynums[fNumberOfLayers++] = tel->GetLayerNumber();
               if (fNumberOfLayers > 9) {
                  Warning("CountLayers", "Too many layers in group");
               }
               if (tel->GetLayerNumber() > fLayNumMax)
                  fLayNumMax = tel->GetLayerNumber();
               if (tel->GetLayerNumber() < fLayNumMin)
                  fLayNumMin = tel->GetLayerNumber();
            }
         } else {
            laynums[fNumberOfLayers++] = tel->GetLayerNumber();
            if (fNumberOfLayers > 9) {
               Warning("CountLayers", "Too many layers in group");
            }
            if (tel->GetLayerNumber() > fLayNumMax)
               fLayNumMax = tel->GetLayerNumber();
            if (tel->GetLayerNumber() < fLayNumMin)
               fLayNumMin = tel->GetLayerNumber();
         }
      }
   }
}

//_________________________________________________________________________________

UInt_t KVGroup::GetLayerNearestTarget() const
{
   //
   //Returns the layer number of the layer in the group which is nearest to the target
   //i.e. the layer with the smallest layer number
   //
   if (!fLayNumMin)
      ((KVGroup *) this)->CountLayers();
   return fLayNumMin;
}

//_________________________________________________________________________________

UInt_t KVGroup::GetLayerFurthestTarget() const
{
   //
   //Returns the layer number of the layer in the group which is furthest from the target
   //i.e. the layer with the largest layer number
   //

   if (!fLayNumMax)
      ((KVGroup *) this)->CountLayers();
   return fLayNumMax;
}

//_________________________________________________________________________________

UInt_t KVGroup::GetNumberOfDetectorLayers()
{
   //Returns the total number of detector layers in the group,
   //including counting the detectors inside the telescopes

   UInt_t ndl = 0;
   UInt_t imin = GetLayerNearestTarget();
   UInt_t imax = GetLayerFurthestTarget();
   for (UInt_t i = imin; i <= imax; i++) {
      TList *list = GetTelescopesInLayer(i);
      //note we take the max number of detectors in telescopes of layer i
      Int_t max=0;
		if (list) {
      	TIter it(list);
			KVTelescope* tel = 0;
			while ( (tel = (KVTelescope* )it.Next()) )
				if (max<tel->GetDetectors()->GetSize())
					max = tel->GetDetectors()->GetSize();
			ndl+= max;
		//
		//before it was assume that all telescopes in same layer are identically constructed
		//	ndl += ((KVTelescope *) list->At(0))->GetDetectors()->GetSize();
      //
		   delete list;
      }
   }

   return ndl;
}

//_________________________________________________________________________________

TList *KVGroup::GetDetectorsInLayer(UInt_t lay)
{
   //Creates and fills a list with all the detectors in the "detector layer"
   //lay. Detector layers are always numbered from 1 (nearest target) to
   //GetNumberOfDetectorLayers().
   //Delete list after use.

	if (lay < 1)
      return 0;
   UInt_t ndl = 0;
   UInt_t imin = GetLayerNearestTarget();
   UInt_t imax = GetLayerFurthestTarget();
   for (UInt_t i = imin; i <= imax; i++) {
      TList *tlist = GetTelescopesInLayer(i);
      if (tlist) {
         //note we take the max number of detectors in telescopes of layer i
         Int_t max=0;
			TIter it(tlist);
			KVTelescope* tel = 0;
			while ( (tel = (KVTelescope* )it.Next()) )
				if (max<tel->GetDetectors()->GetSize())
					max = tel->GetDetectors()->GetSize();
			ndl += max;
			//
			//before it was assume that all telescopes in same layer are identically constructed
			//ndl += ((KVTelescope *) tlist->At(0))->GetDetectors()->GetSize();
         //
			if (ndl >= lay) {
            //the required detector layer is in the telescopes in the list

            //calculate rank of detectors in telescopes
            UInt_t rank = max - ndl + lay;
				TIter next(tlist);
            KVTelescope *tel;
            TList *list = new TList;
            while ((tel = (KVTelescope *) next())) {
               if (rank<=tel->GetSize())
						list->Add(tel->GetDetector(rank));
            }
            delete tlist;
            return list;
         }
         delete tlist;
      }
   }
   return 0;
	
}

//_________________________________________________________________________________

UInt_t KVGroup::GetDetectorLayer(KVDetector * det)
{
   //Find the "detector layer" to which this detector belongs

   TList *list;
   for (UInt_t i = 1; i <= GetNumberOfDetectorLayers(); i++) {
      list = GetDetectorsInLayer(i);
      if (list->FindObject(det)) {
         delete list;
         return i;
      }
      delete list;
   }
   return 0;
}

//_________________________________________________________________________________

TList *KVGroup::GetAlignedDetectors(KVDetector * det, UChar_t dir)
{
   //Fill TList with all detectors aligned with "det" which are closer to the target.
   //These are the detectors through which any particle stopping in "det" will have
   //to pass. By default (dir=KVGroup::kBackwards) the list starts with "det" and
   //goes towards the target. Use dir=KVGroup::kForwards to have the list in the
   //order seen by an impinging particle.
   //
   //Delete TList after use.

   TList *tmp = new TList;

   UInt_t last_layer = GetDetectorLayer(det);
   UInt_t first_layer = 1;

   if (dir == kForwards) {
      for (UInt_t lay = first_layer; lay <= last_layer; lay++) {
         TList *dets = GetDetectorsInLayer(lay);
         if (dets) {
            TIter next(dets);
            KVDetector *d2;
            while ((d2 = (KVDetector *) next())) {
               if (d2->GetTelescope()->
                   IsOverlappingWith(det->GetTelescope())) {
                  tmp->Add(d2);
               }
				}
            delete dets;
         }
      }
   } else {
      for (UInt_t lay = last_layer; lay >= first_layer; lay--) {
         TList *dets = GetDetectorsInLayer(lay);
         if (dets) {
            TIter next(dets);
            KVDetector *d2;
            while ((d2 = (KVDetector *) next())) {
               if (d2->GetTelescope()->
                   IsOverlappingWith(det->GetTelescope())) {
                  tmp->Add(d2);
               }
				}
            delete dets;
         }
      }
   }

   return tmp;
}

//_________________________________________________________________________________

void KVGroup::GetIDTelescopes(TCollection * tel_list)
{
   //Identify all the ways of identifying particles possible from the detectors
   //in the group, create the appropriate KVIDTelescope objects and add them to
   //the list pointed to by tel_list.
   //USER'S RESPONSIBILITY TO DELETE CONTENTS OF LIST!!
   //
   //Starting from each detector in the "detector layer" furthest from the
   //target, we build ID telescopes from all pairs of aligned detectors.
   //We then continue up through the layers of the group
   //
   //For each pair of detectors, it is KVMultiDetArray::GetIDTelescopes
   //which determines which KVIDTelescope class to use (specialise this method
   //in KVMultiDetArray child classes). It must also make sure that
   //each IDTelescope is added only once (i.e. check it is not already in the
   //list).

   TList *det_lay;
   for (int lay = GetNumberOfDetectorLayers(); lay > 0; lay--) {

      det_lay = GetDetectorsInLayer(lay);
      if (det_lay) {

         TIter next_det(det_lay);
         KVDetector *det;
			
         while ((det = (KVDetector *) next_det())) {
				if ( det->IsOK() ){
					//1st call: create ID telescopes, they will be added to the
            	//gMultiDetArray list of IDTelescopes
            	det->GetAlignedIDTelescopes(tel_list);
            	//2nd call: set up in the detector a list of pointers to the
            	//ID telescopes made up of it and all aligned detectors in front
            	//of it
            	det->GetAlignedIDTelescopes(0);
				}
			}
         delete det_lay;
      }
   }
}

//_________________________________________________________________________________

void KVGroup::Destroy()
{
   //This method will destroy all the telescopes of the group and then delete the group.
   //Is that really what you want to do ?
   if (fTelescopes) {
      KVTelescope *tel = 0;
      TIter next(fTelescopes);
      while ((tel = (KVTelescope *) next())) {
         RemoveTelescope(tel, kTRUE);
      }
   }
}

void KVGroup::RemoveTelescope(KVTelescope * tel, Bool_t kDeleteTelescope,
                              Bool_t kDeleteEmptyGroup)
{
   //Remove telescope from the group's list and set its fGroup member pointer = 0.
   //By default, this does not delete the telescope (kDeleteTelescope=kFALSE)
   //A check is made if the telescope belongs to the group.
   //If there are no more telescopes after this, delete the group
   //(unless kDeleteEmptyRing=kFALSE)

   if (fTelescopes) {
      if (fTelescopes->FindObject(tel)) {
         fTelescopes->Remove(tel);
         tel->SetGroup(0);
         SetBit(kIsRemoving);   //////set flag tested in KVTelescope dtor
         if (kDeleteTelescope)
            delete tel;
         ResetBit(kIsRemoving); //unset flag tested in KVTelescope dtor
      }
      CountLayers();            //update
      if (fTelescopes->GetSize() == 0 && kDeleteEmptyGroup)
         Delete();
   }
}

//______________________________________________________________________________________//

KVList *KVGroup::GetDetectors()
{
   //Fill and return a list of all the detectors in the group.
   //The list is created and filled the first time the method is called,
   //for subsequent calls we just return the pointer of the list.

   if (!fDetectors) {
      fDetectors = new KVList(kFALSE);
      fDetectors->SetCleanup();

      //fill the list
      TIter ntel(fTelescopes);
      KVTelescope *tel;
      while ((tel = (KVTelescope *) ntel())) {
         TIter ndet(tel->GetDetectors());
         KVDetector *det;
         while ((det = (KVDetector *) ndet())) {
            fDetectors->Add(det);
         }
      }
   }

   return fDetectors;
}

//______________________________________________________________________________________//

void KVGroup::AnalyseParticles()
{
   //The short description of this method is:
   //
   //      Here we analyse all the particles stopping in this group in order to
   //      determine 'KVReconstructedNucleus::GetStatus()' for each one.
   //      This is a sort of 'first-order' coherency analysis necessary prior to
   //      attempting particle identification.
   //
   //The long description of this method is:
   //
   //Analyse the different particles reconstructed in this group in order to
   //establish the strategy necessary to fully reconstruct each one
   //In a group with >=3-member telescopes, the maximum number of particles which may be
   //reconstructed correctly is equal to the number of telescopes constituting the
   //last layer of the group if all particles stop in the last detector of the last
   //layer.
   //i.e. in rings 4-9 (ChIo-Si-CsI telescopes) any group can detect up to 4 particles
   //all going through to the CsI i.e. either light particles identified from CsI R-L
   //matrices or fragments identified from Si-CsI matrices.
   //
   //Any particle detected in a group on its own is perfectly fine (fAnalStatus=0).
   //
   //Any particle detected in the final layer of a "3-detector layer" group
   //(i.e. ChIo-Si-CsI) is perfectly fine (fAnalStatus=0).
   //
   //A particle stopping in the 2nd layer of a "3-detector layer" group (i.e. in Si)
   //can be reconstructed if all the other particles in its group stop in the 3rd
   //layer (i.e. CsI), once the other particles have been reconstructed and their
   //calculated energy loss in the ChIo has been subtracted from the measured value
   //(fAnalStatus=1).
   //
   //It's a question of "segmented" or "independent" detectors. The ChIo's are not
   //"independent" because a particle passing the ChIo can hit several detectors behind
   //it, whereas any particle passing a given Si can only hit the CsI directly behind
   //that Si, because the Si (and CsI) are more "segmented" than the ChIo's.
   //Soooo...any particle crossing at least 2 "independent" detectors can be identified
   //sans probleme. In fact CsI count for 2 independent detectors themselves, because
   //light particles can be identified just from CsI information i.e. no need for
   //"delta-E" signal. Unless of course the R-L attempt fails, at which moment the
   //particle's AnalStatus may be changed and it may be up for reassessment in
   //subsequent rounds of analysis.
   //Particles hitting 0 independent detectors (I.e. stopped in ChIo)
   //can not be reconstructed. Any particle hitting only 1 independent detector can
   //be reconstructed if alone in the group i.e. no other particles or all others have
   //hit >=2 independent detectors. If more than one particle in the same group only
   //hit 1 independent detector, then one can only make a rough estimation of their
   //nature.
   if (GetNUnidentified() > 1)  //if there is more than one unidentified particle in the group
   {

      UShort_t n_nseg_1 = 0;
      if (!GetParticles()) {
         Error("AnalyseParticles", "No particles in group ?");
         return;
      }
      TIter next(GetParticles());
      KVReconstructedNucleus *nuc;
      //loop over particles counting up different cases
      while ((nuc = (KVReconstructedNucleus *) next())) {
         //ignore identified particles
         if (nuc->IsIdentified())
            continue;

         if (nuc->GetNSegDet() >= 2) {
            //all part.s crossing 2 or more independent detectors are fine
            nuc->SetStatus( KVReconstructedNucleus::kStatusOK );
         } else if (nuc->GetNSegDet() == 1) {
            //only 1 independent detector hit => depends on what's in the rest
            //of the group
            n_nseg_1++;
         } else {
            //part.s crossing 0 independent detectors (i.E. arret ChIo)
            //can not be reconstructed
            nuc->SetStatus( KVReconstructedNucleus::kStatusStopFirstStage );
         }
      }
      next.Reset();
      //loop again, setting status
      while ((nuc = (KVReconstructedNucleus *) next())) {
         if (nuc->IsIdentified())
            continue;           //ignore identified particles

         if (nuc->GetNSegDet() == 1) {
            if (n_nseg_1 == 1) {
               //just the one ? then we can get it no problem
               //after identifying the others and subtracting their calculated
               //energy losses from the "dependent"/"non-segmented" detector
               //(i.E. the ChIo)
               nuc->SetStatus( KVReconstructedNucleus::kStatusOKafterSub );
            } else {
               //more than one ? then we can make some wild guess by sharing the
               //"non-segmented" (i.e. ChIo) contribution between them, but
               //I wouldn't trust it as far as I can spit
               nuc->SetStatus( KVReconstructedNucleus::kStatusOKafterShare );
            }
            //one possibility remains: the particle may actually have stopped e.g. in the Si member
            //of a Ring 1 Si-CsI telescope, in which case AnalStatus = 3
            if (nuc->GetIDTelescopes()->GetSize() == 0) {
               //no ID telescopes with which to identify particle
               nuc->SetStatus( KVReconstructedNucleus::kStatusStopFirstStage );
            }
         }
      }
   } else if (GetNUnidentified() == 1) {
      //only one unidentified particle in group: if NSegDet>=1 then it's OK

      //loop over particles looking for the unidentified one
      TIter next(GetParticles());
      KVReconstructedNucleus *nuc;
      while ((nuc = (KVReconstructedNucleus *) next()))
         if (!nuc->IsIdentified())
            break;

      if (nuc->GetNSegDet() > 0) {
         //OK no problem
         nuc->SetStatus(KVReconstructedNucleus::kStatusOK);
      } else {
         //dead in the water
         nuc->SetStatus(KVReconstructedNucleus::kStatusStopFirstStage);
      }
      //one possibility remains: the particle may actually have stopped e.g. in the 1st member
      //of a telescope, in which case AnalStatus = 3
      if (nuc->GetIDTelescopes()->GetSize() == 0) {
         //no ID telescopes with which to identify particle
         nuc->SetStatus(KVReconstructedNucleus::kStatusStopFirstStage);
      }
   }
#ifdef KV_DEBUG
   Info("AnalyseGroups", "OK after analysis of particles in groups");
#endif
}

#if defined(R__MACOSX)
UInt_t KVGroup::GetNIdentified()
{
   //number of identified particles reconstructed in group
   UInt_t n = 0;
   if (GetHits()) {
      TIter next(fReconstructedNuclei);
      KVReconstructedNucleus *nuc = 0;
      while ((nuc = (KVReconstructedNucleus *) next()))
         n += (UInt_t) nuc->IsIdentified();
   }
   return n;
};

UInt_t KVGroup::GetNUnidentified()
{
   //number of unidentified particles reconstructed in group
   return (GetHits() - GetNIdentified());
};
#endif

void KVGroup::ClearHitDetectors()
{
	// Loop over all detectors in group and clear their list of 'hits'
	// i.e. the lists of particles which hit each detector
	GetDetectors()->R__FOR_EACH(KVDetector, ClearHits)();
}

void KVGroup::PrepareModif(KVDetector* dd)
{
	//Casse tous les liens entre les detecteurs d un meme groupe
	//Retire de la liste gMultiDetArray->GetListOfIDTelescopes() les
	//telescopes associes et les efface apres les avoir
	//retires des grilles auxquelles ils etaient associees
	//pour preparer l ajout ou le retrait d un detecteur
	//voir KVDetector::SetPresent()
	//
	//
	KVNameValueList nv;
	
	KVDetector* det = 0;
	KVIDTelescope* id = 0;
			
	KVList* lgrdet = GetDetectors();
	TIter nextdet(lgrdet);
	while ( (det = (KVDetector* )nextdet()) ){
		//Info("PrepareModif","On retire les detecteurs alignes pour %s",det->GetName());
		det->ResetAlignedDetectors(KVGroup::kForwards);
		det->ResetAlignedDetectors(KVGroup::kBackwards);
		Int_t ntel = det->GetIDTelescopes()->GetEntries();
		for (Int_t ii=0;ii<ntel;ii+=1){
			id = (KVIDTelescope* )det->GetIDTelescopes()->At(0);
			if (id->GetDetectors()->FindObject(dd)){
				nv.SetValue(id->GetName(),"");
				det->GetIDTelescopes()->RemoveAt(0);
			}
		}	
	}
	
	KVHashList* lidtel = (KVHashList* )gMultiDetArray->GetListOfIDTelescopes();		
	for (Int_t ii=0;ii<nv.GetEntries();ii+=1){
		id = (KVIDTelescope* )lidtel->FindObject(nv.GetNameAt(ii));
		//Info("PrepareModif","On retire et on detruit l'ID tel %s",id->GetName());
		
		if (id->GetListOfIDGrids()){
			KVIDGraph* idg = 0;
			for (Int_t kk=0;kk<id->GetListOfIDGrids()->GetEntries();kk+=1){
				idg = (KVIDGraph* )id->GetListOfIDGrids()->At(kk);
				idg->RemoveIDTelescope(id);
			}
		}
		
		//Info("PrepareModif","Je retire et j efface le idtel %s %s",id->GetName(),id->ClassName());
		delete lidtel->Remove(id);
	}
	nv.Clear();

}
