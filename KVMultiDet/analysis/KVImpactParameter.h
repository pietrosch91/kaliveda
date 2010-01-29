//Created by KVClassFactory on Fri Jan 15 18:14:06 2010
//Author: John Frankland,,,

#ifndef __KVIMPACTPARAMETER_H
#define __KVIMPACTPARAMETER_H
#include "KVBase.h"
#include "TH1.h"
#include "TString.h"
#include "TGraph.h"
#include "TF1.h"
#include "KVHistoManipulator.h"

class KVImpactParameter : public KVBase
{
   TH1* fData; // histogram containing distribution of ip-related observable
   TString fEvol; // how the observable evolves with b
   TGraph* fIPScale; // derived relation between observable and impact-parameter
   TF1 *fObsTransform; // function for transforming observable into impact parameter
   KVHistoManipulator HM;
   
   public:
   KVImpactParameter(TH1*, Option_t* evol = "D");
   virtual ~KVImpactParameter();
   
   void MakeScale(Int_t npoints=100, Double_t bmax = 1.0);
   TGraph* GetScale() const
   {
      // Return pointer to TGraph showing relationship between the observable
      // histogrammed in fData and the impact parameter.
      // Call after MakeScale.
      return fIPScale;
   };
   Double_t BTransform(Double_t* , Double_t*);
   Double_t GetImpactParameter(Double_t obs)
   {
      // Calculate value of reduced impact parameter for given value of the observable.
      return fObsTransform->Eval(obs);
   };
   TH1* GetIPDistribution(TH1* obs, Int_t nbinx=100, Double_t xmin=0, Double_t xmax=1, Option_t* norm="");
   TGraph* GetIPEvolution(TH2* obscor, TString moment, TString axis="Y");

   ClassDef(KVImpactParameter,1)//Impact parameter analysis tools
};

#endif
