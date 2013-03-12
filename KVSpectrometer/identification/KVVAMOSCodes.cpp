//Created by KVClassFactory on Tue Mar 12 15:08:25 2013
//Author: Guilain ADEMARD

#include "KVVAMOSCodes.h"

ClassImp(KVVAMOSCodes)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSCodes</h2>
<h4>Status for calibration, trajectory reconstruction and identification in VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVVAMOSCodes::KVVAMOSCodes()
{
   // Default constructor
   fFPMask = 0;
}

//________________________________________________________________

KVVAMOSCodes::~KVVAMOSCodes()
{
   // Destructor
   fFPMask = 0;
}
//________________________________________________________________

Bool_t KVVAMOSCodes::TestFPCode(UShort_t code)
{
   //check Focal plan Position reconstruction code against code mask
   //always kTRUE if no mask set (fFPMask==0)
   if (!fFPMask)
      return kTRUE;
   return (Bool_t) ((fFPMask & code) != 0);
}
