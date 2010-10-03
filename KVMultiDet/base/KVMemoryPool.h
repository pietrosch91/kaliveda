//Created by KVClassFactory on Fri Oct  1 16:50:02 2010
//Author: John Frankland,,,,

#ifndef __KVMEMORYPOOL_H
#define __KVMEMORYPOOL_H

#include "KVMemoryChunk.h"

class KVMemoryPool
{
    KVMemoryChunk* fFirst;//first chunk in pool
    KVMemoryChunk* fLast;//first chunk in pool
    KVMemoryChunk* fLastChunkUsed;
    
   public:
   KVMemoryPool(int nchunks, size_t bytes);
   virtual ~KVMemoryPool();
   
   void *GetMemory(size_t bytes);

   ClassDef(KVMemoryPool,0)//Memory pool
};

#endif
