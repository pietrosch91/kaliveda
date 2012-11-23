//Created by KVClassFactory on Thu Nov 22 15:26:52 2012
//Author: dgruyer

#ifndef __KVNUCLEARCHART_H
#define __KVNUCLEARCHART_H

#include "KVBase.h"
#include "TH2F.h"

class KVNuclearChart : public KVBase
{

   protected:
   Int_t fNmin;
   Int_t fNmax;
   Int_t fZmin;
   Int_t fZmax;
   TH2F* fHisto;

   public:
//    KVNuclearChart();
   KVNuclearChart(Int_t nMin=-1, Int_t nMax=-1, Int_t zMin=-1, Int_t zMax=-1);
   KVNuclearChart (const KVNuclearChart&) ;
   virtual ~KVNuclearChart();
   void Copy (TObject&) const;
   
   void Draw(Option_t* option = "");

   ClassDef(KVNuclearChart,1)//Used to draw nuclear chart
};

#endif
