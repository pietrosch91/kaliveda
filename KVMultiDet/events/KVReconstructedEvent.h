/***************************************************************************
                          kvreconstructedevent.h  -  description
                             -------------------
    begin                : March 11th 2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVReconstructedEvent.h,v 1.11 2007/11/12 15:08:32 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVRECONEVENT_H
#define KVRECONEVENT_H

#include "KVEvent.h"

class KVReconstructedNucleus;
class KVDetectorEvent;
class KVGroup;
class TList;

class KVReconstructedEvent:public KVEvent {

   Float_t fThreshold;          //seuil en energie (MeV/nuc) pour filtre geometrique
   Bool_t fMeanAngles;          //!kTRUE if particle momenta calculated using mean angles of detectors (default: randomised angles)
   TString fPartSeedCond;   //!condition used in AnalyseTelescopes for seeding new reconstructed particle

 public:

   enum {
      kUseGeomFilt = BIT(14)    //flag for filtering simulations; by default kFALSE, if kTRUE then a simple geometric filtering is implemented
   };

   void init();
    KVReconstructedEvent(Int_t mult = 50, const char *classname =
                         "KVReconstructedNucleus");
    virtual ~ KVReconstructedEvent() {
   };

   inline KVReconstructedNucleus *AddParticle();
   inline KVReconstructedNucleus *GetParticle(Int_t npart) const;
   inline KVReconstructedNucleus *GetParticle(const Char_t * name) const;
   inline KVReconstructedNucleus *GetNextParticle(Option_t * opt = "");
   virtual void ReconstructEvent(KVDetectorEvent * kvde);
   virtual Bool_t AnalyseGroup(KVGroup * kvg);
   virtual Bool_t AnalyseTelescopes(TList * kvtl);
   virtual void SetPartSeedCond(const Char_t* cond)
   {
      fPartSeedCond = cond;
   };
   virtual void IdentifyEvent();
   virtual void CalibrateEvent();

   void SetGeometricFilter(Bool_t onoff);
   inline Bool_t UseGeoFilt() {
      return TestBit(kUseGeomFilt);
   };
   virtual void GeometricFilter() {
      Info("GeometricFilter", "Not implemented");
   };
   virtual void SetThreshold(Float_t mev_sur_a);

   virtual void Print(Option_t * t = "") const;

   inline void UseMeanAngles() {
      fMeanAngles = kTRUE;
   };
   inline void UseRandomAngles() {
      fMeanAngles = kFALSE;
   };
   inline Bool_t HasMeanAngles() {
      return fMeanAngles;
   };
   inline Bool_t HasRandomAngles() {
      return !fMeanAngles;
   };

   ClassDef(KVReconstructedEvent, 1)    //Base class for reconstructed experimental multiparticle events
};

inline KVReconstructedNucleus *KVReconstructedEvent::
GetParticle(Int_t npart) const
{
   //Access to event member with index npart (1<=npart<=fMult)

   return (KVReconstructedNucleus *) (KVEvent::GetParticle(npart));
}

/////////////////////////////////////////////////////////////////////////////

inline KVReconstructedNucleus *KVReconstructedEvent::
GetParticle(const Char_t * name) const
{
   //Access to event particle by name

   return (KVReconstructedNucleus *) (KVEvent::GetParticle(name));
}

/////////////////////////////////////////////////////////////////////////////

inline KVReconstructedNucleus *KVReconstructedEvent::AddParticle()
{
   //Wrapper for KVEvent::GetNextParticle casting result to KVReconstructedNucleus*

   KVReconstructedNucleus *tmp =
       (KVReconstructedNucleus *) (KVEvent::AddParticle());
   return tmp;
}

////////////////////////////////////////////////////////////////////////////////////////

inline KVReconstructedNucleus *KVReconstructedEvent::
GetNextParticle(Option_t * opt)
{
   //Use this method to iterate over the list of particles in the event
   //If opt="ok" only those for whom KVReconstructedNucleus::IsOK() returns kTRUE are included.
   //After the last particle GetNextParticle() returns a null pointer and
   //resets itself ready for a new iteration over the particle list.

   return (KVReconstructedNucleus *) KVEvent::GetNextParticle(opt);
}

#endif
