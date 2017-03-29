#include "KVEventSelector.h"
#include "KVZmax.h"

class SimulatedEventAnalysisTemplate : public KVEventSelector {

protected:
   Int_t mult, Z[200], A[200];
   Double_t Vpar[200], Vper[200], E[200], Theta[200], Phi[200];
   KVZmax* ZMAX;

public:
   SimulatedEventAnalysisTemplate() {}
   virtual ~SimulatedEventAnalysisTemplate() {}

   void InitAnalysis();
   void InitRun() {}
   Bool_t Analysis();
   void EndRun() {}
   void EndAnalysis() {}

   ClassDef(SimulatedEventAnalysisTemplate, 1) //event analysis class
};
