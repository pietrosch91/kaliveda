//Created by KVClassFactory on Thu Jun 21 17:01:26 2012
//Author: John Frankland,,,

#ifndef __FilteredEventAnalysis_H
#define __FilteredEventAnalysis_H

#include "KVEventSelector.h"

class FilteredEventAnalysis : public KVEventSelector {

protected:
   Int_t MCHA, Z[200], A[200];
   Float_t Vper[200], Vpar[200];
   Float_t Ek[200], Theta[200], Phi[200];
   Float_t Vperl[200], Vparl[200];
   Float_t Ekl[200], Thetal[200], Phil[200];
   TVector3 fCMVelocity;
   TTree* fTree;
public:
   FilteredEventAnalysis();
   virtual ~FilteredEventAnalysis();

   Bool_t Analysis();
   void InitAnalysis();
   void EndAnalysis();
   void EndRun();
   void InitRun();
   void SetAnalysisFrame() {
      // When analyzing filtered data, we need to calculate the
      // centre of mass frame according to the current system
      // before analysis of the event (global variables etc.)
      if (IsOptGiven("System")) GetEvent()->SetFrame("CM", fCMVelocity);
   }

   ClassDef(FilteredEventAnalysis, 1) //Example of KVEventSelector for analysis of simulated or filtered simulated data
};

#endif
