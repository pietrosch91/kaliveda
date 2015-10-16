//# Example of use of class BackTrack::Simple1DModel
/* simple1Dmodel_ex1.C
 *
 * Example of use of class BackTrack::Simple1DModel in order to demonstrate
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

#include "Simple1DModel.h"

using namespace RooFit;

BackTrack::Simple1DModel* model=0;

void simple1Dmodel_ex1()
{
   // Simple1DModel has a PDF for the observable OBS which has a gaussian
   // distribution about a mean value given by the value of
   // parameter PAR; PAR can have any distribution
   model = new BackTrack::Simple1DModel();
   
   // Exponential parameter distirbution
   RooExponential PARdist("PARdist", "parameter distribution",
         model->GetParameter("PAR"), RooConst(-.05));
   
   // Generate data with exponential parameter distribution
   model->SetNumGen(1000);
   RooDataSet* data = model->GenerateDataSet(PARdist);
   
   // Initialise the BackTracking
   model->SetNumGen(1000);
   model->ImportModelData();
   model->SetKernelSmoothing(2);
   model->ConstructPseudoPDF();
   
   // Do the fit i.e. the BackTracking
   model->fitTo(data);
   
   // Plot the data and the fitted result
   RooPlot* fr = model->GetObservable("OBS").frame();
   data->plotOn(fr);
   model->plotOn(fr);
   fr->Draw();

   new TCanvas;
   model->CompareParameterWeights(PARdist);
}
