/***************************************************************************
                          kvdetectedparticle.h  -  description
                             -------------------
    begin                : Thu Oct 10 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVINDRAReconNuc.h,v 1.39 2009/04/03 14:28:37 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVINDRADETPART_H
#define KVINDRADETPART_H


#include "KVReconstructedNucleus.h"
#include "KVDetector.h"
#include "KVTelescope.h"
#include "KVINDRACodes.h"
#include "KVINDRAReconEvent.h"
#include "KVINDRAIDTelescope.h"

class KVChIo;
class KVSilicon;
class KVCsI;

class KVINDRAReconNuc:public KVReconstructedNucleus {

   KVINDRACodes fCodes;         //VEDA6-style calibration and identification codes
   Bool_t fCoherent;//coherency of CsI & Si-CsI identifications
   Bool_t fPileup;//apparent pileup in Si, revealed by inconsistency between CsI & Si-CsI identifications
   Bool_t fUseFullChIoEnergyForCalib;//decided by coherency analysis
	Float_t fECsI;//csi contribution to energy
	Float_t fESi;//si contribution to energy
	Float_t fEChIo;//chio contribution to energy
   /* for PHD corrections */
   Float_t fESi_old;//!silicon energy before PHD correction
   Float_t fEnergy_old;//!total energy before PHD correction, including target losses
	void CheckCsIEnergy();
   
 public:

   Int_t GetIDSubCode(const Char_t * id_tel_type,
                       KVIDSubCode & code) const;
    const Char_t *GetIDSubCodeString(const Char_t * id_tel_type,
                                     KVIDSubCode & code) const;
	Bool_t AreSiCsICoherent() const
	{
		// RINGS 1-9
		// Returns result of coherency test between Si-CsI and CsI-RL identifications.
		// See CoherencySiCsI(KVIdentificationResult&).
		return fCoherent;
	};
	Bool_t IsSiPileup() const
	{
		// RINGS 1-9
		// Returns result of coherency test between Si-CsI and CsI-RL identifications.
		// See CoherencySiCsI(KVIdentificationResult&).
		return fPileup;
	};
	Bool_t UseFullChIoEnergyForCalib() const
	{
		// RINGS 1-9
		// Returns result of coherency test between ChIo-Si, Si-CsI and CsI-RL identifications.
		// See CoherencyChIoSiCsI(KVIdentificationResult).
		return fUseFullChIoEnergyForCalib;
	};
   KVINDRAReconNuc();
   KVINDRAReconNuc(const KVINDRAReconNuc &);
   void init();
    virtual ~ KVINDRAReconNuc();

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif
   virtual void Clear(Option_t * t = "");
   void Print(Option_t * option = "") const;
   inline Short_t GetTimeMarker() const {
		if(!GetStoppingDetector()) return -1;
		KVACQParam* mqrt = GetStoppingDetector()->GetACQParam("T");
		if(!mqrt || !mqrt->IsWorking()) return -1;
      return (Short_t)mqrt->GetCoderData();
   };
   UInt_t GetRingNumber(void) const {
      if (GetTelescope()) {
         return GetTelescope()->GetRingNumber();
		} else {
         return 0;
      }
   };
   UInt_t GetModuleNumber(void) const {
      if (GetTelescope()) {
			return GetTelescope()->GetNumber();		
      } else {
         return 0;
      }
   };

   virtual void Identify();
	virtual Bool_t CoherencySiCsI(KVIdentificationResult& theID);
	virtual Bool_t CoherencyChIoSiCsI(KVIdentificationResult);
	virtual void CalibrateRings1To9();
   virtual void Calibrate();
   
	Float_t GetEnergyChIo()
	{
   	// Return the calculated ChIo contribution to the particle's energy
  		// (including correction for losses in Mylar windows).
   	// This may be negative, in case the ChIo contribution was calculated
   	// because either (1) the ChIo was not calibrated, or (2) coherency check
   	// between ChIo-Si and Si-CsI/CsI-RL identification indicates contribution
   	// of several particles to ChIo energy
   
   	return fEChIo;
	};
	Float_t GetEnergySi()
	{
   	// Return the calculated Si contribution to the particle's energy
   	// (including correction for pulse height defect).
   	// This may be negative, in case the Si contribution was calculated
   	// because either (1) the Si was not calibrated, or (2) coherency check
   	// indicates pileup in Si, or (3) coherency check indicates measured
   	// Si energy is too small for particle identified in CsI-RL

   	return fESi;
	};
	Float_t GetEnergyCsI()
	{
   	// Return the calculated CsI contribution to the particle's energy
		return fECsI;
	};

   KVChIo *GetChIo();
   KVSilicon *GetSi();
   KVCsI *GetCsI();
   Bool_t StoppedInChIo();
   Bool_t StoppedInSi();
   Bool_t StoppedInCsI();

   KVINDRACodes & GetCodes() {
      return fCodes;
   }
   inline virtual void SetIDCode(UShort_t code_mask);
   inline virtual void SetECode(UChar_t code_mask);

   Int_t GetIDSubCode(const Char_t * id_tel_type = "") const;
   const Char_t *GetIDSubCodeString(const Char_t * id_tel_type = "") const;

   Float_t GetOldSiliconEnergy() const
   {
      // If silicon energy is corrected by 'rustine' in Streamer method,
      // this will return the energy before correction of PHD
      return fESi_old;
   }
   Float_t GetOldTotalEnergy() const
   {
      // If silicon energy is corrected by 'rustine' in Streamer method,
      // this will return the total energy (including target losses)
      // before correction of PHD
      return fEnergy_old;
   }
   ClassDef(KVINDRAReconNuc, 11) //Nucleus identified by INDRA array
};

//____________________________________________________________________________________________//

inline void KVINDRAReconNuc::SetIDCode(UShort_t code_mask)
{
   //Sets code for identification
   GetCodes().SetIDCode(code_mask);
}

//____________________________________________________________________________________________//

inline void KVINDRAReconNuc::SetECode(UChar_t code_mask)
{
   //Sets code for energy calibration
   GetCodes().SetECode(code_mask);
}

#endif
