//Created by KVClassFactory on Tue Mar 27 21:24:44 2018
//Author: John Frankland,,,

#ifndef __EXAMPLESIMDATAANALYSIS_H
#define __EXAMPLESIMDATAANALYSIS_H

#include "KVEventSelector.h"
#include "KVZmax.h"

class ExampleSimDataAnalysis : public KVEventSelector {

protected:
   Int_t mult, Z[200], A[200];
   Double_t Vpar[200], Vper[200], E[200], Theta[200], Phi[200];
   KVZmax* ZMAX;

public:
   ExampleSimDataAnalysis() {}
   virtual ~ExampleSimDataAnalysis() {}

   void InitAnalysis();
   void InitRun() {}
   Bool_t Analysis();
   void EndRun() {}
   void EndAnalysis() {}

   ClassDef(ExampleSimDataAnalysis, 1) //Analysis of simulated events
};


#endif
