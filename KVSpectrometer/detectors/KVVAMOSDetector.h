//Created by KVClassFactory on Wed Jul 25 09:43:37 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSDETECTOR_H
#define __KVVAMOSDETECTOR_H

#include "KVSpectroDetector.h"

class KVVAMOSDetector : public KVSpectroDetector
{
	protected:

		KVACQParam *fTimeParam;       // Time of flight (HF) ACQ parameter 


		void init();
   public:
   KVVAMOSDetector();
   KVVAMOSDetector(UInt_t number, const Char_t* type);
   KVVAMOSDetector (const KVVAMOSDetector&) ;
   virtual ~KVVAMOSDetector();
   void AddACQParam(KVACQParam*);
   void Copy (TObject&) const;

   Float_t GetTimeHF() const;

   virtual void SetCalibrators();

   ClassDef(KVVAMOSDetector,1)//Detectors of VAMOS spectrometer
};

#endif
