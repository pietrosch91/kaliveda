//Created by KVClassFactory on Wed Mar  4 16:50:19 2015
//Author: John Frankland,,,

#ifndef __SIMPLE2DMODEL_H
#define __SIMPLE2DMODEL_H

#include "GenericModel.h"
#include "TH1.h"

namespace BackTrack {

   class Simple2DModel : public GenericModel
   {

      ClassDef(Simple2DModel,1)//Simple model to test backtrack procedures

      Int_t fNGen;        //number of events to generate for each dataset
      public:
      Simple2DModel();
      virtual ~Simple2DModel();

      void generateEvent(const RooArgList& parameters, RooDataSet& data);
      RooDataSet* GetModelDataSet(RooArgList &par);
      void SetNumGen(Int_t n) { fNGen=n; }
      Int_t GetNumGen() const { return fNGen; }

      TH1* GetParameterDistributions();
   };

}
#endif
