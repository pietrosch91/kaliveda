//Created by KVClassFactory on Fri Apr 17 10:19:02 2015
//Author: John Frankland,,,

#ifndef __KVGENPHASESPACE_H
#define __KVGENPHASESPACE_H

#include "KVBase.h"

class KVGenPhaseSpace : public KVBase
{

   public:
   KVGenPhaseSpace();
   KVGenPhaseSpace(const Char_t* name, const Char_t* title = "");
   virtual ~KVGenPhaseSpace();
   void Copy(TObject& obj) const;

   ClassDef(KVGenPhaseSpace,1)//Generate momenta for an event using microcanonical phase space sampling
};

#endif
