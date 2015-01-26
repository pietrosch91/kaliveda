//Created by KVClassFactory on Thu Jan 22 16:02:02 2015
//Author: ,,,

#ifndef __KVFAZIA_2B_H
#define __KVFAZIA_2B_H

#include "KVMultiDetArray.h"

class KVFAZIA_2B : public KVMultiDetArray
{

   TString fFGeoType;  //type of FAZIA geometry (="compact",...)
   Double_t fFDist;    //distance of FAZIA detectors from target (in cm)
   Double_t fFThetaMin;//minimum polar angle for compact geometry (in degrees)
   
   void BuildFAZIA();
   
   public:
   KVFAZIA_2B();
   virtual ~KVFAZIA_2B();
	virtual void Build(Int_t run=-1);
	void GetDetectedEvent(KVDetectorEvent* detev, TClonesArray* fired_params);
   void GetIDTelescopes(KVDetector *, KVDetector *, TCollection *);
   
   ClassDef(KVFAZIA_2B,1)//FAZIA set-up with two blocks used in LNS 2014 commissioning
};

#endif
