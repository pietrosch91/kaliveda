//Created by KVClassFactory on Sat Nov 24 11:07:04 2012
//Author: gruyer,,,

#ifndef __KVNUCLEUSBOX_H
#define __KVNUCLEUSBOX_H

#include "TBox.h"
#include "KVNucleus.h"
#include "KVNuclearChart.h"

class KVNucleusBox : public TBox
{
   protected:
   Int_t fZ, fN, fA;
   Bool_t fDrawSame;
   Bool_t fOwnNucleus;
   KVNucleus* fNucleus;
   KVNuclearChart* fNuclearChart;

   public:
   KVNucleusBox(Int_t Z, Int_t N, Double_t size=0.4);
   KVNucleusBox(KVNucleus* nuc, Double_t size=0.4, Bool_t owner=kTRUE);
   
   KVNucleusBox (const KVNucleusBox&) ;
   virtual ~KVNucleusBox();
   void Copy (TObject&) const;
   
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);  
   void SetDrawMode(Bool_t DrawSame);
   void SetNuclearChart(KVNuclearChart* nuclearchart){fNuclearChart = nuclearchart;};
   void EnableToolTip();

   ClassDef(KVNucleusBox,1)//A TBox representing a nucleus in a KVNuclearChart
};

#endif
