//Created by KVClassFactory on Wed Apr  3 14:15:55 2013
//Author: John Frankland,,,

#ifndef __KVGEBATCHJOB_H
#define __KVGEBATCHJOB_H

#include "KVBatchJob.h"

class KVGEBatchJob : public KVBatchJob
{

   public:
   KVGEBatchJob();
   virtual ~KVGEBatchJob();

   void DeleteJob();

   ClassDef(KVGEBatchJob,1)//Job handled by Grid Engine batch system
};

#endif
