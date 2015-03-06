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

BackTrack::Simple2DModel* model=0;

void simple2Dmodel_ex1()
{
   // Simple1DModel has a PDF for the observable OBS which has a gaussian
   // distribution about a mean value given by the value of
   // parameter PAR; PAR can have any distribution
   model = new BackTrack::Simple2DModel();
   
   // both parameters are Gaussian-distributed,
   // but the mean of PAR2 decreases with increasing PAR1
   RooRealVar a0("a0","a0",50,0,50) ;
   RooRealVar a1("a1","a1",-1,-1,1) ;
   RooPolyVar fx("fx","fx",model->GetParameter("PAR1"),RooArgSet(a0,a1)) ;
   RooGaussian gaussy("gy","Gaussian with shifting mean",model->GetParameter("PAR2"),
         fx,RooConst(20.)) ;
   RooGaussian gx("gx","Model for PAR1",model->GetParameter("PAR1"),
         RooConst(38.),RooConst(20.)) ;
   // Create gaussy(y,sy|x) * gx(x)
   RooProdPdf PARdist("PARdist","model(x|y)*gx(x)",gx,RooFit::Conditional(gaussy,model->GetParameter("PAR2"))) ;
   
   // Generate data with correlated parameter distribution
   model->SetNumGen(1000);
   RooDataSet* data = model->GenerateDataSet(PARdist);
   TH1* hh_data = data->createHistogram("OBS1,OBS2",50,50) ;
   
   hh_data->Draw("col");
}
