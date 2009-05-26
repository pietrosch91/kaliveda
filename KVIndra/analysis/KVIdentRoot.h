/*
$Id: KVIdentRoot.h,v 1.2 2006/10/19 14:32:43 franklan Exp $
$Revision: 1.2 $
$Date: 2006/10/19 14:32:43 $
*/

#ifndef KVIdentRoot_h
#define KVIdentRoot_h

#include "KVSelector.h"

class KVIdentRoot:public KVSelector {

 public:
   KVIdentRoot() {
   };
   virtual ~ KVIdentRoot() {
   };

   virtual void InitRun();
   virtual void EndRun();
   virtual void InitAnalysis();
   virtual Bool_t Analysis();
   virtual void EndAnalysis();

   ClassDef(KVIdentRoot, 0);
};

#endif
