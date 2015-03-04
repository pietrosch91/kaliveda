/*
 * GenericModel.cpp
 *
 *  Created on: Mar 3, 2015
 *      Author: john
 */

#include "GenericModel.h"

#include <RooAbsBinning.h>

ClassImp(BackTrack::GenericModel)

namespace BackTrack {

GenericModel::GenericModel()
{
   fNDataSets=0;
}

GenericModel::~GenericModel() {
}

void GenericModel::AddParameter(const char* name, const char* title, Double_t min, Double_t max, Int_t nbins)
{
   // Define a named parameter for the model, with its associated range
   // of values and the number of 'bins' to be used to scan these values
    RooRealVar p(name,title,min,max);
    p.setBins(nbins);
    fParameters.addClone(p);
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

void GenericModel::ConstructPseudoPDF()
{
   // Build a parameterised pseudo-PDF from all the imported model datasets.
   // Each dataset is transformed into a RooNDKeysPdf kernel estimation p.d.f.
   // This p.d.f. models the distribution of an arbitrary input dataset as a
   // superposition of Gaussian kernels, one for each data point, each
   // contributing 1/N to the total integral of the p.d.f.

   if(!GetNumberOfDataSets()){
      Error("ConstructPseudoPDF", "import model datasets with ImportModelData() first");
      return;
   }
   //generate kernels
   RooArgList kernels;
   RooArgList fractions;
   Double_t initialWeight = 1./GetNumberOfDataSets();

   for(int i=0; i<GetNumberOfDataSets(); i++){

      Info("ConstructPseudoPDF", "Kernel estimation of dataset#%d...", i);
      RooNDKeysPdf* p = new RooNDKeysPdf(Form("NDK%d",i),
                                         Form("Kernel estimation of dataset#%d",i),
                                         GetObservables(),
                                         *((RooDataSet*)fDataSets[i]),
                                         "am");
      fNDKeys.Add(p);
      kernels.add(*p);
      if(i<(GetNumberOfDataSets()-1))
         fractions.addClone(RooRealVar(Form("W%d",i),Form("fractional weight of kernel-PDF #%d",i),initialWeight,0.,1.));
   }
   fModelPseudoPDF = new RooAddPdf("Model", "Pseudo-PDF constructed from kernels for model datasets", kernels, fractions, kTRUE);
}
} /* namespace BackTrack */
