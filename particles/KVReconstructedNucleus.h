/***************************************************************************
                          kvreconstructednucleus.h  -  description
                             -------------------
    begin                : Fri Oct 18 2002
    copyright            : (C) 2002 by Alexis Mignon
    email                : mignon@ganil.fr

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVRECONSTRUCTEDNUCLEUS_H
#define KVRECONSTRUCTEDNUCLEUS_H

#include "KVNucleus.h"
#include "KVRList.h"
#include "KVList.h"
#include "TRef.h"
#include "KVDetector.h"
#include "KVIDSubCodeManager.h"

class KVTelescope;
class KVGroup;
class KVIDTelescope;

class KVReconstructedNucleus:public KVNucleus {


 protected:
#define MAX_NUM_DET 5
#define MAX_LEN_DET_NAME 10
#define MAX_NUM_PAR 15
   Int_t fNumDet;               //number of detectors particle passed through - Int_t imposed by TStreamerInfo
   Float_t fEloss[MAX_NUM_DET]; //[MAX_NUM_DET] measured energy losses in each successive detector
   Float_t fElossCalc[MAX_NUM_DET];     //[MAX_NUM_DET] calculated energy losses in each successive detector
   Int_t fNumPar;               //number of associated acquisition parameters - Int_t imposed by TStreamerInfo
   UShort_t fACQData[MAX_NUM_PAR];      //[MAX_NUM_PAR] values of acquisition parameters

   KVRList fDetectors;          //references to detectors through which particle passed

   TRef fSimPart;               //simulated particle corresponding to this detected particle

   TRef fIDTelescope;           //identification telescope which identified this particle

   enum {
      kIsIdentified = BIT(18),  //flag set when identification of particle is complete
      kIsCalibrated = BIT(19),   //flag set when energy calibration of particle is complete
      kCoherency = BIT(20)   //particle created and/or identified by analysis of energy losses of other particles
   };

   Int_t fNSegDet;              //!number of segmented/independent detectors hit by particle
   Int_t fAnalStatus;           //!status of particle after analysis of reconstructed event
   Float_t fRealZ;              //Z returned by identification routine
   Float_t fRealA;              //A returned by identification routine
   Double_t fTargetEnergyLoss;   //calculated energy lost in target

 public:

    KVReconstructedNucleus();
    KVReconstructedNucleus(const KVReconstructedNucleus &);
   void init();
    virtual ~ KVReconstructedNucleus();
   virtual void Print(Option_t * option = "") const;
   virtual void Clear(Option_t * option = "");
   virtual void Reconstruct(KVDetector * kvd);
   inline const KVRList *GetDetectorList() const;
   inline KVDetector *GetDetector(int i) const;
   inline void SetDetector(int i, KVDetector *);
   inline KVDetector *GetStoppingDetector() const;

   inline Int_t GetNSegDet() const;
   inline void SetNSegDet(Int_t seg) {
      fNSegDet = seg;
   };
   inline Int_t GetStatus() const ;
   inline void SetStatus(Int_t a) {
      fAnalStatus = a;
   }

   virtual void GetAnglesFromTelescope(Option_t * opt = "random");
   inline KVGroup *GetGroup() const;
   inline KVTelescope *GetTelescope() const;

   inline void SetNumDet(UChar_t num) {
      if (num > MAX_NUM_DET) {
         Warning("SetNumDet(num)", "num is greater than max allowed (%d)",
                 MAX_NUM_DET);
         num = MAX_NUM_DET;
      }
      fNumDet = num;
   };
   inline UChar_t GetNumDet() const {
      return fNumDet;
   };
   inline void SetNumPar(UChar_t num) {
      if (num > MAX_NUM_PAR) {
         Warning("SetNumPar(num)", "num is greater than max allowed (%d)",
                 MAX_NUM_PAR);
         num = MAX_NUM_PAR;
      }
      fNumPar = num;
   };
   inline UChar_t GetNumPar() const {
      return fNumPar;
   };
   inline void SetACQData(const UShort_t * acqtab) {
      for (UChar_t i = 0; i < fNumPar; i++) {
         fACQData[i] = acqtab[i];
      }
   };
   inline const UShort_t *GetACQData() const {
      return fACQData;
   }
   void AddDetector(KVDetector *);
   inline void SetElossTable(const Float_t * etab) {
      for (UChar_t i = 0; i < fNumDet; i++) {
         fEloss[i] = etab[i];
      }
   }
   inline const Float_t *GetElossTable() const {
      return fEloss;
   }
   inline void SetElossCalcTable(const Float_t * etab) {
      for (UChar_t i = 0; i < fNumDet; i++) {
         fElossCalc[i] = etab[i];
      }
   }
   inline const Float_t *GetElossCalcTable() const {
      return fElossCalc;
   }
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif

   inline void SetSimPart(KVNucleus * part);
   inline KVNucleus *GetSimPart() const;

   inline KVList *GetIDTelescopes() const;
   virtual void Identify();
   virtual void Calibrate();

   inline KVIDTelescope *GetIdentifyingTelescope() const;
   inline void SetIdentifyingTelescope(KVIDTelescope *);

   void SetElossCalc(KVDetector *, Float_t);
   Float_t GetElossCalc(KVDetector *) const;

   inline virtual void SetIDCode(UShort_t ) {
   };
   inline virtual void SetECode(UChar_t ) {
   };

   inline void SetIsIdentified();
   inline void SetIsCalibrated() {
      SetBit(kIsCalibrated);
   };
   inline void SetIsUnidentified();
   inline void SetIsUncalibrated() {
      ResetBit(kIsCalibrated);
   };
   inline Bool_t IsIdentified() const {
      return TestBit(kIsIdentified);
   };
   inline Bool_t IsCalibrated() const {
      return TestBit(kIsCalibrated);
   };

   Int_t GetIDSubCode(const Char_t * id_tel_type,
                      KVIDSubCode & code) const;
   const Char_t *GetIDSubCodeString(const Char_t * id_tel_type,
                                    KVIDSubCode & code) const;

   void SetRealZ(Float_t Z) {
      fRealZ = Z;
   }
   void SetRealA(Float_t A) {
      fRealA = A;
   }
   Float_t GetRealZ() const {
      if (fRealZ > 0)
         return fRealZ;
      else
         return (Float_t) GetZ();
   }
   Float_t GetRealA() const {
      if (fRealA > 0)
         return fRealA;
      else
         return (Float_t) GetA();
   }
   inline Float_t GetPID() const;
   virtual void SetTargetEnergyLoss(Double_t e){
      // Set energy loss in target (in MeV) of reconstructed nucleus
      fTargetEnergyLoss=e;
   };
   virtual Double_t GetTargetEnergyLoss() const{
      // Return calculated energy loss in target of reconstructed nucleus (in MeV)
      return fTargetEnergyLoss;
   };

   ClassDef(KVReconstructedNucleus, 10)  //Nucleus detected by multidetector array
};

inline Int_t KVReconstructedNucleus::GetNSegDet() const
{
   //used for particle reconstruction algorithm
   return fNSegDet;
}

//_________________________________________________________________________________

inline KVDetector *KVReconstructedNucleus::GetDetector(int i) const
{
   //Returns the detector referenced by fDetectors.At(i)
   //If i=0, this is the detector in which the particle stopped.
   //For i>0 one obtains the names of the detectors through which the particle
   //must have passed before stopping, in inverse order (i.e. i=0 is the last
   //detector, as i increases we get the detectors closer to the target).

   if (i < 0 || i >= MAX_NUM_DET) {
      Warning("GetDetector(int i)",
              "Value of i must be between 0 and %d. Value given = %d",
              (MAX_NUM_DET - 1), i);
      return 0;
   }
   if (!fDetectors.GetSize()) { // If the list contains no detector then return a null pointer
      return 0;
   }
   return (KVDetector *) fDetectors.At(i);
}

//_________________________________________________________________________________

inline void KVReconstructedNucleus::SetDetector(int i, KVDetector * det)
{
   //Stores a reference to the detector "det" in fDetectors[i].

   if (i < 0 || i >= MAX_NUM_DET) {
      Warning("SetDetector(int i, const KVDetector* det)",
              "Value of i must be between 0 and %d. Value given = %d",
              (MAX_NUM_DET - 1), i);
      return;
   }
   if (!det) {
      Warning("SetDetector(int i, const KVDetector* det)",
              "Pointer *det is null");
      return;
   }
   fDetectors.AddAt(det, i);    //this will not expand the array if fNumDet is bigger than MAX_NUM_DET
}

//_________________________________________________________________________________

inline KVNucleus *KVReconstructedNucleus::GetSimPart() const
{
   //For filtered simulations, return a pointer to the particle in
   //the simulation corresponding to this detected nucleus
   return (KVNucleus *) fSimPart.GetObject();
}

//_________________________________________________________________________________

inline void KVReconstructedNucleus::SetSimPart(KVNucleus * part)
{
   //For filtered simulations, set a pointer to the particle in
   //the simulation corresponding to this detected nucleus
   fSimPart = part;
}

//_________________________________________________________________________________

inline KVDetector *KVReconstructedNucleus::GetStoppingDetector() const
{
   //Returns pointer to the detector in which this particle stopped.

   return GetDetector(0);
}

//_________________________________________________________________________________

inline const KVRList *KVReconstructedNucleus::GetDetectorList() const
{
   //Return reference to list of references/pointers to the detectors through which
   //the reconstructed particle has passed. The first detector in the list is the
   //one in which the particle stopped.

   return &fDetectors;
}

//____________________________________________________________________________//

inline KVGroup *KVReconstructedNucleus::GetGroup() const
{
   //Return pointer to group in which the particle is detected
   if (GetStoppingDetector())
      return GetStoppingDetector()->GetGroup();
   return 0;
}

//____________________________________________________________________________//

inline KVTelescope *KVReconstructedNucleus::GetTelescope() const
{
   //Return pointer to telescope in which the particle is detected
   if (GetStoppingDetector())
      return GetStoppingDetector()->GetTelescope();
   return 0;
}

//__________________________________________________________________________________________________

inline KVList *KVReconstructedNucleus::GetIDTelescopes() const
{
   //Gets from detector in which particle stopped the list of all ID telescopes
   //made from the stopping detector and all those aligned in front of it.
   //The first ID telescope in the list is that in which the particle stopped.

   return GetStoppingDetector()->GetAlignedIDTelescopes();
}

//______________________________________________________________________________________________//

inline void KVReconstructedNucleus::SetIsIdentified()
{
   //When the "identification" state of the particle is set, we add 1 identified particle and
   //subtract 1 unidentified particle from each detector in its list
   SetBit(kIsIdentified);
   fDetectors.R__FOR_EACH(KVDetector, IncrementIdentifiedParticles) (1);
   fDetectors.R__FOR_EACH(KVDetector, IncrementUnidentifiedParticles) (-1);
}

//______________________________________________________________________________________________//

inline void KVReconstructedNucleus::SetIsUnidentified()
{
   //When the "identification" state of the particle is reset, i.e. it becomes an "unidentified particle",
   //we add 1 unidentified particle and subtract 1 identified particle from each detector in its list
   ResetBit(kIsIdentified);
   fDetectors.R__FOR_EACH(KVDetector, IncrementIdentifiedParticles) (-1);
   fDetectors.R__FOR_EACH(KVDetector, IncrementUnidentifiedParticles) (1);
}

//______________________________________________________________________________________________//

inline Float_t KVReconstructedNucleus::GetPID() const
{
   //Return particle identification PID for this particle.
   //If "real A" has been set by identification routine, this is "real Z" + 0.2*("real A"-2*"real Z")
   //If only Z identification has been performed, it is the "real Z"
   if (fRealA > 0)
      return (GetRealZ() + 0.2 * (GetRealA() - 2. * GetRealZ()));
   return GetRealZ();
}

//______________________________________________________________________________________________//

#ifndef KVIDTELESCOPE_H
#include "KVIDTelescope.h"
#endif

inline void KVReconstructedNucleus::SetIdentifyingTelescope(KVIDTelescope *
                                                            idt)
{
   //Set reference to IDTelescope which gave successful identification of this nucleus
   fIDTelescope = idt;
}

//______________________________________________________________________________________________//

inline KVIDTelescope *KVReconstructedNucleus::GetIdentifyingTelescope() const
{
   //Returns pointer to IDTelescope which gave successful identification of this nucleus
   return (KVIDTelescope *) fIDTelescope.GetObject();
}

//______________________________________________________________________________________________//

inline Int_t KVReconstructedNucleus::GetStatus() const
{
   // Returns status of reconstructed particle as decided by analysis of the group (KVGroup) in
   // which the particle is reconstructed (see KVGroup::AnalyseParticles).
   // This status is used to decide whether identification of the particle can be attempted
   // straight away or if we need to wait until other particles in the same group have been
   // identified and calibrated (case of >1 particle crossing shared detector in a group).
   //
   //  Status = 0 :   identification is, in principle at least, possible straight away
   //  Status = 1 :   identification is, in principle, possible after identification of other particles
   //                         in the same group which have Status=0
   //  Status = 2 :   the energy loss in the shared detector of the group must be shared
   //                         (arbitrarily) between this and the other particle(s) with Status=2
   //  Status = 3 :   the particle has stopped in the first member of an identification
   //                         telescope; a minimum Z could be estimated from the measured energy loss.
   //                         (see KVDetector::FindZmin)
   return fAnalStatus;
}
#endif
