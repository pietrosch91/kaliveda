//Created by KVClassFactory on Thu Nov 22 15:26:52 2012
//Author: dgruyer

#include "KVNuclearChart.h"
#include "KVNucleus.h"
#include "KVCanvas.h"
#include "TStyle.h"
#include "TBox.h"
#include "TLine.h"
#include "TString.h"

ClassImp(KVNuclearChart)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVNuclearChart</h2>
<h4>Used to draw nuclear chart</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

// KVNuclearChart::KVNuclearChart()
// {
//    // Default constructor
//    fNmin = -1;
//    fNmax = -1;
//    fZmin = -1;
//    fZmax = -1;
//    fHisto = 0;
// }

KVNuclearChart::KVNuclearChart(Int_t nMin, Int_t nMax, Int_t zMin, Int_t zMax)
{
   fNmin = nMin;
   fNmax = nMax;
   fZmin = zMin;
   fZmax = zMax;
   fHisto = 0;
}

//________________________________________________________________

KVNuclearChart::KVNuclearChart (const KVNuclearChart& obj)  : KVBase()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVNuclearChart::~KVNuclearChart()
{
   // Destructor
   if(fHisto) delete fHisto;
}

//________________________________________________________________

void KVNuclearChart::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVNuclearChart::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVBase::Copy(obj);
   //KVNuclearChart& CastedObj = (KVNuclearChart&)obj;
}

void KVNuclearChart::Draw(Option_t* option)
{
  gStyle->SetOptStat(0);
  if((fNmax<0)||(fZmax<0))
    {
     fNmax = 177;
     fZmax = 116;
    }

  TString opt(option);
  Bool_t DrawSame = opt.Contains("same");
  if(!DrawSame)
    {
    KVCanvas* cc = new KVCanvas;
    TPad* pp = (TPad*) cc->cd();
  
    Double_t marging = 0.001;
    pp->SetTopMargin(marging);
    pp->SetRightMargin(marging);
    pp->SetBottomMargin(marging);
    pp->SetLeftMargin(marging);
    
    TH2F* fHisto = new TH2F("NuclearChart", "", 200, fNmin-1, fNmax+1, 200, fZmin-1, fZmax+1);
    TAxis* ax = 0;
    ax = fHisto->GetXaxis();
    ax->SetNdivisions(000);
    ax = fHisto->GetYaxis();
    ax->SetNdivisions(000);
    
    fHisto->Draw();
    }
  
  
  KVNucleus nuc;
  for(int zz=fZmin; zz<fZmax; zz++)
    {
    KVNumberList ll = nuc.GetKnownARange(zz);
    ll.Begin();
    while(!ll.End())
      {
      Int_t aa = ll.Next();
      nuc.SetZandA(zz,aa);
      Int_t nn = aa - zz;
      if((nn>=fNmin)&&(nn<fNmax))
        {
        TBox* bb = new TBox(nn-0.4,zz-0.4,nn+0.4,zz+0.4);
        if(nuc.IsStable())           
	  {
	  bb->SetLineColor(kBlack);
	  bb->SetLineWidth(2);
	  if(DrawSame) bb->SetFillStyle(0);
	  else bb->SetFillColor(kBlack);
 	  }
        else if(nuc.GetLifeTime()>pow(10,-6)) 
	  {
	  bb->SetLineColor(kBlack);
	  if(DrawSame) bb->SetFillStyle(0);
	  else bb->SetFillColor(kGray+2);
	  }
	else 
	  {
	  delete bb;
	  continue;
	  }
	bb->SetToolTipText(Form("%s%d",nuc.GetSymbol(),zz),250);
        bb->Draw("same");
	}
      }
    }
    
  Int_t mN[7] = {2,8,20,28,50,82,126};
  
  Double_t mnMin[7] = {0.,2.,12.,18.,46.,93.,98.};
  Double_t mnMax[7] = {9.,19.,40.,51.,90.,136.,126.*3};
  
  Double_t mzMin[7] = {0.,0.,6.,11.,27.,44.,76.};
  Double_t mzMax[7] = {5.,17.,29.,35.,53.,76.,95.};
  
  for(int i=0; i<7; i++)
    {
    Int_t num = mN[i];
    if((num>=fNmin+1)&&(num<=fNmax-1)&&(mzMin[i]<fZmax))
      {
      TLine* ll1 = new TLine(num-0.5, TMath::Max(fZmin*1.,mzMin[i]), num-0.5, TMath::Min(fZmax*1.,mzMax[i]));
      TLine* ll2 = new TLine(num+0.5, TMath::Max(fZmin*1.,mzMin[i]), num+0.5, TMath::Min(fZmax*1.,mzMax[i]));
      ll1->Draw("same");
      ll2->Draw("same");
      }
    }
    
  for(int i=0; i<7; i++)
    {
    Int_t num = mN[i];
    if((num>=fZmin+1)&&(num<=fZmax-1)&&(mnMin[i]<fNmax))
      {
      TLine* ll1 = new TLine(TMath::Max(fNmin*1.,mnMin[i]), num-0.5, TMath::Min(fNmax*1.,mnMax[i]), num-0.5);
      TLine* ll2 = new TLine(TMath::Max(fNmin*1.,mnMin[i]), num+0.5, TMath::Min(fNmax*1.,mnMax[i]), num+0.5);
      ll1->Draw("same");
      ll2->Draw("same");
      }
    }
    
  return;
}











