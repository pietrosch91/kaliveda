//Created by KVClassFactory on Fri Apr 26 14:01:32 2013
//Author: John Frankland,,,

#ifndef __KVASGROUP_H
#define __KVASGROUP_H

#include "KVGroup.h"

class KVASGroup : public KVGroup, public KVPosition
{

protected:
    UInt_t fNumberOfLayers;      //number of different layers in group
    UInt_t fLayNumMin;           //minimum layer number (nearest to target)
    UInt_t fLayNumMax;           //maximum layer number (furthest from target)
   public:
   KVASGroup();
   void init();
   virtual ~KVASGroup();
   TList *GetTelescopesWithAngles(Float_t theta, Float_t phi) const;
   KVNameValueList *DetectParticle(KVNucleus *part);
   void SetDimensions(KVPosition *, KVPosition *);
   void SetDimensions();
   UInt_t GetNumberOfLayers() {
      if (!fNumberOfLayers)
         CountLayers();
      return fNumberOfLayers;
   };
   void Sort();
   void CountLayers();
   UInt_t GetLayerNearestTarget() const;
   UInt_t GetLayerFurthestTarget() const;
   TList *GetTelescopesInLayer(UInt_t nlayer);
   UInt_t GetNumberOfDetectorLayers();
   TList *GetDetectorsInLayer(UInt_t lay);
   UInt_t GetDetectorLayer(KVDetector * det);
   inline Bool_t Fired(Option_t * opt = "any") const;
   TList *GetAlignedDetectors(KVDetector *, UChar_t dir = kBackwards);
   void AnalyseAndReconstruct(KVReconstructedEvent *);
   void GetIDTelescopes(TCollection *);
   virtual void Print(Option_t * t = "") const;

   ClassDef(KVASGroup,1)//Group in axially-symmetric multidetector
};

inline Bool_t KVASGroup::Fired(Option_t * opt) const
{
   //returns kTRUE if at least one telescope in group has KVTelescope::Fired(opt) = kTRUE (see KVDetector::Fired() method for options)
   KVTelescope *tel;
   TIter nxt(fTelescopes);
   while ((tel = (KVTelescope *) nxt()))
      if (tel->Fired(opt))
         return kTRUE;
   return kFALSE;
}

#endif
