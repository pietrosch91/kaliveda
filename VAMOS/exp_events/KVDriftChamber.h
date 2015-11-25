//Created by KVClassFactory on Mon Sep  3 11:29:00 2012
//Author: Guilain ADEMARD

#ifndef __KVDRIFTCHAMBER_H
#define __KVDRIFTCHAMBER_H

#include "KVVAMOSDetector.h"

class KVDriftChamber : public KVVAMOSDetector {
protected:
   void init();

public:
   KVDriftChamber();
   KVDriftChamber(const KVDriftChamber&) ;
   virtual ~KVDriftChamber();
   void Copy(TObject&) const;

   virtual const Char_t* GetArrayName();
   ClassDef(KVDriftChamber, 1) //Drift Chamber, used at the focal plan of VAMOS
};

#endif
