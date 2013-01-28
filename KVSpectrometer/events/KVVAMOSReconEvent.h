//Created by KVClassFactory on Thu Sep 13 10:49:45 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSRECONEVENT_H
#define __KVVAMOSRECONEVENT_H

#include "KVEvent.h"

class KVVAMOSReconNuc;

class KVVAMOSReconEvent : public KVEvent
{

	TString fNucSeedCond; // condition used in the reconstruction for seeding new reconstructed nucleus

   public:
   KVVAMOSReconEvent(Int_t mult = 3, const Char_t *classname = "KVVAMOSReconNuc");
   virtual ~KVVAMOSReconEvent();
   void init();

   virtual void IdentifyEvent();
   virtual void XYCalibrateEvent();
   virtual void ECalibrateEvent();
   virtual void Print(Option_t *opt="") const;
   virtual void SecondaryIdentCalib();

   virtual void ReconstructEvent();
   virtual void SetNucSeedCond(const Char_t* cond){ fNucSeedCond = cond; }



   //----------------------------------------------------------------

   inline KVVAMOSReconNuc *AddNucleus(){
	   //Wrapper for KVEvent::AddParticle casting result to KVVAMOSReconNuc*
	   KVVAMOSReconNuc *tmp = (KVVAMOSReconNuc *)(KVEvent::AddParticle());
	   return tmp;
   }
   //----------------------------------------------------------------

   inline KVVAMOSReconNuc *GetNucleus(Int_t nnuc) const{
	   // Access to event nucleus with index nnuc (1<=npar<=fMult)
	   return (KVVAMOSReconNuc *)(KVEvent::GetParticle(nnuc));
   }
   //----------------------------------------------------------------

   inline KVVAMOSReconNuc *GetNucleusWithName(const Char_t *name) const{
	   // Access to event nucleus with by name
	   return (KVVAMOSReconNuc *)(KVEvent::GetParticleWithName(name));
   }
   //----------------------------------------------------------------

   inline KVVAMOSReconNuc *GetNextNucleus(Option_t *opt=""){
	   // Use this method to iterate over the list of nuclei in the event.
	   // If opt="ok" only those for whom KVVAMOSReconNuc::IsOK() returns
	   // kTRUE are included.
	   // After the last nucleus GetNextNucleus() returns a null pointer
	   // and resets itself ready for a new iteration over the nucleus list.

	   return (KVVAMOSReconNuc *)(KVEvent::GetNextParticle(opt));
   }

   ClassDef(KVVAMOSReconEvent,1)//Class for reconstructed VAMOS events
};

#endif
