//Created by KVClassFactory on Thu Jun 17 14:19:50 2010
//Author: John Frankland,,,

#ifndef __KVCLASSMONITOR_H
#define __KVCLASSMONITOR_H

#include "KVBase.h"
#include "TEnv.h"

class KVClassMonitor : public KVBase {

   static KVClassMonitor* fgClassMonitor;//! singleton
   TEnv fClassStats;//! store class instance statistics
   TEnv fClassStatsInit;//! intitial class instance statistics
   static Bool_t fDisableCheck;//! disable static DoCheck() method

   KVClassMonitor();
public:
   virtual ~KVClassMonitor();

   virtual void Check();
   virtual void CompareToInit();
   virtual void SetInitStatistics();

   static KVClassMonitor* GetInstance();

   // Print statistics of global singleton if it exists
   static void DoCheck(const TString& method, const TString& message)
   {
      if (!fDisableCheck && fgClassMonitor) {
         printf("<%s> : %s\n", method.Data(), message.Data());
         fgClassMonitor->Check();
      }
   }

   // Disable static DoCheck method
   static void DisableChecking()
   {
      fDisableCheck = kTRUE;
   }
   // Re-enable static DoCheck method
   static void EnableChecking()
   {
      fDisableCheck = kFALSE;
   }

   ClassDef(KVClassMonitor, 0) //Track potential memory leaks
};

#endif
