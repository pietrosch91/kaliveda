//Created by KVClassFactory on Wed Jun 30 15:10:22 2010
//Author: bonnet

#include "KVSimEvent.h"

ClassImp(KVSimEvent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimEvent</h2>
<h4>Events from simulation</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimEvent::KVSimEvent(Int_t mult, const char*classname):KVEvent(mult,classname)
{
   // Default constructor
	init();
	//CustomStreamer(); 
}

void KVSimEvent::Streamer(TBuffer & R__b)
{
   //Stream an object of class KVSimEvent.
   //When reading an event, first Clear() is called (this calls the Clear()
   //method of all the particles in the previous event).

   if (R__b.IsReading()) {
      Clear();
      R__b.ReadClassBuffer(KVSimEvent::Class(), this);
   } else {
      R__b.WriteClassBuffer(KVSimEvent::Class(), this);
   }
}

