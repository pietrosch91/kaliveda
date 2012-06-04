//Created by KVClassFactory on Mon Apr 20 14:02:31 2012
//Author: John Frankland,,,

#include "KVGausGumDistribution.h"
#include "TMath.h"
#include "Riostream.h"

ClassImp(KVGausGumDistribution)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGausGumDistribution</h2>
<h4>Sum of normalised Gaussian and Gumbel distributions</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVGausGumDistribution::KVGausGumDistribution()
   : TF1()
{
   // default ctor
}

//________________________________________________________________

KVGausGumDistribution::KVGausGumDistribution(const Char_t* name, Int_t k, Double_t xmin, Double_t xmax)
   : TF1(name, this, &KVGausGumDistribution::GDk, xmin, xmax, 5,
     "KVGausGumDistribution", "GDk")
{
   // normalised sum of Gaussian distribution and Gumbel distribution of k-th rank
   // f = eta*Gauss(mean,sigma) + (1-eta)*Gumbel(a,b)
   // free parameters: gauss: (mean,sigma) gumbel: (a,b) eta
   fRank = k;
   fkFac = TMath::Power(k,k)/TMath::Factorial(k-1);
   fkGaussNor = 1./sqrt(2*TMath::Pi());
   SetParName(0, "#eta");
   SetParName(1, "#mu");
   SetParName(2, "#sigma");
   SetParName(3, "#mu-a_{m}");
   SetParName(4, "b_{m}");
   SetParLimits(0,0.,1.);
   SetParLimits(1,GetXmin(),GetXmax());
   SetParLimits(2,0.01,20.);
   SetParLimits(3,0.01,30.);
   SetParLimits(4,0.01,20.);
   
   fGaussComp  = 0;
   fGumbelComp = 0;
}

//________________________________________________________________

KVGausGumDistribution::KVGausGumDistribution (const KVGausGumDistribution& obj)  : TF1()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVGausGumDistribution::~KVGausGumDistribution()
{
  if(fGaussComp)  fGaussComp->Delete();
  if(fGumbelComp) fGumbelComp->Delete();
   // Destructor
}

//________________________________________________________________

void KVGausGumDistribution::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVGausGumDistribution::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   TF1::Copy(obj);
   KVGausGumDistribution& CastedObj = (KVGausGumDistribution&)obj;
   CastedObj.fRank = fRank;
   CastedObj.fkFac = fkFac;
}

//________________________________________________________________

void KVGausGumDistribution::Draw(Option_t* option)
{

  TF1::Draw(option);
  
  TString opt(option);
  opt.ToUpper();
  if(!opt.Contains("GG")) return;
  
  SetLineColor(kBlack);
  SetNpx(2000);
  
  if(!fGaussComp)  fGaussComp = new TF1("GaussComp","[0]*exp(-0.5*((x-[1])/[2])**2)",GetXmin(),GetXmax());
  if(!fGumbelComp) fGumbelComp = new KVGumbelDistribution("GumbelComp",fRank,false,GetXmin(),GetXmax());
  
  fGaussComp->SetParameters(fkGaussNor*GetParameter(0)/GetParameter(2),GetParameter(1),GetParameter(2));
  fGumbelComp->SetParameters(GetParameter(1)-GetParameter(3),GetParameter(4),fkFac*(1.-GetParameter(0)));
  
  fGaussComp->SetLineColor(kBlue);
  fGaussComp->SetLineStyle(9);
  fGaussComp->SetLineWidth(1.2);
  fGaussComp->SetNpx(2000);
  
  fGumbelComp->SetLineColor(kRed);
  fGumbelComp->SetLineWidth(1.2);
  fGumbelComp->SetNpx(2000);
  
  fGaussComp->Draw("same");
  fGumbelComp->Draw("same");
    
//   cout << "DEBUG: KVGausGumDistribution::Draw(): am = "<< GetParameter(1)-GetParameter(3) << endl;
//   cout << "DEBUG: KVGausGumDistribution::Draw(): bm = "<< GetParameter(4) << endl;
//   cout << "DEBUG: KVGausGumDistribution::Draw(): Norm = "<< fkFac*(1.-GetParameter(0))/GetParameter(4) << endl;
//   cout << "DEBUG: KVGausGumDistribution::Draw(): eta = "<< GetParameter(0) << endl;
//   cout << "DEBUG: KVGausGumDistribution::Draw(): fkFac = "<< fkFac << endl;
  
  return;
}

//________________________________________________________________

Double_t KVGausGumDistribution::GDk(Double_t* x, Double_t* p)
{
   // Evaluate normalised sum of Gaussian and Gumbel distribution of rank fRank for x
   // with parameters
   // par[0] = eta
   // par[1] = mu
   // par[2] = sigma
   // par[3] = distance between gaussian and gumbel >=0
   // par[4] = b
   
   if(p[4]==0) return 0;
   
   Double_t am = p[1]-p[3];
   Double_t s = (*x - am)/p[4];
   Double_t gum = (fkFac/p[4]);
   Double_t es = -fRank*(s + TMath::Exp(-s));
   gum *= TMath::Exp(es);
   
   Double_t gau = fkGaussNor/p[2];
   s = (*x - p[1])/p[2];
   s = TMath::Power(s,2.)/2.;
   gau *= TMath::Exp(-s);
   
   Double_t gg = p[0]*gau+(1.-p[0])*gum;
   return gg;
}
