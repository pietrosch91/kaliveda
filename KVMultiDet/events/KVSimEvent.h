//Created by KVClassFactory on Wed Jun 30 15:10:22 2010
//Author: bonnet

#ifndef __KVSIMEVENT_H
#define __KVSIMEVENT_H

#include "KVEvent.h"
#include "KVNameValueList.h"

class KVSimEvent : public KVEvent
{
	protected:
	
	KVNameValueList* add_variables;	//-> additional variables
   
	public:
   
	KVSimEvent(Int_t mult = 50, const char *classname = "KVSimNucleus");
	virtual ~KVSimEvent(){
		delete add_variables; add_variables=0;
	}

	void init(){
		add_variables = new KVNameValueList();
	}
	
	void Clear(Option_t * /* opt */ = ""){
		KVEvent::Clear();
		add_variables->Clear();
	
	}
	
	KVNameValueList* GetParameters() { return add_variables; }

   ClassDef(KVSimEvent,1)//Events from simulation
};

#endif
