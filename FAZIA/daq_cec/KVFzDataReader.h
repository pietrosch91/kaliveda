//Created by KVClassFactory on Mon Jan 14 12:10:22 2019
//Author: John Frankland,,,

#ifndef __KVFZDATAREADER_H
#define __KVFZDATAREADER_H

#include "KVProtobufDataReader.h"

#ifndef __CINT__
namespace DAQ {
   class FzEvent;
}
#endif

class KVFzDataReader : public KVProtobufDataReader {
   bool parse_event_from_message();

public:
   KVFzDataReader(const Char_t* filepath, Int_t bufSiz = 16 * 1024 * 1024);
   virtual ~KVFzDataReader() {}

#ifndef __CINT__
   const DAQ::FzEvent& get_fazia_event() const;
#endif

   ClassDef(KVFzDataReader, 1) //Read FAZIA DAQ files
};

#endif
