//Created by KVClassFactory on Tue Sep  8 16:14:25 2015
//Author: ,,,

#ifndef __KVFAZIAIDSiCsI_e789_H
#define __KVFAZIAIDSiCsI_e789_H

#include "KVFAZIAIDSiCsI.h"

class KVFAZIAIDSiCsI_e789 : public KVFAZIAIDSiCsI {

public:

   KVFAZIAIDSiCsI_e789() {}
   virtual ~KVFAZIAIDSiCsI_e789() {}

   Double_t GetIDMapX(Option_t* /*opt*/ = "")
   {
      //X-coordinate for Si2-CsI identification map :
      // computed fast componment
      // of the charge signal of CsI detector
      return fCsI->GetQ3FPGAEnergy();
   }

   Double_t GetIDMapY(Option_t* /*opt*/ = "")
   {
      //Y-coordinate for Si2-CsI identification map :
      // computed amplitude
      // of the charge signal of Si2 detector
      return fSi2->GetQ2FPGAEnergy();
   }

   ClassDef(KVFAZIAIDSiCsI_e789, 1) //id telescope to manage E789 FAZIA Si-CsI identification
};

#endif
