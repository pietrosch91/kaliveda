/***************************************************************************
$Id: KVEvent.cpp,v 1.41 2008/12/17 11:23:12 ebonnet Exp $
                          kvevent.cpp  -  description
                             -------------------
    begin                : Sun May 19 2002
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

#include "Riostream.h"
#include "TClass.h"
#include "KVEvent.h"
#include "TStreamerInfo.h"
#include "KVParticleCondition.h"
#include "TClass.h"

ClassImp(KVEvent);

///////////////////////////////////////////////////////////////////////////////
//KVEvent
//
//Base class for all types of multiparticle event consisting of KVNucleus objects.
//These particles are stored in a TClonesArray and KVEvent provides some basic
//functionality for accessing and manipulating the list.
//

void KVEvent::init()
{
   //Default initialisations

   fOKIter = 0;
}

//_______________________________________________________________________________


KVEvent::KVEvent(Int_t mult, const char *classname)
{
   //Initialise KVEvent to hold mult events of "classname" objects
   //(the class must inherit from KVNucleus).
   //"mult" is the approximate maximum multiplicity of the events (if too
   //small, extra space will be allocated automatically by TClonesArray).
   //
   // Default argument :
   //     classname = "KVNucleus"
   //
   //If the class "classname" has a custom streamer, or if for some other reason you
   //wish to force the TClonesArray to use the "classname" streamer, you must use
   //     KVEvent::CustomStreamer();
   //before reading/writing the event to/from a file etc.

   init();
   fParticles = new TClonesArray(classname, mult);
   TStreamerInfo::SetCanDelete(kFALSE);
}


//_______________________________________________________________________________

KVEvent::~KVEvent()
{
   //Destructor. Destroys all objects stored in TClonesArray and releases
   //allocated memory.
   fParticles->Delete();
   delete fParticles;
   fParticles = 0;
   if (fOKIter) {
      delete fOKIter;
      fOKIter = 0;
   }
   init();
}

//_______________________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVEvent::Copy(TObject & obj) const
#else
void KVEvent::Copy(TObject & obj)
#endif
{
   //Copy this to obj
   KVBase::Copy(obj);
	for (Int_t nn=0;nn<fParticles->GetEntriesFast();nn+=1){
		//printf("avant=%s - ",GetParticle(nn+1)->GetName());
		GetParticle(nn+1)->Copy( *((KVEvent &) obj).AddParticle() );
		//printf("apres=%s\n",((KVEvent &) obj).GetParticle(nn+1)->GetName());
	}
}
//_______________________________________________________________________________

KVNucleus *KVEvent::GetParticle(Int_t npart) const
{
   //Access to event member with index npart (1<=npart<=GetMult() : error if out of bounds)
   //This method may be overridden in event classes derived from KVEvent.

   if (npart < 1 || npart > fParticles->GetEntriesFast()) {
      Error("GetParticle", KVEVENT_PART_INDEX_OOB, npart,
            fParticles->GetEntriesFast());
      return 0;
   }

   return (KVNucleus *) ((*fParticles)[npart - 1]);
}

//________________________________________________________________________________

KVNucleus *KVEvent::AddParticle()
{
   //Method used for building an event particle by particle.
   //DO NOT USE FOR READING EVENTS - use GetParticle(Int_t npart)!!
   //This method increases the multiplicity fMult by one and "creates"
   //a new particle with index (fMult-1).
   //The default constructor for the class corresponding to the
   //"new" particle is called.
   //A reference is set in the particle which points to this event.


   Int_t mult = GetMult();
   KVNucleus *tmp = (KVNucleus *) fParticles->New(mult);
   if (!tmp) {
      Error("AddParticle", "Allocation failure, Mult=%d", mult);
      return 0;
   }
   return tmp;
}

//________________________________________________________________________________

void KVEvent::Clear(Option_t * opt)
{
   //Reset the event to zero ready for new event.

   fParticles->Clear("C");
   init();
}

//________________________________________________________________________________

void KVEvent::Print(Option_t * t) const
{
   //Print a list of all particles in the event with some characteristics.
   //If t="ok" then only particles with IsOK()=kTRUE are shown
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   cout << "\nKVEvent with " << ((KVEvent *) this)->
       GetMult(t) << " particles :" << endl;
   cout << "------------------------------------" << endl;
   KVNucleus *frag = 0;
   const_cast < KVEvent * >(this)->ResetGetNextParticle();
   while ((frag = ((KVEvent *) this)->GetNextParticle(t))) {
      frag->Print();
   }
}

//________________________________________________________________________________

KVNucleus *KVEvent::GetParticleWithName(const Char_t * name) const
{
   //Find particle using its name (SetName()/GetName() methods)
   //In case more than one particle has the same name, the first one found is returned.
   //

	KVNucleus *tmp = (KVNucleus* )fParticles->FindObject(name);
	return tmp;
}

//________________________________________________________________________________

KVNucleus *KVEvent::GetParticle(const Char_t * group_name) const
{
   //Find particle using groups it is belonging
   //In case more than one particle belongs to the same group, the first one found is returned.
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   const_cast < KVEvent * >(this)->ResetGetNextParticle();
   KVNucleus *tmp = const_cast < KVEvent * >(this)->GetNextParticle(group_name);
   const_cast < KVEvent * >(this)->ResetGetNextParticle();
   if (tmp)
      return tmp;
   Warning("GetParticle", "Particle not found: %s", group_name);
   return 0;
}

//__________________________________________________________________________________

Int_t KVEvent::GetMult(Option_t * opt)
{
   //Returns multiplicity (number of particles) of event.
   //If opt = "" (default), returns number of particles in TClonesArray* fParticles
   // i.e. the value of fParticles->GetEntriesFast() (we assume there are no gaps
   // in the list)
   //If opt = "ok" only particles with IsOK()==kTRUE are included.
   //If opt = "name" only particles with GetName()=="name" are included.
   //
   //IN THE LATTER TWO CASES, YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   TString Opt(opt);
   Opt.ToUpper();
   Int_t fMultOK = 0;
   if (Opt == "") {
      //get total multiplicity of event
      return fParticles->GetEntriesFast();
   } else {
      KVNucleus *tmp = 0;
      ResetGetNextParticle();
      while ((tmp = GetNextParticle(opt)))
         fMultOK++;
   }
   return fMultOK;
}

////////////////////////////////////////////////////////////////////////////////////////

KVNucleus *KVEvent::GetNextParticle(Option_t * opt)
{
   //Use this method to iterate over the list of particles in the event
   //After the last particle GetNextParticle() returns a null pointer and
   //resets itself ready for a new iteration over the particle list.
   //
   //Call ResetGetNextParticle() first to make sure that the iterator begins
   //at the start of the list of particles in the event.
   //
   //If opt="" all particles are included in the iteration.
   //If opt="ok" or "OK" only particles whose IsOK() method returns kTRUE are included.
   //
   //Any other value of opt is interpreted as a particle group name: only
   //particles with BelongsToGroup(opt) returning kTRUE are included.
   //
   //If you want to start from the beginning again before getting to the end
   //of the list, use ResetGetNextParticle().

   TString Opt(opt);
   Opt.ToUpper();
   
	Bool_t only_ok = (Opt == "OK");
   //Bool_t label = (Opt != "");
   
   if (!fOKIter) 	//check if iterator exists i.e. if iteration is in progress
   {
      //fOKIter does not exist - begin new iteration
      fOKIter = new TIter(fParticles);
   }
   //look for next particle in event
   KVNucleus *tmp;
   while ((tmp = (KVNucleus *) fOKIter->Next())) {
   	if(only_ok){
   		if(tmp->IsOK()) 
				return tmp;
   	}
      else {
		//if (label){
      	if(tmp->BelongsToGroup(Opt.Data())) 
				return tmp;
      }
      /*
		else
      	return tmp;
   	*/
	}
   //we have reached the end of the list - reset iterator

   ResetGetNextParticle();

   return 0;
}

//__________________________________________________________________________________________________

void KVEvent::ResetGetNextParticle()
{
   //Reset iteration over event particles so that next time GetNextParticle will
   //return the first particle in the list.

   //reset (delete) iterator and set to 0
   if (fOKIter) {
      delete fOKIter;
      fOKIter = 0;
   }
}

//__________________________________________________________________________________________________

Bool_t KVEvent::IsOK()
{
   //Returns kTRUE if the event is OK for analysis.
   //This means there must be at least one particle with IsOK()=kTRUE.

   return (GetMult("ok") > 0);
}

//__________________________________________________________________________________________

void KVEvent::ResetEnergies()
{
   //Used for simulated events after "detection" by some multidetector array.
   //
   //The passage of the event's particles through the different absorbers modifies their
   //kinetic energies, indeed all those which are correctly identified by the detector
   //actually stop. Calling this method will reset all the particles' energies to their
   //initial value i.e. before they entered the first absorber.
   //Particles which have not encountered any absorbers/detectors are left as they are.
   KVNucleus *part = 0;
   ResetGetNextParticle();
   while ((part = GetNextParticle())) {
      part->ResetEnergy();
   }
}

//______________________________________________________________________________________________

void KVEvent::DefineGroup(const Char_t* groupname, const Char_t* from)
{
   //In the same philosophy as KVINDRAReconEvent::AcceptIDCodes() method
	// allow to affiliate a group name to particles of the event
	// if "from" is not null, a test of previously stored group name
	// such as "OK" is checked
	//YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVNucleus *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle(from))) {
      nuc->AddGroup(groupname);
   }
}

//______________________________________________________________________________________________

void KVEvent::DefineGroup(const Char_t* groupname, KVParticleCondition* cond, const Char_t* from)
{
   //In the same philosophy as KVINDRAReconEvent::AcceptIDCodes() method
	// allow to affiliate using a KVParticleCondition a group name to particles of the event
	// if "from" is not null, a test of previously stored group name
	// such as "OK" is checked
	// the method used in KVParticleCondition has to be compatible with the KVNucleus
	// concerned class.
	// This can be done using first KVParticleCondition::SetParticleClassName(const Char_t* cl)
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVNucleus *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle(from))) {
      nuc->AddGroup(groupname,cond);
   }
}


//______________________________________________________________________________________________

void KVEvent::SetFrame(const Char_t * frame, const TVector3 & boost, Bool_t beta)
{
   //Define a Lorentz-boosted frame for all "ok" particles in the event.
   //See KVParticle (method KVParticle::SetFrame()) for details.
   //
   //In order to access the kinematics in the boosted frame, use the GetFrame() method of the
   //individual particles (see KVParticle::GetFrame()).
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVParticle *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle("ok"))) {
      nuc->SetFrame(frame, boost, beta);
   }
}

//___________________________________________________________________________//

void KVEvent::SetFrame(const Char_t * frame, TLorentzRotation & rot)
{
   //Define a Lorentz-rotated frame for all "ok" particles in the event.
   //See KVParticle (method KVParticle::SetFrame()) for details.
   //
   //In order to access the kinematics in the boosted frame, use the GetFrame() method of the
   //individual particles (see KVParticle::GetFrame()).
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVParticle *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle("ok"))) {
      nuc->SetFrame(frame, rot);
   }
}

//___________________________________________________________________________//

void KVEvent::SetFrame(const Char_t * frame, TRotation & rot)
{
   //Define a rotated coordinate frame for all "ok" particles in the event.
   //See KVParticle (method KVParticle::SetFrame()) for details.
   //
   //In order to access the kinematics in the boosted frame, use the GetFrame() method of the
   //individual particles (see KVParticle::GetFrame()).
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVParticle *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle("ok"))) {
      nuc->SetFrame(frame, rot);
   }
}

//___________________________________________________________________________//

void KVEvent::SetFrame(const Char_t * frame, const TVector3 & boost,
                       TRotation & rot, Bool_t beta)
{
   //Define a rotated and Lorentz-boosted coordinate frame for all "ok" particles in the event.
   //See KVParticle (method KVParticle::SetFrame()) for details.
   //
   //In order to access the kinematics in the boosted frame, use the GetFrame() method of the
   //individual particles (see KVParticle::GetFrame()).
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVParticle *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle("ok"))) {
      nuc->SetFrame(frame, boost, rot, beta);
   }
}

//______________________________________________________________________________________________

void KVEvent::SetFrame(const Char_t * newframe, const Char_t * oldframe,
                       const TVector3 & boost, Bool_t beta)
{
   //Define a Lorentz-boosted frame "newframe" for all "ok" particles in the event.
   //The transformation is applied to the particle coordinates in the existing frame "oldframe"
   //
   //See KVParticle (method KVParticle::SetFrame()) for details.
   //
   //In order to access the kinematics in the boosted frame, use the GetFrame() method of the
   //individual particles (see KVParticle::GetFrame()) twice :
   //      KVParticle* newframe = particle->GetFrame("oldframe")->GetFrame("newframe");
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVParticle *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle("ok"))) {
      nuc->SetFrame(newframe, oldframe, boost, beta);
	}
}

//___________________________________________________________________________//

void KVEvent::SetFrame(const Char_t * newframe, const Char_t * oldframe,
                       TLorentzRotation & rot)
{
   //Define a Lorentz-rotated frame "newframe" for all "ok" particles in the event.
   //The transformation is applied to the particle coordinates in the existing frame "oldframe"
   //
   //See KVParticle (method KVParticle::SetFrame()) for details.
   //
   //In order to access the kinematics in the boosted frame, use the GetFrame() method of the
   //individual particles (see KVParticle::GetFrame()) twice :
   //      KVParticle* newframe = particle->GetFrame("oldframe")->GetFrame("newframe");
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVParticle *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle("ok"))) {
	   nuc->SetFrame(newframe, oldframe, rot);
	}

}

//___________________________________________________________________________//

void KVEvent::SetFrame(const Char_t * newframe, const Char_t * oldframe,
                       TRotation & rot)
{
   //Define a rotated coordinate frame "newframe" for all "ok" particles in the event.
   //The transformation is applied to the particle coordinates in the existing frame "oldframe"
   //
   //See KVParticle (method KVParticle::SetFrame()) for details.
   //
   //In order to access the kinematics in the boosted frame, use the GetFrame() method of the
   //individual particles (see KVParticle::GetFrame()) twice :
   //      KVParticle* newframe = particle->GetFrame("oldframe")->GetFrame("newframe");
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   KVParticle *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle("ok"))) {
		nuc->SetFrame(newframe, oldframe, rot);
	}
}

//___________________________________________________________________________//

void KVEvent::SetFrame(const Char_t * newframe, const Char_t * oldframe,
                       const TVector3 & boost, TRotation & rot, Bool_t beta)
{
   //Define a rotated and Lorentz-boosted coordinate frame "newframe" for all "ok" particles in the event.
   //The transformation is applied to the particle coordinates in the existing frame "oldframe"
   //
   //See KVParticle (method KVParticle::SetFrame()) for details.
   //
   //In order to access the kinematics in the boosted frame, use the GetFrame() method of the
   //individual particles (see KVParticle::GetFrame()) twice :
   //      KVParticle* newframe = particle->GetFrame("oldframe")->GetFrame("newframe");
   //
   //YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

	KVParticle *nuc;
   ResetGetNextParticle();
   while ((nuc = GetNextParticle("ok"))) {
		nuc->SetFrame(newframe, oldframe, boost, rot, beta);
	}
}
