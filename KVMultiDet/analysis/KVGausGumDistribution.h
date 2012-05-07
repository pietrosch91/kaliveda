//Created by KVClassFactory on Mon Apr 20 14:02:31 2012
//Author: John Frankland,,,

#ifndef __KVGausGumDistribution_H
#define __KVGausGumDistribution_H

#include "TF1.h"

class KVGausGumDistribution : public TF1
{
   Int_t fRank;//rank of Gumbel distribution
   Double_t fkFac;
   Double_t fkGaussNor;
   
   public:
   KVGausGumDistribution();
   KVGausGumDistribution(const Char_t* name, Int_t k=1, Double_t xmin=0., Double_t xmax=100.);
   KVGausGumDistribution (const KVGausGumDistribution&) ;
   virtual ~KVGausGumDistribution();
   void Copy (TObject&) const;
   
   Double_t GDk(Double_t* x, Double_t* p);

   ClassDef(KVGausGumDistribution,1)//Sum of normalised Gaussian and Gumbel distributions
};

#endif
