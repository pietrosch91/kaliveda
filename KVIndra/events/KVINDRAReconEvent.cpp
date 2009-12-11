/***************************************************************************
                          kvindrareconevent.cpp  -  description
                             -------------------
    begin                : Thu Oct 10 2002
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
#include "TROOT.h"
#include "KVINDRAReconEvent.h"
#include "KVList.h"
#include "KVGroup.h"
#include "KVTelescope.h"
#include "KVDetector.h"
#include "KVCsI.h"
#include "KVDetectorEvent.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRA.h"
#include "TStreamerInfo.h"
#include "KVIDCsI.h"
#include "KVIDGCsI.h"
#include "KVDataSet.h"

ClassImp(KVINDRAReconEvent);

///////////////////////////////////////////////////////////////////////////////
//Event reconstructed from energy losses in INDRA multidetector.
//By default this contains a TClonesArray of KVINDRAReconNuc objects.
//You must have a valid KVINDRA-derived object before using a KVINDRAReconEvent,
//otherwise one will be created (not necessarily the right one).***
//
//                ***NOTE: when using a KVSelector-derived analysis class to read data,
//                      *** this is automatically taken care of.
//
//Random vs. mean angles
//----------------------
//By default reconstructed particles have randomised angles within the
//aperture of the detector where they were stopped. To use the mean angles
//of the detectors, call UseMeanAngles().
//
//"OK" & not "OK" particles
//---------------------------
//The "OK" status of particles is defined by setting acceptable identification and calibration
//codes using AcceptIDCodes and AcceptECodes. The comparison of each particle's codes
//with the "acceptable" codes then determines whether KVParticle::IsOK() is set or not.
//
//Trigger Information
//--------------------
//Use EventTrigger() to access the KVINDRATriggerInfo object which stores the values
//of Selecteur registers STAT_EVE, R_DEC and CONFIG for this event.
///////////////////////////////////////////////////////////////////////////////

void KVINDRAReconEvent::init()
{
   //default initialisations
   fCodeMask = 0;
}


KVINDRAReconEvent::KVINDRAReconEvent(Int_t mult, const char *classname)
:KVReconstructedEvent(mult, classname)
{
   init();
   CustomStreamer();            //because KVINDRAReconNuc has a customised streamer
}

KVINDRAReconEvent::~KVINDRAReconEvent()
{
   if (fCodeMask) {
      delete fCodeMask;
      fCodeMask = 0;
   }
};

/////////////////////////////////////////////////////////////////////////////

KVINDRAReconNuc *KVINDRAReconEvent::GetParticle(Int_t npart) const
{
   //
   //Access to event member with index npart (1<=npart<=fMult)
   //

   return (KVINDRAReconNuc *) (KVEvent::GetParticle(npart));
}

/////////////////////////////////////////////////////////////////////////////

KVINDRAReconNuc *KVINDRAReconEvent::AddParticle()
{
   //Wrapper for KVEvent::GetNextParticle casting result to KVINDRAReconNuc*

   KVINDRAReconNuc *tmp = (KVINDRAReconNuc *) (KVEvent::AddParticle());
   return tmp;
}

/////////////////////////////////////////////////////////////////////////////
void KVINDRAReconEvent::Streamer(TBuffer & R__b)
{
   //Stream an object of class KVINDRAReconEvent.
   //We loop over the newly-read particles in order to set their
   //IsOK() status by comparison with the event's code mask
   //
   //We "correct" the KVReconstructedNucleus::IsAMeasured() flag in cases where particles are wrongly labelled
   //as having measured masses (e.g. 1st campaign Si-CsI identifications).
   //If GetRealA() - GetA() == 0 then the particle's mass was not measured
   //(for CsI R-L identified-particles, no such 'correction' is applied).

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v < 4) {
         Clear();
         KVEvent::Streamer(R__b);
         Float_t fThreshold;
         R__b >> fThreshold;
         SetThreshold(fThreshold);
      } else {
         KVReconstructedEvent::Streamer(R__b);
      }
      if (R__v > 4 && R__v< 6){
         KVINDRATriggerInfo fTrigger;
         fTrigger.Streamer(R__b);
      }
      R__b.CheckByteCount(R__s, R__c, KVINDRAReconEvent::IsA());
      //check codes (&& if R__v<4 set angles)
      //'correct' IsotopeResolve flag
      KVINDRAReconNuc *par;
      while ((par = GetNextParticle())) {
         if (CheckCodes(par->GetCodes()))
            par->SetIsOK();
         else
            par->SetIsOK(kFALSE);
         if(par->IsAMeasured() && !par->GetCodes().TestIDCode( kIDCode_CsI )){
            //check realA != A
            if( (par->GetRealA()-par->GetA()) == 0 ) par->SetAMeasured(kFALSE);
         }
         if (R__v < 4) {
            if (HasMeanAngles())
               par->GetAnglesFromTelescope("mean");
            else
               par->GetAnglesFromTelescope("random");
            //reconstruct fAnalStatus information for KVReconstructedNucleus
            if (par->GetStatus() == 99)     //AnalStatus has not been set for particles in group
               if (par->GetGroup())
                  par->GetGroup()->AnalyseParticles();
         }
      }
   } else {
      KVINDRAReconEvent::Class()->WriteBuffer(R__b,this);
   }
}


//______________________________________________________________________________
void KVINDRAReconEvent::Print(Option_t * option) const
{
   //Print out list of particles in the event.
   //If option="ok" only particles with IsOK=kTRUE are included.

   cout << GetTitle() << endl;  //system
   cout << GetName() << endl;   //run
   cout << "Event number: " << GetNumber() << endl << endl;
   cout << "MULTIPLICITY = " << ((KVINDRAReconEvent *) this)->
       GetMult(option) << endl << endl;

   KVINDRAReconNuc *frag = 0;
   while ((frag = ((KVINDRAReconEvent *) this)->GetNextParticle(option))) {
      frag->Print();
   }
}

//_____________________________________________________________________________

//void KVINDRAReconEvent::GeometricFilter()
//{
//      //Private method, called during event reconstruction if SetGeometricFilter(kTRUE)
//      //was previously called.
//      //From the "coherence" analysis (see AnalyseGroup) we only keep particles which
//      //can be correctly reconstructed either with or without subtraction of other
//      //particles' calculated energy losses, and we make sure that particles actually
//      //get stopped by the detectors.
//      //
//      //An additional energy threshold can be given (SetThreshold) which defines the
//      //minimum energy in MeV/nucleon for a particle to be accepted (i.e. identification
//      //threshold).
//      //
//      //Accepted particles have IsOK = kTRUE, rejected particles have IsOK = kFALSE
//
//      for(Int_t i=1; i<=GetMult(); i++)
//      {
//              KVINDRAReconNuc *nuc = GetParticle(i);
//              nuc->SetIsOK();
//              //from the "coherence" analysis (see AnalyseGroup) we only keep particles which
//              //can be correctly reconstructed either with or without subtraction of other
//              //particles' calculated energy losses
//              if(nuc->GetAnalStatus() > 1) {
//                      nuc->SetIsOK(kFALSE);
//              }
//              //make sure only one particle stopped in "stopping" detector
//              if( nuc->GetStoppingDetector()->GetHits()->GetSize() > 1){
//                      nuc->SetIsOK(kFALSE);
//              }
//              //make sure particle actually stopped in detector
//              if(((KVNucleus*)nuc->GetStoppingDetector()->GetHits()->At(0))->GetEnergy()>0.0) {
//                      nuc->SetIsOK(kFALSE);
//              }
//              if(nuc->IsOK())
//              {
//                      //particle accepted so far - get A, Z and energy from simulated particle
//                      //and detector energy losses.
//                      //Particle may still be rejected by identification threshold - see below !!!
//                              KVNucleus* simpart = (KVNucleus*)nuc->GetStoppingDetector()->GetHits()->At(0);
//                              if(!simpart)cout<<"No simpart"<<endl;
//                              nuc->SetSimPart(simpart);
//                              nuc->SetZ(simpart->GetZ());
//                              if(nuc->GetZ()<3)
//                              {
//                                      //for LCP reaching CsI, use true isotope mass
//                                      if(nuc->StoppedInCsI())
//                                      {
//                                              nuc->SetA(simpart->GetA());
//                                      }
//                              }
//                              //add up corrected energy losses in detectors
//                              Float_t fEnew=0.0;
//                              for(UInt_t d=0; d<nuc->GetNumDet(); d++)
//                              {
//                                      fEnew += nuc->GetDetector(d)->GetCorrectedEnergy(nuc->GetZ(),nuc->GetA());
//                              }
//                              nuc->SetEnergy(fEnew);
//                              nuc->GetAnglesFromTelescope();
//                              nuc->SetEnergy(fEnew);
//                              //identification threshold in mev/nuc - is 0.0 if not set
//                              if((nuc->GetEnergy()/nuc->GetA())<fThreshold){
//                                      nuc->SetIsOK(kFALSE);
//                              }
//              }
//      }
//}

////////////////////////////////////////////////////////////////////////////////////////

KVINDRAReconNuc *KVINDRAReconEvent::GetNextParticle(Option_t * opt)
{
   //Use this method to iterate over the list of particles in the event.
   //
   //If opt="ok" only the particles whose ID codes and E codes correspond to those set using AcceptIDCodes and
   //AcceptECodes will be returned, in the order in which they appear in the event.
   //
   //After the last particle in the event GetNextParticle() returns a null pointer and
   //resets itself ready for a new iteration over the particle list.
   //Therefore, to loop over all particles in an event, use a structure like:
   //
   //      KVINDRAReconNuc* p;
   //      while( (p = GetNextParticle() ){
   //              ... your code here ...
   //      }

   return (KVINDRAReconNuc *) KVEvent::GetNextParticle(opt);
}

//______________________________________________________________________________________________

Bool_t KVINDRAReconEvent::IsOK()
{
   //Returns kTRUE if event is acceptable for analysis.
   //This means that the multiplicity of particles with IsOK()=kTRUE (having good ID codes etc.)
   //is at least equal to the multiplicity trigger of the (experimental) event.

   if (gIndra) {
      if (gIndra->GetTrigger())
         return (GetMult("ok") >= gIndra->GetTrigger());
      else
         return (GetMult("ok") > 0);
   } else {
      return (KVEvent::IsOK());
   }
   return kTRUE;
}

//______________________________________________________________________________________________

void KVINDRAReconEvent::AcceptIDCodes(UShort_t code)
{
   //Define the identification codes that you want to include in your analysis.
   //Example: to keep only (VEDA) codes 2 and 3, use
   //        event.AcceptIDCodes( kIDCode2 | kIDCode3 );
   //Particles which have the correct ID codes will have IsOK() = kTRUE
   //i.e.  if( particle.IsOK() ) { // ... analysis ... }
   //although it is easier to loop over all 'correct' particles using:
   //   while ( particle = GetNextParticle("ok") ){ // ... analysis ... }
   //
   //To remove any previously defined acceptable identification codes, use AcceptIDCodes(0)
   //
   //N.B. : this method is preferable to using directly the KVINDRACodeMask pointer
   //as the 'IsOK' status of all particles of the current event are automatically updated
   //when the code mask is changed using this method.
   GetCodeMask()->SetIDMask(code);
   KVINDRAReconNuc *par = 0;
   while ((par = GetNextParticle())) {
      if (CheckCodes(par->GetCodes()))
         par->SetIsOK();
      else
         par->SetIsOK(kFALSE);
   }
}

//______________________________________________________________________________________________

void KVINDRAReconEvent::AcceptECodes(UChar_t code)
{
   //Define the calibration codes that you want to include in your analysis.
   //Example: to keep only ecodes 1 and 2, use
   //        event.AcceptECodes( kECode2 | kECode3 );
   //Particles which have the correct E codes will have IsOK() = kTRUE
   //i.e.  if( particle.IsOK() ) { // ... analysis ... }
   //although it is easier to loop over all 'correct' particles using:
   //   while ( particle = GetNextParticle("ok") ){ // ... analysis ... }
   //
   //N.B. : this method is preferable to using directly the KVINDRACodeMask pointer
   //as the 'IsOK' status of all particles of the current event are automatically updated
   //when the code mask is changed using this method.
   GetCodeMask()->SetEMask(code);
   KVINDRAReconNuc *par = 0;
   while ((par = GetNextParticle())) {
      if (CheckCodes(par->GetCodes()))
         par->SetIsOK();
      else
         par->SetIsOK(kFALSE);
   }
}

//______________________________________________________________________________________________

void KVINDRAReconEvent::ChangeFragmentMasses(UChar_t mass_formula)
{
   //Changes the mass formula used to calculate A from Z for all nuclei in event
   //For the values of mass_formula, see KVNucleus::GetAFromZ
   //
   //The fragment energy is modified in proportion to its mass, this is due to the
   //contribution from the CsI light-energy calibration:
   //
   //    E -> E + E_CsI*( newA/oldA - 1 )
   //
   //From an original lunch by Remi Bougault.
   //
   //Only particles with 'acceptable' ID & E codes stopping in (or passing through)
   //a CsI detector are affected; particles whose mass was measured
   //(i.e. having KVReconstructedNucleus::IsAMeasured()==kTRUE)
	//are not affected by the change of mass formula.
   
   ResetGetNextParticle();
   KVINDRAReconNuc* frag;
   while( (frag = (KVINDRAReconNuc*)GetNextParticle("ok")) ){
      
      if( !frag->IsAMeasured() ){
         
         Float_t oldA = (Float_t)frag->GetA();
         frag->SetMassFormula(mass_formula);
         
         if( frag->GetCsI() ){
            Float_t oldECsI = frag->GetEnergyCsI();
            Float_t oldE = frag->GetEnergy();
            Float_t newECsI = oldECsI*((Float_t)frag->GetA()/oldA);
            frag->GetCsI()->SetEnergy(newECsI);
            frag->SetEnergy(oldE - oldECsI + newECsI);
         } 
      }
   }
   
}

//____________________________________________________________________________

void KVINDRAReconEvent::IdentifyEvent()
{
   //Performs event identification (see KVReconstructedEvent::IdentifyEvent), and then
   //particles stopping in first member of a telescope (KVReconstructedNucleus::GetStatus=3) are
   //labelled with VEDA ID code kIDCode5 (Zmin)
  //Unidentified particles receive the general ID code for non-identified particles (kIDCode14)

   KVReconstructedEvent::IdentifyEvent();
   KVINDRAReconNuc *d=0;
   while ((d = GetNextParticle())) {
      if (d->IsIdentified() && d->GetStatus() == 3) {
         d->SetIDCode( kIDCode5 ); // Zmin
      }
      else if(!d->IsIdentified()){
         d->SetIDCode( kIDCode14 );
      }
   }
}

