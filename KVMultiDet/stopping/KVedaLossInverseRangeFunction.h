//Created by KVClassFactory on Tue Jan 26 14:52:11 2016
//Author: John Frankland,,,

#ifndef __KVEDALOSSINVERSERANGEFUNCTION_H
#define __KVEDALOSSINVERSERANGEFUNCTION_H

#include "TNamed.h"
#include "TSpline.h"

class TF1;

class KVedaLossInverseRangeFunction : public TNamed {
   TSpline3* fInterpol;//interpolation of inverse range-energy curve

public:
   KVedaLossInverseRangeFunction();
   KVedaLossInverseRangeFunction(TF1* range_func,
                                 Int_t A,
                                 Double_t riso,
                                 Int_t ninter = 50);
   virtual ~KVedaLossInverseRangeFunction();

   Double_t GetEnergy(Double_t range, Double_t riso);

   ClassDef(KVedaLossInverseRangeFunction, 1) //Dedicated optimised inversion of range-energy function
};

#endif
