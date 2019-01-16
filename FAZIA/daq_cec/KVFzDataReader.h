//Created by KVClassFactory on Mon Jan 14 12:10:22 2019
//Author: John Frankland,,,

#ifndef __KVFZDATAREADER_H
#define __KVFZDATAREADER_H

#include "KVProtobufDataReader.h"
#include <KVUniqueNameList.h>

#ifndef __CINT__
namespace DAQ {
   class FzEvent;
}
#endif

class KVFzDataReader : public KVProtobufDataReader {

   unique_ptr<KVUniqueNameList> fListOfFiles;//! list of files for run
   unique_ptr<TIter> fFileListIterator;//! iterator for file list
   KVString fFullFilePath;//! full path to files including "root:" etc. and "/run000000/"
   int run_number;//! run number deduced from filename

   bool parse_event_from_message();
   bool read_buffer();

public:
   KVFzDataReader() {}
   KVFzDataReader(const Char_t* filepath, Int_t bufSiz = 16 * 1024 * 1024);
   virtual ~KVFzDataReader() {}

#ifndef __CINT__
   const DAQ::FzEvent& get_fazia_event() const;
#endif

   static KVFzDataReader* Open(const Char_t* filename, Option_t* opt = "");

   Int_t GetRunNumberReadFromFile() const
   {
      return run_number;
   }

   ClassDef(KVFzDataReader, 1) //Read FAZIA DAQ files
};

#endif
