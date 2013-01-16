//Created by KVClassFactory on Tue Dec  4 09:49:15 2012
//Author: dgruyer

#include "KVZALineFinder.h"
#include "KVCanvas.h"
#include "KVIdentificationResult.h"
#include "KVSpiderLine.h"
#include "KVIDGridEditor.h"
#include "TSystem.h"


ClassImp(KVZALineFinder)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVZALineFinder</h2>
<h4>(try to) find masse lines from charge lines</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVZALineFinder::KVZALineFinder(KVIDZAGrid* gg, TH2* hh)
{
   // constructor
  fGrid  = gg;
  fHisto = hh;
  
  fGeneratedGrid = 0;
  fLinearHisto   = 0;
//  fCanvas        = 0;
  
  fPoints  = new TGraph;
  fPoints->SetMarkerStyle(21);
  fNPoints = 0;
  fLines = new TList;
  SetAList("1,4,7,9,11,12,14,16,19,21,23,25,27,29,31,34,37,40,41");
  fBinsByZ = 60;
}

//________________________________________________________________

KVZALineFinder::KVZALineFinder (const KVZALineFinder& obj)  : KVBase()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVZALineFinder::~KVZALineFinder()
{
   // Destructor
}

//________________________________________________________________
void KVZALineFinder::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVZALineFinder::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVBase::Copy(obj);
   //KVZALineFinder& CastedObj = (KVZALineFinder&)obj;
}

//________________________________________________________________
void KVZALineFinder::SetAList(const char* Alist)
{
  Int_t zmax  = (Int_t)(((KVIDentifier*)fGrid->GetIdentifiers()->Last())->GetPID()+1);
  KVNucleus nuc;
  
  Int_t Z = 1;
  fAList.clear();
  KVNumberList Al(Alist);
  Al.Begin();
  while(!Al.End())
    {
    Int_t A = Al.Next();
    if(A<=0) A = nuc.GetAFromZ(Z,fGrid->GetMassFormula());
    fAList.push_back(A);
    Z++;
    }

  for(int i=Z+1; i<=zmax; i++) fAList.push_back(nuc.GetAFromZ(i,fGrid->GetMassFormula()));
}

//________________________________________________________________
TH2* KVZALineFinder::LinearizeHisto(Int_t nZbin)
{  
  Double_t zmin  = ((KVIDentifier*)fGrid->GetIdentifiers()->First())->GetPID() - 1.0;
  Double_t zmax  = ((KVIDentifier*)fGrid->GetIdentifiers()->Last())->GetPID() + 1.0;
  Int_t    zbins = (Int_t)(zmax-zmin)*nZbin;
  
  Double_t emin  = fHisto->GetXaxis()->GetXmin();
  Double_t emax  = fHisto->GetXaxis()->GetXmax();
  Int_t    ebins = fHisto->GetXaxis()->GetNbins();
  
  if(fLinearHisto) delete fLinearHisto;
  fLinearHisto = new TH2F("fLinearHisto", "", ebins, emin, emax, zbins, zmin, zmax);
  
  KVIdentificationResult *idr = new KVIdentificationResult;  
  KVReconstructedNucleus nuc;

  Int_t events_read = 0;
  for (int i=1; i<=fHisto->GetNbinsX(); i++) 
    {
    for (int j=1; j<=fHisto->GetNbinsY(); j++) 
      {
      Stat_t poids = fHisto->GetBinContent(i, j);
      if (poids == 0) continue;

      Axis_t x0 = fHisto->GetXaxis()->GetBinCenter(i);
      Axis_t y0 = fHisto->GetYaxis()->GetBinCenter(j);
      Axis_t wx = fHisto->GetXaxis()->GetBinWidth(i);
      Axis_t wy = fHisto->GetYaxis()->GetBinWidth(j);

      Double_t x, y;
      Int_t kmax = (Int_t) TMath::Min(20., poids);
      Double_t weight = (kmax == 20 ? poids / 20. : 1.);
      for (int k = 0; k < kmax; k++) 
        {
        nuc.Clear();
        x = gRandom->Uniform(x0 - .5 * wx, x0 + .5 * wx);
        y = gRandom->Uniform(y0 - .5 * wy, y0 + .5 * wy);
        if(fGrid->IsIdentifiable(x, y)) 
	  {
          fGrid->Identify(x, y, idr);
	  nuc.SetIdentification(idr);
          fLinearHisto->Fill(x, nuc.GetPID(), weight);
          }
        events_read += (Int_t) poids;
        IncrementLinear((Float_t) events_read);
        gSystem->ProcessEvents();
        }
      }
    }

  delete idr;  
  
  return fLinearHisto;
}

//________________________________________________________________
void KVZALineFinder::FindZLine(Int_t zz)
{
  fLinearHisto->SetAxisRange(zz-0.5,zz+0.5,"Y");

  KVIDLine* line = (KVIDLine*)fGrid->GetIdentifier(zz,2*zz+1); // A=2*zz+1 : dummy, A is ignored in this case
  if(!line)
    {
    int i=1;
    while(!(line = (KVIDLine*)fGrid->GetIdentifier(zz+i,2*zz+1))) i++;
    }
  if(!line) return;
  
  Double_t lX, lY;
  line->GetStartPoint(lX,lY);
  Int_t xbmin = 1;
  line->GetEndPoint(lX,lY);
  Int_t xbmax = fLinearHisto->GetXaxis()->FindBin(lX);
  Int_t width = (Int_t)((xbmax-xbmin)*1.0/100.);
  
  
  Int_t widthmax = (Int_t)((xbmax-xbmin)*1.0/30.);
  
  KVIDZALine* TheLine = 0;
  TheLine = (KVIDZALine*)((KVIDZAGrid*)fGeneratedGrid)->NewLine("ID");
  TheLine->SetZ(zz);
  TheLine->SetA(fAList.at(zz-1));
  
  TH1* projey = 0;
  Int_t i=0;
  Double_t lasty = zz;
  for(int xx=xbmin; xx<xbmax; xx+=width)
    {
    projey = fLinearHisto->ProjectionY("ProjectionAfterLin",TMath::Max(xx-width/2,xbmin),xx+width/2);
    Double_t xline = fLinearHisto->GetBinCenter(xx);
    Double_t yline = projey->GetMean();
    if((yline>zz-0.5)&&(yline<zz+0.5)) 
      {
      if(i==0) lasty = yline;
      if(TMath::Abs(yline-lasty)<0.2)
        {
        TheLine->SetPoint(i, xline, yline);
	lasty = yline;
        i++;
        if(width<widthmax) width*=1.2;
	}
      }
    delete projey;
    }
    
  if(TheLine->GetN()>5) fGeneratedGrid->Add("ID",TheLine);
}

//________________________________________________________________
void KVZALineFinder::FindALine(Int_t zz, Int_t width)
{
  fLinearHisto->SetAxisRange(zz-0.5,zz+0.5,"Y");

  KVIDLine* line = (KVIDLine*)fGrid->GetIdentifier(zz,2*zz+1); // A=2*zz+1 : dummy, A is ignored in this case
  if(!line)
    {
    int i=1;
    while(!(line = (KVIDLine*)fGrid->GetIdentifier(zz+i,2*zz+1))) i++;
    }
  if(!line) return;
  
  Double_t lX, lY;
  line->GetStartPoint(lX,lY);
  Int_t xbmin = 0;//fLinearHisto->GetYaxis()->FindBin(lX);
  line->GetEndPoint(lX,lY);
  Int_t xbmax = fLinearHisto->GetXaxis()->FindBin(lX);
    
  // create lines
  TList Lines;
  KVSpiderLine* tmp = 0;
  
  fLinearHisto->SetAxisRange(fLinearHisto->GetXaxis()->GetBinCenter(50),lX,"X");//fLinearHisto->GetXaxis()->GetXmax(),"X");
  TH1* tmph = fLinearHisto->ProjectionX(Form("tmph%d",zz));
  Int_t startBin = (Int_t)(tmph->GetMaximumBin()*0.95);
  delete tmph;
  
  TH1* projey = 0;
  if(startBin)
    {
    projey = fLinearHisto->ProjectionY("ProjectionAfterLin",startBin-width*3,startBin+width*3);
    int nfound = fSpectrum.Search(projey,0.05,"goff",0.0001);
    Float_t* xpeaks = fSpectrum.GetPositionX();
    Float_t* ypeaks = fSpectrum.GetPositionY();
    for(int p=0;p<nfound;p++) 
      {
      if(p>8) break;
      if(ypeaks[p]<10) continue;
      Double_t xline = fLinearHisto->GetBinCenter(startBin);
      Double_t yline = xpeaks[p];
      KVSpiderLine* tmp = 0;
      TIter next(&Lines);
      while((tmp=(KVSpiderLine*)next()))
        {
        if(TMath::Abs(tmp->GetY()-yline)<0.05) continue;
        }
      tmp = new KVSpiderLine(zz,-1);
      Lines.AddLast(tmp);
      tmp->AddPoint(xline,yline);
      fPoints->SetPoint(fNPoints,xline,yline);
      fNPoints++;
      }
    if(projey) delete projey;
    }
  SortLines(&Lines);
  
  Int_t nLines = Lines.GetSize();
  tmp = 0;
  for(int xx=startBin-width; xx>xbmin; xx-=width)
    {
    projey = fLinearHisto->ProjectionY("ProjectionAfterLin",xx-width/2,xx+width/2);
    int nfound = fSpectrum.Search(projey,0.05,"goff",0.02);
    Float_t* xpeaks = fSpectrum.GetPositionX();
    Float_t* ypeaks = fSpectrum.GetPositionY();
    for(int p=0;p<nfound;p++) 
      {
      if(p>=nLines+1) continue;
      if(ypeaks[p]<5) continue;
      Double_t xline = fLinearHisto->GetBinCenter(xx);
      Double_t yline = xpeaks[p];
      KVSpiderLine* tmp = 0;
      TIter next(&Lines);
      while((tmp=(KVSpiderLine*)next()))
	{
	if((TMath::Abs(tmp->GetY()-yline)<0.05)) break;
	}
      if(tmp)
        {
        if((TMath::Abs(tmp->GetX()-xline)<10*width)) tmp->AddPoint(xline,yline);
	}
      }
    if(projey) delete projey;
    }

  TIter nextli(&Lines);
  while((tmp=(KVSpiderLine*)nextli()))tmp->Sort(true);
  
  tmp = 0;
  for(int xx=startBin+width; xx<=xbmax-width/2; xx+=width)
    {
    projey = fLinearHisto->ProjectionY("ProjectionAfterLin",xx-width/2,xx+width/2);
    int nfound = fSpectrum.Search(projey,0.05,"goff",0.02);
    Float_t* xpeaks = fSpectrum.GetPositionX();
    Float_t* ypeaks = fSpectrum.GetPositionY();
    for(int p=0;p<nfound;p++) 
      {
      if(p>=nLines+1) continue;
      if(ypeaks[p]<5) continue;
      Double_t xline = fLinearHisto->GetBinCenter(xx);
      Double_t yline = xpeaks[p];
      KVSpiderLine* tmp = 0;
      TIter next(&Lines);
      while((tmp=(KVSpiderLine*)next()))
	{
	if(TMath::Abs(tmp->GetY()-yline)<0.05) break;
	}
      if(tmp)
        {
        if((TMath::Abs(tmp->GetX()-xline)<10*width)) tmp->AddPoint(xline,yline);
	}
      }
    if(projey) delete projey;
    }
  
  fLines->AddAll(&Lines);
}

void KVZALineFinder::SortLines(TList* Lines)
{
  int nn = Lines->GetSize();
  if(!nn) return;
  
  Int_t zz = ((KVSpiderLine*)Lines->At(0))->GetZ();
  
  double* yy = new double[nn];
  Int_t*    ii = new int[nn];
  for(int i=0; i<nn; i++) yy[i] = ((KVSpiderLine*)Lines->At(i))->GetY(0);
  
  KVNucleus nuc;  
  TMath::Sort(nn, yy, ii, kFALSE);
  Int_t iMostProb;
  for(int i=0; i<nn; i++){if(ii[i]==0) iMostProb=i;}
    
  Int_t aMostProb =  fAList.at(zz-1);
//   if(zz<=fAList.size()) aMostProb = fAList.at(zz-1);
//   else aMostProb = 2*zz+1;
  
  for(int i=0; i<nn; i++)
    {
    Int_t aa = aMostProb-(iMostProb-i);
    nuc.SetZandA(zz,aa);
    if((nuc.GetLifeTime()<pow(10,-6))) aa-=1;
    ((KVSpiderLine*)Lines->At(ii[i]))->SetA(aa);
    }

  return;
}


void KVZALineFinder::MakeGrid()
{  
  fGeneratedGrid = new KVIDZAGrid;
  fGeneratedGrid->SetName(Form("%s_Up",fGrid->GetName()));
  fGeneratedGrid->SetVarX(fGrid->GetVarX());
  fGeneratedGrid->SetVarY(fGrid->GetVarY());
  fGeneratedGrid->AddIDTelescopes(fGrid->GetIDTelescopes());
  
  KVIDZALine* TheLine = 0;
  KVSpiderLine* spline = 0;
  TIter next_line(fLines);
  while((spline = (KVSpiderLine*)next_line())) // generate KVLines from KVSpiderLines
    {
    if((spline->GetN()>5))
      {
      TheLine = (KVIDZALine*)((KVIDZAGrid*)fGeneratedGrid)->NewLine("ID");
      TheLine->SetZ(spline->GetZ());
      TheLine->SetA(spline->GetA());
      for(int i=0; i<spline->GetN(); i++)
        {
	TheLine->SetPoint(i, spline->GetX(i), spline->GetY(i));
	}
      fGeneratedGrid->Add("ID",TheLine);
      fLines->Remove(spline);
      }
    }	 

  TheLine = 0;
  spline = 0;
  TIter next(fLines);
  while((spline = (KVSpiderLine*)next())) // scan rejected lines
    {
    Int_t zl = spline->GetZ();
    if(zl<4) continue;
    Int_t aMostProb = 0;
    if(zl<=fAList.size()) aMostProb = fAList.at(zl-1);
    else aMostProb = 2*zl+1;
    int index = 0;
    KVIDZALine* oldLine = 0;
    oldLine=(fGeneratedGrid->GetZALine(spline->GetZ(),aMostProb,index));
    if(!oldLine)
      {
      continue;
      }
    TheLine = (KVIDZALine*)((KVIDZAGrid*)fGeneratedGrid)->NewLine("ID");
    TheLine->SetZ(spline->GetZ());
    TheLine->SetA(spline->GetA());
    Double_t dy = spline->GetY() - oldLine->Eval(spline->GetX());
    for(int i=0; i<oldLine->GetN(); i++)
      {
      TheLine->SetPoint(i, (oldLine->GetX()[i]), (oldLine->GetY()[i])+dy);
      }
    fGeneratedGrid->Add("ID",TheLine);
    }


}


void KVZALineFinder::DrawGrid()
{
  gIDGridEditor->StartViewer();
  gIDGridEditor->SetHisto(fLinearHisto);
  gIDGridEditor->SetGrid(fGeneratedGrid, kFALSE);
}


void KVZALineFinder::ProcessIdentification(Int_t zmin, Int_t zmax)
{
  LinearizeHisto(fBinsByZ);
  
  if(zmin<0) zmin = ((KVIDentifier*)fGrid->GetIdentifiers()->First())->GetZ();
  if(zmax<0) zmax = ((KVIDentifier*)fGrid->GetIdentifiers()->Last())->GetZ();
  
  KVIDLine* line = 0;
  int ww = 10;
  for(int z=zmin; z<=zmax; z++)
    {
    if(z>4)  ww = 20;
    if(z>6)  ww = 30;
    if(z>10) ww = 40;
    if(z>12) ww = 50;
    FindALine(z, ww);
    Increment(z-zmin);
         gSystem->ProcessEvents();
    }
  MakeGrid();
  
  Double_t zmGrid  = ((KVIDentifier*)fGrid->GetIdentifiers()->Last())->GetPID();
  if(zmax>=zmGrid) return;
  
  for(int z=zmax+1; z<=zmGrid; z++) 
    {
    FindZLine(z);
    Increment(z-zmin);
         gSystem->ProcessEvents();
    }
  
}





