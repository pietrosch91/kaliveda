//Created by KVClassFactory on Fri Dec 13 11:56:00 2013
//Author: Guilain ADEMARD

#ifndef __KVIDHARPEEICSI_E503_H
#define __KVIDHARPEEICSI_E503_H

#include "KVIDHarpeeICSi.h"
#include "KVMacros.h" // 'UNUSED' macro

class KVIDHarpeeICSi_e503 : public KVIDHarpeeICSi {

   KVIDZAGrid* FindGridForSegment(Int_t num);

public:
   KVIDHarpeeICSi_e503();
   virtual ~KVIDHarpeeICSi_e503();

   virtual Double_t GetIDMapY(Option_t* opt = "");

   ClassDef(KVIDHarpeeICSi_e503, 1) //DeltaE-E identification with Harpee's IC-Si detectors for e503 experiment
};

#endif
