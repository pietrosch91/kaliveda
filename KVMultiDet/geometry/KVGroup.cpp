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
#include <KVReconstructedEvent.h>

using namespace std;

ClassImp(KVGroup)

////////////////////////////////////////////////////////////////////
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
   fDetectors = new KVList(kFALSE);
   fDetectors->SetCleanup();
   fReconstructedNuclei = 0;
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
   fReconstructedNuclei = 0;
   if (fDetectors && fDetectors->TestBit(kNotDeleted)) {
      fDetectors->Clear();
      delete fDetectors;
      fDetectors = 0;
   }
}

void KVGroup::AddDetector(KVDetector *kvt)
{
    if (kvt) {
       fDetectors->Add(kvt);
       kvt->SetGroup(this);
    } else
        Warning("Add", KVGROUP_ADD_UNKNOWN_TELESCOPE);
}

//_____________________________________________________________________________________

void KVGroup::Add(KVDetector *kvt)
{
    //Add a detector or telescope to the current group.

    if(kvt->InheritsFrom("KVTelescope"))
        AddTelescope((KVTelescope*)kvt);
    else
        AddDetector(kvt);
}

UInt_t KVGroup::GetNumberOfDetectorLayers()
{
    // The number of detector layers is the maximum number of detectors in the
    // group which are placed one in front of the other, i.e. we interrogate
    // each detector as to how many detectors there are in front of it

    UInt_t max = 0;
    TIter next(GetDetectors());
    KVDetector* d;
    while( (d = (KVDetector*)next()) ){
        UInt_t e = d->GetAlignedDetectors()->GetEntries();
        if(e>max) max=e;
    }
    return max;
}

void KVGroup::AddTelescope(KVTelescope *kvt)
{
    // add telescope & all detectors it contains to group

    if (kvt) {
       fTelescopes->Add(kvt);
       kvt->SetGroup(this);
       TIter ndet(kvt->GetDetectors());
       KVDetector *det;
       while ((det = (KVDetector *) ndet())) {
          AddDetector(det);
       }
    } else
        Warning("Add", KVGROUP_ADD_UNKNOWN_TELESCOPE);
}

Bool_t KVGroup::Contains(KVDetector * tele) const
{
   //kTRUE if detector or telescope belongs to this group

    if (fDetectors->FindObject(tele))
       return kTRUE;
    if (fTelescopes->FindObject(tele))
       return kTRUE;
   return kFALSE;
}

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

//_____________________________________________________________________________________

void KVGroup::Print(Option_t * opt) const
{
//Print out the characteristics of the group and its member telescopes.
//  --> KVGroup::Print() just prints out all the detectors in the group.
//  --> KVGroup::Print("fired") only print fired detectors (i.E. KVDetector::Fired()==kTRUE)
   TIter next(fDetectors);
   KVDetector *obj;
   if (!strcmp(opt, "fired")) {
      while ((obj = (KVDetector *) next())) {
         obj->Print(opt);
      }
   } else {
      cout << "\n";
      cout << opt;
      cout << "Structure of KVGroup object: ";
      if (strcmp(GetName(), "")) {
         cout << GetName();
      } else {
         cout << GetNumber();
      }
      cout << endl;
      cout << opt;
      cout << "--------------------------------------------------------" <<
          endl;
      while ((obj = (KVDetector *) next())) {
         obj->Print("        ");
      }
   }
}

//________________________________________________________________________________

KVDetector *KVGroup::GetDetector(const Char_t * name)
{
   //find named detector in group

   KVDetector *det = (KVDetector *) fDetectors->FindObject(name);
   return det;
}

TList *KVGroup::GetDetectorsInLayer(UInt_t lay)
{
    // lay=1 : create and fill list with detectors closest to target
    // lay=GetNumberOfDetectorLayers() : detectors furthest from target

    TList* dets = new TList;
    TIter next(GetDetectors());
    KVDetector* d;

    while( (d = (KVDetector*)next()) ){
        if(lay == d->GetAlignedDetectors()->GetEntries()) dets->Add(d);
    }
    return dets;
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

   while(det){
       tmp->Add(det);
       KVDetectorNode* node = det->GetNode();
       KVSeqCollection* infront = node->GetDetectorsInFront();
       if(!infront) break;
       if(infront->GetEntries()>1){
           Warning("GetAlignedDetectors",
                   "No unique solution. There are %d detectors in front of %s.",
                   infront->GetEntries(), det->GetName());
           return 0;
       }
       else
           det = (KVDetector*)infront->First();
   }

   if (dir == kForwards) {
       TIter next(tmp, kIterBackward );
       TList* tmp2 = new TList;
       while( (det = (KVDetector*)next()) ) tmp2->Add(det);
       delete tmp;
       tmp=tmp2;
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
   //Starting from each detector in the group,
   //we build ID telescopes from all pairs of aligned detectors.
   //
   //For each pair of detectors, it is KVMultiDetArray::GetIDTelescopes
   //which determines which KVIDTelescope class to use (specialise this method
   //in KVMultiDetArray child classes). It must also make sure that
   //each IDTelescope is added only once (i.e. check it is not already in the list).

         TIter next_det(fDetectors);
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
            //one possibility remains: the particle may actually have stopped e.g.
            //in the DE detector of a DE-E telescope, in which case AnalStatus = 3
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

void KVGroup::AnalyseAndReconstruct(KVReconstructedEvent *event)
{
    // Loop over detectors in group, starting from the furthest from the target,
    // and working inwards. Calls KVReconstructedEvent::AnalyseDetectors

    for(Int_t il=GetNumberOfDetectorLayers(); il>0; il--){
        TList* dets = GetDetectorsInLayer(il);
        event->AnalyseDetectors(dets);
        delete dets;
    }
    //perform first-order coherency analysis (set fAnalStatus for each particle)
    AnalyseParticles();
}
