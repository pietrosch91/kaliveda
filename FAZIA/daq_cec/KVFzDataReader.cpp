//Created by KVClassFactory on Mon Jan 14 12:10:22 2019
//Author: John Frankland,,,

#include "KVFzDataReader.h"

#include <google/protobuf/io/coded_stream.h>
#include "FzEventSet.pb.h"

ClassImp(KVFzDataReader)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFzDataReader</h2>
<h4>Read FAZIA DAQ files</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

DAQ::FzEventSet fFzEvSet;

bool KVFzDataReader::parse_event_from_message()
{
   google::protobuf::io::CodedInputStream codedIStream((const uint8_t*)(fBuffer + fEvOffset), fEvSize);
   if (!fFzEvSet.ParseFromCodedStream(&codedIStream)) {
      Error("parse_event_from_message", "problem parsing event set");
      return false;
   }
   Info("parse_event_from_message", "FzEventSet with %d events", fFzEvSet.ev_size());
   Info("parse_event_from_message", "FzEvent with %d blocks", get_fazia_event().block_size());
   return true;
}

KVFzDataReader::KVFzDataReader(const Char_t* filepath, Int_t bufSiz)
   : KVProtobufDataReader(filepath, bufSiz)
{
   // Open FAZIA DAQ file for reading. Filepath URL will be passed to TFile::Open
   // therefore can use same plugins eg. "root://" etc.
   // Note: buffer size given as Int_t, as this is argument type required by TFile::ReadBuffer
}

const DAQ::FzEvent& KVFzDataReader::get_fazia_event() const
{
   return fFzEvSet.ev(0);
}

//____________________________________________________________________________//

