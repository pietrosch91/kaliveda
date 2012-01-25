//Created by KVClassFactory on Mon Jan 16 11:47:30 2012
//Author: bonnet

#ifndef __KVINDRAE613_H
#define __KVINDRAE613_H

#include "KVINDRA4.h"

class KVINDRAe613 : public KVINDRA4
{

	protected:
   virtual void PrototypeTelescopes();

   public:
   KVINDRAe613();
   KVINDRAe613 (const KVINDRAe613&) ;
   virtual ~KVINDRAe613();
   void Copy (TObject&) const;
	
	virtual void Build();

   ClassDef(KVINDRAe613,1)//INDRA multidetector array, configuration for e613 experiment
};

#endif
