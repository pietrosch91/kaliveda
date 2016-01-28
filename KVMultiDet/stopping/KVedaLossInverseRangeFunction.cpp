//Created by KVClassFactory on Tue Jan 26 14:52:11 2016
//Author: John Frankland,,,

#include "KVedaLossInverseRangeFunction.h"
#include "TF1.h"
#include "TMath.h"

#define LOG(x) TMath::Log(x)
#define EXP(x) TMath::Exp(x)

ClassImp(KVedaLossInverseRangeFunction)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVedaLossInverseRangeFunction</h2>
<h4>Dedicated optimised inversion of range-energy function</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVedaLossInverseRangeFunction::KVedaLossInverseRangeFunction()
   : fInterpol(nullptr)
{
   // Default constructor
}

KVedaLossInverseRangeFunction::KVedaLossInverseRangeFunction(TF1* range_func,
      Int_t A,
      Double_t riso,
      Int_t ninter)
   : fInterpol(nullptr)
{
   // Initialise from VEDALOSS range function object
   // A is mass of ion used in call to KVedaLossMaterial::GetRangeFunction
   // riso is value returned by KVedaLossMaterial::get_riso corresponding to
   // previous call to KVedaLossMaterial::GetRangeFunction
   // Default number of points in graph used for interpolation ninter=50
   // We make a graph from 0.01 MeV/nucleon to EMAX

   Double_t emin, emax, estep;
   range_func->GetRange(emin, emax);

   TGraph e_vs_range;
   emin = 0.01;
   emax /= A;

   Double_t logriso = LOG(riso);

   estep = (LOG(emax) - LOG(emin)) / (ninter - 1.);

   for (int i = 0; i < ninter; ++i) {
      double loge = LOG(emin) + i * estep;
      // we divide the calculated range by the current value of riso,
      // thereby effectively ensuring that we always calculate range
      // values corresponding to riso=1
      double R = LOG(range_func->Eval(EXP(loge) * A)) - logriso;
      e_vs_range.SetPoint(i, R, loge);
   }

   fInterpol = new TSpline3(range_func->GetTitle(), &e_vs_range);
}

KVedaLossInverseRangeFunction::~KVedaLossInverseRangeFunction()
{
   // Destructor
   SafeDelete(fInterpol);
}

Double_t KVedaLossInverseRangeFunction::GetEnergy(Double_t range, Double_t riso)
{
   // Given range in g/cm**2 and current value returned by KVedaLossMaterial::get_riso
   // (which takes into account any change in ion mass and/or material mass)
   // evaluate corresponding energy in MeV/nucleon

   if (range <= 0) return 0;
   return EXP(fInterpol->Eval(LOG(range) - LOG(riso)));
}
