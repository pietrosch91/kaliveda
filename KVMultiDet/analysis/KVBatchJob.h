//Created by KVClassFactory on Wed Apr  3 14:15:11 2013
//Author: John Frankland,,,

#ifndef __KVBATCHJOB_H
#define __KVBATCHJOB_H

#include "KVBase.h"
#include <KVDatime.h>

class KVBatchJob : public KVBase
{
    KVDatime submitted;
    Int_t cpu_used;
    TString mem_used;

   public:
   KVBatchJob();
   virtual ~KVBatchJob();

   const Char_t* GetStatus() const { return GetTitle(); }
   void SetStatus(const Char_t* s) { SetTitle(s); }
   const KVDatime& GetSubmitted() const { return submitted; }
   void SetSubmitted(KVDatime&m) { submitted=m; }
   Int_t GetJobID() const { return GetNumber(); }
   void SetJobID(Int_t n) { SetNumber(n); }
   void SetCPUusage(Int_t c) { cpu_used = c; }
   Int_t GetCPUusage() const { return cpu_used; }
   void SetMemUsed(const Char_t* m) { mem_used = m; }
   const Char_t* GetMemUsed() const { return mem_used; }

   void ls(Option_t* /*opt*/ = "") const;

   ClassDef(KVBatchJob,1)//Job handled by batch system
};

#endif
