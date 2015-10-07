/*
 * Binned_Binned_GenericModel_2.h
 *
 *  Created on: Mar 3, 2015
 *      Author: john, quentin
 */

#ifndef __GENERICMODEL_BINNED_H_
#define __GENERICMODEL_BINNED_H_

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
#include "NewRooAddPdf.h"
#include "NewRooFitResult.h"
#include "RooHistPdf.h"
#include "RooChi2Var.h"
#include "NewRooMinuit.h"
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
      
      RooDataHist&  fDataHist;            // Experimental RooDataHist to fit
      RooWorkspace *fWorkspace;           // workspace for the fit
      char* fwk_name;                     // name of the initial saved workspace  
             
      Int_t   fNDataSets;	          // internal counter of model datasets  
      Int_t   fSmoothing;	          // histpdf smoothing factor     
      Bool_t  fBool_extended;	          // extended/not extended fit
      Bool_t  fBool_uniform_weights;      // uniform/not uniform guess for parameters 
      Bool_t  fBool_numint;               // to force integrals to be calculated numerically for the pseudo-PDF
      Bool_t  fBool_saved_workspace;	  // to know if workspace is indeed saved with the SaveWorkspace() method    
        
      RooArgList fWeights;              // fitted weight of each pseudo-pdf in result
      NewRooAddPdf* fModelPseudoPDF;    // pseudo-pdf for model to fit data
      RooArgList fFractions;            // weights of each kernel in pseudo pdf
      vector<Double_t> *fInitWeights;   // initital weights 
      NewRooFitResult* fLastFit;        // result of last fit
      RooHistPdf*   fParameterPDF;      // pdf for parameters after fit to data
      RooDataHist*  fParamDataHist;     // binned parameter dataset used to construct fParameterPDF   
      
          
      
//       RooArgList    fParameters;                 // the parameters of the model
//       RooArgList    fObservables;                // the observables of the data
//       RooArgList    fParObs;                     // the parameters and observables, used for construction of the dataset from the model tree            
//       Bool_t  fBool_good_workspace;	         // to know if worksapce was initialized 
//       Bool_t  fBool_prov_workspace;	         // to know if we have an imported workspace (for the SaveWorkspace() call)                      
//       TObjArray  fDataSetParams;        // list model parameters for each dataset
//       TObjArray  fDataSets;             // list of model datasets
//       TObjArray  fHistPdfs;             // pseudo-pdfs for each dataset

       
      // Internal checking     
      const Bool_t IsExtended() const { return fBool_extended; }
      const Bool_t IsUniformWeights() const { return fBool_uniform_weights; } 
      const Bool_t IsNumInt() const { return fBool_numint; }
      const Bool_t VerifyWorkspace(Bool_t debug=kFALSE);                 
      const Bool_t IsWorkspaceProvided() const { return fBool_prov_workspace; }
      const Bool_t IsWorkspaceSaved() const { return fBool_saved_workspace; } 
      
      // Saving                 
      void SaveInitWorkspace(char* file);
      void SavePseudoPDF(char* file);      
            
	    
	                
    public:
      
      GenericModel_Binned();
      virtual ~GenericModel_Binned();
      
      // Fit caracteristics
      void SetExperimentalDataHist(RooDataHist& data);
      void SetExtended(Bool_t extended=kFALSE);
      void SetNumInt(Bool_t numint=kFALSE); 
      void SetInitWeights(vector<Double_t>* vec);
      void SetUniformInitWeights(Double_t exp_integral);      
      void InitWorkspace(); 
      void SetWorkspace(RooWorkspace* w);
      void SetWorkspaceFileName(char *file);  
      char* GetWorkspaceFileName() { return fwk_name; }    
      
      // Add parameters/Observables
      void AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins);
      void AddParameter(const RooRealVar&, Int_t nbins);
      void AddObservable(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins);
      void AddObservable(const RooRealVar&, Int_t nbins);
      RooRealVar* GetParameter(const char* name) { return dynamic_cast<RooRealVar*> (GetParameters().find(name)); }
      RooRealVar* GetParameter(int num) { return dynamic_cast<RooRealVar*> (GetParameters().at(num)); }	   
      RooRealVar* GetObservable(const char* name) { return  dynamic_cast<RooRealVar*> (GetObservables().find(name)); }
      RooRealVar* GetObservable(int num) { return dynamic_cast<RooRealVar*> (GetObservables().at(num)); } 
      const Int_t GetNumberOfParameters();
      const Int_t GetNumberOfObservables();     
      const RooArgList& GetParameters();
      const RooArgList& GetObservables();
      const RooArgList& GetParObs();
      
      // Model data      
      void AddModelData(RooArgList& params, RooDataHist* data);
      virtual RooDataHist* GetModelDataHist(RooArgList& par);
      void ImportModelData(Int_t parameter_num = 0, RooArgList* plist = 0);
      void ImportModelData(Bool_t save);
      
            
      const Int_t GetNumberOfDataSets();
      TObjArray* GetDataSetParametersList();
      RooArgList* GetParametersForDataset(Int_t i);

      const TObjArray*   GetDataHistsList();
      const RooDataHist* GetDataHist(Int_t i);
      
      //const TObjArray* GetKernelsList();
      //const RooNDKeysPdf* GetKernel(Int_t i);


      // PseudoPDF and Fit
      void  ConstructPseudoPDF(Bool_t save=kFALSE, Bool_t debug=kFALSE);   
      const NewRooAddPdf* GetPseudoPDF() const { return fModelPseudoPDF; }
      const RooArgList& GetPseudoPDFFractions() const { return fFractions; }
      const RooHistPdf* GetParameterPDF() const { return fParameterPDF; }
      const RooDataHist* GetParamDataHist() const { return fParamDataHist; }
      const RooArgList& GetWeights() const { return fWeights; }
      
      NewRooFitResult* fitTo(RooDataHist& data, const RooCmdArg& arg1 = RooCmdArg::none(), const RooCmdArg& arg2 = RooCmdArg::none(), const RooCmdArg& arg3 = RooCmdArg::none(), const RooCmdArg& arg4 = RooCmdArg::none(),
                                                const RooCmdArg& arg5 = RooCmdArg::none(), const RooCmdArg& arg6 = RooCmdArg::none(), const RooCmdArg& arg7 = RooCmdArg::none(), const RooCmdArg& arg8 = RooCmdArg::none(),
						const RooCmdArg& arg9 = RooCmdArg::none(), const RooCmdArg& sarg10 = RooCmdArg::none(), const RooCmdArg& arg11 = RooCmdArg::none(), const RooCmdArg& arg12 = RooCmdArg::none());      						 
      NewRooFitResult* GetLastFit() const { return fLastFit; }
      
      void plotOn(RooPlot*);
   };

} /* namespace BackTrack */

#endif /* GENERICMODEL_BINNED_H_ */
