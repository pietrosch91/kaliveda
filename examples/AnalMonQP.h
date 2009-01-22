/*
$Id: AnalMonQP.h,v 1.3 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.3 $
$Date: 2007/06/27 14:53:12 $
*/

#ifndef AnalMonQP_h
#define AnalMonQP_h

#include "KVSelector.h"

class AnalMonQP : public KVSelector {

   public:
   AnalMonQP() {};
   virtual ~AnalMonQP() {};
   
   virtual void InitRun();
   virtual void EndRun();
   virtual void InitAnalysis();
   virtual Bool_t Analysis();
   virtual void EndAnalysis();

   ClassDef(AnalMonQP,0)//Example of analysis class with a 'user' global variable, KVQPSource
};

#endif
