/***************************************************************************
                          kvreconstructednucleus.cpp  -  description
                             -------------------
    begin                : Fri Oct 18 2002
    copyright            : (C) 2002 by Alexis Mignon
    email                : mignon@ganil.fr

$Id: KVReconstructedNucleus.cpp,v 1.60 2009/03/03 13:36:00 franklan Exp $
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
#include "KVReconstructedNucleus.h"
#include "KVTelescope.h"
#include "KVGroup.h"
#include "KVMultiDetArray.h"
#include "KVACQParam.h"

ClassImp(KVReconstructedNucleus);

//////////////////////////////////////////////////////////////////////////////////////////
//KVReconstructedNucleus
//
//Particles reconstructed from data measured in multidetector arrays using identification
//telescopes (see KVIDTelescope class).
//
//If the particle has been identified (IsIdentified()=kTRUE) and calibrated (IsCalibrated()=kTRUE)
//then information on its nature and kinematics can be obtained using the methods of parent
//classes KVNucleus and KVParticle (GetZ(), GetVelocity(), etc.).
//
//Information specific to the detection and identification of the particle:
//
//   GetDetectorList()         -  list of detectors passed through, in reverse order
//   GetNumDet()               -  number of detectors in list = number of detectors passed through
//   GetDetector(Int_t i)      -  ith detector in list (i=0 is stopping detector)
//   GetStoppingDetector()     -  detector in which particle stopped (first in list)
//
//   GetGroup()                -  group in which particle detected/stopped (see KVGroup)
//   GetTelescope()            -  telescope to which stopping detector belongs (see KVTelescope)
//
//   GetIdentifyingTelescope() -  ID telescope which identified this particle (see KVIDTelescope)
//      IsIdentified()               - =kTRUE if particle identification has been completed
//      IsCalibrated()             - =kTRUE if particle's energy has been set
//      GetIDTelescopes()     - list of ID telescopes through which particle passed.
//                                   this list is used to find an ID telescope which can identify the particle.
//    GetRealZ(), GetRealA()  -  return the Z and A determined with floating-point precision by the
//                               identification of the particle.
//

void KVReconstructedNucleus::init()
{
   //default initialisation
   fRealZ = fRealA = 0.;
   fSimPart = 0;
   fIDTelescope = 0;
   fNumDet = 0;
   fNumPar = 0;
   fNSegDet = 0;
   fAnalStatus = 99;
   ResetBit(kIsIdentified);
   ResetBit(kIsCalibrated);
   ResetBit(kCoherency);
   for (register int i = 0; i < MAX_NUM_DET; i++) {
      fEloss[i] = 0.0;
   }
   for (register int i = 0; i < MAX_NUM_DET; i++) {
      fElossCalc[i] = 0.0;
   }
   for (register int i = 0; i < MAX_NUM_PAR; i++) {
      fACQData[i] = 0;
   }
}

KVReconstructedNucleus::KVReconstructedNucleus():fDetectors(MAX_NUM_DET)
{
   //default ctor.
   //TRefArray is initialised with size MAX_NUM_DET

   init();
}

KVReconstructedNucleus::
KVReconstructedNucleus(const KVReconstructedNucleus &
                       obj):fDetectors(MAX_NUM_DET)
{
   //copy ctor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVReconstructedNucleus &) obj).Copy(*this);
#endif
}

KVReconstructedNucleus::~KVReconstructedNucleus()
{
   //dtor
   if (GetGroup()) {
      GetGroup()->Reset();
   }
   fDetectors.Delete();
   init();
}

//______________________________________________________________________________

void KVReconstructedNucleus::Streamer(TBuffer & R__b)
{
   // Stream an object of class KVReconstructedNucleus.
   //Customized streamer.
   //When a KVReconstructedNucleus object is read (e.g. from a TTree) it will
   //automatically update the fELoss data members of the KVDetector objects
   //associated to the passage of this nucleus through the multidetector array
   //(pointed to by gMultiDetArray).
   //
   //In version 8 we added flags (using TObject bits, no new member variables)
   //to say if the particle has been identified and/or calibrated.
   //These are by default set to kFALSE by the constructor.
   //If we are reading data using version 7 or before, we need to set these flags
   //To be simple, we assume that a particle has been identified if KVNucleus::GetZ
   //returns a non-zero value (CsI identifications were not previously available,
   //so only charged particles i.e. no gammas or neutrons were identified), and we
   //assume that the particle is calibrated if GetEnergy() returns > 0.
   //
   //In order to reconstruct detailed information on the particle's reconstruction,
   //fNSegDet is recalculated from the list of detectors. KVGroup::AddHit is
   //called in order to permit (in KVReconstructedEvent::Streamer) the
   //recalculation of fAnalStatus.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      KVNucleus::Streamer(R__b);
      R__b >> fNumDet;
      R__b.ReadStaticArray(fEloss);
      if (R__v > 6)
         R__b.ReadStaticArray(fElossCalc);
      R__b >> fNumPar;
      R__b.ReadStaticArray(fACQData);
      fDetectors.Streamer(R__b);
      if (GetGroup())
         GetGroup()->AddHit(this);
      fNSegDet = 0;
      //set identification and calibration flags for data written with version < 8
      if (R__v < 8) {
         if (GetZ() > 0)
            SetBit(kIsIdentified);
         if (GetEnergy() > 0.)
            SetIsCalibrated();
      }
      //use arrays to update detectors in multidetector array
      //update energy losses (measured and calculated)
      //& acquisition parameters in array
      if (fNumDet && fNumPar) {
         UInt_t npar = 0;
         TIter next_det(&fDetectors);KVDetector *det; register int ndet = 0;
         while( (det = (KVDetector*)next_det()) ){
            
               fNSegDet += det->GetSegment();
               det->SetEnergy(fEloss[ndet]);
               if (R__v > 6) det->SetECalc((Double_t)fElossCalc[ndet]);
               ndet++;
               if (det->GetACQParamList()) {
                  TIter next_par(det->GetACQParamList());
                  KVACQParam *par;
                  while ((par = (KVACQParam *) next_par())) {
                     par->SetData(fACQData[npar++]);
                  }
               }
               //modify detector's counters depending on particle's identification state
               if (IsIdentified())
                  det->IncrementIdentifiedParticles();
               else
                  det->IncrementUnidentifiedParticles();
            
      			if (R__v < 10) {
						//before v.10 reconstructed particles were not added to 'Hits' list in detectors
						det->AddHit(this);
					}
         }
      }
      fSimPart.Streamer(R__b);
      if (R__v >= 6) {
         fIDTelescope.Streamer(R__b);
      }
      if (R__v > 8) {
         R__b >> fRealZ;
         R__b >> fRealA;
      }
      if (R__v > 9) {
         R__b >> fTargetEnergyLoss;
      }
      R__b.CheckByteCount(R__s, R__c, KVReconstructedNucleus::IsA());
   } else {
      R__c = R__b.WriteVersion(KVReconstructedNucleus::IsA(), kTRUE);
      KVNucleus::Streamer(R__b);
      R__b << fNumDet;
      R__b.WriteArray(fEloss, MAX_NUM_DET);
      R__b.WriteArray(fElossCalc, MAX_NUM_DET);
      R__b << fNumPar;
      R__b.WriteArray(fACQData, MAX_NUM_PAR);
      fDetectors.Streamer(R__b);
      fSimPart.Streamer(R__b);
      fIDTelescope.Streamer(R__b);
      R__b << fRealZ;
      R__b << fRealA;
      R__b << fTargetEnergyLoss;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

//___________________________________________________________________________

void KVReconstructedNucleus::Print(Option_t * option) const
{

   cout << "KVReconstructedNucleus:" << endl;

   if (GetNumDet()) {

      for (int i = GetNumDet() - 1; i >= 0; i--) {
         KVDetector *det = GetDetector(i);
         det->Print("data");
      }
      
   }
}

//_______________________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVReconstructedNucleus::Copy(TObject & obj) const
#else
void KVReconstructedNucleus::Copy(TObject & obj)
#endif
{
   //
   //Copy this to obj
   //
   KVNucleus::Copy(obj);
   if (GetSimPart())
      ((KVReconstructedNucleus &) obj).SetSimPart(GetSimPart());
   ((KVReconstructedNucleus &) obj).SetNumDet(GetNumDet());
   ((KVReconstructedNucleus &) obj).SetNumPar(GetNumPar());
   ((KVReconstructedNucleus &) obj).SetElossTable(GetElossTable());
   ((KVReconstructedNucleus &) obj).SetElossCalcTable(GetElossCalcTable());
   ((KVReconstructedNucleus &) obj).SetACQData(GetACQData());
   ((KVReconstructedNucleus &) obj).
       SetIdentifyingTelescope(GetIdentifyingTelescope());
   ((KVReconstructedNucleus &) obj).SetRealZ(GetRealZ());
   ((KVReconstructedNucleus &) obj).SetRealA(GetRealA());
	// copy the PID of the KVRList fDetectors, otherwise if detectors were not
	// created in this session (i.e. written on file), we will not be able to
	// add them to obj's list
	// this is best achieved by simply copying the TRefArray
	((KVReconstructedNucleus &) obj).fDetectors = fDetectors;
}



void KVReconstructedNucleus::Clear(Option_t * opt)
{
   //Reset nucleus. Calls KVNucleus::Clear.
   //Calls KVGroup::Reset for the group where it was reconstructed.

   KVNucleus::Clear(opt);
   if (GetGroup())
      GetGroup()->Reset();
   fDetectors.Delete();
   init();
}

void KVReconstructedNucleus::AddDetector(KVDetector * det)
{
   //Add a detector to the list of those through which the particle passed.
   //Put reference to detector into fDetectors array, store detector's energy loss value in
   //fEloss array, increase number of detectors by one.
   //As this is only used in initial particle reconstruction, we add 1 unidentified particle to the detector.

   if (fNumDet == MAX_NUM_DET) {
      Warning("AddDetector",
              "Cannot add more than %d detectors to reconstructed nucleus. Detector not taken into account",
              fNumDet);
      return;
   }
   fDetectors.AddAt(det, fNumDet);      //this will not expand the array if fNumDet is bigger than MAX_NUM_DET
   fEloss[fNumDet++] = det->GetEnergy();
   if (det->GetACQParamList()) {
      if ((fNumPar + det->GetACQParamList()->GetSize()) > MAX_NUM_PAR) {
         Warning("AddDetector",
                 "Cannot add more than %d DAQ parameters to reconstructed nucleus. Parameters not taken into account",
                 MAX_NUM_PAR);
         return;
      }
      TIter next_par(det->GetACQParamList());
      KVACQParam *par;
      while ((par = (KVACQParam *) next_par())) {
         fACQData[fNumPar++] = par->GetCoderData();
      }
   }
   //add segmentation index of detector to total segmentation index of particle
   fNSegDet += det->GetSegment();
   //add 1 unidentified particle to the detector
   det->IncrementUnidentifiedParticles();
}

//______________________________________________________________________________________________//

void KVReconstructedNucleus::Reconstruct(KVDetector * kvd)
{
   //Reconstruction of a detected nucleus from the successive energy losses
   //measured in a series of detectors/telescopes.
   //
   //Starting from detector *kvd, collect information from all detectors placed directly
   //in front of *kvd (kvd->GetGroup()->GetAlignedDetectors( kvd )),
   //these are the detectors the particle has passed through.
   //
   //Each one is added to the particle's list (KVReconstructedNucleus::AddDetector), and,
   //if it is not an unsegmented detector, it is marked as having been "analysed"
   //(KVDetector::SetAnalysed) in order to stop it being considered as a starting point for another
   //particle reconstruction.

   fNSegDet = 0;

   //get list of detectors through which particle passed
   if (kvd->GetGroup()) {
      TList *aligned = kvd->GetGroup()->GetAlignedDetectors(kvd);
      if (aligned) {

         TIter next_aligned(aligned);
         KVDetector *d;
         while ((d = (KVDetector *) next_aligned())) {
               AddDetector(d);
               d->AddHit(this);  // add particle to list of particles hitting detector
               d->SetAnalysed(kTRUE);   //cannot be used to seed another particle
         }
         delete aligned;
      }
      kvd->GetGroup()->AddHit(this);
   }
}

//_____________________________________________________________________________________

void KVReconstructedNucleus::Identify()
{
   // Try to identify this nucleus by calling the Identify() function of each
   // ID telescope crossed by it, starting with the telescope where the particle stopped, in order
   //      -  only attempt identification in ID telescopes containing the stopping detector.
   //      -  only telescopes which have been correctly initialised for the current run are used,
   //         i.e. those for which KVIDTelescope::IsReadyForID() returns kTRUE.
   // This continues until a successful identification is achieved or there are no more ID telescopes to try.
   // The identification code corresponding to the identifying telescope is set as the identification code of the particle.

   TList *idt_list = GetStoppingDetector()->GetTelescopesForIdentification();

   if (idt_list && idt_list->GetSize() > 0) {
      
         KVIDTelescope *idt;
         TIter next(idt_list);

         while ((idt = (KVIDTelescope *) next())) {
				
            if( idt->IsReadyForID() ) { // is telescope able to identify for this run ?

            if (idt->Identify(this)) {  //Identify()=kTRUE if ID successful
               //cout << " IDENTIFICATION SUCCESSFUL"<<endl;
               SetIsIdentified();
               SetIdentifyingTelescope(idt);
               break;
            } else {
               //cout << " IDENTIFICATION FAILED"<<endl;
               //ID in this telescope has failed
               //We reduce the "segmentation" index by 1.
               //If this remains >=2, we carry on trying to identify
               //However, if it falls to 1, then the particle's identifiability depends
               //on the rest of the KVGroup where it was detected.
               //If there are no other _unidentified_ particles in the group, it's fine.
               //If there _are_ other unidentified particles in the group, then we should wait until
               //the next ID round (equivalent of particles with GetAnalStatus=1).
               SetNSegDet(TMath::Max(GetNSegDet() - 1, 0));
               //if there are other unidentified particles in the group and NSegDet is < 2
               //then exact status depends on segmentation of the other particles : reanalyse
               //cout << "...........NSegDet now = " << GetNSegDet() << " && number unidentified in group="
               //              << (int)GetGroup()->GetNUnidentified() << endl;
               if (GetNSegDet() < 2 && GetGroup()->GetNUnidentified() > 1)
                  break;
               //if NSegDet = 0 it's hopeless
               if (!GetNSegDet())
                     break;
            }
				 
            }
            
         }
         
      }
}

//______________________________________________________________________________________________//

void KVReconstructedNucleus::SetElossCalc(KVDetector * det, Float_t energy)
{
   //Store calculated energy loss value for this particle corresponding to given detector
   //The particle's contribution is subtracted from the detector's calculated energy
   //loss and the detector's "reanalysis" flag is set to true.

   if (!det)
      return;
   Int_t index = fDetectors.IndexOf(det);
   if (index < 0)
      return;                   //detector not in list
   fElossCalc[index] = energy;
   det->SetECalc(energy);
}

//______________________________________________________________________________________________//

Float_t KVReconstructedNucleus::GetElossCalc(KVDetector * det) const
{
   //Get calculated energy loss value for this particle corresponding to given detector

   Int_t index = fDetectors.IndexOf(det);
   if (index < 0)
      return -1.0;              //detector not in list
   return fElossCalc[index];
}

//______________________________________________________________________________________________//

void KVReconstructedNucleus::GetAnglesFromTelescope(Option_t * opt)
{
   //Calculate angles theta and phi for reconstructed nucleus based
   //on position of telescope in which it was detected. The nucleus'
   //momentum is set using these angles, its mass and its kinetic energy.
   //The (optional) option string can be "random" or "mean".
   //If "random" (default) the angles are drawn at random between the
   //min/max angles of the telescope.
   //If "mean" the (theta,phi) position of the center of the telescope
   //is used to fix the nucleus' direction.

   //don't try if particle has no correctly defined energy
   if (GetEnergy() <= 0.0)
      return;
   if (!GetTelescope())
      return;

   if (!strcmp(opt, "random")) {
      //random angles
      SetRandomMomentum(GetEnergy(), GetTelescope()->GetThetaMin(),
                        GetTelescope()->GetThetaMax(),
                        GetTelescope()->GetPhiMin(),
                        GetTelescope()->GetPhiMax(), "random");
   } else {
      //middle of telescope
      TVector3 dir = GetTelescope()->GetDirection();
      SetMomentum(GetEnergy(), dir);
   }
}

//______________________________________________________________________________________________//

Int_t KVReconstructedNucleus::GetIDSubCode(const Char_t * id_tel_type,
                                           KVIDSubCode & code) const
{
   //If the identification of the particle was attempted using a KVIDTelescope of type "id_tel_type",
   //and 'code' holds all the subcodes from the different identification routines tried for this particle,
   //then this method returns the subcode for this particle from telescope type "id_tel_type".
   //
   //In case of problems (no ID telescope of type 'id_tel_type'), the returned value is -65535.
   //
   //If no type is given (first argument = ""), we use the identifying telescope (obviously if the
   //particle has remained unidentified - IsIdentified()==kFALSE - and the GetIdentifyingTelescope()
   //pointer is not set, we return -65535).

   KVIDTelescope *idtel;
   if (strcmp(id_tel_type, ""))
      idtel =
          (KVIDTelescope *) GetIDTelescopes()->
          FindObjectByType(id_tel_type);
   else
      idtel = GetIdentifyingTelescope();
   if (!idtel)
      return -65535;
   return idtel->GetIDSubCode(code);
}

//______________________________________________________________________________________________//

const Char_t *KVReconstructedNucleus::GetIDSubCodeString(const Char_t *
                                                         id_tel_type,
                                                         KVIDSubCode &
                                                         code) const
{
   //If the identification of the particle was attempted using a KVIDTelescope of type "id_tel_type",
   //and 'code' holds all the subcodes from the different identification routines tried for this particle,
   //then this method returns an explanation for the subcode for this particle from telescope type "id_tel_type".
   //
   //If no type is given (first argument = ""), we use the identifying telescope.
   //
   //In case of problems :
   //       no ID telescope of type 'id_tel_type' :  "No identification attempted in id_tel_type"
   //       particle not identified               :  "Particle unidentified. Identifying telescope not set."

   KVIDTelescope *idtel;
   if (strcmp(id_tel_type, ""))
      idtel =
          (KVIDTelescope *) GetIDTelescopes()->
          FindObjectByType(id_tel_type);
   else
      idtel = GetIdentifyingTelescope();
   if (!idtel) {
      if (strcmp(id_tel_type, ""))
         return Form("No identification attempted in %s", id_tel_type);
      else
         return
             Form("Particle unidentified. Identifying telescope not set.");
   }
   return idtel->GetIDSubCodeString(code);
}

//_________________________________________________________________________________

void KVReconstructedNucleus::Calibrate()
{
   //Calculate and set the energy of a (previously identified) reconstructed particle,
   //including an estimate of the energy loss in the target.
   //
   //Starting from the detector in which the particle stopped, we add up the
   //'corrected' energy losses in all of the detectors through which it passed.
   //Whenever possible, for detectors which are not calibrated or not working,
   //we calculate the energy loss. Measured & calculated energy losses are also
   //compared for each detector, and may lead to new particles being seeded for
   //subsequent identification. This is done by KVIDTelescope::CalculateParticleEnergy().
   //
   //For particles whose energy before hitting the first detector in their path has been
   //calculated after this step we then add the calculated energy loss in the target,
   //using gMultiDetArray->GetTargetEnergyLossCorrection().

   KVIDTelescope* idt = GetIdentifyingTelescope();
   idt->CalculateParticleEnergy(this);
   //if calibration was successful, we now read the energy losses from the detectors
   //and store them in the fEloss array. this is because detectors whose calibration
   //depends on the identity (Z & A) of the particle will only have their energy loss
   //set when this method is called; the energy that was stored at reconstruction time
   //(in AddDectector) will have been zero.
   //Also, whenever possible, we calculate the energy loss for any uncalibrated or
   //non-functioning detectors during this procedure, so this calculated energy loss
   //will also be stored.
   //we also read from each detector and store the calculated remaining energy loss
   //which, if non-zero, indicates the presence of another particle (pile-up)
   if( idt->GetCalibStatus() != KVIDTelescope::kCalibStatus_NoCalibrations ){
      SetIsCalibrated();
      //add correction for target energy loss
		Double_t E_targ = gMultiDetArray->GetTargetEnergyLossCorrection(this); 
      SetTargetEnergyLoss( E_targ );
      Double_t E_tot = GetEnergy() + E_targ;
      SetEnergy( E_tot );		
		TIter nxt(&fDetectors); KVDetector* det; register int ndet = 0;
      while( (det = (KVDetector*)nxt()) ){
         fEloss[ndet] = (Float_t)det->GetEnergy();
         fElossCalc[ndet] = (Float_t)det->GetECalc();
         ++ndet;
      }
   }
}
