//Created by KVClassFactory on Thu Mar  5 12:44:05 2015
//Author: John Frankland,,,

#include "GenericPDFModel.h"
#include "RooUniform.h"

ClassImp(BackTrack::GenericPDFModel)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>GenericPDFModel</h2>
<h4>Generic model described by PDF</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

namespace BackTrack {

   GenericPDFModel::GenericPDFModel()
   {
      // Default constructor
      theModel=0;
      fNGen=1000;
   }

   GenericPDFModel::~GenericPDFModel()
   {
      // Destructor
      SafeDelete(theModel);
   }

   RooDataSet*GenericPDFModel::GenerateDataSet(RooAbsPdf& parDist, const RooArgList* params)
   {
      // Generate dataset for the observables using the given parameter probability distribution
      // If optional parameter list params is given, it is used instead of the default
      // defined parameter list of the model

      Info("GenerateDataSet","Generating %d datapoints with parameter distribution %s",
           GetNumGen(), parDist.GetName());
      RooProdPdf Multimodel("Multimodel","observables for parameter distributions", parDist,
                            Conditional(*theModel,GetObservables()));
      // get marginalized distributions model for observables
      if(!params) params = &GetParameters();
      RooAbsPdf* margin = Multimodel.createProjection(*params);
      RooDataSet* data = margin->generate(GetObservables(),fNGen);
      delete margin;
      return data;
   }

   RooDataSet*GenericPDFModel::GetModelDataSet(RooArgList& par)
   {
      // Generate dataset for model using the parameter definitions in the list.
      // We assume that this method is called with each parameter having its
      // range correctly defined.
      // We then generate events by using the product of the conditional PDF
      // defined for this model and a uniform distribution of parameters in the
      // given ranges.

      // uniform distribution of parameters
      RooUniform uni("uni","uniform parameter distribution",par);
      return GenerateDataSet(uni, &par);
   }

}
