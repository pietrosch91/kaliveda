//Created by KVClassFactory on Tue Sep  8 16:00:28 2015
//Author: ,,,

#ifndef __KVFAZIAIDCsI_e789_H
#define __KVFAZIAIDCsI_e789_H

#include "KVFAZIAIDCsI.h"
#include "KVFAZIADetector.h"

class KVFAZIAIDCsI_e789 : public KVFAZIAIDCsI {

public:
   KVFAZIAIDCsI_e789() {}
   virtual ~KVFAZIAIDCsI_e789() {}

   Double_t GetIDMapX(Option_t* /*opt*/ = "")
   {
      //X-coordinate for CsI identification map :
      // computed slow componment
      // of the charge signal (Q3FPGAEnergy)
      return fCsI->GetQ3FPGAEnergy();
   }
   Double_t GetIDMapY(Option_t* /*opt*/ = "")
   {
      //Y-coordinate for CsI identification map :
      // Q3FastFPGAEnergy / Q3FPGAEnergy
      return fCsI->GetQ3FastFPGAEnergy() / fCsI->GetQ3FPGAEnergy();
   }

   ClassDef(KVFAZIAIDCsI_e789, 1) //id telescope to manage E789 FAZIA CsI identification
};

#endif
