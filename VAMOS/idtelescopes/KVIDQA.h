//Created by KVClassFactory on Fri Jan 23 15:43:30 2015
//Author: ademard

#ifndef __KVIDQA_H
#define __KVIDQA_H

#include "KVVAMOSIDTelescope.h"

class KVIDQAGrid;
class KVVAMOSReconNuc;

class KVIDQA : public KVVAMOSIDTelescope
{

	KVIDQAGrid *fQAGrid; //!

   public:
   KVIDQA();
   virtual ~KVIDQA();

   virtual const Char_t *GetArrayName();
   virtual Bool_t Identify(KVIdentificationResult *, Double_t x=-1., Double_t y=-1.);
   virtual Bool_t Identify(KVVAMOSReconNuc *, Double_t x=-1., Double_t y=-1.);
   virtual void Initialize();


   ClassDef(KVIDQA,1)//ID telescope used to identify Q and A with VAMOS
};

#endif
