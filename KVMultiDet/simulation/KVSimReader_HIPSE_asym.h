//Created by KVClassFactory on Fri Jul  2 15:16:15 2010
//Author: bonnet

#ifndef __KVSIMREADER_HIPSE_ASYM_H
#define __KVSIMREADER_HIPSE_ASYM_H

#include "KVSimReader_HIPSE.h"
#include "KVNameValueList.h"
#include "TRotation.h"

class KVNucleus;
class TH1F;

class KVSimReader_HIPSE_asym : public KVSimReader_HIPSE {
   void init()
   {
      Info("init", "passe");
      tree_name = "HIPSE_asym";
      Info("init", "%s", branch_name.Data());
      h1 = 0;
      fPhiPlan = 0.;
   }

protected:
   TH1F* h1;
   Double_t fPhiPlan;
   TRotation rr;

public:

   KVSimReader_HIPSE_asym();
   KVSimReader_HIPSE_asym(KVString filename);

   virtual ~KVSimReader_HIPSE_asym();

   virtual Bool_t ReadEvent();
   virtual Bool_t ReadNucleus();

   ClassDef(KVSimReader_HIPSE_asym, 1) //Read ascii file for asymptotic events of the HIPSE code after SIMON deexcitation

};

#endif
