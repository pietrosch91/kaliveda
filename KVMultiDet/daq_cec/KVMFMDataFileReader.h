//Created by KVClassFactory on Fri Apr  6 16:33:15 2018
//Author: John Frankland,,,

#ifndef __KVMFMDATAFILEREADER_H
#define __KVMFMDATAFILEREADER_H

#include "KVRawDataReader.h"
#include "MFMFileReader.h"
#include "KVNameValueList.h"

class KVMFMDataFileReader : public KVRawDataReader, public MFMFileReader {

   KVNameValueList fRunInfos;//! informations on run extracted from XML header frame

public:
   KVMFMDataFileReader(const Char_t* filepath);
   virtual ~KVMFMDataFileReader() {}

   Bool_t GetNextEvent()
   {
      return ReadNextFrame();
   }
   KVSeqCollection* GetFiredDataParameters() const
   {
      return nullptr;
   }

   const KVNameValueList& GetRunInfos() const
   {
      return fRunInfos;
   }

   static KVMFMDataFileReader* Open(const Char_t* filepath, Option_t* = "");

   ClassDef(KVMFMDataFileReader, 0) //Read MFM format acquisition data
};

#endif
