/***************************************************************************
                          kvlayer.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVLayer.h,v 1.19 2009/03/03 14:27:15 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVLAYER_H
#define KVLAYER_H

#ifndef KVR_RCPRC_CNXN
#define KVR_RCPRC_CNXN 1
#endif
#ifndef KVR_NORCPRC_CNXN
#define KVR_NORCPRC_CNXN 0
#endif

#define KVLAYER_TELESCOPE_UNKNOWN "Telescope prototype is a null pointer"

#include "TTree.h"
#include "TRef.h"

#include "KVBase.h"
#include "KVList.h"
#include "KVRing.h"

class KVTelescope;
class KVMultiDetArray;
class KVLayerBrowser;

class KVLayer:public KVBase {

   friend class KVLayerBrowser;

 private:

    Char_t fDyName[80];         //[80] used for dynamic labelling of layer according to current layer number
   KVMultiDetArray *fArray;     //reference to the multidetector array to which the layer belongs
   KVLayerBrowser *fBrowser;    //! pointer to the layer browser representing this layer in GUI
   enum {
      kIsRemoving = BIT(14)     //flag set during call to RemoveRing
   };

 protected:

    KVList * fRings;            //->the rings in the layer sorted according to increasing theta_min

 public:

    KVLayer();
    virtual ~ KVLayer();
   void init();

   void AddRing(KVRing * kvr, UInt_t fcon = KVR_RCPRC_CNXN);
   KVRing *AddRing();

   KVList *GetRings() const {
      return fRings;
   };
   Bool_t IsSortable() const {
      return kTRUE;
   };
   Int_t Compare(const TObject * obj) const;

   const Char_t *GetName() const;
   void SetArray(KVMultiDetArray *);
   KVMultiDetArray *GetArray();

   KVRing *GetRing(const Char_t * name) const {
      //Find ring in layer with name
      return (KVRing *) fRings->FindObject(name);
   };
   KVRing *GetRing(UInt_t num) const {
      return (KVRing *) fRings->FindObjectByNumber(num);
   };
   void RemoveRing(const Char_t *);
   void RemoveRing(UInt_t);
   void RemoveRing(KVRing *, Bool_t kDeleteRing = kTRUE,
                   Bool_t kDeleteEmptyLayer = kTRUE);

   virtual void Print(Option_t * opt = "") const;

   Bool_t IsRemoving() {
      return TestBit(kIsRemoving);
   }
   ClassDef(KVLayer, 2)         //Layer of telescopes at the "same distance" from the target in a multidetector array
};

#endif
