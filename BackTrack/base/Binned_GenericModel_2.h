/*
 * Binned_Binned_GenericModel_2.h
 *
 *  Created on: Mar 3, 2015
 *      Author: john, quentin
 */

#ifndef GENERICMODEL_H_
#define GENERICMODEL_H_

#include "KVBase.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataSet.h"
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
#include "RooChi2Var.h"
#include "RooMinuit.h"
#include "RooExtendPdf.h"
#include "TFile.h"
#include "RooWorkspace.h"
#include "KVNumberList.h"

using namespace RooFit ;

namespace BackTrack {

   class Binned_GenericModel_2 : public KVBase {

      ClassDef(Binned_GenericModel_2,1)//Generic model for backtracing studies

      protected:
      
      RooArgList fParameters;           // the parameters of the model
      RooArgList fObservables;          // the observables of the data
      RooArgList fParObs;               // the parameters and observables, used for construction of the dataset from the model tree
      Int_t      fNDataSets;            // internal counter of model datasets
      Bool_t     fBool_extended;        // extended/not extended fit
      Bool_t     fBool_workspace;       // to know if we have an imported workspace
      Bool_t     fBool_good_workspace;  // to check if the workspace is correctly defined
      TObjArray  fDataSetParams;        // list model parameters for each dataset
      TObjArray  fDataSets;             // list of model datasets
      TObjArray  fHistPdfs;             // pseudo-pdfs for each dataset
      RooArgList fWeights;              // fitted weight of each pseudo-pdf in result
      RooAddPdf* fModelPseudoPDF;       // pseudo-pdf for model to fit data
      RooArgList fFractions;            // weights of each kernel in pseudo pdf
      Int_t   fSmoothing;               // histpdf smoothing factor
      vector<Double_t> *fInitWeights;   // initital weights 
      KVNumberList *fRefusedList;       // list of refused DataSet numbers (nentries=0)
      RooWorkspace *fWorkspace;         // workspace for the fit
      RooFitResult* fLastFit;           // result of last fit
      RooHistPdf* fParameterPDF;        // pdf for parameters after fit to data
      RooDataHist* fParamDataHist;      // binned parameter dataset used to construct fParameterPDF

      public:
      
      Binned_GenericModel_2();
      virtual ~Binned_GenericModel_2();
      
      void SetExtended(Bool_t extended=kFALSE);
      Bool_t IsExtended() {return fBool_extended; }
      void AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins);
      void AddParameter(const RooRealVar&, Int_t nbins);
      Int_t GetNumberOfParameters();
      void AddObservable(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins);
      void AddObservable(const RooRealVar&, Int_t nbins);
      Int_t GetNumberOfObservables();
      RooRealVar& GetParameter(const char* name) { return *((RooRealVar*)GetParameters().find(name)); }
      RooRealVar& GetObservable(const char* name) { return *((RooRealVar*)GetObservables().find(name)); }
      const RooArgList& GetParameters();
      const RooArgList& GetObservables();
      const RooArgList& GetParObs();

      void AddModelData(RooArgList& params, RooDataHist* data);
      virtual RooDataHist* GetModelDataHist(RooArgList& par);

      void ImportModelData(Int_t parameter_num = 0, RooArgList* plist = 0);
      Int_t GetNumberOfDataSets();
      const TObjArray* GetDataSetParametersList();
      const RooArgList* GetParametersForDataset(Int_t i);

      const TObjArray* GetDataHistsList();
      const RooDataHist* GetDataHist(Int_t i);
      
      //const TObjArray* GetKernelsList();
      //const RooNDKeysPdf* GetKernel(Int_t i);

      Bool_t initWorkspace();  
      Bool_t initWorkspace(RooWorkspace* w);

      virtual void ConstructPseudoPDF(Double_t ndata_entries, Bool_t debug = kFALSE, Bool_t uni = kTRUE);
      virtual vector<Double_t>* SetInitialWeightsDistribution(TH2* distri, Double_t ndata_entries);      
      
//      virtual vector<Double_t>* SetInitialWeightsDistributionExtended(Double_t ndata_entries, Bool_t uni=kTRUE);
//      virtual vector<Double_t>* SetInitialWeightsDistribution(Bool_t uni=kTRUE);
//      vector<Double_t>* InitWeights(Double_t ndata_entries, Bool_t uni=kTRUE);
      
      const RooAddPdf* GetPseudoPDF() const { return fModelPseudoPDF; }
      const RooArgList& GetPseudoPDFFractions() const { return fFractions; }
      const RooHistPdf* GetParameterPDF() const { return fParameterPDF; }
      const RooDataHist* GetParamDataHist() const { return fParamDataHist; }
      const RooArgList& GetWeights() const { return fWeights; }
      
      RooFitResult* fitTo(RooDataHist& data, const RooCmdArg& arg1 = RooCmdArg::none(), const RooCmdArg& arg2 = RooCmdArg::none(), const RooCmdArg& arg3 = RooCmdArg::none(), const RooCmdArg& arg4 = RooCmdArg::none(), const RooCmdArg& arg5 = RooCmdArg::none(), const RooCmdArg& arg6 = RooCmdArg::none(), const RooCmdArg& arg7 = RooCmdArg::none(), const RooCmdArg& arg8 = RooCmdArg::none(), const RooCmdArg& arg9 = RooCmdArg::none(), const RooCmdArg& arg10 = RooCmdArg::none());      
      RooFitResult* fitTo(RooDataHist& data, const RooLinkedList& cmdList);
      RooFitResult* chi2FitTo(RooDataHist& data, const RooCmdArg& arg1 = RooCmdArg::none(), const RooCmdArg& arg2 = RooCmdArg::none(), const RooCmdArg& arg3 = RooCmdArg::none(), const RooCmdArg& arg4 = RooCmdArg::none(), const RooCmdArg& arg5 = RooCmdArg::none(), const RooCmdArg& arg6 = RooCmdArg::none(), const RooCmdArg& arg7 = RooCmdArg::none(), const RooCmdArg& arg8 = RooCmdArg::none(), const RooCmdArg& arg9 = RooCmdArg::none(), const RooCmdArg& arg10 = RooCmdArg::none()); 
      RooFitResult* chi2FitTo(RooDataHist& data, const RooLinkedList& cmdList);						 
      RooFitResult* GetLastFit() const { return fLastFit; }
      
      void plotOn(RooPlot*);
      void saveWorkspace();
   };

} /* namespace BackTrack */

#endif /* GENERICMODEL_H_ */
