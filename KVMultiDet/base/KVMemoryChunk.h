//Created by KVClassFactory on Fri Oct  1 16:50:15 2010
//Author: John Frankland,,,,

#ifndef __KVMEMORYCHUNK_H
#define __KVMEMORYCHUNK_H
#include "Rtypes.h"

class KVMemoryChunk
{
    char* fMemory;//pointer to start of chunk
    Size_t fSize;//size of chunk in bytes
    Size_t fUsed;//memory used in bytes
    KVMemoryChunk* fNext;//next chunk of memory
    
   public:
   KVMemoryChunk();
   KVMemoryChunk(Size_t);
   virtual ~KVMemoryChunk();
   
   void* GetMemory(Size_t);
   
   void SetNext(KVMemoryChunk* n) { fNext = n; };
   KVMemoryChunk* Next() const { return fNext; };

   ClassDef(KVMemoryChunk,0)//Memory chunk in pool
};

#endif
