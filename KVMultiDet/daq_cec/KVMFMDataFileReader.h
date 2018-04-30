//Created by KVClassFactory on Fri Apr  6 16:33:15 2018
//Author: John Frankland,,,

#ifndef __KVMFMDATAFILEREADER_H
#define __KVMFMDATAFILEREADER_H

#include "KVRawDataReader.h"
#include "MFMFileReader.h"
#include "MFMMergeFrameManager.h"
#include <KVHashList.h>

class KVMFMDataFileReader : public KVRawDataReader, public MFMFileReader {

   KVHashList fEBYEDATfired;//! list of fired parameters in EBYEDAT frames
   MFMMergeFrameManager fMerge;//! for treating merged frames

   void TreatFrame(MFMCommonFrame&);
   void TreatEbyedatFrame(MFMCommonFrame&) {}
   void TreatFaziaFrame(MFMCommonFrame&) {}

public:
   KVMFMDataFileReader(const Char_t* filepath)
      : KVRawDataReader(), MFMFileReader(filepath)
   {}
   virtual ~KVMFMDataFileReader() {}
   Bool_t GetNextEvent();
   KVSeqCollection* GetFiredDataParameters() const;

   static KVMFMDataFileReader* Open(const Char_t* filepath, Option_t* = "");

   ClassDef(KVMFMDataFileReader, 0) //Read MFM format acquisition data
};

#endif
