/*
 * GenericModel.h
 *
 *  Created on: Mar 3, 2015
 *      Author: john
 */

#ifndef GENERICMODEL_H_
#define GENERICMODEL_H_

#include "KVBase.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooPolynomial.h"
#include "RooKeysPdf.h"
#include "RooNDKeysPdf.h"
#include "RooProdPdf.h"
#include "RooPlot.h"
#include "RooAddPdf.h"
#include "RooFitResult.h"
#include "RooHistPdf.h"

using namespace RooFit ;

namespace BackTrack {

   class GenericModel : public KVBase {

      ClassDef(GenericModel,1)//Generic model for backtracing studies

      protected:
      RooArgList fParameters;    // the parameters of the model
      RooArgList fObservables;   // the observables of the data
      Int_t      fNDataSets;     //! internal counter of model datasets
      TObjArray  fDataSetParams; //list model parameters for each dataset
      TObjArray  fDataSets;      //list of model datasets
      TObjArray  fNDKeys;        //pseudo-pdfs for each dataset
      RooArgList fWeights;       //fitted weight of each pseudo-pdf in result
      RooAddPdf* fModelPseudoPDF;//pseudo-pdf for model to fit data
      RooArgList fFractions;     //weights of each kernel in pseudo pdf
      Double_t      fSmoothing;  //kernel smoothing factor
      RooFitResult* fLastFit;    //result of last fit
      RooHistPdf* fParameterPDF;  //pdf for parameters after fit to data
      RooDataHist* fParamDataHist;//binned parameter dataset used to construct fParameterPDF

      public:
      GenericModel();
      virtual ~GenericModel();

      void AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins);
      void AddParameter(const RooRealVar&, Int_t nbins);
      Int_t GetNumberOfParameters() const { return fParameters.getSize(); }
      void AddObservable(const char* name, const char* title, Double_t min, Double_t max);
      void AddObservable(const RooRealVar&);
      Int_t GetNumberOfObservables() const { return fParameters.getSize(); }
      const RooArgList& GetObservables() const { return fObservables; }
      const RooArgList& GetParameters() const { return fParameters; }
      RooRealVar& GetParameter(const char* name) { return *((RooRealVar*)GetParameters().find(name)); }
      RooRealVar& GetObservable(const char* name) { return *((RooRealVar*)GetObservables().find(name)); }

      void AddModelData(RooArgList& params, RooDataSet* data);
      virtual RooDataSet* GetModelDataSet(RooArgList& par);

      void ImportModelData(Int_t parameter_num = 0, RooArgList* plist = 0);
      Int_t GetNumberOfDataSets() const { return fNDataSets; }
      const TObjArray* GetDataSetParametersList() const { return &fDataSetParams; }
      const RooArgList* GetParametersForDataset(Int_t i) const
      {
         // Return the list of parameters associated to the i-th imported dataset
         return (const RooArgList*)fDataSetParams[i];
      }
      const RooDataSet* GetDataSet(Int_t i) const
      {
         // Return the i-th imported dataset
         return (const RooDataSet*)fDataSets[i];
      }
      const RooNDKeysPdf* GetKernel(Int_t i) const
      {
         // Return the kernel estimation PDF for the i-th imported dataset
         return (const RooNDKeysPdf*)fNDKeys[i];
      }
      void SetKernelSmoothing(Double_t rho)
      {
         // Corresponds to argument 'rho' of RooNDKeysPdf constructor
         // Default: rho = 1.0. Increasing bandwidth scale factor
         // promotes smoothness over detail preservation.
         fSmoothing=rho;
      }


      virtual void ConstructPseudoPDF(Bool_t debug = kFALSE);
      const RooAddPdf* GetPseudoPDF() const { return fModelPseudoPDF; }
      const RooArgList& GetPseudoPDFFractions() const { return fFractions; }
      const RooHistPdf* GetParameterPDF() const { return fParameterPDF; }
      const RooDataHist* GetParamDataHist() const { return fParamDataHist; }
      const RooArgList& GetWeights() const { return fWeights; }

      RooFitResult* fitTo(RooDataSet*);
      RooFitResult* GetLastFit() const { return fLastFit; }
      void plotOn(RooPlot*);
   };

} /* namespace BackTrack */

#endif /* GENERICMODEL_H_ */
