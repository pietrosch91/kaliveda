//Created by KVClassFactory on Fri Apr  6 16:33:15 2018
//Author: John Frankland,,,

#include "KVMFMDataFileReader.h"

ClassImp(KVMFMDataFileReader)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVMFMDataFileReader</h2>
<h4>Read MFM format acquisition data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Bool_t KVMFMDataFileReader::GetNextEvent()
{
   // Read next event from file. Return kFALSE if end of file reached.
   // What to do if event is a merge frame?
   return reader.ReadNextFrame();
}

KVSeqCollection* KVMFMDataFileReader::GetFiredDataParameters() const
{
   // This doesn't make sense for MFM data which may not be in Ebyedat format
   return nullptr;
}

KVMFMDataFileReader* KVMFMDataFileReader::Open(const Char_t* filepath, Option_t*)
{
   return new KVMFMDataFileReader(filepath);
}
