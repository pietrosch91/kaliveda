//Created by KVClassFactory on Tue Jan 20 16:29:33 2015
//Author: ,,,

#ifndef __KVFAZIADBRUN_H
#define __KVFAZIADBRUN_H

#include "KVDBRun.h"

class KVFAZIADBRun : public KVDBRun
{

   public:
   KVFAZIADBRun();
   KVFAZIADBRun(Int_t number, const Char_t* title);
   virtual ~KVFAZIADBRun();

   void SetTrigger(Int_t mt) { SetScaler("Trigger multiplicity",mt); };
   Int_t GetTrigger() const { return GetScaler("Trigger multiplicity"); };
   const Char_t* GetTriggerString() const { if(GetTrigger()>0) return Form("M>=%d",GetTrigger());  else return Form("xxx"); };

   ClassDef(KVFAZIADBRun,1)//run description for FAZIA experiment
};

#endif
