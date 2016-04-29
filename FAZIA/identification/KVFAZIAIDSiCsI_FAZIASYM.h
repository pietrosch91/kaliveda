//Created by KVClassFactory on Fri Apr 29 09:21:07 2016
//Author: bonnet,,,

#ifndef __KVFAZIAIDSICSI_FAZIASYM_H
#define __KVFAZIAIDSICSI_FAZIASYM_H

#include "KVFAZIAIDSiCsI.h"

class KVFAZIAIDSiCsI_FAZIASYM : public KVFAZIAIDSiCsI {
public:
   KVFAZIAIDSiCsI_FAZIASYM();

   virtual ~KVFAZIAIDSiCsI_FAZIASYM();
   Double_t GetIDMapX(Option_t* opt = "");

   ClassDef(KVFAZIAIDSiCsI_FAZIASYM, 1) //identification telescope for FAZIASYM experiment
};

#endif
