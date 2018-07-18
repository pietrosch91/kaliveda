//Created by KVClassFactory on Fri Apr  6 16:33:15 2018
//Author: John Frankland,,,

#ifndef __KVMFMDATAFILEREADER_H
#define __KVMFMDATAFILEREADER_H

#include "KVRawDataReader.h"
#include "MFMFileReader.h"
#include "MFMMergeFrameManager.h"
#include "KVNameValueList.h"

class KVMFMDataFileReader : public KVRawDataReader, public MFMFileReader {

   KVNameValueList fRunInfos;//! informations on run extracted from XML header frame
   MFMMergeFrameManager fMergeFrame;//! used to handle merged MFM frames

public:
   KVMFMDataFileReader(const Char_t* filepath);
   virtual ~KVMFMDataFileReader() {}

   /// Read next frame in file. Initialise merge manager if frame is a merge frame
   Bool_t GetNextEvent()
   {
      bool ok = ReadNextFrame();
      if (ok) {
         if (IsFrameReadMerge()) fMergeFrame.SetMergeFrame(GetFrameRead());
      }
      return ok;
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

   TString GetDataFormat() const
   {
      return "MFM";
   }
   const MFMMergeFrameManager& GetMergeManager() const
   {
      return fMergeFrame;
   }
   void SetActionsDirectory(const string&);
   Int_t GetRunNumberReadFromFile() const;

   ClassDef(KVMFMDataFileReader, 0) //Read MFM format acquisition data
};

#endif
