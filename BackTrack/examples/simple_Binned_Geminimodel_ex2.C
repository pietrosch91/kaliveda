/* simple2Dmodel_ex2.C
 *
 * Example of use of class BackTrack::Binned_GeminiModel_2 in order to demonstrate
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
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "MyGlobalFunc.h"  //Class with new RooArgCmd for the fit

#include "Binned_GeminiModel_2.h"


using namespace RooFit;
using namespace MyGlobalFunc;

BackTrack::Binned_GeminiModel_2* model=0;

void simpleGeminimodel_ex2()
{
   model = new BackTrack::Binned_GeminiModel_2();
   
   //Experiment
   TFile *ffile_exp = new TFile("../../Exp_Trees/4048/ExperimentalTree_SYS4048_ZPr13.root");
   
   TString ss0(ffile_exp->GetName());
   TString ss1 = (ss0(49,2));
   TString ss2 = (ss0(41,4));      
   Int_t ZZ    = ss1.Atoi(); 
   Int_t SYS   = ss2.Atoi();
      
   TTree *ftree_exp = (TTree*) ffile_exp->Get(Form("TT%d", ZZ));

   
   //BACKTRACKING
   if(ftree_exp)
   {
     //Generate the Roodata data from experiment with good event recon
     //The experimental TTree must have the same TBranch names for observables
     
     RooArgList *ListExpObsRecon = (RooArgList*) (model->GetObservables()).snapshot("ListExpObsRecon");
     RooRealVar recon("GoodRecon","GoodRecon",-2,1);
     ListExpObsRecon->addClone(recon);
     ListExpObsRecon->Print();
     //TString s("GoodRecon>=0 && EkLab_alpha_mean>0");
     TString s("GoodRecon>=0");
            
     const RooFormulaVar cutexp("cutexp", "cut in parameters in Exp TTree", s, *ListExpObsRecon);	     	
     
     RooDataSet data("data","data to be fitted", ftree_exp, *ListExpObsRecon, cutexp);
     RooDataHist datahist("datahist","datahist to be fitted", model->GetObservables(), data);
     Int_t ndata_entries=datahist.sumEntries();
     printf("########## ndata_entries = %d\n",ndata_entries);
     datahist.Print(); 
     datahist.Draw();
     
     
     //==================
     //==   Fit Ranges ==
     //==================
     //model->GetObservable("APrimaryWoN").setRange("range_apr",23,30);
     //model->GetObservable("EStarPrimaryWoN_MeV").setRange("range_estar",0,80);

     
     //------Initialise model-------
     model->SetExtended(kFALSE);
     model->IsExtended();
     model->ImportModelData();
     //ConstructPseudoPDF(nentries, debug, uniform)
     model->ConstructPseudoPDF(ndata_entries, kTRUE, kFALSE);
     model->saveWorkspace();
     //model->fitTo(datahist, Save(), NumCPU(4,1), SumW2Error(kTRUE), PrintLevel(3), Minimizer("TMinuit","migrad"), Extended(kFALSE), Offset(kTRUE), SetEpsilon(0.1), SetMaxCalls(200000));
     model->fitTo(datahist, Save(), NumCPU(4,1), SumW2Error(kTRUE), PrintLevel(1), Minimizer("GSLMultiMin","conjugatefr"), Extended(kFALSE), Offset(kTRUE), SetMaxCalls(300000));
     //model->fitTo(datahist, Save(), NumCPU(4), SumW2Error(kTRUE), PrintLevel(3), Minimizer("GSLSimAn"), Extended(kFALSE), Offset(kTRUE));
     //model->chi2FitTo(datahist, Save(), NumCPU(4,1), PrintLevel(3), Minimizer("TMinuit","migrad"), Extended(IsExtended()));



     //==================
     //==    Plots     ==
     //==================
     
     //-------------Return fitted parameters (see Simple2DModel.h")------------
     new TCanvas("GetParameterDistributions","GetParameterDistributions");
     model->GetParameterDistributions()->Draw("lego");


     //---------------Results compared to experimental data----------------
     RooPlot *plot = model->GetObservable("APrimaryWoN").frame();
     data.plotOn(plot);
     model->GetPseudoPDF()->plotOn(plot);
     TCanvas*c = new TCanvas("Observables_results","Observables_results");
     c->Divide(2,1);
     c->cd(1);
     plot->Draw();
     
//      plot = model->GetObservable("EkLab_alpha_mean").frame();
//      data.plotOn(plot);
//      model->GetPseudoPDF()->plotOn(plot);
//      c->cd(2);
//      plot->Draw();
     
//      plot = model->GetObservable("Z_plf").frame();
//      data.plotOn(plot);
//      model->GetPseudoPDF()->plotOn(plot);
//      c->cd(3);
//      plot->Draw();	
// 	  
//      plot = model->GetObservable("EStarPrimaryWoN_MeV").frame();
//      data.plotOn(plot);
//      model->GetPseudoPDF()->plotOn(plot);
//      c->cd(2);
//      plot->Draw();
     
     TH2 *hh = (TH2*) (model->GetPseudoPDF())->createHistogram("APrimaryWoN,EStarPrimaryWoN_MeV");
     TH2 *hhdata = (TH2*) data.createHistogram("APrimaryWoN,EStarPrimaryWoN_MeV");

//        TH2 *hh = (TH2*) (model->GetPseudoPDF())->createHistogram("APrimaryWoN,EkLab_alpha_mean");
//        TH2 *hhdata = (TH2*) data.createHistogram("APrimaryWoN,EkLab_alpha_mean");
     
     if(hh && hhdata)
         {
          TCanvas *ccc = new TCanvas("Observables_results_2D", "2D_Observables_results");
	  ccc->Divide(2,2);
          ccc->cd(1);
          hhdata->Draw("colz");
	  ccc->cd(2);
	  hhdata->Draw("lego");
	  ccc->cd(3);
	  hh->Draw("colz");
	  ccc->cd(4);
	  hh->Draw("lego");
	  
          hh->Write();
	  hhdata->Write();
         }
   
     else
         {
          printf("WTF, impossible to createHistogram from GetPseudoPDF ???\n");
         } 
    
    //------------------------------------------------------------------------ 
       
   RooPlot* pl = model->GetParameter("APrimary").frame();
   model->GetParamDataHist()->plotOn(pl);   
   new TCanvas("Plot_result_Apr","Plot PAR1");
   pl->Draw();
   
   pl = model->GetParameter("EStarPrimary").frame();
   model->GetParamDataHist()->plotOn(pl);  
   new TCanvas("Plot_result_EStar","Plot PAR2");
   pl->Draw();
   
   }  
   
   else
   {
    cout<<"Experimental tree not found..."<<endl;
   }
}
