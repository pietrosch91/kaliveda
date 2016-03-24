//Created by KVClassFactory on Wed Mar 23 14:04:55 2016
//Author: John Frankland,,,

#ifndef __KVIDCSI_H
#define __KVIDCSI_H

#include "KVIDTelescope.h"

class KVIDCsI : public KVIDTelescope {
public:
   KVIDCsI();

   virtual ~KVIDCsI();

   ClassDef(KVIDCsI, 1) //A generic 1-member CsI fast-slow identification telescope
};

#endif
