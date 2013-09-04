//Created by KVClassFactory on Tue Jul  2 17:16:49 2013
//Author: John Frankland,,,

#ifndef __KVIDCHIOSI_E613_H
#define __KVIDCHIOSI_E613_H

#include "KVIDChIoSi.h"

class KVIDChIoSi_e613 : public KVIDChIoSi
{

   public:
   KVIDChIoSi_e613();
   virtual ~KVIDChIoSi_e613();

   Double_t GetIDMapX(Option_t * opt = "");
   Double_t GetIDMapY(Option_t * opt = "");

   ClassDef(KVIDChIoSi_e613,1)//ChIo-Si identification (E613)
};

#endif
