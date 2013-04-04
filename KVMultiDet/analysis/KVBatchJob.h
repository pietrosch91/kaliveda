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
    Int_t cpu_max;
    TString mem_max;
    TString disk_max;

   public:
   KVBatchJob();
   virtual ~KVBatchJob();

   const Char_t* GetStatus() const { return GetTitle(); }
   void SetStatus(const Char_t* s) { SetTitle(s); }
   const KVDatime& GetSubmitted() const { return submitted; }
   void SetSubmitted(KVDatime&m) { submitted=m; }
   Int_t GetJobID() const { return GetNumber(); }
   void SetJobID(Int_t n) { SetNumber(n); }
   void SetCPUusage(Int_t m) { cpu_used = m; }
   virtual Int_t GetCPUusage() const { return cpu_used; }
   void SetMemUsed(const Char_t* m) { mem_used = m; }
   const Char_t* GetMemUsed() const { return mem_used; }
   void SetCPUmax(Int_t c) { cpu_max=c; }
   Int_t GetCPUmax() const { return cpu_max; }
   void SetMemMax(const Char_t* m) { mem_max = m; }
   const Char_t* GetMemMax() const { return mem_max; }
   void SetDiskMax(const Char_t* m) { disk_max = m; }
   const Char_t* GetDiskMax() const { return disk_max; }
   
   virtual void DeleteJob() {;}

   void ls(Option_t* /*opt*/ = "") const;

   ClassDef(KVBatchJob,1)//Job handled by batch system
};

#endif
