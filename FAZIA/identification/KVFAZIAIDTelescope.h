//Created by KVClassFactory on Mon Feb 17 13:51:39 2014
//Author: John Frankland,,,

#ifndef __KVFAZIAIDTELESCOPE_H
#define __KVFAZIAIDTELESCOPE_H

#include "KVIDTelescope.h"

class KVFAZIAIDTelescope : public KVIDTelescope
{

   public:
   KVFAZIAIDTelescope();
   virtual ~KVFAZIAIDTelescope();
   
   ClassDef(KVFAZIAIDTelescope,1)//Identification for FAZIA array
};

#endif
