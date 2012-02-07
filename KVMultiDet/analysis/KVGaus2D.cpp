//Created by KVClassFactory on Tue Feb  7 16:49:15 2012
//Author: bonnet

#include "KVGaus2D.h"
#include "TH2.h"
#include "Riostream.h"
#include "TMath.h"

ClassImp(KVGaus2D)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGaus2D</h2>
<h4>Fit gaussien a deux dimensions</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//________________________________________________________________
KVGaus2D::KVGaus2D()
{
   // Default constructor
	SetName("Gaus2D");
	is2D=kTRUE;
	
}

//________________________________________________________________

KVGaus2D::KVGaus2D (const KVGaus2D& obj)  : KVAutoFit()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

}

//________________________________________________________________
KVGaus2D::~KVGaus2D()
{
   // Destructor
}

//________________________________________________________________
Bool_t KVGaus2D::NewFunction_2D()
{	
	
	Int_t nf = lfunc->GetEntries();
	
	f2Dfit = new TF2(Form("func_%d",++nf),
		this,
		&KVGaus2D::f2D,
		Xmin,
		Xmax,
		Ymin,
		Ymax,
		6,
		GetName(),
		"f2D"
	);
		
	lfunc->Add(f2Dfit);
	
	hclone->GetXaxis()->SetRangeUser(Xmin,Xmax);
	hclone->GetYaxis()->SetRangeUser(Ymin,Ymax);
		
	f2Dfit->SetParameters(
		hclone->GetMean(1),
		hclone->GetRMS(1),
		hclone->GetMean(2),
		hclone->GetRMS(2),
		((TH2*)hclone)->GetCorrelationFactor(),
		hclone->Integral()
	);

	return kTRUE;
}

//________________________________________________________________
Double_t	KVGaus2D::f2D(Double_t *xx,Double_t *para)
{

//Function gaussienne 2D
//
// E1 vs E2

Double_t e2 = xx[0];
Double_t e1 = xx[1];

Double_t E2 	= 	para[0];
Double_t sigE2	=	para[1];
Double_t E1		= 	para[2];
Double_t sigE1	=	para[3];
Double_t rho 	= 	para[4];
Double_t norm 	=	para[5];

Double_t dE1 	= 	E1-e1;	
Double_t dE2 	= 	E2-e2;	
 
Double_t A = -0.5/( (1.-TMath::Power(rho,2.))*TMath::Power(sigE1*sigE2,2.) )*( TMath::Power(dE1*sigE2,2.) + TMath::Power(dE2*sigE1,2.) - 2.*rho*dE1*dE2*sigE1*sigE2 );
Double_t det = (1.-TMath::Power(rho,2.))*TMath::Power(sigE1*sigE2,2.);
Double_t P = norm * 1./det*exp(A);

return P;

}

//________________________________________________________________
void KVGaus2D::Save()
{

//Warning("Save","Do Nothing");
if (lfunc->GetEntries()==0) return;

TF1* f1;
ofstream fout(Form("%s",hfit->GetName()));
TIter it(lfunc);
fout<< lfunc->GetEntries() << endl;
while ( (f1 = (TF1* )it.Next()) ){
	fout<<f1->GetName() << endl;
	fout << GetName() << "::f2D" << endl;
	
	Double_t x1,x2,y1,y2;
	f1->GetRange(x1,y1,x2,y2);
	fout << x1 << " " << x2 << " " << y1 << " " << y2 << endl;
	for (Int_t ii=0;ii<f1->GetNpar();ii+=1){
		fout << ii << " " << f1->GetParameter(ii) << " " << f1->GetParError(ii) << endl;
	}
}
fout.close();

}
