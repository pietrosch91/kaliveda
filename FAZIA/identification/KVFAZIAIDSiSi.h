//Created by KVClassFactory on Mon Mar 16 11:45:14 2015
//Author: ,,,

#ifndef __KVFAZIAIDSISI_H
#define __KVFAZIAIDSISI_H

#include "KVFAZIAIDTelescope.h"
#include "KVIdentificationResult.h"

class KVIDZAGrid;

class KVFAZIAIDSiSi : public KVFAZIAIDTelescope
{
	private:
	KVIDZAGrid*	fGrid;
   
	public:
   KVFAZIAIDSiSi();
   virtual ~KVFAZIAIDSiSi();
   void Copy(TObject& obj) const;
   virtual void	Initialize();
	Bool_t Identify(KVIdentificationResult*, Double_t x=-1., Double_t y=-1.);
	

   ClassDef(KVFAZIAIDSiSi,1)//identification telescope for FAZIA Si-Si idcards
};

#endif
