//Created by KVClassFactory on Fri Apr  6 16:33:15 2018
//Author: John Frankland,,,

#ifndef __KVMFMDATAFILEREADER_H
#define __KVMFMDATAFILEREADER_H

#include "KVRawDataReader.h"
#include "MFMFileReader.h"

class KVMFMDataFileReader : public KVRawDataReader {
   MFMFileReader reader;//!

public:
   KVMFMDataFileReader(const Char_t* filepath)
      : reader(filepath)
   {}
   virtual ~KVMFMDataFileReader() {}
   Bool_t GetNextEvent();
   KVSeqCollection* GetFiredDataParameters() const;

   static KVMFMDataFileReader* Open(const Char_t* filepath, Option_t* = "");

   ClassDef(KVMFMDataFileReader, 0) //Read MFM format acquisition data
};

#endif
