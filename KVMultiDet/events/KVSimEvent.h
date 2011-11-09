//Created by KVClassFactory on Wed Jun 30 15:10:22 2010
//Author: bonnet

#ifndef __KVSIMEVENT_H
#define __KVSIMEVENT_H

#include "KVEvent.h"
#include "KVNameValueList.h"

class KVSimEvent : public KVEvent, public KVNameValueList
{
	protected:
	void init();
	
	public:
   
	KVSimEvent(Int_t mult = 50, const char *classname = "KVSimNucleus");
	virtual ~KVSimEvent();
	
	void Copy(TObject& obj) const;
	void Clear(Option_t * opt = "");
	void Print(Option_t* opt = "") const;
	
   ClassDef(KVSimEvent,2)//Events from simulation
};

#endif
