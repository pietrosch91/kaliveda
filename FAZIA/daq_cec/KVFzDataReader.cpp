//Created by KVClassFactory on Mon Jan 14 12:10:22 2019
//Author: John Frankland,,,

#include "KVFzDataReader.h"

#include <google/protobuf/io/coded_stream.h>
#include "FzEventSet.pb.h"
#include "KVDataRepository.h"

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
   if (fFzEvSet.ev_size() > 1) {
      Error("parse_event_from_message", "FzEventSet with %d events", fFzEvSet.ev_size());
      return false;
   }
   return true;
}

bool KVFzDataReader::read_buffer()
{
   // if previous read reached the end of a file, we try to open the next file in the list

   if (fReachedEndOfFile) {
      TObject* o = fFileListIterator->operator()();
      if (!o) return false; // no more files to read
      // open next file
      KVString url = fFullFilePath + o->GetName();
      open_file(url);
   }
   return KVProtobufDataReader::read_buffer();
}

KVFzDataReader::KVFzDataReader(const Char_t* filepath, Int_t bufSiz)
   : KVProtobufDataReader(bufSiz)
{
   // Open set of FAZIA DAQ files for reading. Filepath URL will be passed to TFile::Open
   // therefore can use same plugins eg. "root://" etc.
   // Note: buffer size given as Int_t, as this is argument type required by TFile::ReadBuffer

   fFullFilePath = filepath;
   fFullFilePath += "/";
   fListOfFiles.reset(gDataRepository->GetDirectoryListing(gDataSet, "raw", gSystem->BaseName(filepath)));
   run_number = gDataSet->GetAvailableRunsFile("raw")->IsRunFileName(gSystem->BaseName(filepath));

   // each run has a set of files with names like
   //   FzEventSet-1490951242-9758.pb
   //   FzEventSet-1490951250-9759.pb
   //   FzEventSet-1490951257-9760.pb
   // they should be read in sequential (chronological) order. the directory listing given above
   // may not automatically be in the right order, therefore we sort the file list according to
   // the final number (i.e. 9758, 9759, 9760 in the example above)

   std::map<int, std::string> filelist;
   fFileListIterator.reset(new TIter(fListOfFiles.get()));
   KVBase* b;
   while ((b = (KVBase*)fFileListIterator->operator()())) {
      KVString bb(b->GetName());
      bb.Begin("-.");
      for (int i = 0; i < 2; ++i) bb.Next();
      filelist[bb.Next().Atoi()] = b->GetName();
   }
   fListOfFiles->Clear();
   for (std::map<int, std::string>::iterator mapit = filelist.begin(); mapit != filelist.end(); ++mapit) {
      fListOfFiles->Add(new KVBase((*mapit).second.c_str()));
   }
   fListOfFiles->ls();
   fFileListIterator->Reset();
   // open first file
   KVString url = fFullFilePath + fFileListIterator->operator()()->GetName();
   open_file(url);
}

const DAQ::FzEvent& KVFzDataReader::get_fazia_event() const
{
   return fFzEvSet.ev(0);
}

KVFzDataReader* KVFzDataReader::Open(const Char_t* filename, Option_t*)
{
   return new KVFzDataReader(filename);
}

//____________________________________________________________________________//

