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
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooPolynomial.h"
#include "RooKeysPdf.h"
#include "RooNDKeysPdf.h"
#include "RooProdPdf.h"
#include "RooPlot.h"
#include "RooAddPdf.h"
using namespace RooFit ;

namespace BackTrack {

   class GenericModel : public KVBase {

      ClassDef(GenericModel,1)//Generic model for backtracing studies

      RooArgList fParameters;  // the parameters of the model
      RooArgList fObservables; // the observables of the data
      Int_t      fNDataSets;   //! internal counter of model datasets
      TObjArray  fDataSetParams;//list model parameters for each dataset
      TObjArray  fDataSets;     //list of model datasets
      TObjArray  fNDKeys;       //pseudo-pdfs for each dataset
      RooAddPdf* fModelPseudoPDF; //pseudo-pdf for model to fit data

      public:
      GenericModel();
      virtual ~GenericModel();

      void AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins);
      Int_t GetNumberOfParameters() const { return fParameters.getSize(); }
      void AddObservable(const char* name, const char* title, Double_t min, Double_t max);
      Int_t GetNumberOfObservables() const { return fParameters.getSize(); }
      const RooArgList& GetObservables() const { return fObservables; }

      void AddModelData(RooArgList& params, RooDataSet* data);
      RooDataSet* GetModelDataSet(RooArgList& par);

      void ImportModelData(Int_t parameter_num = 0, RooArgList* plist = 0);
      Int_t GetNumberOfDataSets() const { return fNDataSets; }
      const TObjArray* GetDataSetParametersList() const { return &fDataSetParams; }

      void ConstructPseudoPDF();
   };

} /* namespace BackTrack */

#endif /* GENERICMODEL_H_ */
