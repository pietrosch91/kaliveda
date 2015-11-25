//Created by KVClassFactory on Wed Mar  4 16:50:19 2015
//Author: John Frankland,,,

#include "Simple2DModel.h"

ClassImp(BackTrack::Simple2DModel)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Simple2DModel</h2>
<h4>Simple model to test backtrack procedures</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

namespace BackTrack {

   Simple2DModel::Simple2DModel()
   {
      // <obs1> = par1+par2
      // <obs2> = par1-par2
      AddParameter("par1", "parameter #1", 0, 10, 4);
      AddParameter("par2", "parameter #2", -20, 20, 5);
      AddObservable("obs1", "observable #1", -20, 30);
      AddObservable("obs2", "observable #2", -30, 30);
   }

   Simple2DModel::~Simple2DModel()
   {
      // Destructor
   }

   void Simple2DModel::generateEvent(const RooArgList& parameters, RooDataSet& data)
   {
      // generate an event (i.e. a pair of observables) given the parameter values
      // and add it to the dataset

      Double_t par1 = ((RooRealVar*)parameters.at(0))->getVal();
      Double_t par2 = ((RooRealVar*)parameters.at(1))->getVal();
      Double_t obs1 = gRandom->Gaus(par1 + par2, abs(par1 + par2) / 5.);
      Double_t diff = (par1 - par2);
      Double_t obs2 = gRandom->Gaus(diff, abs(diff) / 10.);
      GetObservable("obs1").setVal(obs1);
      GetObservable("obs2").setVal(obs2);
      data.add(GetObservables());
   }

   RooDataSet* Simple2DModel::GetModelDataSet(RooArgList& par)
   {
      // create and fill data set using uniform distributions of parameters in
      // currently defined ranges

      static int dsnum = 1;
      RooDataSet* data = new RooDataSet(Form("DATA#%d", dsnum++), "dataset", GetObservables());
      for (int i = 0; i < GetNumGen(); i++) {
         RooRealVar* par1 = (RooRealVar*)par.at(0);
         RooRealVar* par2 = (RooRealVar*)par.at(1);
         Double_t P1 = gRandom->Uniform(par1->getMin(), par1->getMax());
         Double_t P2 = gRandom->Uniform(par2->getMin(), par2->getMax());
         par1->setVal(P1);
         par2->setVal(P2);
         generateEvent(par, *data);
      }
      return data;
   }

   TH1* Simple2DModel::GetParameterDistributions()
   {
      // Return 2D histo of fitted parameter distributions

      TH1* h = GetParamDataHist()->createHistogram("par1,par2", GetParameter("par1").getBins(), GetParameter("par2").getBins());
      h->SetName("fittedParameters");
      return h;
   }

}
