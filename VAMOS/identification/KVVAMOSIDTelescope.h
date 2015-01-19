//Created by KVClassFactory on Mon May 27 10:38:29 2013
//Author: Guilain ADEMARD

#ifndef __KVVAMOSIDTELESCOPE_H
#define __KVVAMOSIDTELESCOPE_H

#include "KVINDRAIDTelescope.h"
#include "KVIDZAGrid.h"

class KVVAMOSIDTelescope : public KVINDRAIDTelescope
{
	protected:

		KVDetector *fDEdet;//!
		KVDetector *fEdet; //!
		KVIDZAGrid *fGrid; //!

   	public:

   		KVVAMOSIDTelescope();
   		virtual ~KVVAMOSIDTelescope();

   		virtual const Char_t *GetArrayName();

   		virtual Bool_t Identify(KVIdentificationResult *, Double_t x=-1., Double_t y=-1.);
   		virtual void Initialize();

   	//	virtual const Char_t *GetIDSubCodeString(KVIDSubCode & concat) const;

   		ClassDef(KVVAMOSIDTelescope,1)//A deltaE-E identification telescope in VAMOS
};

#endif
