//Created by KVClassFactory on Fri May  4 15:26:02 2007
//Author: franklan

#ifndef __KVINDRARAWDATAANALYSER_H
#define __KVINDRARAWDATAANALYSER_H

#include "KVRawDataAnalyser.h"
#include "KVINDRATriggerInfo.h"
#include "KVINDRARawDataReader.h"
#include "KVDetectorEvent.h"
#include "KVINDRAReconEvent.h"
#include "KVDataSet.h"

class KVINDRARawDataAnalyser : public KVRawDataAnalyser
{
   friend class KVDataSet;
   
   protected:
         
   KVINDRATriggerInfo* fTrig;//INDRA trigger for current event
   virtual KVNumberList PrintAvailableRuns(KVString & datatype);
   
   virtual void ProcessRun();
   
   public:

   KVINDRARawDataAnalyser();
   virtual ~KVINDRARawDataAnalyser();
   
   Bool_t IsINDRAEvent() const {
		return ((KVINDRARawDataReader*)fRunFile)->IsINDRAEvent();
	};
   const KVINDRATriggerInfo* GetINDRATrigger() const { return fTrig; };
   const KVDetectorEvent* GetINDRADetectorEvent() const { return fDetEv; };
   
   virtual void SubmitTask();
   static void Make(const Char_t * kvsname = "MyOwnRawDataAnalyser");   
   
   ClassDef(KVINDRARawDataAnalyser,1)//Analysis of raw INDRA data
};

#endif
