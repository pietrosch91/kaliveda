//Created by KVClassFactory on Wed Feb 23 10:21:20 2011
//Author: bonnet

#ifndef __KVNUCLDATA_H
#define __KVNUCLDATA_H

#include "TObject.h"
#include "TString.h"

class KVNuclData : public TObject
{

	Double_t	kValue;	//numerical value
	TString  fName;	//name of the object
	
	void init();
	
	public:
   KVNuclData();
   KVNuclData(const Char_t* name);
   virtual ~KVNuclData();
	
	/*
	TObject's bits can be used as flags, bits 0 - 13 and 24-31 are
	reserved as  global bits while bits 14 - 23 can be used in different
	*/
	enum EStatusBits {
      kMeasured = BIT(14)
	};
	
	void SetName(TString name) {fName = name;}
	const Char_t* GetName() const;
	
	void SetValue(Double_t val);
	Double_t GetValue() const;
	
	virtual const Char_t* GetUnit() const;
	
	void SetMeasured(Bool_t measured) { SetBit(kMeasured,measured); }
	Bool_t IsMeasured() const { return TestBit(kMeasured); }
		
	virtual void Print(Option_t* option = "") const;	


   ClassDef(KVNuclData,1)//Simple abstract class to store value related to nuclear data
};

#endif
