//Created by KVClassFactory on Wed Jan 30 12:13:12 2013
//Author: bonnet

#ifndef __KVCHARGERADIUS_H
#define __KVCHARGERADIUS_H

#include "KVNuclData.h"

class KVChargeRadius : public KVNuclData
{
   
	protected:
	void init();
	Double_t fError;

   public:
   KVChargeRadius();
   KVChargeRadius(const Char_t* name);
   virtual ~KVChargeRadius();
   
	void SetError(Double_t err) {fError = err; }
	Double_t GetError() const {return fError; }
	const Char_t* GetUnit() const {return "fm"; }

   ClassDef(KVChargeRadius,1)////Simple class for store charge radius information of nucleus
};

#endif
