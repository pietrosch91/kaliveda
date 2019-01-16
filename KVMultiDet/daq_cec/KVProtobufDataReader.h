//Created by KVClassFactory on Mon Jan 14 12:10:22 2019
//Author: John Frankland,,,

#ifndef __KVProtobufDataReader_H
#define __KVProtobufDataReader_H

#include "KVRawDataReader.h"

class KVProtobufDataReader : public KVRawDataReader {
protected:
   Int_t fBufSize;//! buffer size
   char* fBuffer;//! current buffer
   unique_ptr<TFile> fFile;//! TFile plugin handle
   Long64_t fFileSize;//! size of file in bytes
   UInt_t fEvSize;//! size of next event in buffer
   ptrdiff_t fEvOffset;//! next position to read in buffer
   ptrdiff_t fFillOffset;//! next position in buffer to fill from file
   bool fNeedToReadBuffer;//! true when the buffer is empty/incomplete
   bool fReachedEndOfFile;//! true when we have read all bytes from file

   bool read_next_event();
   bool read_buffer();

   UInt_t get_remaining_readable_buffer() const
   {
      // number of bytes remaining to read in buffer starting from current
      // position fEvOffset
      return (UInt_t)(fBufSize - fEvOffset);
   }

   virtual bool parse_event_from_message() = 0;

public:
   KVProtobufDataReader(const Char_t* filepath, Int_t bufSiz = 16 * 1024 * 1024);
   virtual ~KVProtobufDataReader();

   Bool_t GetNextEvent();
   KVSeqCollection* GetFiredDataParameters() const;
   TString GetDataFormat() const
   {
      return "PROTOBUF";
   }
   Int_t GetRunNumberReadFromFile() const;

   ClassDef(KVProtobufDataReader, 1) //Read Google Protobuf DAQ files
};

#endif
