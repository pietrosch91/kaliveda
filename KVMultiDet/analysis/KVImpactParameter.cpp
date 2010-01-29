//Created by KVClassFactory on Fri Jan 15 18:14:06 2010
//Author: John Frankland,,,

#include "KVImpactParameter.h"

ClassImp(KVImpactParameter)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVImpactParameter</h2>
<h4>Impact parameter analysis tools</h4>
Use a histogram containing the distribution of some observable strongly-correlated with
the impact parameter in order to transform distributions of said observable into
impact parameter distributions, and to calculate the evolution of other quantities as a function
of the impact parameter.<br>
To use, start with a pointer to a 1-D histogram of the observable, TH1* data:<br>
<pre>
KVImpactParameter ip(data);
ip.MakeScale(npoints, bmax);
</pre>
<br>
The <a href="#KVImpactParameter:MakeScale">MakeScale</a> method calculates the relationship between the observable and the
impact parameter, using
End_Html
Begin_Latex
\hat{b}=\sqrt{\frac{\intop_{x=X}^{\infty}Y(x)\, dx}{\intop_{0}^{\infty}Y(x)\, dx}}
End_Latex
Begin_Html
To obtain the impact parameter distribution for some selection of events,
you need the distribution of the observable for the selection, TH1* obs_sel,
and then use:<br>
<pre>
TH1* ip_dist_sel = ip.GetIPDistribution(obs_sel);
</pre>
To obtain the impact-parameter evolution of some quantity, take a TH2* obscor, containing
the bidimensional plot of the quantity as a function of the observable, and then use<br>
<pre>
TGraph* ip_evol = ip.GetIPEvolution(obscor, "GetMean");
</pre>
e.g. to have the mean value of the quantity as a function of impact parameter.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVImpactParameter::KVImpactParameter(TH1* data, Option_t* evol)
{
   // Default constructor
   // Argument 'data' is pointer to data histogram containing distribution of the observable
   // which is used to calculate the impact parameter. Usually, this will be an observable
   // which is supposed to increase or decrease monotonically as a function of b.
   // By default, evol = "D" which means observable increases as b decreases.
   // Call with evol = "C" if the observable increases as b increases.
   fData = data;
   fEvol = evol;
   fIPScale = 0;
   fObsTransform = 0;
   Bmax= 1.0;
}

KVImpactParameter::~KVImpactParameter()
{
   // Destructor
   SafeDelete(fIPScale);
   SafeDelete(fObsTransform);
}

void KVImpactParameter::MakeScale(Int_t npoints, Double_t bmax)
{
   // Calculate the relationship between the impact parameter and the observable
   // whose distribution is contained in the histogram fData.
   // For a given value X of the observable x, the reduced impact parameter
   // b_hat is calculated from the distribution of x, Y(x), using the following formula:
   /*
Begin_Latex
\hat{b}=\sqrt{\frac{\intop_{x=X}^{\infty}Y(x)\, dx}{\intop_{0}^{\infty}Y(x)\, dx}}
End_Latex
   */
   // npoints = number of points for which to calculate the impact parameter.
   // The greater the number of points, the more accurate the results.
   // Default value is 100. Maximum value is number of bins in histogram of observable, fData.
   // bmax = 1.0 by default (reduced impact parameter scale).
   // Any other value for bmax will scale all the reduced impact parameters
   // (i.e. to give an absolute impact parameter in fm).
   
   Bmax = bmax;
   TH1*cumul = HM.CumulatedHisto(fData, fEvol.Data() ,-1,-1,"max");
   Int_t nbins = cumul->GetNbinsX();
   Int_t first_bin = 1;
   Int_t last_bin = nbins;
   npoints = TMath::Min(nbins,npoints);
   fIPScale = new TGraph(npoints);
   Double_t delta_bin = 1.*(last_bin-first_bin)/(npoints-1.);
   Int_t bin;
   for(int i=0;i<npoints;i++){
      bin = first_bin + i*delta_bin;
      Double_t et12 = cumul->GetBinCenter(bin);
      Double_t b = bmax * sqrt(cumul->GetBinContent(bin));
      fIPScale->SetPoint(i,et12,b);
   }
   delete cumul;
   
   fObsTransform = new TF1("fObsTransform", this, &KVImpactParameter::BTransform,
         0, 1, 0, "KVImpactParameter", "BTransform");
}

Double_t KVImpactParameter::BTransform(Double_t *x, Double_t *)
{
   // Function using the TGraph calculated with MakeScale in order to
   // transform distributions of the observable histogrammed in fData
   // into distributions of the impact parameter.
   // This function is used to generate the TF1 fObsTransform
   
   return fIPScale->Eval(*x);
}

TH1* KVImpactParameter::GetIPDistribution(TH1* obs, Int_t nbinx, Option_t* norm)
{
   // Transform the distribution of the observable contained in the histogram 'obs'
   // into a distribution of the impact parameter.
   // User's responsibility to delete histo.
   //
   // nbinx = number of bins in I.P. histo (default = 100)
	//  norm = "" (default) : no adjustment is made for the change in bin width due to the transformation
	//  norm = "width" : bin contents are adjusted for width change, so that the integral of the histogram
	//                   contents taking into account the bin width (i.e. TH1::Integral("width")) is the same.
   
   if(!fObsTransform){
      Error("GetIPDistribution", "Call MakeScale first to calculate correspondance observable<->i.p.");
      return 0;
   }
   return HM.ScaleHisto(obs, fObsTransform, 0, nbinx, -1, 0., Bmax, -1, -1, norm);
}

TGraph* KVImpactParameter::GetIPEvolution(TH2* obscor, TString moment, TString axis)
{
   // obscor = pointer to histogram containing bidim correlating some observable Y with
   // the observable used to calculate the impact parameter.
   // Return pointer to TGraph giving evolution of any given moment of Y as a function
   // of the impact parameter, with moment = "GetMean", "GetRMS", "GetKurtosis", etc.
   // (methods of TH1)
   // If the impact parameter observable is on the Y-axis of obscor, use axis="X"
   // (by default axis="Y", i.e. we assume that the I.P. observable is on the x axis).
   
   if(!fObsTransform){
      Error("GetIPEvolution", "Call MakeScale first to calculate correspondance observable<->i.p.");
      return 0;
   }
   TGraphErrors *gre = HM.GetMomentEvolution(obscor,moment,"",axis);
   TGraph* gr = HM.ScaleGraph(gre, fObsTransform, 0);
   delete gre;
   return gr;
}

