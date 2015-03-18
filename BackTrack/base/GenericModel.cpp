/*
 * GenericModel.cpp
 *
 *  Created on: Mar 3, 2015
 *      Author: john
 */

#include "GenericModel.h"

#include <RooAbsBinning.h>
#include <RooUniform.h>

ClassImp(BackTrack::GenericModel)

namespace BackTrack {

GenericModel::GenericModel()
{
   fNDataSets=0;
   fSmoothing=1;
   fModelPseudoPDF=0;
   fLastFit=0;
   fParameterPDF=0;
   fParamDataHist=0;
}

GenericModel::~GenericModel()
{
   SafeDelete(fParameterPDF);
   SafeDelete(fParamDataHist);

   if(fModelPseudoPDF){
      delete fModelPseudoPDF;
      fModelPseudoPDF=0;
      fNDKeys.Delete();
      fFractions.removeAll();
   }
   if(fNDataSets){
      fDataSets.Delete();
      fDataSetParams.Delete();
      fNDataSets=0;
   }
   SafeDelete(fLastFit);
}

void GenericModel::AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins)
{
   // Define a named parameter for the model, with its associated range
   // of values and the number of 'bins' to be used to scan these values
    RooRealVar p(name,title,min,max);
    p.setBins(nbins);
    fParameters.addClone(p);
}

void GenericModel::AddParameter(const RooRealVar& var, Int_t nbins)
{
   // Define a named parameter for the model, with its associated range
   // of values and the number of 'bins' to be used to scan these values
   fParameters.addClone(var);
   GetParameter(var.GetName()).setBins(nbins);
}

void GenericModel::AddObservable(const char* name, const char* title, Double_t min, Double_t max)
{
   // Define a named observable for the model, with its associated range of values
    RooRealVar p(name,title,min,max);
    fObservables.addClone(p);
}

void GenericModel::AddObservable(const RooRealVar& var)
{
   // Define a named observable for the model, with its associated range of values
    fObservables.addClone(var);
}

void GenericModel::AddModelData(RooArgList& params, RooDataSet* data)
{
   // Add a set of data calculated with the model for the given set of parameter values.

   if(!data) return;

   fDataSets.Add(data);
   RooArgList* ral = new RooArgList;
   ral->addClone(params);
   fDataSetParams.Add(ral);//keep a copy of the parameter values
   ++fNDataSets;
}

RooDataSet*GenericModel::GetModelDataSet(RooArgList&)
{
   // Generate/fill unbinned dataset corresponding to model having
   // the parameter values in the list.

   AbstractMethod("GetModelDataSet");
   return nullptr;
}

void GenericModel::ImportModelData(Int_t parameter_num, RooArgList* plist)
{
   // Import all model datasets corresponding to the defined parameters, ranges & binnings
   RooArgList PLIST;
   if(plist) PLIST.addClone(*plist);
   RooRealVar* par = dynamic_cast<RooRealVar*>(fParameters.at(parameter_num));
   RooAbsBinning& bins = par->getBinning();
   Int_t N = bins.numBins();
   RooRealVar* par_in_list = (RooRealVar*)PLIST.find(par->GetName());
   if(!par_in_list) {
      PLIST.addClone(RooRealVar(par->GetName(),par->GetTitle(),0.));
      par_in_list = (RooRealVar*)PLIST.find(par->GetName());
   }
   for(int i=0; i<N; i++){
      par_in_list->setMax(bins.binHigh(i));
      par_in_list->setMin(bins.binLow(i));
      par_in_list->setVal(bins.binCenter(i));
      if((parameter_num+1)<GetNumberOfParameters())
         ImportModelData(parameter_num+1,&PLIST);
      else
      {
         AddModelData(PLIST, GetModelDataSet(PLIST));
      }
   }
}

void GenericModel::ConstructPseudoPDF(Bool_t)
{
   // Build a parameterised pseudo-PDF from all the imported model datasets.
   // Each dataset is transformed into a RooNDKeysPdf kernel estimation p.d.f.
   // This p.d.f. models the distribution of an arbitrary input dataset as a
   // superposition of Gaussian kernels, one for each data point, each
   // contributing 1/N to the total integral of the p.d.f.
   //
   // If debug=kTRUE, each dataset and kernel PDF will be displayed in a canvas.
   // This can help to decide if the kernel smoothing parameter needs attention.

   if(!GetNumberOfDataSets()){
      Error("ConstructPseudoPDF", "import model datasets with ImportModelData() first");
      return;
   }

   if(fModelPseudoPDF){
      delete fModelPseudoPDF;
      fModelPseudoPDF=0;
      fNDKeys.Delete();
      fFractions.removeAll();
   }
   //generate kernels
   RooArgList kernels;
   Double_t initialWeight = 1./GetNumberOfDataSets();

   for(int i=0; i<GetNumberOfDataSets(); i++){

      Info("ConstructPseudoPDF", "Kernel estimation of dataset#%d...", i);
      RooNDKeysPdf* p = new RooNDKeysPdf(Form("NDK%d",i),
                                         Form("Kernel estimation of dataset#%d",i),
                                         GetObservables(),
                                         *((RooDataSet*)fDataSets[i]),
                                         "am", fSmoothing);
      fNDKeys.Add(p);
      kernels.add(*p);
      if(i<(GetNumberOfDataSets()-1))
         fFractions.addClone(RooRealVar(Form("W%d",i),Form("fractional weight of kernel-PDF #%d",i),initialWeight,0.,1.));

   }
   fModelPseudoPDF = new RooAddPdf("Model", "Pseudo-PDF constructed from kernels for model datasets", kernels, fFractions, kTRUE);
}

RooFitResult* GenericModel::fitTo(RooDataSet* data)
{
   // Perform fit of the pseudo-PDF to the data
   // On multi-core machines, this automatically uses all available processor cores

   SafeDelete(fLastFit);
#ifdef WITH_MULTICORE_CPU
   fLastFit = fModelPseudoPDF->fitTo(*data,Save(),NumCPU(WITH_MULTICORE_CPU));
#else
   fLastFit = fModelPseudoPDF->fitTo(*data,Save());
#endif

   SafeDelete(fParamDataHist);
   fParamDataHist = new RooDataHist("params","params",GetParameters());

   // store weights of component pdfs => distribution of parameters
   fWeights.removeAll();
   const RooArgList& coefs = fModelPseudoPDF->coefList();
   for(int i=0;i<GetNumberOfDataSets();i++){
      RooAbsReal* coef = (RooAbsReal*)coefs.at(i);
      RooRealVar w(Form("w%d",i),Form("Fitted weight of kernel#%d",i),coef->getVal());
      if(coef->InheritsFrom(RooRealVar::Class())){
         w.setError(((RooRealVar*)coef)->getError());
      }
      else
      {
         w.setError(coef->getPropagatedError(*fLastFit));
      }
      fWeights.addClone(w);
      fParamDataHist->set(*GetParametersForDataset(i),w.getVal(),w.getError());
   }

   SafeDelete(fParameterPDF);
   fParameterPDF = new RooHistPdf("paramPDF","paramPDF",GetParameters(),*fParamDataHist);

   return fLastFit;
}

void GenericModel::plotOn(RooPlot* frame)
{
   // Add the pseudo-PDF to the RooPlot
   // We plot:
   //    - the fit result with a 1-sigma error band;
   //    - each of the individual kernel PDF's

   //plot fit result with 1-sigma error band
   fModelPseudoPDF->plotOn(frame, VisualizeError(*fLastFit,1),FillColor(kMagenta));
   fModelPseudoPDF->plotOn(frame);
   //plot individual kernels
   for(int i=0; i<GetNumberOfDataSets(); i++){
      fModelPseudoPDF->plotOn(frame,Components(*GetKernel(i)),LineStyle(kDashed),LineColor(kBlue+4*i));
   }
}

} /* namespace BackTrack */
