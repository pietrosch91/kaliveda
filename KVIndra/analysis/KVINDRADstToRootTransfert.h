/*
$Id: KVINDRADstToRootTransfert.h,v 1.3 2007/05/31 09:59:22 franklan Exp $
$Revision: 1.3 $
$Date: 2007/05/31 09:59:22 $
*/

//Created by KVClassFactory on Fri May  4 15:26:02 2007
//Author: franklan

#ifndef __KVINDRADSTTOROOTTRANSFERT_H
#define __KVINDRADSTTOROOTTRANSFERT_H

#include "KVINDRADataAnalyser.h"
#include "KVINDRATriggerInfo.h"
#include "KVINDRARawDataReader.h"
#include "KVDetectorEvent.h"
#include "KVINDRAReconEvent.h"
#include "KVDataSet.h"

class KVINDRADstToRootTransfert : public KVINDRADataAnalyser
{
   friend class KVDataSet;
   
   protected:
         
   KVINDRARawDataReader* fRunFile;//currently analysed run file
   Int_t fRunNumber;//run number of current file
   Long64_t fEventNumber;//event number in current run
   KVINDRATriggerInfo* fTrig;//INDRA trigger for current event
   KVDetectorEvent* fDetEv;//list of hit groups for current event
   
   virtual void ProcessRun();
   
   public:

   KVINDRADstToRootTransfert();
   virtual ~KVINDRADstToRootTransfert();
   
   virtual void InitAnalysis()=0;
   virtual void InitRun()=0;
   virtual Bool_t Analysis()=0;
   virtual void EndRun()=0;
   virtual void EndAnalysis()=0;
   
   Bool_t IsINDRAEvent() const { return fRunFile->IsINDRAEvent(); };
   const KVINDRATriggerInfo* GetINDRATrigger() const { return fTrig; };
   const KVDetectorEvent* GetINDRADetectorEvent() const { return fDetEv; };
   
   Bool_t FileHasUnknownParameters() const { return (fRunFile->GetUnknownParameters()->GetSize()>0); };
   virtual void SubmitTask();
   static void Make(const Char_t * kvsname = "MyOwnRawDataAnalyser");
   
   
   ClassDef(KVINDRADstToRootTransfert,1)//Analysis of raw INDRA data
};

#endif
