/***************************************************************************
$Id: KVSilicon.h,v 1.34 2008/02/21 10:14:38 franklan Exp $
                          kvsilicon.h  -  description
                             -------------------
    begin                : Fri Oct 4 2002
    copyright            : (C) 2002 by A. Mignon & J.D. Frankland
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

#ifndef KVSILICON_H
#define KVSILICON_H

#include "KVINDRADetector.h"
#include "TF1.h"
#include "KVACQParam.h"
#include "KVChannelVolt.h"
#include "KVPulseHeightDefect.h"
#include "KVVoltEnergy.h"

class KVChIo;
class KVDBParameterSet;

Double_t ELossSiPHD(Double_t * x, Double_t * par);

class KVSilicon:public KVINDRADetector {

 protected:

   Float_t fPGtoGG_0;           //conversion factor = offset
   Float_t fPGtoGG_1;           //conversion factor = slope

   KVChIo* fChIo;//!pointer to INDRA ionisation chamber associated to this detector
   KVChIo* FindChIo();

   KVChannelVolt* fChVoltGG;//!channel-volt conversion (GG)
   KVChannelVolt* fChVoltPG;//!channel-volt conversion (PG)
   KVVoltEnergy* fVoltE;//!volt-energy conversion
   KVPulseHeightDefect* fPHD;//!pulse height defect
   Int_t fZminPHD;//PHD correction applied to energy for Z>fZminPHD

   void init();

 public:

    KVSilicon();
    KVSilicon(Float_t thick);
    virtual ~ KVSilicon();

   Double_t GetVoltsFromCanalPG(Double_t chan = 0.0);
   Double_t GetVoltsFromCanalGG(Double_t chan = 0.0);
   Double_t GetCalibratedEnergy();
   Double_t GetVolts();
   Double_t GetVoltsFromEnergy(Double_t);

   Int_t GetCanalPGFromVolts(Float_t volts);
   Int_t GetCanalGGFromVolts(Float_t volts);

   Float_t GetPG() {
      return GetACQData("PG");
   }
   Float_t GetGG() {
      return GetACQData("GG");
   }
   UShort_t GetMT() {
      return GetACQParam("T")->GetCoderData();
   }

   virtual Double_t GetEnergy();

   virtual KVChIo *GetChIo() const;

   void SetACQParams();
   void SetCalibrators();

   Float_t GetGGfromPG(Float_t PG = -1);
   Float_t GetPGfromGG(Float_t GG = -1);

   void SetPGtoGG(Float_t alph, Float_t beta) {
      fPGtoGG_0 = alph;
      fPGtoGG_1 = beta;
   };

   Double_t GetPHD(Double_t Einc, UInt_t Z);
   Double_t GetCorrectedEnergy(UInt_t z, UInt_t a, Double_t e = -1., Bool_t transmission=kTRUE);
   virtual void SetELossParams(Int_t Z, Int_t A);

   inline Bool_t IsCalibrated() const;

   virtual void SetMoultonPHDParameters(Double_t a1, Double_t a2, Double_t b1, Double_t b2);

   void SetZminPHD(Int_t zmin) { fZminPHD = zmin; };
   Int_t GetZminPHD() { return fZminPHD; };
   virtual Short_t GetCalcACQParam(KVACQParam*) const;

   ClassDef(KVSilicon, 6)       //INDRA Silicon detectors with associated electronics and power supply
};

//____________________________________________________________________________________________

inline Bool_t KVSilicon::IsCalibrated() const
{
   //Redefined.
   //Only one of the channel-volts calibrators needs to be ready,
   //plus the volts-energy calibrator

      if (fVoltE && fVoltE->GetStatus()) {
         Bool_t ok_gg = (fChVoltGG!=0) ? fChVoltGG->GetStatus() : 0;
         Bool_t ok_pg = (fChVoltPG!=0) ? fChVoltPG->GetStatus() : 0;
         return (ok_gg || ok_pg);
      }

   return kFALSE;
}


class KVSi75:public KVSilicon {
 public:

   KVSi75();
   KVSi75(Float_t thick);
   virtual ~ KVSi75();
   const Char_t *GetArrayName();

    ClassDef(KVSi75, 1)         //80um silicon detector for etalon telescopes
};

class KVSiLi:public KVSilicon {
 public:

   KVSiLi();
   KVSiLi(Float_t thick);
   virtual ~ KVSiLi();
   const Char_t *GetArrayName();

    ClassDef(KVSiLi, 1)         //2mm Si(Li) detector for etalon telescopes
};
#endif
