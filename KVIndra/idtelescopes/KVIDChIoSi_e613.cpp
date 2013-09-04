//Created by KVClassFactory on Tue Jul  2 17:16:49 2013
//Author: John Frankland,,,

#include "KVIDChIoSi_e613.h"

ClassImp(KVIDChIoSi_e613)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDChIoSi_e613</h2>
<h4>ChIo-Si identification (E613)</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDChIoSi_e613::KVIDChIoSi_e613()
{
   // Default constructor
}


KVIDChIoSi_e613::~KVIDChIoSi_e613()
{
   // Destructor
}

//________________________________________________________________________________________//

Double_t KVIDChIoSi_e613::GetIDMapX(Option_t *)
{
   return (fsi->GetACQData("PG") - fsipgped);
}

Double_t KVIDChIoSi_e613::GetIDMapY(Option_t *)
{
   return (fchio->GetACQData("PG") - fchiopgped);
}

