//Created by KVClassFactory on Fri Jan 23 18:44:27 2015
//Author: ,,,

#ifndef __KVFAZIADETECTOR_H
#define __KVFAZIADETECTOR_H

#include "KVDetector.h"

class KVSignal;

class KVFAZIADetector : public KVDetector
{
	protected:
   KVList *fSignals;         //list of electronics signal (current, charge, etc... )
   void init();	//initialisatino method called by the constructors
   
   public:
   KVFAZIADetector();
   KVFAZIADetector(const Char_t* type, const Float_t thick = 0.0);
   virtual ~KVFAZIADetector();
   void Copy(TObject& obj) const;
	virtual void	Clear(Option_t* opt = "");
   
   virtual Bool_t Fired(Option_t * opt = "any");

	void AddSignal(KVSignal* signal);
	Bool_t HasSignal() const;
	KVSignal* GetSignal(const Char_t* name) const;
	KVSignal* GetSignal(Int_t idx) const;
	Int_t GetNumberOfSignals() const;
	KVList* GetListOfSignals() const;

   ClassDef(KVFAZIADetector,1)//Base class for FAZIA detector
};

#endif
