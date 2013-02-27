//Created by KVClassFactory on Wed Jul 25 10:14:20 2012
//Author: Guilain ADEMARD

#ifndef __KVHARPEEIC_H
#define __KVHARPEEIC_H
#define ARPEEIC_NSEG  7

#include "KVVAMOSDetector.h"
#include "KVUnits.h"

class KVHarpeeIC : public KVVAMOSDetector
{
	protected:
		void init();

   	public:
   		KVHarpeeIC();
   		KVHarpeeIC(UInt_t number, Float_t pressure, Float_t temp=19., Float_t thick = 10.457*KVUnits::cm);
   		KVHarpeeIC (const KVHarpeeIC&) ;
   		virtual ~KVHarpeeIC();
   		void Copy (TObject&) const;

		virtual const Char_t* GetArrayName();
   		virtual const Char_t *GetEBaseName() const;

   		virtual Double_t GetCalibE();
		virtual Int_t    GetMult(Option_t *opt="");
   		virtual Bool_t IsECalibrated() const;


		virtual void SetACQParams();

   ClassDef(KVHarpeeIC,1)//Ionisiation chamber of Harpee, used at the focal plan of VAMOS
};

#endif
