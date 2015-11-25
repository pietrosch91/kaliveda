//Created by KVClassFactory on Tue Jan 20 16:29:33 2015
//Author: ,,,

#ifndef __KVFAZIADBRUN_H
#define __KVFAZIADBRUN_H

#include "KVDBRun.h"

class KVFAZIADBRun : public KVDBRun {

public:
   KVFAZIADBRun();
   KVFAZIADBRun(Int_t number, const Char_t* title);
   virtual ~KVFAZIADBRun();

   void SetTrigger(Int_t mt);
   Int_t GetTrigger() const;
   const Char_t* GetTriggerString() const;

   void SetACQStatus(const KVString& status);
   const Char_t* GetACQStatus() const;

   void SetGoodEvents(Int_t evt_number);
   Int_t GetGoodEvents() const;

   void SetError_WrongNumberOfBlocks(Int_t evt_number);
   Int_t GetError_WrongNumberOfBlocks() const;

   void SetError_InternalBlockError(Int_t evt_number);
   Int_t GetError_InternalBlockError() const;

   void SetNumberOfAcqFiles(Int_t number);
   Int_t GetNumberOfAcqFiles() const;

   void SetDuration(Double_t duration);
   Double_t GetDuration() const;

   void SetFrequency(Double_t frequency);
   Double_t GetFrequency() const;

   ClassDef(KVFAZIADBRun, 1) //run description for FAZIA experiment
};

#endif
