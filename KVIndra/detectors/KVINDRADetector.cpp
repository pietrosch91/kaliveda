//Created by KVClassFactory on Sat Oct  3 14:18:09 2009
//Author: John Frankland,,,

#include "KVINDRADetector.h"

ClassImp(KVINDRADetector)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRADetector</h2>
<h4>Detectors of INDRA array</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRADetector::KVINDRADetector()
{
   // Default constructor
}

KVINDRADetector::~KVINDRADetector()
{
   // Destructor
}

const Char_t *KVINDRADetector::GetArrayName()
{
   // Name of detector given in the form
   //     CI_0213, SI_0911, CSI_1705
   // to be compatible with GANIL acquisition parameters
   //
   // The root of the name is the detector type
	
   fFName =
       Form("%s_%02d%02d", GetType(), GetRingNumber(),
            GetTelescopeNumber());
   return fFName.Data();
}

//_______________________________________________________________________________
void KVINDRADetector::AddACQParamType(const Char_t * type)
{
   // Add an acquisition parameter of given type to this detector
	// The parameter will be named according to INDRA/GANIL acquisition
	// convention, i.e. name_of_detector_type e.g.
	//    CI_0201_PG (type="PG")
	//    CSI_0913_R (type="R")

   KVACQParam *par = new KVACQParam();
   TString name;
   name = this->GetName();
   name.Append("_");
   name.Append(type);
   par->SetName(name);
   par->SetType(type);
   AddACQParam(par);
}

//________________________________________________________________________________
KVACQParam *KVINDRADetector::GetACQParam(const Char_t *type)
{
   // Look for acquisition parameter with given type in list
	// of parameters associated with this detector.
	//
	//    *** WARNING ***
	// This method overrides the KVDetector::GetACQParam(const Char_t* name)
	// method, which interprets the argument as the FULL name of the parameter.
	// In this version, only the 'type' needs to be given,
	//   e.g.  GetACQParam("PG") to retrieve e.g. the "CI_0201_PG" parameter
	//    associated to detector "CI_0201".
	// If using this method in a loop over a list of INDRA- and non-INDRA
	// detectors, the behaviour of the method will be different for the two
	// types of detectors, and results may not be what you wanted.
	
   TString name;
   name = GetName();
   name.Append("_");
   name.Append(type);
   return KVDetector::GetACQParam(name.Data());
}

