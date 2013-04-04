//Created by KVClassFactory on Wed Apr  3 14:15:55 2013
//Author: John Frankland,,,

#ifndef __KVGEBATCHJOB_H
#define __KVGEBATCHJOB_H

#include "KVBatchJob.h"

class KVGEBatchJob : public KVBatchJob
{
   TString resources;

   public:
   KVGEBatchJob();
   virtual ~KVGEBatchJob();

   void DeleteJob();
   void SetResources(TString r) { resources=r; }
   const Char_t* GetResources() const { return resources; }
   virtual Int_t GetCPUusage() const { return cpu_used; }
   
   ClassDef(KVGEBatchJob,1)//Job handled by Grid Engine batch system
};

#endif
