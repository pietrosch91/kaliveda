/*
$Id: KVFortranAnalysis.h,v 1.1 2007/03/22 17:48:03 cussol Exp $
$Revision: 1.1 $
$Date: 2007/03/22 17:48:03 $
*/

#ifndef KVFortranAnalysis_h
#define KVFortranAnalysis_h

#include "KVSelector.h"
#include "TCutG.h"

class KVFortranAnalysis : public KVSelector {
   private:
   TList los;
   TCutG *AprilFish;
   TCutG *Eye;

   public:
   static void BuildKVSelectorFromFortran(const Char_t *f="");
   
   KVFortranAnalysis() {};
   virtual ~KVFortranAnalysis() {};
   
   virtual void InitRun();
   virtual void EndRun();
   virtual void InitAnalysis();
   virtual Bool_t Analysis();
   virtual void EndAnalysis();
   
   ClassDef(KVFortranAnalysis,0);//For analysing INDRA data with fortran files
};

#endif
