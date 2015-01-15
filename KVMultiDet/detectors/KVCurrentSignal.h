//Created by KVClassFactory on Tue Jan 13 15:11:11 2015
//Author: ,,,

#ifndef __KVCURRENTSIGNAL_H
#define __KVCURRENTSIGNAL_H

#include "KVSignal.h"

class KVCurrentSignal : public KVSignal
{

   public:
   KVCurrentSignal();
   KVCurrentSignal(const char* name, const char* title);
   KVCurrentSignal(const TString& name, const TString& title);
   virtual ~KVCurrentSignal();
   void Copy(TObject& obj) const;

   ClassDef(KVCurrentSignal,1)//digitized intensity signal
};

#endif
