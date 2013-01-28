//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#ifndef __KVINDRA_VAMOS_H
#define __KVINDRA_VAMOS_H

#include "KVINDRAe503.h"
#include "KVVAMOS.h"

class KVINDRA_VAMOS : public KVINDRAe503
{

	private:

	protected:

		KVVAMOS *fVamos;        // VAMOS spectrometer coupled to INDRA

   public:
   KVINDRA_VAMOS();
   virtual ~KVINDRA_VAMOS();

   void init();

   virtual void Build();



  inline KVVAMOS* GetVAMOS(){return fVamos;}; 

   ClassDef(KVINDRA_VAMOS,1)//INDRA + VAMOS experimental setup for the e494s and the e503 experiments performed at GANIL
};

#endif
