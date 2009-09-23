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
#include "KVGANILDataReader.h"

class KVINDRARawDataReader:public KVGANILDataReader, public KVINDRATriggerInfo {

protected:   
   KVACQParam *fSTAT_EVE_PAR;   // STAT_EVE parameter read from raw data
   KVACQParam *fR_DEC_PAR;      // R_DEC parameter read from raw data
   KVACQParam *fVXCONFIG_PAR;   // VXCONFIG parameter read from raw data

   void ConnectArrayDataParameters();

 public:
    KVINDRARawDataReader() {
      init();
   };
   KVINDRARawDataReader(const Char_t *);
   virtual ~ KVINDRARawDataReader();
   void init();
   Bool_t GetNextEvent();
   Bool_t IsINDRAEvent();
   
   static KVINDRARawDataReader* Open(const Char_t* filename, Option_t* opt = "");
   
   ClassDef(KVINDRARawDataReader, 0)      //Class handling reading of raw INDRA data from GANIL acquisition files
};

#endif
