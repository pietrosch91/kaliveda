//Created by KVClassFactory on Wed Mar  4 16:50:19 2015
//Author: John Frankland,,,

#ifndef __SIMPLE2DMODEL_H
#define __SIMPLE2DMODEL_H

#include "GenericPDFModel.h"
#include "RooPolyVar.h"

namespace BackTrack {

   class Simple2DModel : public GenericPDFModel
   {

      ClassDef(Simple2DModel,1)//Simple model to test backtrack procedures

      RooRealVar fPar1,fObs1,fPar2,fObs2,a10,a11,a00,a01;
      RooPolyVar MEAN1,MEAN2;
      RooGaussian *gauss1,*gauss2;

      public:
      Simple2DModel();
      virtual ~Simple2DModel();

      //void CompareParameterWeights(RooAbsPdf& pdist);
   };

}
#endif
