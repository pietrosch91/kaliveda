/*
 * GenericModel_Binned.cpp
 *
 *  Created on: Mar 24, 2015
 *      Author: john, quentin
 */

#include "GenericModel_Binned.h"

#include "RooFit.h"
#include "RooCmdConfig.h"
#include "Riostream.h"
#include "TMath.h"
#include "TObjString.h"
#include "TPaveText.h"
#include "TList.h"
#include "TH1.h"
#include "TH2.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooArgProxy.h"
#include "RooRealProxy.h"
#include "RooRealVar.h"
#include "RooGenContext.h"
#include "RooBinnedGenContext.h"
#include "RooPlot.h"
#include "RooCurve.h"
#include "RooNLLVar.h"
#include "RooMinuit.h"
#include "RooCategory.h"
#include "RooNameReg.h"
#include "RooCmdConfig.h"
#include "RooAddition.h"
#include "RooRandom.h"
#include "RooNumIntConfig.h"
#include "RooProjectedPdf.h"
#include "RooInt.h"
#include "RooCustomizer.h"
#include "RooConstraintSum.h"
#include "RooParamBinning.h"
#include "RooNumCdf.h"
#include "RooFitResult.h"
#include "RooNumGenConfig.h"
#include "RooCachedReal.h"
#include "RooXYChi2Var.h"
#include "RooChi2Var.h"
#include "RooMinimizer.h"
#include "RooRealIntegral.h"
#include "Math/CholeskyDecomp.h"
#include <RooAbsBinning.h>
#include <RooUniform.h>

ClassImp(BackTrack::GenericModel_Binned)

namespace BackTrack {

  GenericModel_Binned::GenericModel_Binned()
  {  
    fBool_extended        =kFALSE;    //By default not extended fit
    fBool_saved_workspace =kFALSE;       	  
    fBool_prov_workspace  =kFALSE;    
    fBool_good_workspace  =kFALSE;
    fBool_init_weights    =kFALSE;
    
    fInitWeights = new vector<Double_t>();
    
    fwk_name=0;	  
   
    fNDataSets     =0;
    fModelPseudoPDF=0;
    fLastFit       =0;
    fParameterPDF  =0;
    fParamDataHist =0;
    fSmoothing     =0;
  }

  GenericModel_Binned::~GenericModel_Binned()
  {
    SafeDelete(fParameterPDF);
    SafeDelete(fParamDataHist);

    if(fModelPseudoPDF)
      {
	delete fModelPseudoPDF;
	fHistPdfs.Delete();
	fModelPseudoPDF=0;
	fFractions.removeAll();
      }
      
    if(fNDataSets)
      {
	fDataSets.Delete();
	fDataSetParams.Delete();
	fNDataSets=0;
      }
            
    SafeDelete(fLastFit);
    SafeDelete(fWorkspace);
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void GenericModel_Binned::SetExtended(Bool_t ext)
  {
    //Extended/ Not extended fit
    //By default not extended
    fBool_extended = ext;
  }
  
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void GenericModel_Binned::AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins)
  {
    // Define a named parameter for the model, with its associated range
    // of values and the number of 'bins' to be used to scan these values
    {
      RooRealVar p(name,title,min,max);
      p.setBins(nbins);
	
      ((RooArgList*) fWorkspace->obj("_parameters"))->addClone(p);
      ((RooArgList*) fWorkspace->obj("_parobs"))->addClone(p);
    }   	
  }

  void GenericModel_Binned::AddParameter(const RooRealVar& var, Int_t nbins)
  {
    // Define a named parameter for the model, with its associated range
    // of values and the number of 'bins' to be used to scan these values
    ((RooArgList*) fWorkspace->obj("_parameters"))->addClone(var);
    ((RooArgList*) fWorkspace->obj("_parobs"))->addClone(var);
   
    GetParameter(var.GetName())->setBins(nbins);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void GenericModel_Binned::AddObservable(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins)
  {
    // Define a named observable for the model, with its associated range of values
    RooRealVar p(name,title,min,max);
    p.setBins(nbins);
    ((RooArgList*) fWorkspace->obj("_observables"))->addClone(p);
    ((RooArgList*) fWorkspace->obj("_parobs"))->addClone(p);
  }

  void GenericModel_Binned::AddObservable(const RooRealVar& var, Int_t nbins)
  {
    // Define a named observable for the model, with its associated range of values
    ((RooArgList*) fWorkspace->obj("_observables"))->addClone(var);
    ((RooArgList*) fWorkspace->obj("_parobs"))->addClone(var);
    
    GetObservable(var.GetName())->setBins(nbins);
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  RooArgList& GenericModel_Binned::GetParameters()
  {
    return *((RooArgList*) fWorkspace->obj("_parameters")); 
  }

  RooArgList& GenericModel_Binned::GetObservables()
  {
    return *((RooArgList*) fWorkspace->obj("_observables")); 
  }

  RooArgList& GenericModel_Binned::GetParObs() 
  {
    return *((RooArgList*) fWorkspace->obj("_parobs")); 
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Int_t GenericModel_Binned::GetNumberOfParameters()
  {
    return GetParameters().getSize();
  } 

  Int_t GenericModel_Binned::GetNumberOfObservables()
  {
    return GetObservables().getSize();
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TObjArray* GenericModel_Binned::GetDataSetParametersList()
  {
    return (TObjArray*) fWorkspace->obj("_datasetparams");
  }
  
              
  RooArgList* GenericModel_Binned::GetParametersForDataset(Int_t i)
  {
    return (RooArgList*) GetDataSetParametersList()->At(i); 
  }
  
    
    
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  const TObjArray* GenericModel_Binned::GetDataHistsList()
  {
    // Return the kernel estimation PDF for the i-th imported dataset
    return (TObjArray*) fWorkspace->obj("_datahistset");
  }
  
  const RooDataHist* GenericModel_Binned::GetDataHist(Int_t i)
  {
    // Return the kernel estimation PDF for the i-th imported dataset
    return (RooDataHist*) GetDataHistsList()->At(i);
  } 
    

  Int_t GenericModel_Binned::GetNumberOfDataSets() 
  {  
    const Int_t size = (int) ((TObjArray*) fWorkspace->obj("_datahistset"))->GetEntriesFast();
    return size; 
  }
  
   
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //   const TObjArray* GenericModel_Binned::GetKernelsList()
  //   {
  //     // Return the kernel estimation PDF for the i-th imported dataset
  //     return (TObjArray*) fWorkspace->obj("_histpdfset");
  //   }
  //   
  //   const RooNDKeysPdf* GenericModel_Binned::GetKernel(Int_t i)
  //   {
  //     // Return the kernel estimation PDF for the i-th imported dataset
  //     return (RooNDKeysPdf*) GetKernelsList()->At(i);
  //   }   
  
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void GenericModel_Binned::AddModelData(RooArgList& params, RooDataHist* data)
  {
    // Add a set of data calculated with the model for the given set of parameter values.
    // Keep a copy of the parameter values for saving the fit results
    if(!data) return;   

    ((TObjArray*) fWorkspace->obj("_datahistset"))->Add(data);
    RooArgList* ral = new RooArgList;
    ral->addClone(params);
    ((TObjArray*) fWorkspace->obj("_datasetparams"))->Add(ral);     
  }


  RooDataHist*GenericModel_Binned::GetModelDataHist(RooArgList&)
  {
    // Generate/fill unbinned dataset corresponding to model having
    // the parameter values in the list.

    AbstractMethod("GetModelDataHist");
    return nullptr;
  }


/////////////////////////////////////////////////////////////////////////////////////////////////
  void GenericModel_Binned::ImportModelData(Bool_t save)
  {  
   if(IsWorkspaceProvided()==kTRUE)
      {
        Info("ImportModelData", "RooWorspace for fit already provided, no importation needed !!!");
        return;
      } 
  
   else
      { 
       Int_t parameter_num=0;
       RooArgList* plist=0;
       ImportModelData(parameter_num, plist);
       if(save==kTRUE) SaveInitWorkspace(GetWorkspaceFileName());
       	
//        //debug
//        for(Int_t ii=0; ii< GetNumberOfDataSets();ii++)
//           {
//            RooArgList* listpar = GetParametersForDataset(ii);
//            RooRealVar *par1 = (RooRealVar*) listpar->at(0);
//            RooRealVar *par2 = (RooRealVar*) listpar->at(1);	   
//            Double_t val1 = par1->getVal();
//            Double_t val2 = par2->getVal();
//            Double_t max1 = par1->getMax();
//            Double_t max2 = par2->getMax(); 
//            Double_t min1 = par1->getMin();
//            Double_t min2 = par2->getMin();
//            
//            printf("[datasetparam%d] min1=%e , max1=%e, val1=%e, min2=%e, max2=%e, val2=%e\n", ii, min1, max1, val1, min2, max2, val2); 
//           }        
      }
  }


  void GenericModel_Binned::ImportModelData(Int_t parameter_num, RooArgList* plist)
  {
    // Import all model datasets corresponding to the defined parameters, ranges & binnings  
	RooArgList PLIST;
	if(plist) PLIST.addClone(*plist);
	RooRealVar* par = GetParameter(parameter_num);
	RooAbsBinning& bins = par->getBinning();
	Int_t N = bins.numBins();
	RooRealVar* par_in_list = (RooRealVar*)PLIST.find(par->GetName());
   
	if(!par_in_list)
	  {
	    PLIST.addClone(RooRealVar(par->GetName(),par->GetTitle(),0.));
	    par_in_list = (RooRealVar*)PLIST.find(par->GetName());
	  }
   
	for(int i=0; i<N; i++)
	  {
	    par_in_list->setMax(bins.binHigh(i));
	    par_in_list->setMin(bins.binLow(i));
	    par_in_list->setVal(bins.binCenter(i));
	    	
	    if((parameter_num+1)<GetNumberOfParameters()) ImportModelData(parameter_num+1,&PLIST);
	    
	    else AddModelData(PLIST, GetModelDataHist(PLIST));
	  }	         
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  void GenericModel_Binned::SetInitWeights(vector<Double_t>* weights)
  {
    /*
     * Initialize weights for parameters sets
     * /!\ For an extended fit, the entries of experimental RooHistPdf must be taken into account by the user when creating the weights vector /!\
     * the vector given must have the same size of the product of parameters bins
     * (defined by the user with the AddParameter() method
     * the values must be ranked like in the call of GetModelDataSet().
     */

    Int_t vec_size        = (int) weights->size();
    const Int_t num_data  = GetNumberOfDataSets(); 
     
    if(vec_size==num_data)
      {       
        fBool_init_weights=kTRUE;
	fInitWeights = weights;
      }
        

    else
      {
        Error("SetInitWeights", "...Wrong vector size (size=%d) compared to number of provided model DataSet (n_datasets=%d)...", vec_size, num_data);
	
	//if not-extended fit we can still set uniform parameters
	if(IsExtended()==kFALSE)
	  {
	    Info("SetInitWeights", "...Setting uniform initial values for the not extended fit...");
	    SetUniformInitWeights(1.); 
	  }
	   
	else 
	  {
	   fBool_init_weights=kFALSE;
	   return;
	  }   
      }              	
  }
   
  void GenericModel_Binned::SetUniformInitWeights(Double_t exp_integral)
  {  
    //Initialize the same weights for each parameter set
    //For an extended fit the experimental integral of observable distributions should be provided
    //For a not extended no need to provide the experimental integral
    
    Double_t integral  =1.;
    fBool_init_weights = kFALSE;
        
    if(IsExtended()==kTRUE)
      {
        if(exp_integral<=0)
	  {
	    Error("SetUniformInitWeights", "...Warning extended fit required but given experimental integral is zero/negative...");
	    return;
	  }
	   
	else integral=exp_integral;	     
      }
   
    else
      {
        if(exp_integral!=1.)  Info("SetUniformInitWeights", "...Not extended fit required, experimental integral provided will be ignored...");
      }
     
    //Loop on parameters  
    Double_t ww = integral/GetNumberOfDataSets(); 
                 
    for(Int_t ii=0; ii<GetNumberOfDataSets(); ii++) 
      {
        fInitWeights->push_back(ww);
      } 
  } 
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  void GenericModel_Binned::ConstructPseudoPDF(vector<Double_t> *weights, Bool_t numint, Bool_t save, Bool_t debug) 
  {
    //For a chosen parameter initial distribution
    ConstructPseudoPDF(weights, 1., numint, save, debug);
  }
  
  void GenericModel_Binned::ConstructPseudoPDF(Int_t exp_integral, Bool_t numint, Bool_t save, Bool_t debug)
  {
   //For uniform parameter initial distribution
   ConstructPseudoPDF(0, exp_integral, numint, save, debug);
  } 
  
  
  
  void GenericModel_Binned::ConstructPseudoPDF(vector<Double_t> *weights, Int_t exp_integral, Bool_t numint, Bool_t save, Bool_t debug)
  {
    //Build a parameterised pseudo-PDF from all the imported model datasets.
    //Each dataset will be transformed into a RooHistPdf.  
    //
    //The user can decide to initialize the parameters values using a vector<const Double_t>*
    //For an extended fit the user need to declare the experimental DataSet entries
    //If debug=kTRUE, each dataset and kernel PDF will be displayed in a canvas. 
    //If numint=kTRUE the integrals are force numerical
    
    //-------------Control if DataSets list is given------------
    if(!GetNumberOfDataSets() || GetNumberOfDataSets()==0)
      {
	if(IsWorkspaceProvided()==kFALSE) Error("ConstructPseudoPDF", "... !!! No DataSets !!! Import model datasets with ImportModelData() first...");
	else                              Error("ConstructPseudoPDF", "... !!! No DataSets !!! Verify provided worspace...");
	
	return;
      }    
       
    //-------------For the weights initialization--------------       
    if(weights!=0)  SetInitWeights(weights);        
    else SetUniformInitWeights(exp_integral);


    if(fModelPseudoPDF)
      {
	delete fModelPseudoPDF;
	fModelPseudoPDF=0;
	fHistPdfs.Delete();
	fFractions.removeAll();
      }
   
    //-----------generate kernels------------
    RooArgList kernels;

    //number of the last set added in kernels
    //used for not extended pdf to remove last coef
    Int_t last = -1;

    for(int i=0; i<GetNumberOfDataSets(); i++)
      {                    
        const RooDataHist *set = GetDataHist(i);
	
	//If stat in the DataSet
	//Else error : RooFit can't create the RooHistPdf without statistic
	if(set->sumEntries()>0)
	  {
	    ++fNDataSets;  //For modifying counting if no stat
	    last=i;        //For extended fit  
      
	    RooHistPdf *pdf = new RooHistPdf(Form("HistPdf%d",i), Form("RooHistPdf from datahist#%d",i), GetObservables(), *set, fSmoothing);
	    if(numint==kTRUE) pdf->forceNumInt(kTRUE); 
	 		      
	    ((TObjArray*) fWorkspace->obj("_histpdfset"))->Add(pdf);      
	    kernels.add(*pdf);
	
	    if(debug) Info("ConstructPseudoPDF", "...datahist#%d... added into kernel list (entries=%d)...", i, (int) set->sumEntries());
            
	    //Extended pdf
	    if(IsExtended()==kTRUE)
	      {
		RooRealVar pp(Form("W%d",i),Form("fractional weight of kernel-PDF #%d",i),(*fInitWeights)[i],0.,exp_integral);				   
		fFractions.addClone(pp);	  	      	
	      }
      
	    //Not-Extended pdf
	    else
	      {
		RooRealVar pp(Form("W%d",i),Form("fractional weight of kernel-PDF #%d",i),(*fInitWeights)[i],0.,1.);
		fFractions.addClone(pp);	       
	      }
	  } 
	 
	 
	else
	  {
	    //Remove this parameters set from _datasetparams used to save the results
	    GetDataSetParametersList()->RemoveAt(i);
	    Info("ConstructPseudoPDF", "...datahist#%d... refused (nentries=%d)...", i, (int) set->sumEntries());
	    Info("ConstructPseudoPDF", "...RooArgList%d... removed from _datasetparams for the results...", i);	    
	  } 
      }
   
          
    //-------------Remove last coefficient for not-extended fit-----------
    if(IsExtended()==kFALSE)
      {
	RooRealVar *p = (RooRealVar*) fFractions.find(Form("W%d",last));
	  
	if(p) 
	  {
	    fFractions.remove(*p);
	    Info("ConstructPseudoPdf", "...last dataset (dataset#%d) removed for not-extended fit...", last);
	  } 
	     
	else Error("ConstructPseudoPdf", "...error in finding the last dataset to remove for not-extended fit...");  
      }
   
    //Extended
    if(IsExtended()==kTRUE)
      {
	fModelPseudoPDF = new RooAddPdf("Model", "Pseudo-PDF constructed from kernels for model datahists", kernels, fFractions, kFALSE);
	if(numint==kTRUE) fModelPseudoPDF->forceNumInt();	
      }
      
    else
      {
        //fFractions->remove();
	fModelPseudoPDF = new RooAddPdf("Model", "Pseudo-PDF constructed from kernels for model datahists", kernels, fFractions, kTRUE);
	if(numint==kTRUE) fModelPseudoPDF->forceNumInt();
      }  
      
    if(save==kTRUE)
       { 
         SavePseudoPDF(GetWorkspaceFileName());
       } 
  }
    
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  RooFitResult* GenericModel_Binned::fitTo(RooDataHist& data, const RooCmdArg& arg1, const RooCmdArg& arg2, const RooCmdArg& arg3, const RooCmdArg& arg4, 
					                      const RooCmdArg& arg5, const RooCmdArg& arg6, const RooCmdArg& arg7, const RooCmdArg& arg8)
  {
    //Print informations
    Info("fiTo...","performing fit with %d parameters and %d observables", GetNumberOfParameters(), GetNumberOfObservables());
    for(Int_t ii=0; ii<GetNumberOfParameters(); ii++)
      {
        RooRealVar *p = dynamic_cast<RooRealVar*>(GetParameters().at(ii));
	RooAbsBinning& bins = p->getBinning();
        Int_t N = bins.numBins();
        Info("fiTo...","parameter%d: name=%s, min=%e, max=%e, nbin=%d",ii, p->GetName(),p->getMin(),p->getMax(),N); 
      }
    for(Int_t ii=0; ii<GetNumberOfObservables(); ii++)
      {
        RooRealVar *p = dynamic_cast<RooRealVar*>(GetObservables().at(ii));
	RooAbsBinning& bins = p->getBinning();
        Int_t N = bins.numBins();
        Info("fiTo...","observable%d: name=%s, min=%e, max=%e, nbin=%d",ii, p->GetName(),p->getMin(),p->getMax(),N); 
      }  
    
    //Fit
    fLastFit = fModelPseudoPDF->fitTo(data, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
     
    //Save Coefs
    SafeDelete(fParamDataHist);
    fParamDataHist = new RooDataHist("params","params",GetParameters());
   
    fWeights.removeAll();
    const RooArgList& coefs = fModelPseudoPDF->coefList();
    for(int i=0;i<fNDataSets;i++)
      { 
	RooAbsReal* coef = (RooAbsReal*)coefs.at(i);
	RooRealVar w(Form("w%d",i),Form("Fitted weight of kernel#%d",i),coef->getVal());
	printf("coef%d=%e\n",i, coef->getVal());
	
	if(coef->InheritsFrom(RooRealVar::Class()))
	  {
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
    

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  void GenericModel_Binned::plotOn(RooPlot* frame)
  {
    //     // Add the pseudo-PDF to the RooPlot
    //     // We plot:
    //     //    - the fit result with a 1-sigma error band;
    //     //    - each of the individual kernel PDF's
    // 
    //     //plot fit result with 1-sigma error band
    //     fModelPseudoPDF->plotOn(frame, VisualizeError(*fLastFit,1),FillColor(kMagenta));
    //     fModelPseudoPDF->plotOn(frame);
    //     //plot individual kernels
    //     for(int i=0; i<GetNumberOfDataSets(); i++)
    //       {
    // 	fModelPseudoPDF->plotOn(frame,Components(*GetKernel(i)),LineStyle(kDashed),LineColor(kBlue+4*i));
    //       }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  void GenericModel_Binned::InitWorkspace()
  {   	
    fBool_prov_workspace = kFALSE;
    fBool_good_workspace = kTRUE;
        
    //init new workspace
    Info("initWorkspace ...", "Workspace not given...creating one...");  
    
    fWorkspace = new RooWorkspace("_workspace","RooWorkspace for the fit");
    fWorkspace->import(fParameters, "_parameters");
    fWorkspace->import(fObservables, "_observables");
    fWorkspace->import(fParObs,"_parobs");
    fWorkspace->import(fDataSets,"_datahistset");
    fWorkspace->import(fDataSetParams,"_datasetparams");
    fWorkspace->import(fHistPdfs,"_histpdfset");    
  }
  
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////    
  void GenericModel_Binned::SetWorkspace(RooWorkspace* workspace)
  {  
    fBool_prov_workspace = kTRUE;
     
    RooArgList* par    = NULL;
    RooArgList* obs    = NULL;
    RooArgList* parobs = NULL;
    TObjArray*  dat    = NULL;
    TObjArray*  datpar = NULL;
      
    if(workspace!=0)
      { 
        //------Parameters------       
	par  = (RooArgList*) workspace->obj("_parameters");
	if(par!=0) Info("SetWorkspace ...", "_parameters found...");
	   
	else
	  {
	    Error("SetWorkspace ...", "Workspace provided but _parameters NOT found...");
	    fBool_good_workspace=kFALSE;
	    return;
	  }	 
   
        //------Observables-----
	obs = (RooArgList*) workspace->obj("_observables");
	if(obs!=0) Info("SetWorkspace ...", "_observables found...");
	   
	else
	  {
	    Error("SetWorkspace ...", "Workspace provided but _observables NOT found...");
	    fBool_good_workspace=kFALSE;
	    return;
	  }
       	 
	//----------ParObs-------  
	parobs  = (RooArgList*) workspace->obj("_parobs");
	if(parobs!=0) Info("SetWorkspace ...", "_parobs found...");

	   
	else 
	  {
	    Error("SetWorkspace ...", "Workspace provided but _parobs NOT found...");
	    fBool_good_workspace=kFALSE;
	    return;
	  }	
   
        //-------DataSets------
	dat = (TObjArray*)  workspace->obj("_datahistset");
	if(dat!=0) Info("SetWorkspace ...", "_datahistset found...");
 
	   
	else
	  {
	    Error("SetWorkspace ...", "Workspace provided but _datahistset NOT found...");
	    fBool_good_workspace=kFALSE;
	    return;
	  }
	 
	//-------DataSets Parameters-------
	datpar = (TObjArray*)  workspace->obj("_datasetparams");
	if(datpar!=0) Info("SetWorkspace ...", "_datasetparams found...");

	   
	else
	  {
	    Error("SetWorkspace ...", "Workspace provided but _datasetparams NOT found...");
	    fBool_good_workspace=kFALSE;
	    return;
	  }
	  
	 
	//Workspace creation  
	fWorkspace = new RooWorkspace(Form("_workspace"),"RooWorkspace for the fit");
        fWorkspace->import(*par, "_parameters");
        fWorkspace->import(*obs, "_observables");
        fWorkspace->import(*parobs,"_parobs");
        fWorkspace->import(*dat,"_datahistset");
        fWorkspace->import(*datpar,"_datasetparams");
        fWorkspace->import(fHistPdfs,"_histpdfset");  
	 	  
        fBool_good_workspace = kTRUE;
	par    = NULL;
        obs    = NULL;
        parobs = NULL;
        dat    = NULL;
        datpar = NULL; 
      }
   
    else
      {
        Error("InitWorkspace...", "Empty workspace provided...");
	fBool_good_workspace=kFALSE; 
	return;
      }      		      
  }



  //////////////////////////////////////////////////////////////////////////////////////////////////////////  
  void GenericModel_Binned::SetWorkspaceFileName(char *file)
  {
   fwk_name = file;
  }
  
   
  void GenericModel_Binned::SaveInitWorkspace(char *file)
  {  
    /*
    * To save an initial worksapce for further fits
    * Will save the needed objects for the performing another fit
     without generating the new RooDataSets (can be long)
     will contain: _parameters
                   _obsevables
		   _parobs
		   _datahistset
		   _datasetparams
    * This workspace can then be used with SetWorkspace() method	   
    * By default not saved and saved in a file which name will contain
     observables informations
    * If workspace already provided, will not save it again   
    */ 
      
    if(fBool_prov_workspace==kFALSE)
      {	      
        RooWorkspace *InitWorkspace = new RooWorkspace(Form("init_workspace"),"RooWorkspace for the fit");
	
	RooArgList par     = *((RooArgList*) fWorkspace->obj("_parameters"));
	RooArgList obs     = *((RooArgList*) fWorkspace->obj("_observables"));
        RooArgList parobs  = *((RooArgList*) fWorkspace->obj("_parobs"));
	TObjArray dat      = *((TObjArray*)  fWorkspace->obj("_datahistset"));
	TObjArray datpar   = *((TObjArray*)  fWorkspace->obj("_datasetparams"));
			
	InitWorkspace->import(par, "_parameters");
        InitWorkspace->import(obs, "_observables");
        InitWorkspace->import(parobs,"_parobs");
        InitWorkspace->import(dat,"_datahistset");
	InitWorkspace->import(datpar,"_datasetparams");
	       
        if(file==0)
           {
	   //For name
	    TString ss0("_PARA");
            for(Int_t ii=0;ii<GetNumberOfParameters();ii++)
              {	    	  
	       RooArgList ll = GetParameters();
	       RooRealVar vv = *((RooRealVar*) ll.at(ii));
	       TString ss1(Form("_%s[%.1lf,%.1lf,%d]",vv.GetName(),vv.getMin(),vv.getMax(),vv.getBins()));
	       ss0+=ss1;	    
              }
	
	   TString ss2("_OBS");  
	   for(Int_t ii=0;ii<GetNumberOfObservables();ii++)
             {	    	  
	       RooArgList ll = GetObservables();
	       RooRealVar vv = *((RooRealVar*) ll.at(ii));
	       TString ss3(Form("_%s[%.1lf,%.1lf,%d]",vv.GetName(),vv.getMin(),vv.getMax(),vv.getBins()));
	       ss2+=ss3;	    
             }  
	
	   ss0+=ss2;
		  			  
	   InitWorkspace->writeToFile(Form("workspace%s.root",ss0.Data()),"recreate");
	
	   Info("SaveWorkspace", "Saving created RooDataSet workspace in file 'workspace%s.root'", ss0.Data());
	   fBool_saved_workspace = kTRUE;
	   }
	
	else
	   {
	     InitWorkspace->writeToFile(Form("%s.root",file),"recreate");	
	     Info("SaveWorkspace", "Saving Created RooDatSet workspace in file '%s.root'", file);	
             fBool_saved_workspace = kTRUE;  
	   }
      }
    
    //If workspace given by SetWorkspace() method 
    else
      {
        Info("SaveWorkspace", "RooWorkspace already provided, new one will not be saved");  
	fBool_saved_workspace = kFALSE; 
      }     
  }
  
  
  void GenericModel_Binned::SavePseudoPDF(char *file)
  {  
    //To save the PDF generated with ConstructPseudoPDF() method
  
    if(file==0)
       {       
        fWorkspace->writeToFile("_PseudoPDF.root","recreate");
        Info("ConstructPseudoPdf","...workspace of results saved in file '_fitresults.root'..."); 
       }	 
     
    else
       {
        fWorkspace->writeToFile(Form("%s.root", file), "recreate");
	Info("ConstructPseudoPdf","...workspace of results saved in file '%s.root'...", file); 
       }     
  }
    
    
} 
