/***************************************************************************
                          kvevent.h  -  description
                             -------------------
    begin                : Sun May 19 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVEvent.h,v 1.29 2008/12/17 11:23:12 ebonnet Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVEVENT_H
#define KVEVENT_H

#define KVEVENT_PART_INDEX_OOB "Particle index %d out of bounds [1,%d]"

#include "TVector3.h"
#include "TClonesArray.h"
#include "KVNucleus.h"
#include "KVBase.h"
#include "TRotation.h"
#include "TLorentzRotation.h"
#include "KVParticleCondition.h"

class KVEvent:public KVBase {

 protected:

   TClonesArray * fParticles;   //->array of particles in event
   TIter *fOKIter;              //!used for iterating over particles

 public:

    KVEvent(Int_t mult = 50, const char *classname = "KVNucleus");
    virtual ~ KVEvent();
   void init();

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif	
	
	KVNucleus *AddParticle();
   KVNucleus *GetParticle(Int_t npart) const;
   virtual Int_t GetMult(Option_t * opt = "");

   virtual void Clear(Option_t * opt = "");
   virtual void Print(Option_t * t = "") const;
   KVNucleus *GetParticle(const Char_t * name) const;
   KVNucleus *GetNextParticle(Option_t * opt = "");
   void ResetGetNextParticle();

   void ResetEnergies();

   virtual Bool_t IsOK();

   void CustomStreamer() {
      fParticles->BypassStreamer(kFALSE);
   };
	
	void DefineGroup(const Char_t* groupname, const Char_t* from="");
	void DefineGroup(const Char_t* groupname, KVParticleCondition* cond, const Char_t* from="");
   
	void SetFrame(const Char_t * frame, const TVector3 & boost, Bool_t beta =
                 kFALSE);
   void SetFrame(const Char_t * frame, TLorentzRotation & rot);
   void SetFrame(const Char_t * frame, TRotation & rot);
   void SetFrame(const Char_t * frame, const TVector3 & boost, TRotation & rot,
                 Bool_t beta = kFALSE);

   void SetFrame(const Char_t * newframe, const Char_t * oldframe,
                 const TVector3 & boost, Bool_t beta = kFALSE);
   void SetFrame(const Char_t * newframe, const Char_t * oldframe,
                 TLorentzRotation & rot);
   void SetFrame(const Char_t * newframe, const Char_t * oldframe,
                 TRotation & rot);
   void SetFrame(const Char_t * newframe, const Char_t * oldframe,
                 const TVector3 & boost, TRotation & rot, Bool_t beta = kFALSE);

   ClassDef(KVEvent, 3)         //Base class for all types of multiparticle event
};

#endif
