//Created by KVClassFactory on Mon Jul 16 15:20:21 2018
//Author: eindra

#ifndef __KVRAWDATARECONSTRUCTOR_H
#define __KVRAWDATARECONSTRUCTOR_H

#include "KVRawDataAnalyser.h"

class KVRawDataReconstructor : public KVRawDataAnalyser {
public:
   KVRawDataReconstructor();
   virtual ~KVRawDataReconstructor();

   virtual void InitAnalysis();
   virtual void InitRun();
   virtual Bool_t Analysis();
   virtual void EndRun();
   virtual void EndAnalysis();

   ClassDef(KVRawDataReconstructor, 1) //Reconstruct physical events from raw data
};

#endif
