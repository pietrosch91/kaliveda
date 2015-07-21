//Created by KVClassFactory on Tue Mar 24 09:39:10 2015
//Author: Quentin Fable,,,

#include "Binned_GeminiModel_2.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF2.h"
#include "TCanvas.h"

ClassImp(BackTrack::Binned_GeminiModel_2)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
   <h2>Binned_GeminiModel_2</h2>
   <h4></h4>
   <!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

namespace BackTrack {


  Binned_GeminiModel_2::Binned_GeminiModel_2()
  {      
    //TFile *ff = new TFile("./fit_workspaces.root");
    //TFile *ff = new TFile("./workspace_APrimaryWoN[21.0,30.0,9]_EkLab_alpha_mean[0.0,250.0,125].root");
    //TFile *ff = new TFile("./workspace_APrimaryWoN[15.0,40.0,25]_EStarPrimaryWoN_MeV[0.0,150.0,150].root");
    TFile *ff = new TFile("./workspace_APrimaryWoN[20.0,35.0,15].root");
    
    //Initial weights histogram
    TH2 *hh_init = (TH2*) gFile->FindObject();

    RooWorkspace *w = (RooWorkspace*) ff->Get("workspace");
    
    if(w)
      {
	printf("GEMINI: workspace found!!!\n");
	w->Print();
	Bool_t init = initWorkspace(w);
	  
	//if corrupted workspace
	if(init==kFALSE)
	  {
	    //Tree 
	    TFile *fgem_file = new TFile("../../Gem_Trees/GoodFilter/GeminiModelTree_GoodFilter_ZPrimary13.root");
	    fgem_tree = (TTree*) fgem_file->Get("TT"); 
	      
	    if(fgem_tree) InitParObs();
	    else printf("Gemini tree not found...\n");
	  } 
      }
	 
    else
      {	  
	printf("GEMINI: workspace not found !!!\n");
	//Tree 
	TFile *fgem_file = new TFile("../../Gem_Trees/GoodFilter/GeminiModelTree_GoodFilter_ZPrimary13.root");
	fgem_tree = (TTree*) fgem_file->Get("TT");
	  
	if(fgem_tree)
	  {
	    //Create workspace
	    initWorkspace();
	    InitParObs();	      
	  }
	   
	else printf("Gemini tree not found...\n");	      
      } 
	  
    finit_weight = new vector<Double_t>();  	 	         
  }


  Binned_GeminiModel_2::~Binned_GeminiModel_2()
  {    
     
  }
   
  void Binned_GeminiModel_2::InitParObs()
  {
    //Parameters
    AddParameter("APrimary",  "Apr source GEMINI",     20,  40, 20);
    AddParameter("EStarPrimary",  "E*/Apr source GEMINI", 0.5, 6.5, 12);
    //AddParameter("APrimary",  "Apr source GEMINI",     20,  40, 10);
    //AddParameter("EStarPrimary",  "E*/Apr source GEMINI", 0.5, 6.5, 6);  


    //AddParameter("EStarPrimary",  "E*/Apr source GEMINI", 3.0, 4.0, 2);     
    //AddParameter("ZPrimary",  "Zpr source GEMINI",     12,  24, 12); 
    //AddParameter("JPrimary",  "J source GEMINI",        0,  45, 3); 


    AddObservable("APrimaryWoN", "Apr without neutrons",  20, 35, 15);
    //AddObservable("EStarPrimaryWoN_MeV", "E* without neutrons", 0,140,70);
    //AddObservable("EkLab_alpha_mean", "mean Ek_alpha", 0,250,125);
    //AddObservable("A_plf",        "Mass of the PLF"    ,   12, 32, 20);
    //AddObservable("Z_plf",        "Charge of the PLF"  ,   6, 18, 12); 
    //AddObservable("Ek_p",         "Ek of protons"       ,   0, 220,220);
    //AddObservable("Ek_a",         "Ek of alphas"        ,   0, 300);     
    //AddObservable("M_p",          "Protons multiplicity",   0, 20);
    //AddObservable("M_a",          "Alphas  multiplicity",   0, 20);
  }
  
  
  void Binned_GeminiModel_2::InitParObs(TH2D* histo)
  {
    //Guess for input model parameters
    Int_t nxbins = histo->GetNbinsX();
    Int_t nybins = histo->GetNbinsY();
    Double_t xwidth = histo->GetXaxis()->GetBinWidth(1);
    Double_t ywidth = histo->GetYaxis()->GetBinWidth(1);
    
    Double_t xmin = histo->GetXaxis()->GetBinCenter(1)+xwidth/2.;
    Double_t xmax = histo->GetXaxis()->GetBinCenter(nxbins)+xwidth/2.;
    Double_t ymin = histo->GetYaxis()->GetBinCenter(1)+ywidth/2.;
    Double_t ymax = histo->GetYaxis()->GetBinCenter(nybins)+ywidth/2.;
    
    //Add Parameters and Observables
    AddParameter("APrimary",  "Apr source GEMINI",     xmin,  xmax, nxbins);
    AddParameter("EStarPrimary",  "E*/Apr source GEMINI", ymin, ymax, nybins);
    
    AddObservable("APrimaryWoN", "Apr without neutrons",  20, 35, 15);
    AddObservable("EStarPrimaryWoN_MeV", "E* without neutrons", 0,140,70);
    
  }
    
   
  RooDataHist*Binned_GeminiModel_2::GetModelDataHist(RooArgList& param_list)
  {
    //Create a RooDataSet data from the GEMINI TTree for defined parameters      
    static int dsnum=1;
       
    Int_t size = param_list.getSize();
                  
    //List of parameters we make a cut in GEMINI Tree
    RooArgList *dependents = (RooArgList*) GetParameters().snapshot("dependents");
    RooRealVar rec("GoodRecon","GoodRecon",-2,2);
    dependents->addClone(rec);
    //TString s("GoodRecon>=1 && EkLab_alpha_mean>0 && "); 
    TString s("GoodRecon>=1 && "); 
      
    dependents->Print();
      
    //new RooArgList for the tree cut
    RooArgList *ParObsRecon = (RooArgList*) GetParObs().snapshot("ParObsRecon");
    ParObsRecon->addClone(rec);
      
    ParObsRecon->Print();
          
    //Creation of the RooFormulaVar      
    TString ss0(((RooRealVar*)GetParameters().at(0))->GetName());      
    TString ss1(Form(">=%e && ",((RooRealVar*)param_list.at(0))->getMin()));
    TString ss2(Form("<%e",((RooRealVar*)param_list.at(0))->getMax()));
    TString ss=s+ss0+ss1+ss0+ss2;
      
    for(Int_t ii=1; ii<size; ii++)
      {
	TString ss3(" && ");	  
	TString ss4(((RooRealVar*)GetParameters().at(ii))->GetName());
	TString ss5(Form(">=%e && ",((RooRealVar*)param_list.at(ii))->getMin()));
	TString ss6(Form("<%e",((RooRealVar*)param_list.at(ii))->getMax()));
	  
	ss+=ss3+ss4+ss5+ss4+ss6;
      }
	 
    cout << "RooForumlaVar for cut: " << ss << endl;	 

    //RooFormulaVar (const char* name, const char* title, const char* formula, const RooArgList& dependents)
    const RooFormulaVar cut("cut", "cut in parameters in GEMINI TTree", ss, *ParObsRecon);	
        
	       	  
    //Create the RooDataSet
    RooDataSet *data = new RooDataSet(Form("GEMINI_DATASET#%d",dsnum), Form("RooDataSet from cut %s", ss.Data()), fgem_tree, *ParObsRecon, cut);
    RooDataHist *datahist = new RooDataHist(Form("GEMINI_DATAHIST#%d",dsnum++), Form("RooDataHist from cut %s", ss.Data()), GetObservables(), *data);
     
	
//     if(data->numEntries()>1000)
//       {		
//             
// 	Double_t min_apr     = GetObservable("APrimaryWoN").getMin();
// 	Double_t max_apr     = GetObservable("APrimaryWoN").getMax();
// 	Double_t min_estar   = GetObservable("EStarPrimaryWoN_MeV").getMin();
// 	Double_t max_estar   = GetObservable("EStarPrimaryWoN_MeV").getMax();
//        
// 	//Double_t min_estar   = GetObservable("EkLab_alpha_mean").getMin();
// 	//Double_t max_estar   = GetObservable("EkLab_alpha_mean").getMax();
//       
// 	Int_t    nbin_apr    = GetObservable("APrimaryWoN").numBins();
// 	Int_t    nbin_estar  = GetObservable("EStarPrimaryWoN_MeV").numBins();
//        
// 	//Int_t    nbin_estar  = GetObservable("EkLab_alpha_mean").numBins();
//        	       
// 	TH2D *hh = new TH2D("init","init", nbin_apr,min_apr,max_apr, nbin_estar,min_estar,max_estar);
// 
//        	    
// 	for(Int_t nx=0; nx<=hh->GetNbinsX();nx++)
//           {
// 	    for(Int_t ny=0; ny<=hh->GetNbinsY();ny++)
// 	      {
// 		//hh->SetBinContent(nx,ny,1./(datahist->numEntries()));
// 		hh->SetBinContent(nx,ny,1./10000);
// 	      }
// 	  }
// 	 
// 	RooDataHist *datahistinit = new RooDataHist("init","init", GetObservables(), hh);  
// 	 
// 	datahist->add(*datahistinit); 
//       
//       }	 
       			                                                 
    datahist->Print();
      
    return datahist;
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<Double_t>*Binned_GeminiModel_2::SetInitialWeightsDistribution()
{
 //Initialize the initial weights of fit parameters according to the Apr-n & Estar-En distributions
 //Control the binning first
 //Generic model parameters
 Double_t min_apr_mod	  = GetParameter("APrimary").getMin();
 Double_t max_apr_mod	  = GetParameter("APrimary").getMax();
 Double_t min_estar_mod   = GetParameter("EStarPrimary").getMin();
 Double_t max_estar_mod   = GetParameter("EStarPrimary").getMax();      
 Int_t    nbin_apr_mod    = GetParameter("APrimary").numBins();
 Int_t    nbin_estar_mod  = GetParameter("EStarPrimary").numBins();
 
 //Given distribution values
 TH1F *distri_x = (TH1F*) distri->ProjectionX();
 TH1F *distri_y = (TH1F*) distri->ProjectionY(); 
 
 Int_t    nbin_apr_distri    = distri_x->GetNbinsX();
 Int_t    nbin_estar_distri  = distri_y->GetNbinsY();
 Double_t delta_bin_apr      = distri_x->GetBinWith(1);
 Double_t delta_bin_estar    = distri_y->GetBinWith(1);
 Double_t min_apr_distri     = GetBinCenter(1) - delta_bin_apr/2.;
 Double_t max_apr_distri     = GetBinCenter(nbin_apr_distri) + delta_bin_apr/2.; 
 Double_t min_estar_distri   = GetBinCenter(1) - delta_bin_estar/2.;
 Double_t max_estar_distri   = GetBinCenter(nbin_estar_distri) + delta_bin_estar/2.;  
 
 Int_t    nset	   = nbin_apr*nbin_estar;
 Bool_t good_histo = kTRUE;
 
 if(min_apr_distri!=min_apr_mod) 
   {
     Error("InitializeWeightDistribution","min_apr_distri(%e)!=min_apr_mod(%e)", min_apr_distri, min_apr_mod);  
     good_histo = kFALSE;
   }  
 if(max_apr_distri!=max_apr_mod) 
   {       
     Error("InitializeWeightDistribution","max_apr_distri(%e)!=max_apr_mod(%e)", max_apr_distri, max_apr_mod);
     good_histo = kFALSE;
   } 
 if(min_estar_distri!=min_estar_mod)
   { 
     Error("InitializeWeightDistribution","min_estar_distri(%e)!=min_estar_mod(%e)", min_estar_distri, min_estar_mod);
     good_histo = kFALSE;
   }  
 if(max_estar_distri!=max_estar_mod)
   {
     Error("InitializeWeightDistribution","max_estar_distri(%e)!=max_estar_mod(%e)", max_estar_distri, max_estar_mod);
     good_histo = kFALSE;
   } 
 if(nbin_apr_distri!=nbin_apr_mod)   
   {
     Error("InitializeWeightDistribution","nbin_apr_distri(%e)!=nbin_apr_mod(%e)", nbin_apr_distri, nbin_apr_mod);
     good_histo = kFALSE;
   } 
 if(nbin_estar_distri!=nbin_estar_mod)   
   {
     Error("InitializeWeightDistribution","nbin_estar_distri(%e)!=nbin_estar_mod(%e)", nbin_estar_distri, nbin_aestar_mod);
     good_histo = kFALSE;
   }  
 
 if(good_histo==kTRUE)
    {
      Bool_t extended = IsExtended();
       
     //------------Not-extended fit-------------
     if(extended==kFALSE)
        {
	  if(nexp_entries!=0) Info("InitializeWeightDistribution","...experimental integral given but not taken into account because proceeding a not-extended fit...");
	  distri->Scale(1./distri->Integral());
	  for(Int_t aa=1; aa<=nbin_apr; aa++)
	   { 
    	    for(Int_t ee=1; ee<=nbin_estar; ee++)
              {
		finit_weight->push_back(h2->GetBinContent(aa,ee));	    
	        printf("################# [Init weights not-Extended] nset=%d [Apr=%e, Estar=%e] || init_weight=%e ##################\n", ii, min_apr+aa*delta_apr, min_estar+ee*delta_estar,   (*finit_weight)[ii]); 
	        ii++;
    	      }    
	   }
	}
	
      //____________Extended fit----------------	
      else
        {
	  if(nexp_entries==0) Error("InitializeWeightDistribution","...no experimental integral given but trying to proceed an extended-fit !!!...");
	  else
	     {
	      Info("InitializeWeightDistribution","...experimental integral given :int=%e  proceeding an extended fit...", nexp_entries);
	      for(Int_t aa=1; aa<=nbin_apr; aa++)
	        { 
    	         for(Int_t ee=1; ee<=nbin_estar; ee++)
                   {    	        
		     Double_t tt = entries/nset;
		     finit_weight->push_back(tt);	    
	             printf("################# [Init weights Extended] nset=%d [Apr=%e, Estar=%e] || init_weight=%e ##################\n", ii, min_apr+aa*delta_apr, min_estar+ee*delta_estar, (*finit_weight)[ii]); 
	             ii++;
    	           }    
	        }	      
	     }
	}	
    }
    
 return finit_weight;    
}     

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  TH1*Binned_GeminiModel_2::GetParameterDistributions()
  {
    // Return 2D histo of fitted parameter distributions

    TH1* h = GetParamDataHist()->createHistogram("APrimary,EStarPrimary",GetParameter("APrimary").getBins(),GetParameter("EStarPrimary").getBins());
    h->SetName("fittedParameters");
    return h;
  }
   
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
//    
//   vector<Double_t>*Binned_GeminiModel_2::SetInitialWeightsDistributionExtended(Double_t nexp_entries, Bool_t uni)
//   {
//     // Create a N-dim distribution of the initial weights for the N parameters
//     // If uni=kTRUE-> uniform weights
//     // If uni=kFALSE-> weights distributed according to Apr-n and E*-En
//     // Important step for the fit, if initial parameters are too dumb, fit won't be correct
//      
//     // For Gemini, gaussian 2D distri of apr vs estar  
//     Double_t entries     = nexp_entries;   
//     Double_t mean_apr    = 28.;
//     Double_t mean_estar  = 1.5;
//     Double_t sigma_apr   = 1.5;
//     Double_t sigma_estar = 2.;
//     Double_t min_apr	  = GetParameter("APrimary").getMin();
//     Double_t max_apr	  = GetParameter("APrimary").getMax();
//     Double_t min_estar   = GetParameter("EStarPrimary").getMin();
//     Double_t max_estar   = GetParameter("EStarPrimary").getMax();
//       
//     Int_t    nbin_apr    = GetParameter("APrimary").numBins();
//     Int_t    nbin_estar  = GetParameter("EStarPrimary").numBins();
//     Int_t    nset	  = nbin_apr*nbin_estar;
//      
//     //finit_weight = new Double_t[nset];     
//     Int_t ii=0;
//      
//     printf("################ [Init weights] nset=%d || min_apr=%e, max_apr=%e, nbin_apr=%d || min_estar=%e, max_estar=%e, nbin_estar=%d ###############\n", nset, min_apr, max_apr, nbin_apr, min_estar, max_estar, nbin_estar);
//      
//     Double_t delta_apr   = (max_apr-min_apr)/nbin_apr;
//     Double_t delta_estar = (max_estar-min_estar)/nbin_estar;
//      
//     if(uni==kFALSE)
//       {
// 	TF2 *f2 = new TF2("f2","[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",min_apr, max_apr, min_estar, max_estar);
// 	f2->SetParameters(1.,mean_apr,sigma_apr,mean_estar,sigma_estar);
//           
// 	TCanvas *cc = new TCanvas("cc_weight");
// 	cc->cd();
//      
// 	TH2D* h2= new TH2D("init_par_distri","initial parameter distribution for the fit", nbin_apr, min_apr, max_apr, nbin_estar, min_estar, max_estar);
// 	h2->FillRandom("f2",10000);
// 	h2->Scale(nexp_entries*1./h2->Integral());
// 	h2->Draw("surf");
//     
//      
// 	for(Int_t aa=1; aa<=nbin_apr; aa++)
// 	  { 
//     	    for(Int_t ee=1; ee<=nbin_estar; ee++)
//               {
//     	        //finit_weight[ii] = h2->GetBinContent(aa,ee);
// 		finit_weight->push_back(h2->GetBinContent(aa,ee));	    
// 	        printf("################# [Init weights Extended not uniform] nset=%d [Apr=%e, Estar=%e] || init_weight=%e ##################\n", ii, min_apr+aa*delta_apr, min_estar+ee*delta_estar, (*finit_weight)[ii]); 
// 	        ii++;
//     	      }    
// 	  }
//       }
//       
//     else
//       {
// 	for(Int_t aa=1; aa<=nbin_apr; aa++)
// 	  { 
//     	    for(Int_t ee=1; ee<=nbin_estar; ee++)
//               {
//     	        //finit_weight[ii] = (float) nexp_entries/GetNumberOfDataSets();
// 		Double_t tt = entries/nset;
// 		finit_weight->push_back(tt);	    
// 	        printf("################# [Init weights Extended uniform] nset=%d [Apr=%e, Estar=%e] || init_weight=%e ##################\n", ii, min_apr+aa*delta_apr, min_estar+ee*delta_estar, (*finit_weight)[ii]); 
// 	        ii++;
//     	      }    
// 	  }
//       }	 
// 	        
//     return finit_weight;                 
//   }
//     
//   vector<Double_t>*Binned_GeminiModel_2::SetInitialWeightsDistribution(Bool_t uni)
//   {
//     // Create a N-dim distribution of the initial weights for the N parameters for not extended fits
//     // If uni=kTRUE-> uniform weights
//     // If uni=kFALSE-> weights distributed according to Apr-n and E*-En
//     // Important step for the fit, if initial parameters are too dumb, fit won't be correct
//      
//     // For Gemini, gaussian 2D distri of apr vs estar   
//     Double_t mean_apr    = 28.;
//     Double_t mean_estar  = 1.5;
//     Double_t sigma_apr   = 1.5;
//     Double_t sigma_estar = 2.;
//     Double_t min_apr	  = GetParameter("APrimary").getMin();
//     Double_t max_apr	  = GetParameter("APrimary").getMax();
//     Double_t min_estar   = GetParameter("EStarPrimary").getMin();
//     Double_t max_estar   = GetParameter("EStarPrimary").getMax();
//       
//     Int_t    nbin_apr    = GetParameter("APrimary").numBins();
//     Int_t    nbin_estar  = GetParameter("EStarPrimary").numBins();
//     Int_t    nset	  = nbin_apr*nbin_estar;
// 	      
//     Int_t ii=0;
//        
//     printf("################ [Init weights] nset=%d || min_apr=%e, max_apr=%e, nbin_apr=%d || min_estar=%e, max_estar=%e, nbin_estar=%d ###############\n", nset, min_apr, max_apr, nbin_apr, min_estar, max_estar, nbin_estar);
//      
//     Double_t delta_apr   = (max_apr-min_apr)/nbin_apr;
//     Double_t delta_estar = (max_estar-min_estar)/nbin_estar;
//      
//     if(uni==kFALSE)
//       {
// 	TF2 *f2 = new TF2("f2","[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",min_apr, max_apr, min_estar, max_estar);
// 	f2->SetParameters(1.,mean_apr,sigma_apr,mean_estar,sigma_estar);
//           
// 	TCanvas *cc = new TCanvas("cc_weight");
// 	cc->cd();
//      
// 	TH2D* h2= new TH2D("init_par_distri","initial parameter distribution for the fit", nbin_apr, min_apr, max_apr, nbin_estar, min_estar, max_estar);
// 	h2->FillRandom("f2",10000);
// 	h2->Scale(1./h2->Integral());
// 	h2->Draw("surf");
//     
//      
// 	for(Int_t aa=1; aa<=nbin_apr; aa++)
// 	  { 
//     	    for(Int_t ee=1; ee<=nbin_estar; ee++)
//               {
// 	        if(ii<nset)
// 		  {
// 		    finit_weight->push_back(h2->GetBinContent(aa,ee));
// 		    // finit_weight[ii] = h2->GetBinContent(aa,ee);	    
// 		    printf("################# [Init weights Not Extended not uniform] nset=%d [Apr=%e, Estar=%e] || init_weight=%e ##################\n", ii, min_apr+aa*delta_apr, min_estar+ee*delta_estar, (*finit_weight)[ii]); 
// 		  }
// 	        ii++;
//     	      }    
// 	  }
//       }
//       
//     else
//       {
// 	for(Int_t aa=1; aa<=nbin_apr; aa++)
// 	  { 
//     	    for(Int_t ee=1; ee<=nbin_estar; ee++)
//               {
// 	        if(ii<nset)
// 		  {
// 		    //finit_weight[ii] = 1./GetNumberOfDataSets();
// 		    finit_weight->push_back(1./nset);  
// 		    printf("################# [Init weights Not Extended uniform] nset=%d [Apr=%e, Estar=%e] || init_weight=%e ##################\n", ii, min_apr+aa*delta_apr, min_estar+ee*delta_estar, (*finit_weight)[ii]); 
// 		  }
// 	        ii++;
//     	      }    
// 	  }
//       }	 
// 	        
//     return finit_weight;                 
//   }	
}	

