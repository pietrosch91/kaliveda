//Created by KVClassFactory on Thu Jun  9 10:34:54 2011
//Author: frankland,,,,

#include "KVINDRATelescope.h"

ClassImp(KVINDRATelescope)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRATelescope</h2>
<h4>A detector telescope in the INDRA array</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRATelescope::KVINDRATelescope()
{
   // Default constructor
}

KVINDRATelescope::~KVINDRATelescope()
{
   // Destructor
}

//_________________________________________________________________________
const Char_t *KVINDRATelescope::GetName() const
{
   // Name of telescope given in the form Det1_Det2_..._Ring-numberTelescope-number
   // where Det1 etc. are the ACTIVE detector layers of the telescope
   // The detectors are signified by their TYPE names i.e. KVDetector::GetType
   //
   //Just a wrapper for GetArrayName to allow polymorphism
   return const_cast<KVINDRATelescope*>(this)->GetArrayName();
}

const Char_t *KVINDRATelescope::GetArrayName()
{
   // Name of telescope given in the form Det1_Det2_..._Ring-numberTelescope-number
   // where Det1 etc. are the ACTIVE detector layers of the telescope
   // The detectors are signified by their TYPE names i.e. KVDetector::GetType
    
   TIter next_det(fDetectors);
   KVDetector *kdet;
   TString dummy;
   while ((kdet = (KVDetector *) next_det())) { //loop over detectors in telescope
      if (dummy == "")
         dummy = kdet->GetType();
      else
         dummy += kdet->GetType();
      dummy += "_";
   }
   fName.Form("%s%02d%02d", dummy.Data(), GetRingNumber(), GetNumber());
   return fName.Data();
}


