//Created by KVClassFactory on Tue Jan 17 15:10:46 2012
//Author: bonnet

#ifndef __KVDALITZPLOT_H
#define __KVDALITZPLOT_H

#include "TH2F.h"

class KVDalitzPlot : public TH2F
{

   public:
   KVDalitzPlot();
	KVDalitzPlot(const char* name, const char* title, Int_t nbinsx=120, Double_t xlow=0., Double_t xup=1.2, Int_t nbinsy=120, Double_t ylow=0., Double_t yup=1.2);
   KVDalitzPlot (const KVDalitzPlot&) ;
   virtual ~KVDalitzPlot();
   void Copy (TObject&) const;
	
	Int_t FillAsDalitz(Double_t a1,Double_t a2,Double_t a3);
   
	ClassDef(KVDalitzPlot,1)//Fill 3D observables in a dalitz plot
};

#endif
