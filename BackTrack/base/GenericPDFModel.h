//Created by KVClassFactory on Thu Mar  5 12:44:05 2015
//Author: John Frankland,,,

#ifndef __GENERICPDFMODEL_H
#define __GENERICPDFMODEL_H

#include "GenericModel.h"

using namespace RooFit ;

namespace BackTrack {

   class GenericPDFModel : public GenericModel
   {

      ClassDef(GenericPDFModel,1)//Generic model described by PDF

      protected:
      RooAbsPdf* theModel;//(conditional) PDF describing observable dependence on parameters
      Int_t fNGen;        //number of events to generate for each dataset

      public:
      GenericPDFModel();
      virtual ~GenericPDFModel();

      const RooAbsPdf* GetPDFModel() const { return theModel; }
      void SetNumGen(Int_t n) { fNGen=n; }
      Int_t GetNumGen() const { return fNGen; }

      RooDataSet* GenerateDataSet(RooAbsPdf& parDist, const RooArgList* params = 0);

      virtual RooDataSet* GetModelDataSet(RooArgList& par);
   };

}
#endif
