//Created by KVClassFactory on Fri Apr  6 16:33:15 2018
//Author: John Frankland,,,

#include "KVMFMDataFileReader.h"
#include "TSystem.h"
#include "TError.h"
#include "MFMXmlFileHeaderFrame.h"

ClassImp(KVMFMDataFileReader)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVMFMDataFileReader</h2>
<h4>Read MFM format acquisition data</h4>

This class uses the mfmlib package available from: https://gitlab.in2p3.fr/jdfcode/mfmlib.git
This class is enabled if you build kaliveda with cmake option -DUSE_MFM=yes
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVMFMDataFileReader::KVMFMDataFileReader(const Char_t* filepath)
   : KVRawDataReader(), MFMFileReader(filepath, 100 * 1024 * 1024)
{
   // Open the datafile with given path
   // Read the first frame in the file (should be a MFMXmlFileHeaderFrame) and extract informations

   if (!ReadNextFrame()) {
      Error("KVMFMDataFileReader", "Cannot read file %s", filepath);
      MakeZombie();
      return;
   }
   if (GetFrameReadType() != MFM_XML_FILE_HEADER_FRAME_TYPE) {
      Warning("KVMFMDataFileReader", "First frame in file is not MFM_XML_FILE_HEADER_FRAME_TYPE: type is %s",
              GetFrameReadTypeSymbol().c_str());
   }
   else {
      GetFrameRead().Print();
      fRunInfos.SetValue("ExperimentName", GetFrameRead<MFMXmlFileHeaderFrame>().GetExperimentName());
      fRunInfos.SetValue("FileName", GetFrameRead<MFMXmlFileHeaderFrame>().GetFileName());
      fRunInfos.SetValue("FileCreationTime", GetFrameRead<MFMXmlFileHeaderFrame>().GetFileCreationTime());
      fRunInfos.SetValue("RunNumber", GetFrameRead<MFMXmlFileHeaderFrame>().GetRunNumber());
      fRunInfos.SetValue("RunIndex", GetFrameRead<MFMXmlFileHeaderFrame>().GetRunIndex());
      fRunInfos.SetValue("RunStartTime", GetFrameRead<MFMXmlFileHeaderFrame>().GetRunStartTime());
   }
}

KVMFMDataFileReader* KVMFMDataFileReader::Open(const Char_t* filepath, Option_t*)
{
   TString fp(filepath);
   if (fp.Contains('$')) gSystem->ExpandPathName(fp);
   ::Info("KVMFMDataFileReader::Open", "Opening file %s...", fp.Data());
   return new KVMFMDataFileReader(fp);
}

void KVMFMDataFileReader::SetActionsDirectory(const string& d)
{
   // Set directory in which to look for Ebyedat ACTIONS_* files
   // Defaults to data directory
   GetFrameLibrary().SetEbyedatActionsDirectory(d);
}

Int_t KVMFMDataFileReader::GetRunNumberReadFromFile() const
{
   // Return run number of file currently being read.
   // Only call once file has been successfully opened.

   return fRunInfos.GetIntValue("RunNumber");
}
