//Created by KVClassFactory on Thu Jan 27 17:23:54 2011
//Author: Eric Bonnet,,,

#ifndef __KVMASSEXCESS_H
#define __KVMASSEXCESS_H

#include "KVNuclData.h"

class KVMassExcess : public KVNuclData
{

   protected:
	
	void init(){}	//do nothing
  	
	public:
   KVMassExcess();
	KVMassExcess(const Char_t* name);
	virtual ~KVMassExcess();
	
	const Char_t* GetUnit() const;
   
	ClassDef(KVMassExcess,1)//Simple class to store mass information of nucleus
};

#endif
