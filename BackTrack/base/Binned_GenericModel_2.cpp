/*
 * Binned_GenericModel_2.cpp
 *
 *  Created on: Mar 24, 2015
 *      Author: john, quentin
 */

#include "Binned_GenericModel_2.h"

#include "RooCmdConfig.h"
#include "TClass.h"
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

ClassImp(BackTrack::Binned_GenericModel_2)

namespace BackTrack {

  Binned_GenericModel_2::Binned_GenericModel_2()
  {  
    fBool_workspace=kFALSE;
   
    fNDataSets     =0;
    fModelPseudoPDF=0;
    fLastFit       =0;
    fParameterPDF  =0;
    fParamDataHist =0;
    fSmoothing     =0;
    fRefusedList = new KVNumberList();
  }

  Binned_GenericModel_2::~Binned_GenericModel_2()
  {
    //ffile->Close();

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
      
    if(fRefusedList) SafeDelete(fRefusedList);
      
      
    SafeDelete(fLastFit);
    SafeDelete(fWorkspace);
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Binned_GenericModel_2::SetExtended(Bool_t ext)
  {
    fBool_extended = ext;
    
    if(IsExtended()==kTRUE) Info("Starting creating RooHistPdf...", "Extended fit...");
    else Info("Starting creating RooHistPdf...", "Not-Extended fit...");
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Binned_GenericModel_2::AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins)
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

  void Binned_GenericModel_2::AddParameter(const RooRealVar& var, Int_t nbins)
  {
    // Define a named parameter for the model, with its associated range
    // of values and the number of 'bins' to be used to scan these values
    ((RooArgList*) fWorkspace->obj("_parameters"))->addClone(var);
    ((RooArgList*) fWorkspace->obj("_parobs"))->addClone(var);
   
    GetParameter(var.GetName()).setBins(nbins);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Binned_GenericModel_2::AddObservable(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins)
  {
    // Define a named observable for the model, with its associated range of values
    RooRealVar p(name,title,min,max);
    p.setBins(nbins);
    ((RooArgList*) fWorkspace->obj("_observables"))->addClone(p);
    ((RooArgList*) fWorkspace->obj("_parobs"))->addClone(p);
  }

  void Binned_GenericModel_2::AddObservable(const RooRealVar& var, Int_t nbins)
  {
    // Define a named observable for the model, with its associated range of values
    ((RooArgList*) fWorkspace->obj("_observables"))->addClone(var);
    ((RooArgList*) fWorkspace->obj("_parobs"))->addClone(var);
    
    GetObservable(var.GetName()).setBins(nbins);
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  const RooArgList& Binned_GenericModel_2::GetParameters()
  {
    return *((RooArgList*) fWorkspace->obj("_parameters")); 
  }

  const RooArgList& Binned_GenericModel_2::GetObservables()
  {
    return *((RooArgList*) fWorkspace->obj("_observables")); 
  }

  const RooArgList& Binned_GenericModel_2::GetParObs() 
  {
    return *((RooArgList*) fWorkspace->obj("_parobs")); 
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Int_t Binned_GenericModel_2::GetNumberOfParameters()
  {
    return GetParameters().getSize();
  } 

  Int_t Binned_GenericModel_2::GetNumberOfObservables()
  {
    return GetObservables().getSize();
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  const TObjArray* Binned_GenericModel_2::GetDataSetParametersList()
  {
    return (TObjArray*) fWorkspace->obj("_datasetparams");
  }
  
    
  const RooArgList* Binned_GenericModel_2::GetParametersForDataset(Int_t i)
  {
    return (RooArgList*) GetDataSetParametersList()->At(i); 
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //   const TObjArray* Binned_GenericModel_2::GetKernelsList()
  //   {
  //     // Return the kernel estimation PDF for the i-th imported dataset
  //     return (TObjArray*) fWorkspace->obj("_histpdfset");
  //   }
  //   
  //   const RooNDKeysPdf* Binned_GenericModel_2::GetKernel(Int_t i)
  //   {
  //     // Return the kernel estimation PDF for the i-th imported dataset
  //     return (RooNDKeysPdf*) GetKernelsList()->At(i);
  //   } 
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  const TObjArray* Binned_GenericModel_2::GetDataHistsList()
  {
    // Return the kernel estimation PDF for the i-th imported dataset
    return (TObjArray*) fWorkspace->obj("_dataset");
  }
  
  const RooDataHist* Binned_GenericModel_2::GetDataHist(Int_t i)
  {
    // Return the kernel estimation PDF for the i-th imported dataset
    return (RooDataHist*) GetDataHistsList()->At(i);
  } 
  
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Binned_GenericModel_2::AddModelData(RooArgList& params, RooDataHist* data)
  {
    // Add a set of data calculated with the model for the given set of parameter values.

    if(!data) return;   

    ((TObjArray*) fWorkspace->obj("_dataset"))->Add(data);
    RooArgList* ral = new RooArgList;
    ral->addClone(params);
    ((TObjArray*) fWorkspace->obj("_datasetparams"))->Add(ral); //keep a copy of the parameter values
     
  }


  RooDataHist*Binned_GenericModel_2::GetModelDataHist(RooArgList&)
  {
    // Generate/fill unbinned dataset corresponding to model having
    // the parameter values in the list.

    AbstractMethod("GetModelDataHist");
    return nullptr;
  }

  void Binned_GenericModel_2::ImportModelData(Int_t parameter_num, RooArgList* plist)
  {
    // Import all model datasets corresponding to the defined parameters, ranges & binnings
    if(fBool_workspace==kTRUE)
      {
        Error("ImportModelData", "RooWorspace for fit already provided, no importation needed (check object names)");
        return;
      }
   
    RooArgList PLIST;
    if(plist) PLIST.addClone(*plist);
    RooRealVar* par = dynamic_cast<RooRealVar*>(((RooArgList*) fWorkspace->obj("_parameters"))->at(parameter_num));
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
	if((parameter_num+1)<GetNumberOfParameters())
	  ImportModelData(parameter_num+1,&PLIST);
	else
	  { 
	    AddModelData(PLIST, GetModelDataHist(PLIST));
	  }
      }
  }


  Int_t Binned_GenericModel_2::GetNumberOfDataSets() 
  {  
    Int_t size = (int) ((TObjArray*) fWorkspace->obj("_dataset"))->GetEntriesFast();
    
    if((int) fInitWeights->size()==size)
      {
	return size;
      }
       
    else
      {
        Error("GetNumberOfDataSets", "Number of DataSets is different from number of weights, use InitWeights() first");
	return -1;
      }      
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  vector<Double_t>*Binned_GenericModel_2::SetInitialWeightsDistribution(TH1* distri, Double_t nexp_entries)
  {
    // Initialize weights for parameters sets for fit according to the given distribution "distri" 
    // nexp_entries is by default 0. It must be equal to the experimental integral for an extended fit

    AbstractMethod("SetInitialWeightsDistribution");
    return nullptr;
  }
  
//   vector<Double_t>*Binned_GenericModel_2::SetInitialWeightsDistributionExtended(Double_t ndata_entries, Bool_t uni)
//   {
//     // Initialize weights for parameters sets for an extended fit according to the given distribution
//     // if uni=kFALSE or with equal weigths if uni=kFALSE
//     // Don't take into account the refused DataSets with 0 entries
// 
//     AbstractMethod("GetInitialWeightsDistributionExtended");
//     return nullptr;
//   }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
//   vector<Double_t>*Binned_GenericModel_2::InitWeights(Double_t ndata_entries, Bool_t uni)
//   {
//     if(IsExtended()==kTRUE) fInitWeights = SetInitialWeightsDistributionExtended(ndata_entries,uni);
//     else fInitWeights = SetInitialWeightsDistribution(uni);
//     
//     return fInitWeights;
//   }
//   
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  void Binned_GenericModel_2::ConstructPseudoPDF(Bool_t debug)
  {
    // Build a parameterised pseudo-PDF from all the imported model datasets.
    // Each dataset is transformed into a RooHistPdf. 
    // Depending on uni=kTRUE->uniform weights according to extended/not extended fit
    // If debug=kTRUE, each dataset and kernel PDF will be displayed in a canvas.
    // This can help to decide if the kernel smoothing parameter needs attention.

    //InitWeights(ndata_entries, uni);
    fInitWeights = SetInitialWeightsDistribution();

    if(!GetNumberOfDataSets())
      {
	Error("ConstructPseudoPDF", "import model datasets with ImportModelData() first");
	return;
      }

    if(fBool_workspace)
      {
	Error("ConstructPseudoPDF", "RooWorspace for fit already provided, no importation needed (check object names)");
      }
   
    if(fModelPseudoPDF)
      {
	delete fModelPseudoPDF;
	fModelPseudoPDF=0;
	fHistPdfs.Delete();
	fFractions.removeAll();
      }
   
    //generate kernels
    RooArgList kernels;

    //number of the last set added in kernels
    //used for not extended pdf to remove last coef
    Int_t last = -1;

    for(int i=0; i<GetNumberOfDataSets(); i++)
      {                    
        RooDataHist *set = (RooDataHist*) ((TObjArray*) fWorkspace->obj("_dataset"))->At(i);
	
	if(set->sumEntries()>1000 && (*fInitWeights)[i]>0)
	  {
	    ++fNDataSets;
	    last=i;
      
	    RooHistPdf *pdf = new RooHistPdf(Form("HistPdf%d",i), Form("RooHistPdf from datahist#%d",i), GetObservables(), *set, fSmoothing);
	    pdf->forceNumInt(kTRUE); 
	 		      
	    ((TObjArray*) fWorkspace->obj("_histpdfset"))->Add(pdf);      
	    kernels.add(*pdf);
	
	    Info("ConstructPseudoPDF", "...datahist#%d... added into kernel list (entries=%d)...", i, (int) set->sumEntries());
            
	    //Extended pdf
	    if(IsExtended()==kTRUE)
	      {
		RooRealVar pp(Form("W%d",i),Form("fractional weight of kernel-PDF #%d",i),(*fInitWeights)[i],0.,ndata_entries);
		
		//fix coefficient according to initial weight
		//if(pp.getVal()==0) pp.setConstant(kTRUE); 
		   
		fFractions.addClone(pp);	  	      	
	      }
      
	    //Not-Extended pdf
	    else
	      {
		RooRealVar pp(Form("W%d",i),Form("fractional weight of kernel-PDF #%d",i),(*fInitWeights)[i],0.,1.);
		
	        //fix coefficient according to initial weight
		//if(pp.getVal()==0) pp.setConstant(kTRUE);
		
		fFractions.addClone(pp);    	       
	      }
	  } 
	 
	 
	else
	  {
	    Info("ConstructPseudoPDF", "...datahist#%d... refused (nentries=%d)...", i, (int) set->sumEntries());	    
	  } 
      }
   
          
    //Remove last coefficient for not-extended fit
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
	fModelPseudoPDF->forceNumInt();	
      }
      
    else
      {
        //fFractions->remove();
	fModelPseudoPDF = new RooAddPdf("Model", "Pseudo-PDF constructed from kernels for model datahists", kernels, fFractions, kTRUE);
	fModelPseudoPDF->forceNumInt();
      }  
  }
    
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  RooFitResult* Binned_GenericModel_2::fitTo(RooDataHist& data, const RooCmdArg& arg1, const RooCmdArg& arg2, const RooCmdArg& arg3, const RooCmdArg& arg4, 
					     const RooCmdArg& arg5, const RooCmdArg& arg6, const RooCmdArg& arg7, const RooCmdArg& arg8, const RooCmdArg& arg9, const RooCmdArg& arg10)
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
    
    //RooLinkedList creation
    RooLinkedList l ;
    l.Add((TObject*)&arg1) ;  l.Add((TObject*)&arg2) ;  
    l.Add((TObject*)&arg3) ;  l.Add((TObject*)&arg4) ;
    l.Add((TObject*)&arg5) ;  l.Add((TObject*)&arg6) ;  
    l.Add((TObject*)&arg7) ;  l.Add((TObject*)&arg8) ;
    l.Add((TObject*)&arg9) ;  l.Add((TObject*)&arg10) ;    
    
    return fitTo(data, l); 
  }
  
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  RooFitResult* Binned_GenericModel_2::fitTo(RooDataHist& data, const RooLinkedList& cmdList)   
  {
    // Perform fit of the pseudo-PDF to the data
    // On multi-core machines, this automatically uses all available processor cores
    // Some options were added compared to the RooAbsPdf::fitTo() method: number of calls, iteration and the tolerance factor epsilon can be changed
    
    //=================================
    //==      Add some fit options   ==
    //=================================
    
    RooCmdConfig pc(Form("Binned_GenericModel_2::fitTo(%s)",GetName()));

    //Global list of RooCmdArg for the fit
    RooLinkedList fitCmdList(cmdList);
    //Filter some RooCmdArg to be nll RooCmdArg list
    RooLinkedList nllCmdList =  pc.filterCmdList(fitCmdList,"ProjectedObservables,Extended,Range,RangeWithName,SumCoefRange,NumCPU,SplitRange,Constrained,Constrain,ExternalConstraints,CloneData,GlobalObservables,GlobalObservablesTag,OffsetLikelihood");	
    
      
    pc.defineString("fitOpt","FitOptions",0,"") ;
    pc.defineInt("optConst","Optimize",0,2) ;
    pc.defineInt("verbose","Verbose",0,0) ;
    pc.defineInt("doSave","Save",0,0) ;
    pc.defineInt("doTimer","Timer",0,0) ;
    pc.defineInt("plevel","PrintLevel",0,1) ;
    pc.defineInt("strat","Strategy",0,1) ;
    pc.defineInt("initHesse","InitialHesse",0,0) ;
    pc.defineInt("hesse","Hesse",0,1) ;
    pc.defineInt("minos","Minos",0,0) ;
    pc.defineInt("ext","Extended",0,2) ;
    pc.defineInt("numcpu","NumCPU",0,1) ;
    pc.defineInt("numee","PrintEvalErrors",0,10) ;
    pc.defineInt("doEEWall","EvalErrorWall",0,1) ;
    pc.defineInt("doWarn","Warnings",0,1) ;
    pc.defineInt("doSumW2","SumW2Error",0,-1) ;
    pc.defineInt("doOffset","OffsetLikelihood",0,0) ;
    pc.defineString("mintype","Minimizer",0,"OldMinuit") ;
    pc.defineString("minalg","Minimizer",1,"minuit") ;
    pc.defineObject("minosSet","Minos",0,0) ;
    pc.defineSet("cPars","Constrain",0,0) ;
    pc.defineSet("extCons","ExternalConstraints",0,0) ;
    pc.defineMutex("FitOptions","Verbose") ;
    pc.defineMutex("FitOptions","Save") ;
    pc.defineMutex("FitOptions","Timer") ;
    pc.defineMutex("FitOptions","Strategy") ;
    pc.defineMutex("FitOptions","InitialHesse") ;
    pc.defineMutex("FitOptions","Hesse") ;
    pc.defineMutex("FitOptions","Minos") ;
    pc.defineMutex("Range","RangeWithName") ;
    pc.defineMutex("InitialHesse","Minimizer") ;
  
    //added RooCmdArg
    //Modify number of max iteration and max calls (by default maxiter = maxcalls = 500*nparameters (see RooMinimizer class))
    pc.defineInt("numiter", "SetMaxIter", 0, 0);
    pc.defineInt("numcalls","SetMaxCalls", 0, 0);
    //Modify tolerance value of the fit (convergence)
    pc.defineDouble("eps", "SetEpsilon", 0, 1.0);

  
    // Process and check varargs 
    pc.process(fitCmdList) ;
    if (!pc.ok(kTRUE)) {
      return 0 ;
    }

    // Decode command line arguments
    const char* fitOpt = pc.getString("fitOpt",0,kTRUE) ;
    Int_t optConst = pc.getInt("optConst") ;
    Int_t verbose  = pc.getInt("verbose") ;
    Int_t doSave   = pc.getInt("doSave") ;
    Int_t doTimer  = pc.getInt("doTimer") ;
    Int_t plevel   = pc.getInt("plevel") ;
    Int_t strat    = pc.getInt("strat") ;
    Int_t initHesse= pc.getInt("initHesse") ;
    Int_t hesse    = pc.getInt("hesse") ;
    Int_t minos    = pc.getInt("minos") ;
    Int_t numee    = pc.getInt("numee") ;
    Int_t doEEWall = pc.getInt("doEEWall") ;
    Int_t doWarn   = pc.getInt("doWarn") ;
    Int_t doSumW2  = pc.getInt("doSumW2") ;
    Int_t numiter  = pc.getInt("numiter") ;
    Int_t numcalls = pc.getInt("numcalls") ;
    Double_t eps   = pc.getDouble("eps");
    const RooArgSet* minosSet = static_cast<RooArgSet*>(pc.getObject("minosSet")) ;
#ifdef __ROOFIT_NOROOMINIMIZER
    const char* minType =0 ;
#else
    const char* minType = pc.getString("mintype","OldMinuit") ;
    const char* minAlg  = pc.getString("minalg","minuit") ;
#endif

    // Determine if the dataset has weights  
    Bool_t weightedData = data.isNonPoissonWeighted() ;

    // Warn user that a SumW2Error() argument should be provided if weighted data is offered
    if (weightedData && doSumW2==-1) {
      coutW(InputArguments) << "RooAbsPdf::fitTo(" << GetName() << ") WARNING: a likelihood fit is request of what appears to be weighted data. " << endl
			    << "       While the estimated values of the parameters will always be calculated taking the weights into account, " << endl 
			    << "       there are multiple ways to estimate the errors on these parameter values. You are advised to make an " << endl 
			    << "       explicit choice on the error calculation: " << endl
			    << "           - Either provide SumW2Error(kTRUE), to calculate a sum-of-weights corrected HESSE error matrix " << endl
			    << "             (error will be proportional to the number of events)" << endl 
			    << "           - Or provide SumW2Error(kFALSE), to return errors from original HESSE error matrix" << endl 
			    << "             (which will be proportional to the sum of the weights)" << endl 
			    << "       If you want the errors to reflect the information contained in the provided dataset, choose kTRUE. " << endl
			    << "       If you want the errors to reflect the precision you would be able to obtain with an unweighted dataset " << endl 
			    << "       with 'sum-of-weights' events, choose kFALSE." << endl ;
    }


    // Warn user that sum-of-weights correction does not apply to MINOS errrors
    if (doSumW2==1 && minos) {
      coutW(InputArguments) << "RooAbsPdf::fitTo(" << GetName() << ") WARNING: sum-of-weights correction does not apply to MINOS errors" << endl ;
    }
   
    RooAbsReal* nll = fModelPseudoPDF->createNLL(data,nllCmdList);        

    //=============================
    //==   Instantiate MINUIT    ==
    //=============================

    if (string(minType)!="OldMinuit") {
    
#ifndef __ROOFIT_NOROOMINIMIZER

      RooMinimizer m(*nll) ;
      m.setMinimizerType(minType) ;    
      m.setEvalErrorWall(doEEWall) ;
    
      if(numiter>0) m.setMaxIterations(numiter);
      if(numcalls>0) m.setMaxFunctionCalls(numcalls);   
    
      m.setEps(eps);
        
      if (doWarn==0) 
	{
	  // m.setNoWarn() ; WVE FIX THIS
	}
    
      m.setPrintEvalErrors(numee) ;
      if (plevel!=1) {
	m.setPrintLevel(plevel) ;
      }
    
      if (optConst) {
	// Activate constant term optimization
	m.optimizeConst(optConst) ;
      }
    
      if (fitOpt)
	{
      
	  // Play fit options as historically defined
	  fLastFit = m.fit(fitOpt) ;
      
	} 
    
      else {
      
	if (verbose) {
	  // Activate verbose options
	  m.setVerbose(1) ;
	}
	if (doTimer) {
	  // Activate timer options
	  m.setProfile(1) ;
	}
      
	if (strat!=1) {
	  // Modify fit strategy
	  m.setStrategy(strat) ;
	}
      
	if (initHesse) {
	  // Initialize errors with hesse
	  m.hesse() ;
	}
      
	// Minimize using chosen algorithm
	m.minimize(minType,minAlg) ;
      
	if (hesse) {
	  // Evaluate errors with Hesse
	  m.hesse() ;
	}
      
	if (doSumW2==1 && m.getNPar()>0) {
	  // Make list of RooNLLVar components of FCN
	  RooArgSet* comps = nll->getComponents();
	  vector<RooNLLVar*> nllComponents;
	  nllComponents.reserve(comps->getSize());
	  TIterator* citer = comps->createIterator();
	  RooAbsArg* arg;
	  while ((arg=(RooAbsArg*)citer->Next())) {
	    RooNLLVar* nllComp = dynamic_cast<RooNLLVar*>(arg);
	    if (!nllComp) continue;
	    nllComponents.push_back(nllComp);
	  }
	  delete citer;
	  delete comps; 
	
	  // Calculated corrected errors for weighted likelihood fits
	  RooFitResult* rw = m.save();
	  for (vector<RooNLLVar*>::iterator it = nllComponents.begin(); nllComponents.end() != it; ++it) {
	    (*it)->applyWeightSquared(kTRUE);
	  }
	  coutI(Fitting) << "RooAbsPdf::fitTo(" << GetName() << ") Calculating sum-of-weights-squared correction matrix for covariance matrix" << endl ;
	  m.hesse();
	  RooFitResult* rw2 = m.save();
	  for (vector<RooNLLVar*>::iterator it = nllComponents.begin(); nllComponents.end() != it; ++it) {
	    (*it)->applyWeightSquared(kFALSE);
	  }
	
	  // Apply correction matrix
	  const TMatrixDSym& matV = rw->covarianceMatrix();
	  TMatrixDSym matC = rw2->covarianceMatrix();
	  using ROOT::Math::CholeskyDecompGenDim;
	  CholeskyDecompGenDim<Double_t> decomp(matC.GetNrows(), matC);
	  if (!decomp) {
	    coutE(Fitting) << "RooAbsPdf::fitTo(" << GetName() 
			   << ") ERROR: Cannot apply sum-of-weights correction to covariance matrix: correction matrix calculated with weight-squared is singular" <<endl ;
	  } else {
	    // replace C by its inverse
	    decomp.Invert(matC); 
	    // the class lies about the matrix being symmetric, so fill in the
	    // part above the diagonal
	    for (int i = 0; i < matC.GetNrows(); ++i)
	      for (int j = 0; j < i; ++j) matC(j, i) = matC(i, j);
	    matC.Similarity(matV);
	    // C now contiains V C^-1 V
	    // Propagate corrected errors to parameters objects
	    m.applyCovarianceMatrix(matC);
	  }
	
	  delete rw;
	  delete rw2;
	}
      
	if (minos) {
	  // Evaluate errs with Minos
	  if (minosSet) {
	    m.minos(*minosSet) ;
	  } else {
	    m.minos() ;
	  }
	}
      
	// Optionally return fit result
	if (doSave) {
	  string name = Form("fitresult_%s_%s",GetName(),data.GetName()) ;
	  string title = Form("Result of fit of p.d.f. %s to dataset %s",GetName(),data.GetName()) ;
	  fLastFit = m.save(name.c_str(),title.c_str()) ;
	} 
      
      }
      if (optConst) {
	m.optimizeConst(0) ;
      }

#endif

    }
    
    else {

      RooMinuit m(*nll) ;
   
      m.setEps(eps);
        
      m.setEvalErrorWall(doEEWall) ;
      if (doWarn==0) {
	m.setNoWarn() ;
      }
    
      m.setPrintEvalErrors(numee) ;
      if (plevel!=1) {
	m.setPrintLevel(plevel) ;
      }
    
      if (optConst) {
	// Activate constant term optimization
	m.optimizeConst(optConst) ;
      }
    
      if (fitOpt) {
      
	// Play fit options as historically defined
	fLastFit = m.fit(fitOpt) ;
      
      } else {
      
	if (verbose) {
	  // Activate verbose options
	  m.setVerbose(1) ;
	}
	if (doTimer) {
	  // Activate timer options
	  m.setProfile(1) ;
	}
      
	if (strat!=1) {
	  // Modify fit strategy
	  m.setStrategy(strat) ;
	}
      
	if (initHesse) {
	  // Initialize errors with hesse
	  m.hesse() ;
	}
      
	// Minimize using migrad
	m.migrad() ;
      
	if (hesse) {
	  // Evaluate errors with Hesse
	  m.hesse() ;
	}
      
	if (doSumW2==1 && m.getNPar()>0) {
	
	  // Make list of RooNLLVar components of FCN
	  list<RooNLLVar*> nllComponents ;
	  RooArgSet* comps = nll->getComponents() ;
	  RooAbsArg* arg ;
	  TIterator* citer = comps->createIterator() ;
	  while((arg=(RooAbsArg*)citer->Next())) {
	    RooNLLVar* nllComp = dynamic_cast<RooNLLVar*>(arg) ;
	    if (nllComp) {
	      nllComponents.push_back(nllComp) ;
	    }
	  }
	  delete citer ;
	  delete comps ;  
	
	  // Calculated corrected errors for weighted likelihood fits
	  RooFitResult* rw = m.save() ;
	  for (list<RooNLLVar*>::iterator iter1=nllComponents.begin() ; iter1!=nllComponents.end() ; iter1++) {
	    (*iter1)->applyWeightSquared(kTRUE) ;
	  }
	  coutI(Fitting) << "RooAbsPdf::fitTo(" << GetName() << ") Calculating sum-of-weights-squared correction matrix for covariance matrix" << endl ;
	  m.hesse() ;
	  RooFitResult* rw2 = m.save() ;
	  for (list<RooNLLVar*>::iterator iter2=nllComponents.begin() ; iter2!=nllComponents.end() ; iter2++) {
	    (*iter2)->applyWeightSquared(kFALSE) ;
	  }
	
	  // Apply correction matrix
	  const TMatrixDSym& matV = rw->covarianceMatrix();
	  TMatrixDSym matC = rw2->covarianceMatrix();
	  using ROOT::Math::CholeskyDecompGenDim;
	  CholeskyDecompGenDim<Double_t> decomp(matC.GetNrows(), matC);
	  if (!decomp) {
	    coutE(Fitting) << "RooAbsPdf::fitTo(" << GetName() 
			   << ") ERROR: Cannot apply sum-of-weights correction to covariance matrix: correction matrix calculated with weight-squared is singular" <<endl ;
	  } else {
	    // replace C by its inverse
	    decomp.Invert(matC); 
	    // the class lies about the matrix being symmetric, so fill in the
	    // part above the diagonal
	    for (int i = 0; i < matC.GetNrows(); ++i)
	      for (int j = 0; j < i; ++j) matC(j, i) = matC(i, j);
	    matC.Similarity(matV);
	    // C now contiains V C^-1 V
	    // Propagate corrected errors to parameters objects
	    m.applyCovarianceMatrix(matC);
	  }

	  delete rw ;
	  delete rw2 ;
	}
      
	if (minos) {
	  // Evaluate errs with Minos
	  if (minosSet) {
	    m.minos(*minosSet) ;
	  } else {
	    m.minos() ;
	  }
	}
      
	// Optionally return fit result
	if (doSave) {
	  string name = Form("fitresult_%s_%s",GetName(),data.GetName()) ;
	  string title = Form("Result of fit of p.d.f. %s to dataset %s",GetName(),data.GetName()) ;
	  fLastFit = m.save(name.c_str(),title.c_str()) ;
	} 
      
      }

      if (optConst) {
	m.optimizeConst(0) ;
      }
    
    }
  
  
    //====================================
    //==            Save Coef           ==
    //====================================
  
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

  
  
    // Cleanup
    delete nll ;
    return fLastFit ;
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  RooFitResult* Binned_GenericModel_2::chi2FitTo(RooDataHist& data, const RooCmdArg& arg1, const RooCmdArg& arg2, const RooCmdArg& arg3, const RooCmdArg& arg4, 
						 const RooCmdArg& arg5, const RooCmdArg& arg6, const RooCmdArg& arg7, const RooCmdArg& arg8, const RooCmdArg& arg9, const RooCmdArg& arg10)
  {
    //Print informations
    Info("fiTo...","performing chi2fit with %d parameters and %d observables", GetNumberOfParameters(), GetNumberOfObservables());
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
    
    //RooLinkedList creation
    RooLinkedList l ;
    l.Add((TObject*)&arg1) ;  l.Add((TObject*)&arg2) ;  
    l.Add((TObject*)&arg3) ;  l.Add((TObject*)&arg4) ;
    l.Add((TObject*)&arg5) ;  l.Add((TObject*)&arg6) ;  
    l.Add((TObject*)&arg7) ;  l.Add((TObject*)&arg8) ;
    l.Add((TObject*)&arg9) ;  l.Add((TObject*)&arg10) ;    
    
    return chi2FitTo(data, l); 
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  RooFitResult* Binned_GenericModel_2::chi2FitTo(RooDataHist& data, const RooLinkedList& cmdList) 
  {
    fLastFit = fModelPseudoPDF->chi2FitTo(data, cmdList);
  
    //====================================
    //==            Save Coef           ==
    //====================================
  
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

    return fLastFit ;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////S

  void Binned_GenericModel_2::plotOn(RooPlot* frame)
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
  Bool_t Binned_GenericModel_2::initWorkspace()
  {   	   
    //init new workspace
    Info("initWorkspace ...", "workspace not given...creating one...");  
    
    fWorkspace = new RooWorkspace(Form("workspace"),"RooWorkspace for the fit");
    fWorkspace->import(fParameters, "_parameters");
    fWorkspace->import(fObservables, "_observables");
    fWorkspace->import(fParObs,"_parobs");
    fWorkspace->import(fDataSets,"_dataset");
    fWorkspace->import(fDataSetParams,"_datasetparams");
    fWorkspace->import(fHistPdfs,"_histpdfset");
    
    fBool_good_workspace;
  }
  
  
  
  Bool_t Binned_GenericModel_2::initWorkspace(RooWorkspace* workspace)
  {  
    if(workspace!=0)
      {
	fBool_good_workspace = kTRUE;
	fBool_workspace      = kTRUE;
        
	RooArgList par  = *((RooArgList*) workspace->obj("_parameters"));
	if(&par!=0) Info("initWorkspace ...", "_parameters found...");
	else
	  {
	    Error("initWorkspace ...", "workspace provided but _parameters NOT found...");
	    fBool_workspace=kFALSE;
	  }	 
   
	RooArgList obs = *((RooArgList*) workspace->obj("_observables"));
	if(&obs!=0) Info("initWorkspace ...", "_observables found...");
	else
	  {
	    Error("initWorkspace ...", "workspace provided but _observables NOT found...");
	    fBool_workspace=kFALSE;
	  }
       	  
	RooArgList parobs  = *((RooArgList*) workspace->obj("_parobs"));
	if(&parobs!=0) Info("initWorkspace ...", "_parobs found...");
	else 
	  {
	    Error("initWorkspace ...", "workspace provided but _parobs NOT found...");
	    fBool_workspace=kFALSE;
	  }	
   
	TObjArray dat = *((TObjArray*)  workspace->obj("_dataset"));
	if(&dat!=0) Info("initWorkspace ...", "_dataset found...");
	else
	  {
	    Error("initWorkspace ...", "workspace provided but _dataset NOT found...");
	    fBool_good_workspace=kFALSE;
	  }
	   
	TObjArray *datpar = ((TObjArray*)  workspace->obj("_datasetparams"));
	if(datpar!=0) Info("initWorkspace ...", "_datasetparams found...");
	else
	  {
	    Error("initWorkspace ...", "workspace provided but _datasetparams NOT found...");
	    fBool_good_workspace=kFALSE;
	  }       
   
        
	TObjArray set= *((TObjArray *) workspace->obj("_histpdfset")); 
	if(&set!=0) Info("initWorkspace ...", "_histpdf found...");
	else
	  {
	    Error("initWorkspace ...", "workspace provided but _histpdf NOT found...");  
	    fBool_good_workspace=kFALSE; 
	  }
      }
   
    else
      {
        fBool_workspace=kFALSE;
	fBool_good_workspace=kFALSE;
      }
      
    //////////////////////////////////////////////////////////////////
    if(fBool_good_workspace==kTRUE)
      {
	Info("initWorkspace ...", "workspace correctly set...continue...");
        fWorkspace = workspace; 
	return fBool_good_workspace;
      }
	    
    else 
      {
        Error("initWorkspace ...", "workspace not correctly set...");
        return fBool_good_workspace;        
      }	       		      
  }



  //////////////////////////////////////////////////////////////////////////////////////////////////////////
   
  void Binned_GenericModel_2::saveWorkspace()
  { 
    if(fBool_workspace==kFALSE)
      {	
	//For name
	TString ss0("");
        for(Int_t ii=0;ii<GetNumberOfObservables();ii++)
          {
	    RooArgList ll = GetObservables();
	    RooRealVar vv = *((RooRealVar*) ll.at(ii));
	    TString ss1(Form("_%s[%.1lf,%.1lf,%d]",vv.GetName(),vv.getMin(),vv.getMax(),vv.getBins()));
	    ss0+=ss1;	    
          }
	  
	  
	fWorkspace->writeToFile(Form("workspace%s.root",ss0.Data()),"recreate");
	
	Info("saveWorkspace", "Saving fit workspace %s", fWorkspace->GetName());
	
	//ffile->Close(); 
      }
     
    else
      {
	Info("saveWorkspace", "workspace already provided new one will not be saved");
      }     
  }

} /* namespace BackTrack */
