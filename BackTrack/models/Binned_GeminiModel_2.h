//Created by KVClassFactory on Tue Mar 24 09:39:10 2015
//Author: Quentin Fable,,,

#ifndef __BINNED_GEMINIMODEL_2_H
#define __BINNED_GEMINIMODEL_2_H
#include "Binned_GenericModel_2.h"
#include "TH1.h"
#include <vector>

namespace BackTrack {

   class Binned_GeminiModel_2 : public Binned_GenericModel_2
   {

    protected:
    TTree *fgem_tree;
    TFile *fout;
    vector<Double_t> *finit_weight;

    public:
    Binned_GeminiModel_2();
    virtual ~Binned_GeminiModel_2();
    
    void InitParObs();
    RooDataHist* GetModelDataHist(RooArgList &obs);
    vector<Double_t>* SetInitialWeightsDistribution(TH2* distri, Double_t nexp_entries=0);
    TH1* GetParameterDistributions();
 
 
    
//     vector<Double_t>* SetInitialWeightsDistributionExtended(Double_t nexp_entries, Bool_t uni=kTRUE);
//     vector<Double_t>* SetInitialWeightsDistribution(Bool_t uni=kTRUE);
   
    ClassDef(Binned_GeminiModel_2,1)//
   };
}
#endif

