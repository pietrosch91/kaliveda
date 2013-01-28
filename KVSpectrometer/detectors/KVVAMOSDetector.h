//Created by KVClassFactory on Wed Jul 25 09:43:37 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSDETECTOR_H
#define __KVVAMOSDETECTOR_H

#include "KVSpectroDetector.h"

class KVVAMOSDetector : public KVSpectroDetector
{

   public:
   KVVAMOSDetector();
   KVVAMOSDetector(UInt_t number, const Char_t* type);
   KVVAMOSDetector (const KVVAMOSDetector&) ;
   virtual ~KVVAMOSDetector();
   void Copy (TObject&) const;

   ClassDef(KVVAMOSDetector,1)//Detectors of VAMOS spectrometer
};

#endif
