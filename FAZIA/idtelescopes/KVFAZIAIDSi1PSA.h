//Created by KVClassFactory on Mon Feb 17 13:52:51 2014
//Author: John Frankland,,,

#ifndef __KVFAZIAIDSI1PSA_H
#define __KVFAZIAIDSI1PSA_H

#include "KVFAZIAIDTelescope.h"
#include "TGraph.h"

class KVFAZIAIDSi1PSA : public KVFAZIAIDTelescope
{

	TGraph *fRearThreshold;//! Z-dependent threshold extracted from published article
	
   public:
   KVFAZIAIDSi1PSA();
   virtual ~KVFAZIAIDSi1PSA();
	Bool_t CheckTheoreticalIdentificationThreshold(KVNucleus*, Double_t = 0.);

   ClassDef(KVFAZIAIDSi1PSA,1)//PSA identification in first silicon of FAZIA telescopes
};

#endif
