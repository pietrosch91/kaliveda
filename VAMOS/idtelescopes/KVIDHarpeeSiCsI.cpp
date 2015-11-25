//Created by KVClassFactory on Thu Dec 12 16:37:53 2013
//Author: Guilain ADEMARD

#include "KVIDHarpeeSiCsI.h"
#include "KVVAMOSCodes.h"

ClassImp(KVIDHarpeeSiCsI)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDHarpeeSiCsI</h2>
<h4>DeltaE-E identification telescope in VAMOS with Harpee's Si-CsI detectors</h4>
<!-- */
// --> END_HTML
//
// This telescope has to be exclusively composed of detectors derived from
// KVVAMOSDetector.
//
// The code corresponding to correct identification by this type of telescope
// is kIDCode3 ( the same as for INDRA's telescope SI_CSI )
//
////////////////////////////////////////////////////////////////////////////////

KVIDHarpeeSiCsI::KVIDHarpeeSiCsI()
{
   // Default constructor

   fIDCode = kIDCode3;
}
//________________________________________________________________

KVIDHarpeeSiCsI::~KVIDHarpeeSiCsI()
{
   // Destructor
}
//________________________________________________________________

Double_t KVIDHarpeeSiCsI::GetIDMapX(Option_t* opt)
{
   // Calculates current X coordinate for identification.
   // It is the CsI detector's total light output. 'opt' has no effect.

   return fEdet->GetRawE();
}
