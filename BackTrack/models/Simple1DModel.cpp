//Created by KVClassFactory on Wed Mar  4 16:50:19 2015
//Author: John Frankland,,,

#include "Simple1DModel.h"

#include <TCanvas.h>
#include <TGraphErrors.h>

ClassImp(BackTrack::Simple1DModel)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Simple1DModel</h2>
<h4>Simple model to test backtrack procedures</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

namespace BackTrack {

   Simple1DModel::Simple1DModel()
      : fPar("PAR","Model parameter",0.,50.),
        fObs("OBS","Model observable",-20,70.),
        a0("a0","a0",0,0,5),a1("a1","a1",1,-1,1),
        MEAN("MEAN","MEAN",fPar,RooArgSet(a0,a1))
   {
      // Default constructor
      AddParameter(fPar.GetName(),fPar.GetTitle(),0.,50.,5);
      AddObservable(fObs.GetName(),fObs.GetTitle(),-20.,70);

      // conditional distribution of observable for a given mean
      theModel = new RooGaussian("Simple1DModel","observable distribution for fixed parameter",fObs,MEAN,RooConst(5)) ;

   }

   Simple1DModel::~Simple1DModel()
   {
      // Destructor
   }

   void Simple1DModel::ConstructPseudoPDF(Bool_t debug)
   {
      GenericModel::ConstructPseudoPDF(debug);
      if(!debug) return;
      // draw datasets and kernels
      for(int i=0; i<GetNumberOfDataSets(); i++){
         const RooArgList* pars = GetParametersForDataset(i);
         RooRealVar* par = (RooRealVar*)pars->find("PAR");
         int pv = (int)par->getVal();
         new TCanvas(Form("c%d",pv),Form("PAR=%d",pv));
         RooPlot* frame = fObs.frame();
         GetDataSet(i)->plotOn(frame);
         GetKernel(i)->plotOn(frame);
         frame->Draw();
      }
   }

   void Simple1DModel::CompareParameterWeights(RooAbsPdf& pdist)
   {
      // After fit, draw weights for each parameter range superposed on the
      // original distribution

      TGraphErrors* graph = new TGraphErrors(GetNumberOfDataSets());
      for(int i=0; i<GetNumberOfDataSets(); i++)
      {
         RooRealVar* w = (RooRealVar*)GetWeights().at(i);
         RooRealVar* p = (RooRealVar*)((RooArgList*)GetDataSetParametersList()->At(i))->find("PAR");
         graph->SetPoint(i, p->getVal(), w->getVal());
         graph->SetPointError(i, 0.5*(p->getMax()-p->getMin()), w->getError());
      }
      graph->SetMarkerStyle(20);
      RooPlot* pl = GetParameter("PAR").frame();
      pdist.plotOn(pl);
      pl->Draw();
      graph->Draw("p");
   }

}
