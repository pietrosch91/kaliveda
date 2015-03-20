//Created by KVClassFactory on Wed Mar  4 16:50:19 2015
//Author: John Frankland,,,

#ifndef __SIMPLE1DMODEL_H
#define __SIMPLE1DMODEL_H

#include "GenericPDFModel.h"
#include "RooPolyVar.h"

namespace BackTrack {

   class Simple1DModel : public GenericPDFModel
   {

      ClassDef(Simple1DModel,1)//Simple model to test backtrack procedures

      RooRealVar fPar,fObs,a0,a1;
      RooPolyVar MEAN;

      public:
      Simple1DModel();
      virtual ~Simple1DModel();

      virtual void ConstructPseudoPDF(Bool_t debug = kFALSE);

      void CompareParameterWeights(RooAbsPdf& pdist);
   };

}
#endif
