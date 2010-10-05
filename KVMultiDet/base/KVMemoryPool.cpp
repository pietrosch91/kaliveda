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
   fChunkSize = bytes;
}
 
void *KVMemoryPool::GetMemory(size_t bytes)
{
    // return pointer to memory of size 'bytes'
    void* p = 0;
    if(fLastChunkUsed) p=fLastChunkUsed->GetMemory(bytes);
    if( !p && fLastChunkUsed ){
    	// search for next available chunk which can provide memory
        fLastChunkUsed = fLastChunkUsed->Next();
        while(fLastChunkUsed && !p) p = fLastChunkUsed->GetMemory(bytes);
    }
    if( !p ){
    	// add new chunk to pool
    	KVMemoryChunk * chunk = new KVMemoryChunk( fChunkSize );
    	fLast->SetNext(chunk);
    	fLast=chunk;
    	fLastChunkUsed = chunk;
    	p=fLastChunkUsed->GetMemory(bytes);
    }
    return p;
}

KVMemoryPool::~KVMemoryPool()
{
   // Destructor
   KVMemoryChunk* p=fFirst;
   KVMemoryChunk* next=p;
   while(p){
   		next = p->Next();
   		delete p;
   		p=next;
   }
}

void KVMemoryPool::Print()
{
   KVMemoryChunk* p=fFirst;
   while(p){
   		p->Print();
   		p = p->Next();
   }
}
