//Created by KVClassFactory on Thu Jun  9 10:34:54 2011
//Author: frankland,,,,

#ifndef __KVINDRATELESCOPE_H
#define __KVINDRATELESCOPE_H

#include "KVTelescope.h"

class KVINDRATelescope : public KVTelescope
{

   public:
   KVINDRATelescope();
   virtual ~KVINDRATelescope();

   const Char_t *GetName() const;
   virtual const Char_t *GetArrayName();

   ClassDef(KVINDRATelescope,1)//A detector telescope in the INDRA array
};

#endif
