//Created by KVClassFactory on Fri Apr  6 16:33:15 2018
//Author: John Frankland,,,

#include "KVMFMDataFileReader.h"
#include "TSystem.h"
#include "TError.h"
#include "MFMMergeFrame.h"
#include "MFMEbyedatFrame.h"
#include "MFMFaziaFrame.h"

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

void KVMFMDataFileReader::TreatFrame(MFMCommonFrame& f)
{
   switch (f.GetFrameType()) {
      case MFM_MERGE_EN_FRAME_TYPE:
      case MFM_MERGE_TS_FRAME_TYPE:
         fMerge.SetMergeFrame(f);
         while (fMerge.ReadNextFrame()) TreatFrame(fMerge.GetFrameRead());
         break;

      case MFM_EBY_EN_FRAME_TYPE:
      case MFM_EBY_TS_FRAME_TYPE:
      case MFM_EBY_EN_TS_FRAME_TYPE:
         TreatEbyedatFrame(f);
         break;

      case MFM_FAZIA_FRAME_TYPE:
         TreatFaziaFrame(f);
         break;

      default:
         break;
   }
}

Bool_t KVMFMDataFileReader::GetNextEvent()
{
   // Read next event from file. Return kFALSE if end of file reached.

   if (ReadNextFrame()) {
      TreatFrame(GetFrameRead());
      return true;
   }
   return false;
}

KVSeqCollection* KVMFMDataFileReader::GetFiredDataParameters() const
{
   // return list of fired parameters in frame with Ebyedat format
   return (KVSeqCollection*)&fEBYEDATfired;
}

KVMFMDataFileReader* KVMFMDataFileReader::Open(const Char_t* filepath, Option_t*)
{
   TString fp(filepath);
   if (fp.Contains('$')) gSystem->ExpandPathName(fp);
   ::Info("KVMFMDataFileReader::Open", "Opening file %s...", fp.Data());
   return new KVMFMDataFileReader(fp);
}

