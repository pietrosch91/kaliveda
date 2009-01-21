/*
$Id: KVIDChIoSi5.cpp,v 1.1 2007/12/10 13:02:13 franklan Exp $
$Revision: 1.1 $
$Date: 2007/12/10 13:02:13 $
*/

//Created by KVClassFactory on Mon Dec 10 12:58:26 2007
//Author: franklan

#include "KVIDChIoSi5.h"

ClassImp(KVIDChIoSi5)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDChIoSi5</h2>
<h4>INDRA 5th campaign ChIo-Si identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDChIoSi5::KVIDChIoSi5()
{
   // Default constructor
}

KVIDChIoSi5::~KVIDChIoSi5()
{
   // Destructor
}

//________________________________________________________________________________________//

Double_t KVIDChIoSi5::GetIDMapX(Option_t *)
{
   //X coordinate for 5th campaign ChIo-Si identification is KVSilicon::GetEnergy
   return GetDetector(2)->GetEnergy();
}

Double_t KVIDChIoSi5::GetIDMapY(Option_t *)
{
   //Y coordinate for 5th campaign ChIo-Si identification is KVChIo::GetEnergy
   return GetDetector(1)->GetEnergy();
}

