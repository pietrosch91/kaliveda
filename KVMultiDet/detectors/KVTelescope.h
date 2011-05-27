/***************************************************************************
$Id: KVTelescope.h,v 1.19 2008/12/17 13:01:26 franklan Exp $
// Author: J.D. Frankland, A. Mignon
                          kvtelescope.h  -  description
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

#ifndef KVTELESCOPE_H
#define KVTELESCOPE_H

#define KVTELESCOPE_ADD_UNKNOWN_DETECTOR "Attempt to add unknown or undefined detector"
#define KVTELESCOPE_RANK_UNKNOWN_DETECTOR "Rank sought for detector not contained in this telescope"

#include "KVPosition.h"
#include "KVDetector.h"
#include "KVList.h"
#include "TRef.h"
#include "KVNameValueList.h"

class KVNucleus;
class KVRing;
class KVLayer;
class KVGroup;
class TGraph;

class KVTelescope:public KVPosition {

 private:
   KVList * fDetectors;         //-> list of detectors in telescope
   KVGroup *fGroup;             //group to which telescope belongs
   KVRing *fRing;               //ring to which telescope belongs
   enum {
      kIsRemoving = BIT(14)     //flag set during call to RemoveDetector
   };
   Int_t fNdets;                //number of detectors in telescope
   Float_t *fDepth;             //[fNdets] depth of each element starting from nearest target

 public:

    KVTelescope();
    virtual ~ KVTelescope();
   void init();
   virtual void AddDetector(KVDetector * d, const int fcon =
                            KVD_RECPRC_CNXN);
   void AddClone(KVDetector * d, const int fcon = KVD_RECPRC_CNXN);
   void Add(KVDetector * d, const int fcon = KVD_RECPRC_CNXN);

   KVList *GetDetectors() const {
      return fDetectors;
   };

   Bool_t ContainsType(const Char_t *) const;
   void RemoveDetectors(const Char_t *);

   KVDetector *GetDetector(UInt_t n) const {
      //returns the nth detector in the telescope structure
      if (n > GetSize() || n < 1) {
         Error("GetDetector(UInt_t n)", "n must be between 1 and %u",
               GetSize());
         return 0;
      }
      KVDetector *d = (KVDetector *) fDetectors->At((n - 1));
       return d;
   };
   KVDetector *GetDetector(const Char_t * name) const;
   KVDetector *GetDetectorType(const Char_t * type) const;
   void RemoveDetector(const Char_t * name);
   void RemoveDetector(KVDetector *, Bool_t kDeleteDetector =
                       kTRUE, Bool_t kDeleteEmtpyTelescope = kTRUE);

   UInt_t GetDetectorRank(KVDetector * kvd);
   UInt_t GetSize() const {
      //returns number of detectors in telescope
      return (fDetectors ? fDetectors->GetSize() : 0);
   };
   void AddToRing(KVRing * kvr, const int fcon = KVD_RECPRC_CNXN);
   virtual void DetectParticle(KVNucleus * kvp,KVNameValueList* nvl=0);
   virtual void Print(Option_t * opt = "") const;
   void ResetDetectors();

   KVLayer *GetLayer() const;
   KVRing *GetRing() const;
   KVGroup *GetGroup() const;
   void SetGroup(KVGroup * kvg);
   UInt_t GetGroupNumber();

   UInt_t GetRingNumber();
   UInt_t GetLayerNumber();

   const Char_t *GetRingName() const;
   const Char_t *GetLayerName() const;
   const Char_t *GetName() const;
   virtual const Char_t *GetArrayName();

   Bool_t IsSortable() const {
      return kTRUE;
   };
   Int_t Compare(const TObject * obj) const;

   void ReplaceDetector(KVDetector * kvd, KVDetector * new_kvd);
   void ReplaceDetector(const Char_t * name, KVDetector * new_kvd);

   void SetDepth(UInt_t ndet, Float_t depth);
   Float_t GetDepth(UInt_t ndet) const;
   Double_t GetTotalLengthInCM() const;
   Double_t GetDepthInCM(UInt_t ndet) const
	{
		// return depth inside telescope of detector number ndet in centimetres
		return ((Double_t)GetDepth(ndet))/10.;
	}
   Float_t GetDepth() const;

   Bool_t IsRemoving() {
      return TestBit(kIsRemoving);
   }

   inline virtual Bool_t Fired(Option_t * opt = "any") const;

   virtual void SetDetectorThickness(const Char_t * detector,
                                     Double_t thickness);
   virtual void SetDetectorTypeThickness(const Char_t * detector_type,
                                         Double_t thickness);
                                         
   virtual TGeoVolume* GetGeoVolume();
   virtual void AddToGeometry();

   ClassDef(KVTelescope, 2)     //Multi-layered telescopes composed of different absorbers
};

inline Bool_t KVTelescope::Fired(Option_t * opt) const
{
   //returns kTRUE if at least one detector has KVDetector::Fired(opt) = kTRUE (see KVDetector::Fired() method for options)
   KVDetector *det;
   TIter nxt(fDetectors);
   while ((det = (KVDetector *) nxt()))
      if (det->Fired(opt))
         return kTRUE;
   return kFALSE;
}
#endif
