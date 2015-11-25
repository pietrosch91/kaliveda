//# Example of use of class BackTrack::Simple2DModel
/* simple2Dmodel_ex1.C
 *
 * Example of use of class BackTrack::Simple2DModel in order to demonstrate
 * BackTracking fit of model parameter distribution.
 *
 * Simple1DModel has 1 parameter and 1 observable; the observable is gaussian-
 * distributed about a mean value given by the parameter (width is fixed).
 * In this example we generate a dataset for the observable using an exponential
 * distribution of the parameter; BackTracking is then performed in order to
 * retrieve the initial parameter distribution
*/
#include "RooRealVar.h"
#include "RooConstVar.h"
#include "RooDataSet.h"
#include "RooGenericPdf.h"
#include "RooProdPdf.h"
#include "RooPlot.h"
#include "RooPolyVar.h"
#include "RooGaussian.h"
#include "RooUniform.h"
#include "RooExponential.h"
#include "TCanvas.h"
#include "TH1.h"

#include "Simple2DModel.h"

using namespace RooFit;

BackTrack::Simple2DModel* model = 0;

void simple2Dmodel_ex1()
{
   model = new BackTrack::Simple2DModel();

   // Generate data with correlated parameter distribution
   model->SetNumGen(5000);
   RooDataSet data("data", "data to be fitted", model->GetObservables());
   RooDataSet params("params", "input parameters", model->GetParameters());
   RooRealVar& PAR1 = model->GetParameter("par1");
   RooRealVar& PAR2 = model->GetParameter("par2");
   for (int i = 0; i < model->GetNumGen(); i++) {
      Double_t par1 = gRandom->Gaus(5, 2);
      Double_t par2 = gRandom->Gaus(0, 5);
      PAR1.setVal(par1);
      PAR2.setVal(par2);
      model->generateEvent(RooArgList(PAR1, PAR2), data);
      params.add(model->GetParameters(), 1. / 5000.);
   }
   TH1* hh_data = data.createHistogram("obs1,obs2", 50, 50) ;
   hh_data->Draw("col");

   TH1* hh_params = params.createHistogram("par1,par2", PAR1.getBins(), PAR2.getBins()) ;
   new TCanvas;
   hh_params->SetName("inputParameters");
   hh_params->Draw("lego");

   // Initialise the BackTracking
   model->SetNumGen(1000);
   model->ImportModelData();
   model->SetKernelSmoothing(2);
   model->ConstructPseudoPDF();

   model->fitTo(&data);

   new TCanvas;
   model->GetParameterDistributions()->Draw("lego");

   RooPlot* p = PAR1.frame();
   model->GetParamDataHist()->plotOn(p, MarkerColor(kRed));
   params.plotOn(p);
   new TCanvas;
   p->Draw();
   p = PAR2.frame();
   model->GetParamDataHist()->plotOn(p, MarkerColor(kRed));
   params.plotOn(p);
   new TCanvas;
   p->Draw();

//   p = model->GetObservable("obs1").frame();
//   data.plotOn(p);
//   model->GetPseudoPDF()->plotOn(p);
//   TCanvas*c = new TCanvas;
//   c->Divide(2,1);
//   c->cd(1);
//   p->Draw();
//   p = model->GetObservable("obs2").frame();
//   data.plotOn(p);
//   model->GetPseudoPDF()->plotOn(p);
//   c->cd(2);
//   p->Draw();
}

