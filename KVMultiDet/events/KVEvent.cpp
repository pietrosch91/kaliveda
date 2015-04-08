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

using namespace std;

ClassImp(KVEvent);

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVEvent</h2>
<h4>Base class for all types of multiparticle event consisting of KVNucleus objects</h4>
<!-- */
// --> END_HTML
/*Particles are stored in a TClonesArray and KVEvent provides some basic
functionality for accessing and manipulating the list.

Events can be built using any class derived from KVNucleus to represent particles.
These classes can allocate memory in their default ctor: when filling events in a loop,
the same 'particle' objects are re-used for each new event, the ctor of each object will
only be called once when the object is first created (e.g. in the first event).
The particle class Clear() method will be called before each new event.
Therefore the cycle of use of the particle objects in a loop over many events is:

<particle ctor>
   Building 1st event
   <particle Clear()>
   Building 2nd event
   <particle Clear()>
   ...
   Building last event
<particle dtor>

When writing events in a TTree, it is very important to call the TBranch::SetAutoDelete(kFALSE)
method of the branch which is used to store the event object.
If not, when the events are read back, the KVEvent constructor and destructor will be called
every time an event is read from the TTRee!! Leading to very slow reading times (& probably
memory leaks)
*/
/////////////////////////////////////////////////////////////////////////////://


KVEvent::KVEvent(Int_t mult, const char *classname) : fParameters("EventParameters","Parameters associated with an event")
{
   //Initialise KVEvent to hold mult events of "classname" objects
   //(the class must inherit from KVNucleus).
   //"mult" is the approximate maximum multiplicity of the events (if too
   //small, extra space will be allocated automatically by TClonesArray).
   //
   // Default argument :
   //     classname = "KVNucleus"
   //
   
   fOKIter = 0;
   fParticles = new TClonesArray(classname, mult);
   CustomStreamer();//force use of KVEvent::Streamer function for reading/writing
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
   fParameters.Copy( ((KVEvent&)obj).fParameters );
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
   // Method used for building an event particle by particle.
   // DO NOT USE FOR READING EVENTS - use GetParticle(Int_t npart)!!
   //
   // This method increases the multiplicity fMult by one and "creates" a new particle with index (fMult-1).
   // In actual fact a new object is only created if needed i.e. if the new multiplicity is greater
   // than previously. Particle objects in the array are reused from one event to another.
   // The default constructor for the class corresponding to the "new" particle will only be called
   // once during its lifetime (i.e. if N events are generated, the particle ctor will be called only once,
   // not N times). Once created, in subsequent events we just call the particle's Clear() method
   // in order to reset its internal variables ready for a new event.

   Int_t mult = GetMult();
#ifdef __WITHOUT_TCA_CONSTRUCTED_AT
   KVNucleus *tmp = (KVNucleus *) ConstructedAt(mult, "C");
#else
   KVNucleus *tmp = (KVNucleus *) fParticles->ConstructedAt(mult, "C");
#endif
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
   fParameters.Clear();
   ResetGetNextParticle();
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
   fParameters.Print();
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

//__________________________________________________________________________________

Double_t KVEvent::GetSum(const Char_t* KVNucleus_method,Option_t * opt)
{
   //Returns sum other particles of the observable given by the indicated KVNucleus_method
   //for example
   //if  the method is called this way GetSum("GetZ"), it returns the sum of the charge
   //of particles in the current event
   //
   //If opt = "ok" only particles with IsOK()==kTRUE are considered.
   //If opt = "name" only particles with GetName()=="name" are considered.
   //
   //IN ANY CASE, YOU MUST NOT USE THIS METHOD INSIDE A LOOP
   //OVER THE EVENT USING GETNEXTPARTICLE() !!!

   TString Opt(opt);
   Opt.ToUpper();
   Double_t fSum = 0;
	KVNucleus *tmp = new KVNucleus();
	TMethodCall mt;
   mt.InitWithPrototype(tmp->IsA(), KVNucleus_method,"");
   delete tmp;
   
   if (mt.IsValid()){
   	ResetGetNextParticle();
      if (mt.ReturnType()==TMethodCall::kLong){
      	Long_t ret;
         while ((tmp = GetNextParticle(Opt))){
   			mt.Execute(tmp,"",ret);
      	   fSum+=ret;
   		}
      }
      else if (mt.ReturnType()==TMethodCall::kDouble){
      	Double_t ret;
         while ((tmp = GetNextParticle(Opt))){
   			mt.Execute(tmp,"",ret);
      	   fSum+=ret;
   		}
      }   
   }
   
   return fSum;
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

//______________________________________________________________________________

void KVEvent::Streamer(TBuffer &R__b)
{
   // Customised Streamer for KVEvent.
   // This is just the automatic Streamer with the addition of a call to the Clear()
   // method before reading a new object (avoid memory leaks with lists of parameters).

   if (R__b.IsReading()) {
      Clear();
      R__b.ReadClassBuffer(KVEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(KVEvent::Class(),this);
   }
}

//______________________________________________________________________________

#ifdef __WITHOUT_TCA_CONSTRUCTED_AT
TObject* KVEvent::ConstructedAt(Int_t idx)
{
   // Get an object at index 'idx' that is guaranteed to have been constructed.
   // It might be either a freshly allocated object or one that had already been
   // allocated (and assumingly used).  In the later case, it is the callers 
   // responsability to insure that the object is returned to a known state,
   // usually by calling the Clear method on the TClonesArray.
   //
   // Tests to see if the destructor has been called on the object.  
   // If so, or if the object has never been constructed the class constructor is called using
   // New().  If not, return a pointer to the correct memory location.
   // This explicitly to deal with TObject classes that allocate memory
   // which will be reset (but not deallocated) in their Clear()
   // functions.
   
   TObject *obj = (*fParticles)[idx];
   if ( obj && obj->TestBit(TObject::kNotDeleted) ) {
      return obj;
   }
   return (fParticles->GetClass()) ? static_cast<TObject*>(fParticles->GetClass()->New(obj)) : 0;
}
//______________________________________________________________________________
TObject *KVEvent::ConstructedAt(Int_t idx, Option_t *clear_options)
{
   // Get an object at index 'idx' that is guaranteed to have been constructed.
   // It might be either a freshly allocated object or one that had already been
   // allocated (and assumingly used).  In the later case, the function Clear
   // will be called and passed the value of 'clear_options'
   //
   // Tests to see if the destructor has been called on the object.  
   // If so, or if the object has never been constructed the class constructor is called using
   // New().  If not, return a pointer to the correct memory location.
   // This explicitly to deal with TObject classes that allocate memory
   // which will be reset (but not deallocated) in their Clear()
   // functions.
   
   TObject *obj = (*fParticles)[idx];
   if ( obj && obj->TestBit(TObject::kNotDeleted) ) {
      obj->Clear(clear_options);
      return obj;
   }
   return (fParticles->GetClass()) ? static_cast<TObject*>(fParticles->GetClass()->New(obj)) : 0;
}
#endif
	
void KVEvent::FillArraysV(Int_t& mult, Int_t* Z, Int_t* A, Double_t* vx, Double_t* vy, Double_t* vz)
{
	// "Translate" this event into a simple array form
	// mult will be set to number of nuclei in event
	// (vx,vy,vz) velocity components in cm/ns

	mult = GetMult();
	KVNucleus* nuc;
	Int_t i=0;
	while( (nuc = GetNextParticle()) ){
		Z[i] = nuc->GetZ();
		A[i] = nuc->GetA();
		vx[i] = nuc->GetVelocity().X();
		vy[i] = nuc->GetVelocity().Y();
		vz[i] = nuc->GetVelocity().Z();
		i++;
	}
}
	
void KVEvent::FillArraysEThetaPhi(Int_t& mult, Int_t* Z, Int_t* A, Double_t* E, Double_t* Theta, Double_t* Phi)
{
	// "Translate" this event into a simple array form
	// mult will be set to number of nuclei in event
	// E = kinetic energy in MeV
	// Theta,Phi in degrees

	mult = GetMult();
	KVNucleus* nuc;
	Int_t i=0;
	while( (nuc = GetNextParticle()) ){
		Z[i] = nuc->GetZ();
		A[i] = nuc->GetA();
		E[i] = nuc->GetEnergy();
		Theta[i] = nuc->GetTheta();
		Phi[i] = nuc->GetPhi();
		i++;
	}
} 
