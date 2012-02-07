//Created by KVClassFactory on Tue Feb  7 15:17:39 2012
//Author: bonnet

#include "KVAutoFit.h"
#include "Riostream.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH2.h"

ClassImp(KVAutoFit)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVAutoFit</h2>
<h4>Manage SemiAutomatic Fits</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVAutoFit::KVAutoFit()
{
   // Default constructor
	// 
	// le booleen is2D indique si les histo et donc les fonctions
	// doivent etre a 2D ou 1D
	// par defaut is2D= kFALSE dans la methode init
	
	SetName("KVAutoFit");
	init();
	Clear();
	/*
	is2D = kTRUE;
	*/
}

//________________________________________________________________
void KVAutoFit::init()
{
   lfunc = new KVHashList();
	lfunc->SetOwner(kTRUE);
	
	c1 = new TCanvas(Form("%s_canvas",GetName()),Form("%s_canvas",GetName()),0,0,1200,600);
	
	c1->AddExec("interval","ap->GetInterval()");
	c1->AddExec("recommence","ap->Recommence()");
	c1->AddExec("gather","ap->Gather()");
	
	hfit=0;
	hclone=0;
	
	f1Dfit=0;
	f2Dfit=0;
	
	lplabel = new KVHashList(); lplabel->SetOwner(kTRUE);
	lplabel->Add(new TPaveLabel(0.10,0.92,0.25,0.99, "Annule Tout","NDC"));
	lplabel->Add(new TPaveLabel(0.27,0.92,0.44,0.99, "Annule Dernier","NDC"));  		
	lplabel->Add(new TPaveLabel(0.46,0.92,0.55,0.99, "Gather","NDC")); 			 
	lplabel->Add(new TPaveLabel(0.57,0.92,0.65,0.99, "Suivant","NDC"));			 
	
	lplabel->Execute("SetTextSize","0.625");
	lplabel->Execute("SetFillColor","10");
	
	gStyle->SetOptTitle(0);
	
	is2D = kFALSE;
	
	lhisto = 0;
 	nhisto = 0;
}

//________________________________________________________________

KVAutoFit::KVAutoFit (const KVAutoFit& obj)  : KVBase()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

//________________________________________________________________

void KVAutoFit::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVAutoFit::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVBase::Copy(obj);
   //KVAutoFit& CastedObj = (KVAutoFit&)obj;
}

//________________________________________________________________
void KVAutoFit::Clear(Option_t* opt)
{
   //Efface les fonctions de fit
	//et le contenu du canvas
	lfunc->Clear();
	c1->Clear();
	ClearRange();
	
}
//________________________________________________________________
void KVAutoFit::ClearRange(void)
{
   //Efface les bornes min et max pour le prochain fit
	XminSet = kFALSE;
	XmaxSet = kFALSE;
	
	Xmin = Xmax = -1;
	Ymin = Ymax = -1;
	
}

//________________________________________________________________
KVAutoFit::~KVAutoFit()
{
   // Destructor
	Clear();
	
	delete lfunc;
	delete c1;
	hfit=0;
	if (hclone) delete hclone; hclone=0;
	f1Dfit=0;
	f2Dfit=0;
	delete lplabel;
	
}

//________________________________________________________________
void KVAutoFit::SetHistos(KVHashList* lh)
{
	
	lhisto = lh;
	nhisto=0;
	SetHisto((TH1F* )lhisto->At(nhisto));


}

//________________________________________________________________
void KVAutoFit::Print(Option_t* opt) const
{
	Info("Print","Liste des fonctions pour l'histogram %s",hfit->GetName());
	lfunc->Print();

}

//________________________________________________________________
Bool_t KVAutoFit::NewFunction()
{
	//Generation de la fonction 1D ou 2D
	//definie dans la methode NewFunction_1D ou NewFunction_2D
	
	if (is2D)
		return NewFunction_2D();
	else 
		return NewFunction_1D();
	
}
//________________________________________________________________
Bool_t KVAutoFit::NewFunction_1D()
{
	
	Info("NewFunction_1D","To be defined in child class");
	return kFALSE;
}
//________________________________________________________________
Bool_t KVAutoFit::NewFunction_2D()
{	
	Info("NewFunction_2D","To be defined in child class");
	return kFALSE;

}

//________________________________________________________________
Double_t	KVAutoFit::f2D(Double_t *xx,Double_t *para)
{

return 0;

}

//________________________________________________________________
Double_t	KVAutoFit::f1D(Double_t *xx,Double_t *para)
{

return 0;

}

//________________________________________________________________
void KVAutoFit::SetHisto(TH1* hh)
{
	Clear();

	Bool_t ok = kFALSE;
	if ( hh->InheritsFrom("TH2") ){
		if (is2D)
			ok = kTRUE;
	}
	else if ( hh->InheritsFrom("TH1") ){
		if (!is2D)
			ok = kTRUE;
	}
	
	if (!ok) {
		Warning("SetHisto","La dimension de lhisto n'est pas compatible avec celle definie ds cette classe");
		return;
	}
	if (hclone)
		delete hclone;
	hclone=(TH1* )hh->Clone("ap_clone");
	
	c1->cd();

	hfit = hh;
	if (is2D)	hfit->Draw("colz");
	else			hfit->Draw();
	
	lplabel->Execute("Draw","");
	
	c1->Update();

}

//________________________________________________________________
void KVAutoFit::GetInterval()
{
   
	Int_t event = gPad->GetEvent();
	TObject *select = gPad->GetSelected();
	if (!select) {}
	else {
		if (select->InheritsFrom("TPaveLabel"))
			return;
	}
	
	if (event==kButton1Down){
		
		Int_t xx = gPad->GetEventX();
		Int_t yy = gPad->GetEventY();
		
		Xmin = gPad->AbsPixeltoX(xx);
		Ymin = gPad->AbsPixeltoY(yy);
		
		XminSet=kTRUE;
	}
	else if (event==kButton2Up){
		
		Int_t xx = gPad->GetEventX();
		Int_t yy = gPad->GetEventY();

		Xmax = gPad->AbsPixeltoX(xx);
		Ymax = gPad->AbsPixeltoY(yy);

		if (Xmax>Xmin){
			XmaxSet=kTRUE;
			if (Ymin>Ymax){
				Double_t temp = Ymax;
				Ymax = Ymin;
				Ymin = temp;
			}
		}
	}
	else if (event==kButton1Double){
		Save();
		/*
		if (lhisto){
			nhisto+=1;
			if (nhisto<lhisto->GetEntries())
				SetHisto((TH1F* )lhisto->At(nhisto));
		}
		*/
	}
	
	if (XminSet && XmaxSet){
	
		printf("\tX : %lf %lf\n",Xmin,Xmax);
		printf("\tY : %lf %lf\n",Ymin,Ymax);
		
		
		//-------------
		//fit a 1D ou 2D
		if (NewFunction()){
			if (!is2D) {
				hfit->Fit(f1Dfit,"0N","",Xmin,Xmax);
				f1Dfit->Draw("same");
			}
			else {
				hfit->Fit(f2Dfit,"0N","",Xmin,Xmax);
				f2Dfit->Draw("same,cont2");
			}
		}
		//-------------
		
		ClearRange();
		c1->Update();
	}

}

//________________________________________________________________
void KVAutoFit::Recommence()
{

Int_t event = gPad->GetEvent();
TObject *select = gPad->GetSelected();
if (!select) return;
//else printf("Cliquage sur %s %s\n",select->GetName(),select->ClassName());

if (event==kButton1Down){
	if ( !strcmp("Annule Tout",select->GetTitle()) ){
		Info("","On efface les fonctions...");
		lfunc->Clear();
		ClearRange();
	}

	if ( !strcmp("Annule Dernier",select->GetTitle()) ){
		Info("","On retire la derniere fonction ...");
		Int_t nf = lfunc->GetEntries();
		if (nf>0)
			delete lfunc->RemoveAt(nf-1);
		ClearRange();
	}
	if ( !strcmp("Suivant",select->GetTitle()) ){
		if (lhisto){
			nhisto+=1;
			if (nhisto<lhisto->GetEntries())
				SetHisto((TH1F* )lhisto->At(nhisto));
		}
	}

}

}
//________________________________________________________________
void KVAutoFit::Gather()
{

Int_t event = gPad->GetEvent();
TObject *select = gPad->GetSelected();
if (!select) return;
if (event==kButton1Down){
	if ( !strcmp("Gather",select->GetTitle()) ){
		Info("Gather","Do nothing");
	}
}

}

//________________________________________________________________
void KVAutoFit::Save()
{

//Warning("Save","Do Nothing");
if (lfunc->GetEntries()==0) return;

TF1* f1;
ofstream fout(Form("%s",hfit->GetName()));
TIter it(lfunc);
fout<< lfunc->GetEntries() << endl;
while (f1 = (TF1* )it.Next()){
	fout<<f1->GetName() << endl;
	fout<<f1->GetExpFormula() << endl;
	Double_t x1,x2,y1,y2;
	if (!is2D){
		f1->GetRange(x1,x2);
		fout << x1 << " " << x2 << endl;
	}
	else {
		f1->GetRange(x1,y1,x2,y2);
		fout << x1 << " " << x2 << " " << y1 << " " << y2 << endl;
	}
	for (Int_t ii=0;ii<f1->GetNpar();ii+=1){
		fout << ii << " " << f1->GetParameter(ii) << " " << f1->GetParError(ii) << endl;
	}
	
}
fout.close();

}
