//Created by KVClassFactory on Thu Nov 22 15:26:52 2012
//Author: dgruyer

#include "KVNuclearChart.h"
#include "KVNucleus.h"
#include "KVNucleusBox.h"
#include "KVCanvas.h"
#include "TStyle.h"
#include "TBox.h"
#include "TLine.h"
#include "TString.h"
#include "TLatex.h"

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

KVNuclearChart::KVNuclearChart(Int_t nMin, Int_t nMax, Int_t zMin, Int_t
zMax):fNucleusBoxList(kTRUE),fMagicList(kTRUE)
{
  fNmin = nMin;
  fNmax = nMax;
  fZmin = zMin;
  fZmax = zMax;
  
  if((fNmax<0)||(fZmax<0))
    {
     fNmax = 177;
     fZmax = 116;
    }
  
  fOwnHisto = 0;
  fHisto = 0;
  fShownNucleus = 0;
  fCanvas = 0;
  
  fSymbol = new TPaveText(0.65,0.16,0.75,0.01,"NDC");
  fSymbol->SetBorderSize(1);
  fInfo = new TPaveText(0.76,0.16,0.99,0.01,"NDC");
  fInfo->SetTextAlign(12);
//  fInfo->SetTextFont(42);
  fInfo->SetFillColor(kWhite);
  fInfo->SetLineColor(kBlack);
  fInfo->SetTextColor(kBlack);
  fInfo->SetBorderSize(1);
  
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
        if(nuc.IsStable()||nuc.GetLifeTime()>1.e-06)           
	  {
	  KVNucleusBox* nb = new KVNucleusBox(zz,nn);
	  nb->SetNuclearChart(this);
	  fNucleusBoxList.Add(nb);
	  }
	}
      }
    }
    
  Int_t mN[7] = {2,8,20,28,50,82,126};
  
  Double_t mnMin[7] = {0.,1.,12.,18.,46.,93.,98.};
  Double_t mnMax[7] = {10.,22.,40.,52.,90.,136.,126.*3};
  
  Double_t mzMin[7] = {0.2,0.2,6.,10.,26.,44.,76.};
  Double_t mzMax[7] = {9.,17.,30.,35.,53.,76.,95.};
  
  for(int i=0; i<7; i++)
    {
    Int_t num = mN[i];
    if((num>=fNmin+1)&&(num<=fNmax-1)&&(mzMin[i]<fZmax))
      {
      TLine* ll1 = new TLine(num-0.5, TMath::Max(fZmin*1.,mzMin[i]), num-0.5, TMath::Min(fZmax*1.,mzMax[i]));
      TLine* ll2 = new TLine(num+0.5, TMath::Max(fZmin*1.,mzMin[i]), num+0.5, TMath::Min(fZmax*1.,mzMax[i]));
      fMagicList.AddLast(ll1);
      fMagicList.AddLast(ll2);
      if(num>=20)
        {
	TLatex* label = new TLatex(num*1.,mzMin[i]-1.5,Form("N=%d",num));
	label->SetTextAlign(23);
	label->SetTextSize(0.025);
	label->SetTextFont(42);
        fMagicList.AddLast(label);
	}
      }
    }
    
  for(int i=0; i<7; i++)
    {
    Int_t num = mN[i];
    if((num>=fZmin+1)&&(num<=fZmax-1)&&(mnMin[i]<fNmax))
      {
      TLine* ll1 = new TLine(TMath::Max(fNmin*1.,mnMin[i]), num-0.5, TMath::Min(fNmax*1.,mnMax[i]), num-0.5);
      TLine* ll2 = new TLine(TMath::Max(fNmin*1.,mnMin[i]), num+0.5, TMath::Min(fNmax*1.,mnMax[i]), num+0.5);
      fMagicList.AddLast(ll1);
      fMagicList.AddLast(ll2);
      if(num>=20)
        {
	TLatex* label = new TLatex(mnMin[i]-1.5,num*1.,Form("Z=%d",num));
	label->SetTextAlign(32);
	label->SetTextSize(0.025);
	label->SetTextFont(42);
        fMagicList.AddLast(label);
	}
      }
    }
    
  return;
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
   if(fCanvas)
     {
     fCanvas->ResetDisabledClass();
     fCanvas->ResetDisabledObject();
     }
   if((fHisto)&&(fOwnHisto)) delete fHisto;
   if(fSymbol) delete fSymbol;
   if(fInfo) delete fInfo;
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

  TString opt(option);
  Bool_t DrawSame = opt.Contains("same");
  
  if(DrawSame&&gPad)
    {
    TObject* obj = 0;
    TIter next(gPad->GetListOfPrimitives());
    while((obj=next()))
      {
      if(obj->InheritsFrom("TH2"))
        {
	fHisto = (TH2*) obj;
	break;
	}
      }
    }
  else
    {
    KVCanvas* cc = new KVCanvas;
    TPad* pp = (TPad*) cc->cd();
  
    Double_t marging = 0.001;
    pp->SetTopMargin(marging);
    pp->SetRightMargin(marging);
    pp->SetBottomMargin(marging);
    pp->SetLeftMargin(marging);
    
    fHisto = new TH2F("NuclearChart", "", fNmax-fNmin+2, fNmin-1, fNmax+1, fZmax-fZmin+2, fZmin-1, fZmax+1);
    fOwnHisto = kTRUE;
    TAxis* ax = 0;
    ax = fHisto->GetXaxis();
    ax->SetNdivisions(000);
    ax = fHisto->GetYaxis();
    ax->SetNdivisions(000);
    
    fHisto->Draw();
    }
    
  if(gPad->GetCanvas()->InheritsFrom("KVCanvas"))
    {
    KVCanvas* cc = (KVCanvas*) gPad->GetCanvas();
    cc->DisableClass("TLine");
    cc->DisableClass("KVNucleusBox");
    cc->DisableClass("TLatex");
    cc->DisableObject(fSymbol);
    cc->DisableObject(fInfo);
    fCanvas = cc;
    }
    
   fNucleusBoxList.Execute("SetDrawMode",Form("%d",DrawSame));
   fNucleusBoxList.Execute("EnableToolTip","");
   fNucleusBoxList.Execute("Draw","");
   fMagicList.Execute("Draw","");
      
  return;
}

void KVNuclearChart::ShowNucleusInfo(KVNucleus* nuc)
{
  if((fShownNucleus)&&(fShownNucleus==nuc))
    {
    fSymbol->Clear();
    fInfo->Clear();
    gPad->GetListOfPrimitives()->Remove(fSymbol);
    gPad->GetListOfPrimitives()->Remove(fInfo);
    gPad->Modified();
    gPad->Update();
    fShownNucleus = 0;
    return;  
    }
  
  fShownNucleus = nuc;
  fInfo->Clear();
  fInfo->AddText(Form("Z = %d  N = %d", nuc->GetZ(), nuc->GetN()));
  if(nuc->GetLifeTime()>1.e+30) fInfo->AddText("T_{1/2} > 1.00e+30 s");
  else fInfo->AddText(Form("T_{1/2} = %3.2e s",nuc->GetLifeTime()));
  if(!fOwnHisto&&fHisto) 
    {
    TAxis* ax = fHisto->GetXaxis();
    Int_t xmin = ax->FindBin(nuc->GetN()-0.5);
    Int_t xmax = ax->FindBin(nuc->GetN()+0.5);
    ax = fHisto->GetYaxis();
    Int_t ymin = ax->FindBin(nuc->GetZ()-0.5);
    Int_t ymax = ax->FindBin(nuc->GetZ()+0.5);
    Int_t M = fHisto->Integral(xmin,xmax,ymin,ymax);
    fInfo->AddText(Form("M = %d", M));
    }
  gPad->GetListOfPrimitives()->Remove(fInfo);
  fInfo->Draw("same");
  
  TString symbText = nuc->GetSymbol();
  symbText.ReplaceAll(Form("%d",nuc->GetA()),Form("^{%d}",nuc->GetA()));
//  symbText += Form("_{%d}", nuc->GetN());
  
  if((!fSymbol)||(!fInfo)) return;
  
  fSymbol->Clear();
  fSymbol->AddText(symbText.Data());
  gPad->GetListOfPrimitives()->Remove(fSymbol);
  fSymbol->SetFillColor(kBlack);
  fSymbol->SetLineColor(kBlack);
  fSymbol->SetTextColor(kWhite);
  fSymbol->Draw("same");
  gPad->Modified();
  gPad->Update();
  
}









