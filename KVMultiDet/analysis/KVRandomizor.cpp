//Created by KVClassFactory on Fri Jun 23 12:02:13 2017
//Author: Eric BONNET

#include "KVRandomizor.h"
#include "TMath.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TRandom3.h"

ClassImp(KVRandomizor)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVRandomizor</h2>
<h4>Test of generic class to perform sample on fonctions</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//-------------------------
KVRandomizor::KVRandomizor(Int_t ndim) : TNamed()
//-------------------------
{
   // Default constructor
   fNdMax = 6;
   if (fNd > fNdMax) {
      Warning("KVRandomizor", "To high dimensions (max : %d)", fNdMax);
      //return;
   }

   fNd = ndim;
   fMin = new Double_t[fNd];
   fMax = new Double_t[fNd];

}

//-------------------------
KVRandomizor::~KVRandomizor()
//-------------------------
{
   // Destructor
}

//-------------------------
void KVRandomizor::SetExtrema(Double_t vmax, Double_t vmin)
//-------------------------
{
   fVmax = vmax;
   fVmin = vmin;
}

//-------------------------
void KVRandomizor::SetRange(Double_t* min, Double_t* max)
//-------------------------
{
   for (Int_t ii = 0; ii < fNd; ii += 1) {
      fMin[ii] = min[ii];
      fMax[ii] = max[ii];
   }
}

//-------------------------
Double_t* KVRandomizor::GetPosition()
//-------------------------
{

   Double_t* pos = new Double_t[fNd];
   for (Int_t ii = 0; ii < fNd; ii += 1) {
      pos[ii] = GetPosition(ii);
   }
   return pos;

}

//-------------------------
Double_t KVRandomizor::GetPosition(Int_t idx)
//-------------------------
{

   return  gRandom->Uniform(fMin[idx], fMax[idx]);

}

//-------------------------
Double_t  KVRandomizor::ComputeValue(Double_t*)
//-------------------------
{

   Info("ComputeValue", "To be defined in child class");
   return 1;

}

//-------------------------
Bool_t KVRandomizor::TestValue(Double_t val)
//-------------------------
{
   Double_t prob = gRandom->Uniform(fVmin, fVmax);

   return (prob <= val);
}

//-------------------------
TH1* KVRandomizor::FillHisto(Int_t ntimes)
//-------------------------
{
   fNtest = 0;
   TH1* h1 = 0;
   if (fNd == 1) {
      h1 = FillHisto1D(ntimes);
   } else if (fNd == 2) {
      h1 = FillHisto2D(ntimes);
   } else if (fNd == 3) {
      h1 = FillHisto3D(ntimes);
   } else {
      Info("FillHisto", "method implemented for dimansion <= 3\nDo nothing ...");
      return h1;
   }
   Info("FillHisto", "stat=%d in %d tentatives => %lf", ntimes, fNtest, Double_t(ntimes) / fNtest);

   return h1;
}


//-------------------------
TH1* KVRandomizor::FillHisto1D(Int_t ntimes)
//-------------------------
{
   TH1* h1 = new TH1F(
      Form("histo%dD", fNd),
      Form("histo%dD", fNd),
      (fMax[0] - fMin[0]) * 100, fMin[0], fMax[0]
   );
   for (Int_t ii = 0; ii < ntimes; ii += 1) {
      fNtest += 1;
      Double_t* pos = GetPosition();
      if (TestValue(ComputeValue(pos))) {
         h1->Fill(pos[0]);
      } else {
         ii -= 1;
      }
      delete [] pos;
   }

   return h1;
}

//-------------------------
TH1* KVRandomizor::FillHisto2D(Int_t ntimes)
//-------------------------
{
   TH2* h1 = new TH2F(
      Form("histo%dD", fNd),
      Form("histo%dD", fNd),
      (fMax[0] - fMin[0]) * 100, fMin[0], fMax[0],
      (fMax[1] - fMin[1]) * 100, fMin[1], fMax[1]
   );
   for (Int_t ii = 0; ii < ntimes; ii += 1) {
      fNtest += 1;
      Double_t* pos = GetPosition();
      if (TestValue(ComputeValue(pos))) {
         h1->Fill(pos[0], pos[1]);
      } else {
         ii -= 1;
      }
      delete [] pos;
   }
   return h1;
}

//-------------------------
TH1* KVRandomizor::FillHisto3D(Int_t ntimes)
//-------------------------
{
   TH3* h1 = new TH3F(
      Form("histo%dD", fNd),
      Form("histo%dD", fNd),
      (fMax[0] - fMin[0]) * 20, fMin[0], fMax[0],
      (fMax[1] - fMin[1]) * 20, fMin[1], fMax[1],
      (fMax[2] - fMin[2]) * 20, fMin[2], fMax[2]
   );
   for (Int_t ii = 0; ii < ntimes; ii += 1) {
      fNtest += 1;
      Double_t* pos = GetPosition();
      if (TestValue(ComputeValue(pos))) {
         h1->Fill(pos[0], pos[1], pos[2]);
      } else {
         ii -= 1;
      }
      delete [] pos;
   }

   return h1;
}

//-------------------------
TH1* KVRandomizor::Test(Int_t ntimes)
//-------------------------
{
   Double_t min[3] = { -5, -5, -5};
   Double_t max[3] = {5, 5, 5};

   this->SetRange(min, max);
   this->SetExtrema(1, 0);

   return this->FillHisto(ntimes);
}
