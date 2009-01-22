/***************************************************************************
                          kvdetectorevent.h  -  description
                             -------------------
    begin                : Sun May 19 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVDetectorEvent.h,v 1.8 2006/10/19 14:32:43 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVDETECTOREVENT_H
#define KVDETECTOREVENT_H

#include "KVBase.h"
#include "KVRList.h"
#include "KVEvent.h"
#include "KVGroup.h"

class KVDetectorEvent:public KVBase {

 private:

   KVRList * fHitGroups;        //->TRefArray of groups hit by particles in the event
   KVEvent *fSimEvent;          //! points to simulated event in case of filtering

 public:

    KVDetectorEvent();
    virtual ~ KVDetectorEvent();
   virtual void init();
   inline void AddGroup(KVGroup * grp);
   virtual void Clear(Option_t * opt = "");
   virtual void Print(Option_t * t = "") const;
   KVRList *GetGroups() {
      return fHitGroups;
   };
   inline Bool_t ContainsGroup(KVGroup * grp);
   virtual UInt_t GetMult() const {
      if (fHitGroups)
         return fHitGroups->GetSize();
      else
         return 0;
   };

   void SetSimEvent(KVEvent * e) {
      fSimEvent = e;
   }
   KVEvent *GetSimEvent() {
      return fSimEvent;
   }

   ClassDef(KVDetectorEvent, 1) //Events detected by multidetector arrays
};

//____________________________________________________________________________

inline void KVDetectorEvent::AddGroup(KVGroup * grp)
{
   //add a group to the event structure
   //automatically check that the group is not already part of the event

   if (!ContainsGroup(grp))
      fHitGroups->Add(grp);
}

//_______________________________________________________________________________

inline Bool_t KVDetectorEvent::ContainsGroup(KVGroup * grp)
{
   //returns kTRUE if group is already a member of detected event

   return (fHitGroups ? (fHitGroups->IndexOf(grp) >= 0) : kFALSE);
}

#endif
