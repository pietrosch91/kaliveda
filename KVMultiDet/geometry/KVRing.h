/***************************************************************************
                          kvring.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRing.h,v 1.15 2007/10/23 14:09:02 ebonnet Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVRING_H
#define KVRING_H

#define KVR_RCPRC_CNXN 1
#define KVR_NORCPRC_CNXN 0

#include "KVPosition.h"
#include "KVGeoStrucElement.h"
#include "KVList.h"
#include "KVTelescope.h"

class KVLayer;

class KVRing : public KVGeoStrucElement, public KVPosition {
 private:                      // Private attributes
   KVList * fTelescopes;        //-> list of telescopes belonging to the ring sorted by phi
   KVLayer *fLayer;             //layer to which the ring belongs
   KVTelescope *fProto;         //pointer to prototype for ring telescopes
   Char_t fDyName[10];          //[10] used for dynamic labelling of ring according to current ring number
   UInt_t fStep;                //step used for numbering telescopes in ring
   enum {
      kIsRemoving = BIT(14)     //flag set during call to RemoveTelescope
   };

 public:
    KVRing();
   void init();
    virtual ~ KVRing();
   void AddTelescope(KVDetector * tele, const int fcon = KVD_RECPRC_CNXN);
   void AddTelescope();
   void AddToLayer(KVLayer * kvl, UInt_t fcon = KVR_RCPRC_CNXN);
    KVRing(const UInt_t, const Float_t, const Float_t, const Float_t,
           const UInt_t, const Float_t, KVTelescope *, UInt_t step =
           1, UInt_t num_first = 1, Float_t dphi = -1.);
   void Print(Option_t * opt = "") const;
   KVTelescope *GetTelescope(Float_t phi) const;
   KVTelescope *GetTelescope(const Char_t * name) const;
   inline KVList *GetTelescopes() const {
      return fTelescopes;
   };
   void RemoveTelescope(const Char_t *);
   void RemoveDetectors(const Char_t *);
   void RemoveTelescope(KVTelescope * tel, Bool_t kDeleteTelescope = kTRUE,
                        Bool_t kDeleteEmptyRing = kTRUE);
   inline KVLayer *GetLayer() const {
      return fLayer;
   };
   void SetLayer(KVLayer * lay);
   Bool_t IsSortable() const {
      return kTRUE;
   };
   Int_t Compare(const TObject * obj) const;
   const Char_t *GetName() const;
   const Char_t *GetRingName();
   void SetNumberTelescopes(UInt_t num);
   void SetPrototype(KVTelescope * tel) {
      fProto = tel;
   };
   KVTelescope *GetPrototype() const {
      return fProto;
   };
   void ReplaceTelescope(KVTelescope *, KVTelescope *);

   Bool_t IsRemoving() {
      return TestBit(kIsRemoving);
   }
	
	Double_t GetSolidAngle(void){
		// redefinition of KVPostition::GetSolidAngle() to take into account only KVTelescope of the considering KVLayer which define the KVRing
		Double_t sol_ang=0;
		KVTelescope *tel; TIter nxttel(GetTelescopes()); while ((tel = (KVTelescope *) nxttel())) sol_ang+=tel->GetSolidAngle();
		return sol_ang;
	}
   virtual TGeoVolume* GetGeoVolume();
   virtual void AddToGeometry();

   ClassDef(KVRing, 2)          //Class representing one ring of an axially symmetric multidetector array
};

#endif
