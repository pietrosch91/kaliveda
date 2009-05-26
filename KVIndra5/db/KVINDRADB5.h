/***************************************************************************
$Id: KVINDRADB5.h,v 1.20 2007/11/13 07:52:56 franklan Exp $
                          kvdatabase5.h  -  description
                             -------------------
    begin                : mer fév 19 2003
    copyright            : (C) 2003 by Alexis Mignon
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
#ifndef KV_DATA_BASE_5_H
#define KV_DATA_BASE_5_H

#include "KVINDRADB.h"
#include "KVDBRun.h"
#include "KVDBSystem.h"
#include "KVDBTape.h"

class KVINDRADB5:public KVINDRADB {

   UInt_t kDuplicatedRun;       //number of run which occurs twice = 8449
   //General information
   KVDBTable *fPedestals;       //-> table of pedestal files

   //Calibration parameters
   KVDBTable *fChanVolt;        //-> ChIo/Si channel-volt calibration parameters
   KVDBTable *fVoltMeVChIoSi;   //-> ChIo/Si volt-energy calibration
   KVDBTable *fLitEnerCsI;      //->CsI light-energy calibration for Z>1
   KVDBTable *fLitEnerCsIZ1;      //->CsI light-energy calibration for Z=1

   void init();

 public:
    KVINDRADB5();
    KVINDRADB5(const Char_t * name);
    virtual ~ KVINDRADB5();

   virtual void Build();
   virtual void LinkRecordToRunRange(KVDBRecord * rec, UInt_t first_run,
                                     UInt_t last_run);
   virtual void LinkListToRunRanges(TList * list, UInt_t rr_number,
                                    UInt_t run_ranges[][2]);
   virtual void GoodRunLine();

   virtual void ReadChannelVolt();
   virtual void ReadVoltEnergyChIoSi();
   virtual void ReadLightEnergyCsI(const Char_t*,KVDBTable*);
   virtual void ReadCalibCsI();
   virtual void ReadPedestalList();

   virtual KVDBTape *GetDLT(UInt_t tape) const {
      return GetTape(tape);
   };
   virtual TList *GetDLTs() const {
      return GetTapes();
   };

    ClassDef(KVINDRADB5, 9)     // DataBase of parameters for 5th campaign of Indra
};

#endif
