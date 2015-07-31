/*
 * Binned_Binned_GenericModel_2.h
 *
 *  Created on: Mar 3, 2015
 *      Author: john, quentin
 */

#ifndef GENERICMODEL_BINNED_H_
#define GENERICMODEL_BINNED_H_

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
#include "TH2.h"

using namespace RooFit ;

namespace BackTrack {

   class GenericModel_Binned : public KVBase
    {
      ClassDef(GenericModel_Binned,1)//Generic model for backtracing studies

      protected:
      
      RooArgList    fParameters;                 // the parameters of the model
      RooArgList    fObservables;                // the observables of the data
      RooArgList    fParObs;                     // the parameters and observables, used for construction of the dataset from the model tree
      
      Int_t   fNDataSets;		         // internal counter of model datasets  
      Int_t   fSmoothing;		         // histpdf smoothing factor  	 
      Bool_t  fBool_extended;		         // to know if fit is extended or not
      Bool_t  fBool_good_workspace;	         // to know if worksapce was initialized 
      Bool_t  fBool_prov_workspace;	         // to know if we have an imported workspace (for the SaveWorkspace() call)          
      Bool_t  fBool_saved_workspace;		 // to know if workspace is indeed saved with the SaveWorkspace() method
      Bool_t  fBool_init_weights;		 // to know if initial weights are given
      
      
      TObjArray  fDataSetParams;        // list model parameters for each dataset
      TObjArray  fDataSets;             // list of model datasets
      TObjArray  fHistPdfs;             // pseudo-pdfs for each dataset
      RooArgList fWeights;              // fitted weight of each pseudo-pdf in result
      RooAddPdf* fModelPseudoPDF;       // pseudo-pdf for model to fit data
      RooArgList fFractions;            // weights of each kernel in pseudo pdf
      vector<Double_t> *fInitWeights;   // initital weights 
      RooWorkspace *fWorkspace;         // workspace for the fit
      RooFitResult* fLastFit;           // result of last fit
      RooHistPdf*   fParameterPDF;      // pdf for parameters after fit to data
      RooDataHist*  fParamDataHist;     // binned parameter dataset used to construct fParameterPDF
      
      const char* fwk_name;             // name of the initial saved workspace 
      
      
      Bool_t IsExtended() { return fBool_extended; }
      Bool_t IsWorkspaceInitialized() { return fBool_good_workspace; }
      Bool_t IsWorkspaceProvided() { return fBool_prov_workspace; }
      Bool_t IsWorkspaceSaved() { return fBool_saved_workspace; }                  
      void SaveWorkspace(const char* file); 
      
      
            
      public:
      
      GenericModel_Binned();
      virtual ~GenericModel_Binned();
      
      void SetExtended(Bool_t extended=kFALSE);
      void InitWorkspace();  
      void SetWorkspace(RooWorkspace* w);
      void SetSavedWorkspaceName(const char *file);  
      const char* GetSavedWorkspaceName() { return fwk_name; }    
      
      void AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins);
      void AddParameter(const RooRealVar&, Int_t nbins);
      Int_t GetNumberOfParameters();
      void AddObservable(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins);
      void AddObservable(const RooRealVar&, Int_t nbins);
      Int_t GetNumberOfObservables();
      RooRealVar* GetParameter(const char* name) { return dynamic_cast<RooRealVar*> (GetParameters().find(name)); }
      RooRealVar* GetParameter(int num) { return dynamic_cast<RooRealVar*> (GetParameters().at(num)); }	   
      RooRealVar* GetObservable(const char* name) { return  dynamic_cast<RooRealVar*> (GetObservables().find(name)); }
      RooRealVar* GetObservable(int num) { return dynamic_cast<RooRealVar*> (GetObservables().at(num)); }
      
      RooArgList& GetParameters();
      RooArgList& GetObservables();
      RooArgList& GetParObs();
            
      void AddModelData(RooArgList& params, RooDataHist* data);
      virtual RooDataHist* GetModelDataHist(RooArgList& par);

      void ImportModelData(Int_t parameter_num = 0, RooArgList* plist = 0, Bool_t save=kFALSE);
      void ImportModelData(Bool_t save);
      
      Int_t GetNumberOfDataSets();
      TObjArray* GetDataSetParametersList();
      RooArgList* GetParametersForDataset(Int_t i);

      const TObjArray*   GetDataHistsList();
      const RooDataHist* GetDataHist(Int_t i);
      
      //const TObjArray* GetKernelsList();
      //const RooNDKeysPdf* GetKernel(Int_t i);

      void SetInitWeights(vector<Double_t>* vec);
      void SetUniformInitWeights(Double_t exp_integral);
      virtual void ConstructPseudoPDF(vector<Double_t> *weights=0, Int_t exp_integral=1., Bool_t numint=kFALSE, Bool_t save=kFALSE, Bool_t debug=kFALSE);
          
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
   };

} /* namespace BackTrack */

#endif /* GENERICMODEL_BINNED_H_ */
