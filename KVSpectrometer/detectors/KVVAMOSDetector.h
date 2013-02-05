//Created by KVClassFactory on Wed Jul 25 09:43:37 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSDETECTOR_H
#define __KVVAMOSDETECTOR_H

#include "KVSpectroDetector.h"
#include "KVFunctionCal.h"

class KVVAMOSDetector : public KVSpectroDetector
{
	protected:

		KVACQParam *fTimeParam;       //! Time of flight (HF) ACQ parameter 
		KVFunctionCal *fCh_ns;         //! Volt-ns conversion 


		void init();
   public:
   KVVAMOSDetector();
   KVVAMOSDetector(UInt_t number, const Char_t* type);
   KVVAMOSDetector (const KVVAMOSDetector&) ;
   virtual ~KVVAMOSDetector();
   void Copy (TObject&) const;

   Double_t GetCalibTimeHF() const;
   Float_t GetTimeHF() const;
   void    Initialize();

   virtual void SetCalibrators();

   inline Bool_t IsTimeCalibrated() const{
	   return ( fCh_ns && fCh_ns->GetStatus()  );
   };

   ClassDef(KVVAMOSDetector,1)//Detectors of VAMOS spectrometer
};

#endif
