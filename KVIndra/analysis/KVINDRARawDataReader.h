/***************************************************************************
                          KVINDRARawDataReader.h  -  description
                             -------------------
    begin                : Thu Nov 21 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr
$Id: KVINDRARawDataReader.h,v 1.5 2008/10/17 10:58:07 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVINDRARAWDATAREADER_H
#define KVINDRARAWDATAREADER_H

#include "KVBase.h"
#include "KVACQParam.h"
#include "KVINDRATriggerInfo.h"
#include "KVList.h"
#include "KVRawDataReader.h"

class GTGanilData;

class KVINDRARawDataReader:public KVRawDataReader, public KVINDRATriggerInfo {

protected:
   GTGanilData* fGanilData;//object used to read GANIL acquisition file
   
   KVACQParam *fSTAT_EVE_PAR;   // STAT_EVE parameter read from raw data
   KVACQParam *fR_DEC_PAR;      // R_DEC parameter read from raw data
   KVACQParam *fVXCONFIG_PAR;   // VXCONFIG parameter read from raw data

   void ConnectRawDataParameters();
   void ConnectArrayDataParameters();

   KVList *fExtParams;//->list of unknown (i.e. non-INDRA) data parameters
	KVList *fParameters;//->list of all data parameters contained in file
   
   virtual GTGanilData* NewGanTapeInterface();
   virtual KVACQParam* CheckACQParam(const Char_t*);
   
 public:
    KVINDRARawDataReader() {
      init();
   };
   KVINDRARawDataReader(const Char_t *);
   void OpenFile(const Char_t *);
   virtual ~ KVINDRARawDataReader();
   void init();
   Bool_t GetNextEvent();
   Bool_t IsINDRAEvent();
   virtual GTGanilData* GetGanTapeInterface();
   
   const KVList* GetUnknownParameters() const { return fExtParams; };
   const KVList* GetRawDataParameters() const { return fParameters; };
   
   static KVINDRARawDataReader* Open(const Char_t* filename, Option_t* opt = "");
   
   ClassDef(KVINDRARawDataReader, 0)      //Class handling reading of raw INDRA data from GANIL acquisition files
};

#endif
