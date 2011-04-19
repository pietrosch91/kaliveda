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
	
	void CheckCsIEnergy();
   
 public:

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
   inline UShort_t GetTimeMarker() const {
      return (GetStoppingDetector()?
              (GetStoppingDetector()->
               GetACQParam("T") ? GetStoppingDetector()->GetACQParam("T")->
               GetCoderData() : 0) : 0);
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
	virtual void CalibrateRings1To10();
   virtual void Calibrate();
   
   Float_t GetEnergyChIo();
   Float_t GetEnergySi();
   Float_t GetEnergyCsI();
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

   ClassDef(KVINDRAReconNuc, 8) //Nucleus identified by INDRA array
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
