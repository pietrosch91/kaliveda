//Created by KVClassFactory on Fri Apr 26 14:01:32 2013
//Author: John Frankland,,,

#include "KVASGroup.h"
#include "KVReconstructedEvent.h"

ClassImp(KVASGroup)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVASGroup</h2>
<h4>Group in axially-symmetric multidetector</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVASGroup::KVASGroup()
{
   // Default constructor
    init();
}
void KVASGroup::init()
{
   //Default initialisation

   fNumberOfLayers = 0;
   fLayNumMin = 0;
   fLayNumMax = 0;
}

void KVASGroup::Add(KVBase *b)
{
    // Overrides KVGeoStrucElement method
    // Only KVTelescope-derived objects can be added to KVASGroups
    // All detectors in the telescope are added to the group's list.

    if(!b->InheritsFrom("KVTelescope")) return;
    KVGeoStrucElement::Add(b);
    KVTelescope* tel = dynamic_cast<KVTelescope*>(b);
    TIter next(tel->GetDetectors());
    KVDetector* d;
    while( (d = (KVDetector*)next()) ) KVGeoStrucElement::Add(d);
}

KVASGroup::~KVASGroup()
{
   // Destructor
    fNumberOfLayers = 0;
    fLayNumMin = 0;
    fLayNumMax = 0;
    fReconstructedNuclei = 0;
}

//_____________________________________________________________________________________

void KVASGroup::SetDimensions()
{
   //Set dimensions of group according to dimensions of all its telescopes.
   KVTelescope *tel, *tel1;
   TIter next(GetTelescopes());
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

void KVASGroup::SetDimensions(KVPosition * p1, KVPosition * p2)
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

void KVASGroup::Sort()
{
   //Make sure telescopes are ordered by increasing layer number i.e. increasing distance from target.
   //This is so that when simulating the energy losses of a charged particle passing
   //through the telescopes of the group, we get it in the right order!

   SortStructures();
}
//_____________________________________________________________________________________

KVNameValueList*  KVASGroup::DetectParticle(KVNucleus * part)
{
   //Calculate energy losses of a charged particle traversing the telescopes of the group.
    //This method return a list of TNamed where each detector which is throught in the particle
    //are written with the corrresponding energy loss
    //WARNING : this KVNameValueList has to be deleted by the user
    //				after use
    //return 0 if no telescope are on the path of the particle (DEAD zone)
   TList *d = GetTelescopesWithAngles(part->GetTheta(), part->GetPhi());
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

TList *KVASGroup::GetTelescopesWithAngles(Float_t theta, Float_t phi) const
{
   //Create and fill list of telescopes in group at position (theta,phi),
   //sorted according to distance from target (smallest layer number i.e. closest first).
   //User must delete list after use.

    TIter next(GetTelescopes());
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

TList *KVASGroup::GetTelescopesInLayer(UInt_t nlayer)
{
   //Create and fill list of telescopes belonging to Layer number nlayer in the group.
   //User must delete list after use.

   TIter next(GetTelescopes());
   KVTelescope *t;
   TList *list = 0;
   while ((t = (KVTelescope *) next())) {
       if (t->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber() == nlayer) {
         if (!list)
            list = new TList;
         list->Add(t);
      }
   }
   return list;
}
//_________________________________________________________________________________

void KVASGroup::CountLayers()
{
   //Count the number of different layers to which telescopes in the group belong.
   //This is based on different layers having different numbers.
   //The layer closest to the target is assumed to have the smallest layer number,
   //the layer furthest from the target is assumed to have the largest layer number.
   fNumberOfLayers = 0;
   fLayNumMin = 99;
   fLayNumMax = 0;
   if (GetTelescopes()) {
      TIter ntel(GetTelescopes());
      KVTelescope *tel;
      UInt_t laynums[10];
      while ((tel = (KVTelescope *) ntel())) {
         if (fNumberOfLayers) {
            Bool_t found = kFALSE;
            //Check to make sure layer number not already in array
            for (UInt_t i = 0; i < fNumberOfLayers; i++) {
               if (tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber() == laynums[i])
                  found = kTRUE;
            }
            if (!found) {
               laynums[fNumberOfLayers++] = tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber();
               if (fNumberOfLayers > 9) {
                  Warning("CountLayers", "Too many layers in group");
               }
               if (tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber() > fLayNumMax)
                  fLayNumMax = tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber();
               if (tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber()< fLayNumMin)
                  fLayNumMin = tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber();
            }
         } else {
            laynums[fNumberOfLayers++] = tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber();
            if (fNumberOfLayers > 9) {
               Warning("CountLayers", "Too many layers in group");
            }
            if (tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber() > fLayNumMax)
               fLayNumMax = tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber();
            if (tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber() < fLayNumMin)
               fLayNumMin = tel->GetParentStructure("RING")->GetParentStructure("LAYER")->GetNumber();
         }
      }
   }
}
//_________________________________________________________________________________

UInt_t KVASGroup::GetNumberOfDetectorLayers()
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

TList *KVASGroup::GetDetectorsInLayer(UInt_t lay)
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
               if (rank<=(UInt_t)tel->GetSize()){
                	KVDetector* ddd = tel->GetDetector(rank);
                  if (ddd)
                  	list->Add(ddd);
                  else
                  	Warning("GetDetectorsInLayer","pb d index pour GetDetector");  
            	}
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

UInt_t KVASGroup::GetDetectorLayer(KVDetector * det)
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

TList *KVASGroup::GetAlignedDetectors(KVDetector * det, UChar_t dir)
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
               if (((KVTelescope*)d2->GetParentStructure("TELESCOPE"))->IsOverlappingWith(
                           (KVTelescope*)det->GetParentStructure("TELESCOPE"))) {
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
               if (((KVTelescope*)d2->GetParentStructure("TELESCOPE"))->
                   IsOverlappingWith((KVTelescope*)det->GetParentStructure("TELESCOPE"))) {
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

void KVASGroup::GetIDTelescopes(TCollection * tel_list)
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

void KVASGroup::AnalyseTelescopes(KVReconstructedEvent* event, TList* kvtl)
{

    KVTelescope *t;
    TIter nxt_tel(kvtl);
    //get max number of detectors in telescopes of layer
    Int_t max=0;
    while ( (t = (KVTelescope* )nxt_tel()) )
        if (max<t->GetDetectors()->GetSize())
            max = t->GetDetectors()->GetSize();
    //before, we assumed all telescopes to be same in layer:
    //	but in fact it's not true
    //UInt_t ndet = ((KVTelescope *) (kvtl->First()))->GetSize();
    UInt_t ndet=max;

    for (register UInt_t i = ndet; i > 0; i--) {

        nxt_tel.Reset();
        TList detlist;

        //start from last detectors and move inwards
        while ((t = (KVTelescope *) nxt_tel())) {
            //loop over detectors in each telescope
            if ((UInt_t)t->GetDetectors()->GetSize()>=i){
            	KVDetector *d = t->GetDetector(i);
            	if (d)
            		detlist.Add(d);
        			else
            		Warning("AnalyseTelescopes","pointeur KVDetector NULL");
        		}	
        }

        event->AnalyseDetectors(&detlist);

    }
}


void KVASGroup::AnalyseAndReconstruct(KVReconstructedEvent* event)
{

    UInt_t nLayers = GetNumberOfLayers();

    UInt_t initial_hits_in_group = GetHits();

    if (nLayers > 1) {
        //multilayer group
        //Start with layer furthest from target and work inwards (but don't look at layer
        //nearest to target)
        for (UInt_t i = GetLayerFurthestTarget();
             i > GetLayerNearestTarget(); i--) {
            TList *kvtl = GetTelescopesInLayer(i);
            if (kvtl) {
                AnalyseTelescopes(event,kvtl);
                delete kvtl;
            }
        }


        //if nothing has been found, then check for particles stopping in layer nearest target
        if (GetHits() == initial_hits_in_group) {
            TList *kvtl =
                    GetTelescopesInLayer(GetLayerNearestTarget());
            if (kvtl) {
                AnalyseTelescopes(event,kvtl);
                delete kvtl;
            }
        }

    } else {
        //single layer group
#ifdef KV_DEBUG
        Info("AnalyseGroup", "Single layer group");
#endif
        //for a single layer group we should have
        //kvg->GetLayerNearestTarget() = kvg->GetLayerFurthestTarget()
        //so we can use either one as argument for kvg->GetTelescopesInLayer
        TList *kvtl =
                GetTelescopesInLayer(GetLayerNearestTarget());
        if (kvtl) {
            AnalyseTelescopes(event,kvtl);
            delete kvtl;
        }
    }

#ifdef KV_DEBUG
    Info("AnalyseGroup", "OK after analysis of hit groups");
#endif

    //perform first-order coherency analysis (set fAnalStatus for each particle)
    AnalyseParticles();
}
//_________________________________________________________________________________

UInt_t KVASGroup::GetLayerNearestTarget() const
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

UInt_t KVASGroup::GetLayerFurthestTarget() const
{
   //
   //Returns the layer number of the layer in the group which is furthest from the target
   //i.e. the layer with the largest layer number
   //

   if (!fLayNumMax)
      ((KVGroup *) this)->CountLayers();
   return fLayNumMax;
}

Bool_t KVASGroup::Contains(KVBase *name) const
{
    // Returns true if telescope belongs to this group
    return (GetTelescopes()->FindObject(name)!=0);
}

