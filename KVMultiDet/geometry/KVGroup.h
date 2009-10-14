/***************************************************************************
                          kvgroup.h  -  description
                             -------------------
    begin                : Fri May 24 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVGroup.h,v 1.25 2007/01/04 16:38:50 franklan Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVGROUP_H
#define KVGROUP_H

#define KVGROUP_ADD_UNKNOWN_TELESCOPE "Attempt to add undefined telescope to group"

#include "KVPosition.h"
#include "KVList.h"
#include "KVTelescope.h"

class KVDetector;
class KVNucleus;
class KVReconstructedNucleus;
class KVMultiDetArray;

class KVGroup:public KVPosition {
   enum {
      kIsRemoving = BIT(14)     //flag set during call to RemoveTelescope
   };
 protected:
    KVList * fTelescopes;       //->Sorted list of telescopes belonging to the group.
   KVList *fDetectors;          //!list of all detectors in group
   UInt_t fNumberOfLayers;      //number of different layers in group
   UInt_t fLayNumMin;           //minimum layer number (nearest to target)
   UInt_t fLayNumMax;           //maximum layer number (furthest from target)
   KVList *fReconstructedNuclei;        //!Particles reconstructed in this group
 public:
   enum {
      kForwards,
      kBackwards
   };
    KVGroup();
    KVGroup(const KVGroup &);
   void init();
    virtual ~ KVGroup();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   void Copy(TObject & obj) const;
#else
   void Copy(TObject & obj);
#endif
   void Add(KVTelescope *);
   Bool_t Contains(KVTelescope *) const;
   void Reset();
   virtual void Print(Option_t * t = "") const;
   KVList *GetTelescopes() const {
      return fTelescopes;
   };
   TList *GetTelescopes(Float_t theta, Float_t phi) const;
   void SetTelescopes(KVList * list);
   void SetDimensions(KVPosition *, KVPosition *);
   void SetDimensions();
   KVDetector *GetDetector(const Char_t * name);
   KVTelescope *GetTelescope(const Char_t * name);
   void Sort();
   void DetectParticle(KVNucleus * part);
   UInt_t GetNumberOfLayers() {
      if (!fNumberOfLayers)
         CountLayers();
      return fNumberOfLayers;
   };
   void CountLayers();
   UInt_t GetLayerNearestTarget() const;
   UInt_t GetLayerFurthestTarget() const;
   TList *GetTelescopesInLayer(UInt_t nlayer);
   UInt_t GetNumberOfDetectorLayers();
   TList *GetDetectorsInLayer(UInt_t lay);
   TList *GetAlignedDetectors(KVDetector *, UChar_t dir = kBackwards);
   UInt_t GetDetectorLayer(KVDetector * det);

   inline UInt_t GetHits() {
      if (fReconstructedNuclei)
         return fReconstructedNuclei->GetSize();
      else
         return 0;
   };
#if !defined(R__MACOSX)
   inline UInt_t GetNIdentified();
   inline UInt_t GetNUnidentified();
#else
   UInt_t GetNIdentified();
   UInt_t GetNUnidentified();
#endif
   KVList *GetParticles() {
      return fReconstructedNuclei;
   }
   void AddHit(KVReconstructedNucleus * kvd);
   void RemoveHit(KVReconstructedNucleus * kvd);

   void Destroy();
   void RemoveTelescope(KVTelescope * tel, Bool_t kDeleteTelescope =
                        kFALSE, Bool_t kDeleteEmptyGroup = kTRUE);

   void GetIDTelescopes(TList *);

   Bool_t IsRemoving() {
      return TestBit(kIsRemoving);
   }

   KVList *GetDetectors();

   void AnalyseParticles();
   inline Bool_t Fired(Option_t * opt = "any") const;

   ClassDef(KVGroup, 1)         //Group of telescopes in different layers having similar angular positions.
};

inline Bool_t KVGroup::Fired(Option_t * opt) const
{
   //returns kTRUE if at least one telescope in group has KVTelescope::Fired(opt) = kTRUE (see KVDetector::Fired() method for options)
   KVTelescope *tel;
   TIter nxt(fTelescopes);
   while ((tel = (KVTelescope *) nxt()))
      if (tel->Fired(opt))
         return kTRUE;
   return kFALSE;
}

#if !defined(R__MACOSX)
#ifndef KVRECONSTRUCTEDNUCLEUS_H
#include "KVReconstructedNucleus.h"
#endif
inline UInt_t KVGroup::GetNIdentified()
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
inline UInt_t KVGroup::GetNUnidentified()
{
   //number of unidentified particles reconstructed in group
   return (GetHits() - GetNIdentified());
};
#endif

#endif
