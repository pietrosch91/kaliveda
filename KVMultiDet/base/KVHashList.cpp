//Created by KVClassFactory on Mon Nov 30 15:00:00 2009
//Author: John Frankland,,,

#include "KVHashList.h"
#include "THashList.h"

ClassImp(KVHashList)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVHashList</h2>
<h4>Extended version of ROOT THashList</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVHashList::KVHashList(Int_t capacity, Int_t rehash)
	: KVSeqCollection()
{
   // Create a THashList object. Capacity is the initial hashtable capacity
   // (i.e. number of slots), by default kInitHashTableCapacity = 17, and
   // rehash is the value at which a rehash will be triggered. I.e. when the
   // average size of the linked lists at a slot becomes longer than rehash
   // then the hashtable will be resized and refilled to reduce the collision
   // rate to about 1. The higher the collision rate, i.e. the longer the
   // linked lists, the longer lookup will take. If rehash=0 the table will
   // NOT automatically be rehashed. Use Rehash() for manual rehashing.
   // WARNING !!!
   // If the name of an object in the HashList is modified, The hashlist
   // must be Rehashed
   
   fCollection = new THashList(capacity, rehash);
}

KVHashList::~KVHashList()
{
   // Destructor
}

