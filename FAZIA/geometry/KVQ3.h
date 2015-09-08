//Created by KVClassFactory on Mon Sep  7 11:38:39 2015
//Author: ,,,

#ifndef __KVQ3_H
#define __KVQ3_H

#include "KVSignal.h"

class KVQ3 : public KVSignal
{

   protected:
	void init();
   Double_t fFastTrapRiseTime;	 // rise time of the trapezoidal shaper for fast componment
	Double_t fFastTrapFlatTop; 	 // flat top of the trapezoidal shaper for fast componment
	Double_t fFastAmplitude;			// fast amplitude for CsI
	
	public:
   KVQ3();
   KVQ3(const char* name);
   
	virtual ~KVQ3();
   void Copy(TObject& obj) const;
   
	virtual void SetDefaultValues();
	virtual void LoadPSAParameters();
	virtual Bool_t IsCharge() const { return kTRUE; }

	void SetFastTrapShaperParameters(Double_t rise, Double_t flat) {fFastTrapRiseTime=rise; fFastTrapFlatTop=flat;}
	
	virtual void TreateSignal();
   virtual KVPSAResult* GetPSAResult() const;
	
	ClassDef(KVQ3,1)//Q3 channel of CSI
};

#endif
