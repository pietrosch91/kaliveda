#include "KVEventSelector.h"
#include "KVZmax.h"
#include "KVDataAnalyser.h"

class FilteredEventAnalysisTemplate : public KVEventSelector {

protected:
   Int_t mult, Z[200], A[200], idcode[200], ecode[200], Ameasured[200], array[200];
   Double_t Vpar[200], Vper[200], ELab[200], ThetaLab[200], PhiLab[200];
   KVZmax* ZMAX;

   Bool_t link_to_unfiltered_simulation;

public:
   FilteredEventAnalysisTemplate() {}
   virtual ~FilteredEventAnalysisTemplate() {}

   void InitAnalysis();
   void InitRun();
   Bool_t Analysis();
   void EndRun() {}
   void EndAnalysis() {}

   void SetAnalysisFrame()
   {
      // Calculate centre of mass kinematics
      GetEvent()->SetFrame("CM", gDataAnalyser->GetKinematics()->GetCMVelocity());
   }

   ClassDef(FilteredEventAnalysisTemplate, 1) //event analysis class
};
