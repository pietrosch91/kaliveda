//Created by KVClassFactory on Thu Jan 27 17:23:54 2011
//Author: Eric Bonnet,,,

#ifndef __KVLIFETIME_H
#define __KVLIFETIME_H

#include "KVNuclData.h"

class KVLifeTime : public KVNuclData
{

   protected:
	
	void init();
  
	public:
   KVLifeTime();
	KVLifeTime(const Char_t* name);
	virtual ~KVLifeTime();
	
	/*
	TObject's bits can be used as flags, bits 0 - 13 and 24-31 are
	reserved as  global bits while bits 14 - 23 can be used in different
	bit 14 used by KVNuclData class
	*/
	enum EStatusBits {
      kResonnance = BIT(15)	//Is Resonnance MeV (1) / Is "Nucleus" s (0)
	};
	
	const Char_t* GetUnit() const;

	void SetResonnance(Bool_t resonnance) { SetBit(kResonnance,resonnance); }
	Bool_t IsAResonnance() const { return TestBit(kResonnance); }
	
	ClassDef(KVLifeTime,1)//Simple class for store life time information of nucleus
};

#endif
