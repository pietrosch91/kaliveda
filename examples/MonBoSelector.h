#ifndef MonBoSelector_h
#define MonBoSelector_h

#include "KVSelector.h"
#include "TH2.h"
#include "KVMultIMF.h" // Ces includes sont indispensables
#include "KVEkin.h"    // car ces variables globales
#include "KVEtrans.h"  // vont etre declarees comme
#include "KVZmean.h"   // champ de la classe MonBoSelector
#include "KVTensP.h"
#include "KVZmax.h"
#include "KVZtot.h"
#include "KVPtot.h"
#include "KVZVtot.h"

class MonBoSelector : public KVSelector {

    private:
     // Variables globales
     KVMultIMF multIMF;
     KVEkin SEkin;
     KVEtrans SEtrans;
     KVZmean Zmean;
     KVTensP tens;
     KVZmax Zmax;
     KVZtot Ztot;
     KVPtot Ptot;
     KVZVtot ZVtot;

     // Histogrammes
     TH2F *hztzvt;
     TH2F *hztpt;
     TH2F *hEtEk;
     TH2F *hZmEk;
     TH2F *hsZEk;
     TH2F *hZEk;
     TH2F *hTfEk;
     TH2F *hZmaxEk;
     TH2F *hZVparZmax;
     TH2F *hSC;
     TH2F *hZmItrieur;

     //Variables de voie d'entree pour les renormalisations
     Double_t PStot,ZStot,ZVStot;

   public:
   MonBoSelector() {};
   ~MonBoSelector() {};
   
   virtual void InitRun();
   virtual void EndRun();
   virtual void InitAnalysis();
   virtual Bool_t Analysis();
   virtual void EndAnalysis();


   ClassDef(MonBoSelector,0)//The prettiest selector of them all
};

#endif
