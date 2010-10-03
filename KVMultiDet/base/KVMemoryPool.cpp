//Created by KVClassFactory on Fri Oct  1 16:50:02 2010
//Author: John Frankland,,,,

#include "KVMemoryPool.h"

ClassImp(KVMemoryPool)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVMemoryPool</h2>
<h4>Memory pool</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVMemoryPool::KVMemoryPool(int nchunks, size_t bytes)
{
   // Create nchunks chunks each of size 'bytes'
   fFirst = fLast = fLastChunkUsed = 0;
   for(int i=0; i<nchunks; i++){
       KVMemoryChunk * chunk = new KVMemoryChunk(bytes);
       if(!fFirst) fFirst=chunk;
       if(fLast) fLast->SetNext( chunk );
       fLast = chunk;
   }
   fLastChunkUsed = fFirst;
}
 
void *KVMemoryPool::GetMemory(size_t bytes)
{
    // return pointer to memory of size 'bytes'
    void* p = fLastChunkUsed->GetMemory(bytes);
    if( !p ){
        fLastChunkUsed = fLastChunkUsed->Next();
        while(fLastChunkUsed && !p) p = fLastChunkUsed->GetMemory(bytes);
    }
    if( !p ){
       KVMemoryChunk * chunk = new KVMemoryChunk(bytes);
       if(!fFirst) fFirst=chunk;
       if(fLastChunkUsed) fLastChunkUsed->SetNext( chunk );
       fLastChunkUsed = chunk;
    }
}

KVMemoryPool::~KVMemoryPool()
{
   // Destructor
}

