/*
$Id: KVIVRawDataReader.h,v 1.3 2007/06/26 13:26:25 franklan Exp $
$Revision: 1.3 $
$Date: 2007/06/26 13:26:25 $
*/

//Created by KVClassFactory on Fri Jun  1 12:16:31 2007
//Author: e_ivamos

#ifndef __KVIVRAWDATAREADER_H
#define __KVIVRAWDATAREADER_H

#include "KVINDRARawDataReader.h"

class KVIVRawDataReader : public KVINDRARawDataReader
{
   protected:
   virtual GTGanilData* NewGanTapeInterface();
   virtual KVACQParam* CheckACQParam(const Char_t*);
   
   public:

   KVIVRawDataReader();
   KVIVRawDataReader(const Char_t *);
   virtual ~KVIVRawDataReader();

   static KVIVRawDataReader* Open(const Char_t* filename, Option_t* opt = "");
   
   ClassDef(KVIVRawDataReader,0)//Reads raw data from INDRA-VAMOS experiments
};

#endif
