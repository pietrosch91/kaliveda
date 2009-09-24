//Created by KVClassFactory on Thu Sep 24 11:07:45 2009
//Author: John Frankland,,,

#ifndef __KVRAWDATAANALYSER_H
#define __KVRAWDATAANALYSER_H

#include "KVDataAnalyser.h"
#include "KVDetectorEvent.h"
#include "KVRawDataReader.h"

class KVRawDataAnalyser : public KVDataAnalyser
{
	protected:
			
   KVRawDataReader* fRunFile; 	//currently analysed run file
   Int_t fRunNumber; 				//run number of current file
   Long64_t fEventNumber;			//event number in current run
   KVDetectorEvent* fDetEv;		//list of hit groups for current event
   
   virtual void ProcessRun();
   
   public:
   KVRawDataAnalyser();
   virtual ~KVRawDataAnalyser();

   virtual void InitAnalysis()=0;
   virtual void InitRun()=0;
   virtual Bool_t Analysis()=0;
   virtual void EndRun()=0;
   virtual void EndAnalysis()=0;

   const KVDetectorEvent* GetDetectorEvent() const { return fDetEv; };
   
   virtual Bool_t FileHasUnknownParameters() const { return (fRunFile->GetUnknownParameters()->GetSize()>0); };
   virtual void SubmitTask();
   static void Make(const Char_t * kvsname = "MyOwnRawDataAnalyser");
   
   ClassDef(KVRawDataAnalyser,1)//Abstract base class for user analysis of raw data
};

#endif
