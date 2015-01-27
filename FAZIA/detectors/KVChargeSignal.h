//Created by KVClassFactory on Tue Jan 13 15:10:39 2015
//Author: ,,,

#ifndef __KVCHARGESIGNAL_H
#define __KVCHARGESIGNAL_H

#include "KVSignal.h"
#include "TF1.h"

class KVPSAResult;

class KVChargeSignal : public KVSignal
{
	
   protected: 
   TF1* fFunc1;
   TF1* fFunc2;
   
   public:
   TF1* GetFitFunction() {return fFunc1; }
   KVChargeSignal();
   KVChargeSignal(const char* name);
   
   virtual ~KVChargeSignal();
   void Copy(TObject& obj) const;
	virtual KVPSAResult* TreateSignal(void);
   virtual KVPSAResult* TreateSignal(TF1* filter);
   
   ClassDef(KVChargeSignal,1)//digitized charge signal
};

#endif
