/***************************************************************************
$Id: KVChIo.h,v 1.32 2008/02/21 10:14:38 franklan Exp $
                          kvchio.h  -  description
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

#ifndef KVCHIO_H
#define KVCHIO_H

#include "KVINDRADetector.h"
#include "KVACQParam.h"
#include "KVChannelVolt.h"
#include "KVVoltEnergy.h"

class KVDBParameterSet;

class KVChIo:public KVINDRADetector {

 protected:

   Float_t fPGtoGG_0;           //conversion factor = offset
   Float_t fPGtoGG_1;           //conversion factor = slope
   KVChannelVolt* fChVoltGG;//!channel-volt conversion (GG)
   KVChannelVolt* fChVoltPG;//!channel-volt conversion (PG)
   KVVoltEnergy* fVoltE;//!volt-energy conversion

   void init();

 public:

    KVChIo();
    KVChIo(Float_t pressure, Float_t thick = 50.0);
//    KVChIo(Float_t thick = 50.0);
    virtual ~ KVChIo();

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

   void SetACQParams();
   void SetCalibrators();

   Float_t GetGGfromPG(Float_t PG = -1);

   void SetPGtoGG(Float_t alph, Float_t beta) {
      fPGtoGG_0 = alph;
      fPGtoGG_1 = beta;
   };

   Double_t GetELossMylar(UInt_t z, UInt_t a, Double_t egas = -1.0, Bool_t stopped=kFALSE);
   Double_t GetCorrectedEnergy(UInt_t z, UInt_t a, Double_t egas = -1.0, Bool_t transmission=kTRUE);

   inline Bool_t IsCalibrated() const;
   virtual Short_t GetCalcACQParam(KVACQParam*) const;

   ClassDef(KVChIo, 3)          //The ionisation chamber detectors (ChIo) of the INDRA array
};

//____________________________________________________________________________________________

inline Bool_t KVChIo::IsCalibrated() const
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

#endif
