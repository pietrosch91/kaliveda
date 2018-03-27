//Created by KVClassFactory on Tue Mar 27 21:25:05 2018
//Author: John Frankland,,,

#ifndef __EXAMPLEFILTEREDSIMDATAANALYSIS_H
#define __EXAMPLEFILTEREDSIMDATAANALYSIS_H

#include "KVEventSelector.h"
#include "KVZmax.h"
#include "KVDataAnalyser.h"

class ExampleFilteredSimDataAnalysis : public KVEventSelector {

protected:
   Int_t mult, Z[200], A[200], idcode[200], ecode[200], Ameasured[200], array[200];
   Double_t Vpar[200], Vper[200], ELab[200], ThetaLab[200], PhiLab[200];
   KVZmax* ZMAX;

public:
   ExampleFilteredSimDataAnalysis() {}
   virtual ~ExampleFilteredSimDataAnalysis() {}

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

   ClassDef(ExampleFilteredSimDataAnalysis, 1) //Analysis of filtered simulated events
};


#endif
