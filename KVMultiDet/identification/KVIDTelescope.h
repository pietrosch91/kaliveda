/***************************************************************************
$Id: KVIDTelescope.h,v 1.33 2009/04/01 15:58:10 ebonnet Exp $
                          KVIDTelescope.h  -  description
                             -------------------
    begin                : Wed Jun 18 2003
    copyright            : (C) 2003 by J.D Frankland
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

#ifndef KVIDTELESCOPE_H
#define KVIDTELESCOPE_H

#include "KVBase.h"
#include "KVDetector.h"
#include "KVRList.h"
#include "TGraph.h"
#include "KVIDSubCodeManager.h"

class KVReconstructedNucleus;
class KVGroup;
class KVIDGraph;
class KVIDGrid;
class KVMultiDetArray;

class KVIDTelescope:public KVBase, public KVIDSubCodeManager {

 protected:
   KVList * fDetectors;         //->list of detectors in telescope
   KVGroup *fGroup;             //group to which telescope belongs
   UShort_t fIDCode;            //!code corresponding to correct identification by this type of telescope
   UShort_t fZminCode;          //!code corresponding to particle stopping in first member of this type of telescope
   UChar_t fECode;              //!code corresponding to correct calibration by this type of telescope
   KVList *fIDGrids;           //->identification grid(s)
   enum {
      kMassID = BIT(15),         //set if telescope is capable of mass identification i.e. isotopic resolution
      kReadyForID = BIT(16)         //set if telescope is ready and able for identification. set in Initialize()
   };
   Int_t fCalibStatus;//!temporary variable holding status code for last call to Calibrate(KVReconstructedNucleus*)
   
   void SetLabelFromURI(const Char_t* uri);

 public:
      
       // status of particle calibration after Calibrate(KVReconstructedNucleus*) is called 
   enum {
      kCalibStatus_OK,                    // fine, OK, all detectors calculated and functioning properly
      kCalibStatus_Calculated,        // one or more detectors not calibrated/functioning, energy loss calculated
      kCalibStatus_NoCalibrations,  // no calibrations available for any detectors
      kCalibStatus_Multihit,  // one or more detectors hit by more than one particle, energy loss calculated for this detector
      kCalibStatus_Coherency  // comparison of calculated and measured energy loss in one or more detectors reveals presence of other particles
   };

    KVIDTelescope();
    virtual ~ KVIDTelescope();
   void init();
   virtual void AddDetector(KVDetector * d);
   KVList *GetDetectors() const {
      return fDetectors;
   };
   KVDetector *GetDetector(UInt_t n) const {
      //returns the nth detector in the telescope structure
      if (n > GetSize() || n < 1) {
         Error("GetDetector(UInt_t n)", "n must be between 1 and %u",
               GetSize());
         return 0;
      }
      KVDetector *d = (KVDetector *) fDetectors->At((n - 1));
       return d;
   };
   KVDetector *GetDetector(const Char_t * name) const;
   UInt_t GetDetectorRank(KVDetector * kvd);
   UInt_t GetSize() const {
      //returns number of detectors in telescope
      return (fDetectors ? fDetectors->GetSize() : 0);
   };

   KVGroup *GetGroup() const;
   void SetGroup(KVGroup * kvg);
   UInt_t GetGroupNumber();

   const Char_t *GetName() const;
   virtual const Char_t *GetArrayName();

   virtual TGraph *MakeIDLine(KVNucleus * nuc, Float_t Emin, Float_t Emax,
                              Float_t Estep = 0.0);

   // Default initialisation for ID telescopes. kReadyForID is set to kFALSE.
   // To implement identification, make a class derived from KVIDTelescope
   // and set kReadyForID to kTRUE in Initialize() method of derived class.
   virtual void Initialize(void) {
      ResetBit(kReadyForID);
   };
   
   virtual Bool_t Identify(KVReconstructedNucleus *);
   
   virtual void CalculateParticleEnergy(KVReconstructedNucleus * nuc);
   // When called just after CalculateParticleEnergy(KVReconstructedNucleus*)
   // this method returns a status code which is one of
   //   KVIDTelescope::kCalibStatus_OK,            : fine, OK, all detectors calculated and functioning properly
   //   KVIDTelescope::kCalibStatus_Calculated,    : one or more detectors not calibrated/functioning, energy loss calculated
   //   KVIDTelescope::kCalibStatus_NoCalibrations : no calibrations available for any detectors
   virtual Int_t GetCalibStatus() const
   {
      return fCalibStatus;
   };

   virtual void Print(Option_t * opt = "") const;

   UShort_t GetIDCode() {
      return fIDCode;
   };
   UShort_t GetZminCode() {
      return fZminCode;
   };
   UChar_t GetECode() {
      return fECode;
   };

   virtual Bool_t SetIDGrid(KVIDGraph *);
   virtual KVIDGraph *GetIDGrid();
   virtual KVIDGraph *GetIDGrid(Int_t);
   virtual KVIDGraph *GetIDGrid(const Char_t*);
   virtual KVList* GetListOfIDGrids() const { return fIDGrids; };

   virtual void RemoveGrids();

   virtual Double_t GetIDMapX(Option_t * opt = "");
   virtual Double_t GetIDMapY(Option_t * opt = "");

   void SetHasMassID(Bool_t yes=kTRUE) {
      SetBit(kMassID,yes);
   };
   
   // Returns kTRUE if this telescope is capable of identifying particles' mass (A)
   // as well as their charge (Z).
   Bool_t HasMassID() const {
      return TestBit(kMassID);
   };

   inline void SetSubCodeManager(UChar_t n_bits,
                                 UChar_t most_significant_bit) {
      SetNbits(n_bits);
      SetMSB(most_significant_bit);
   };

   static KVIDTelescope *MakeIDTelescope(const Char_t * name);
   
   virtual Bool_t SetIdentificationParameters(const KVMultiDetArray*);
   virtual void RemoveIdentificationParameters();
   
   // Returns kTRUE if telescope has been correctly initialised for identification.
   // Test after Initialize() method has been called.
   virtual Bool_t IsReadyForID()
   {
      return TestBit(kReadyForID);
   };

	const Char_t* GetDefaultIDGridClass();
	KVIDGrid* CalculateDeltaE_EGrid(const Char_t* Zrange,Int_t deltaMasse,Int_t npoints);
   
	ClassDef(KVIDTelescope, 4)   //A delta-E - E identification telescope
};

#endif
