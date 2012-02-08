//Created by KVClassFactory on Tue Feb  7 15:17:39 2012
//Author: bonnet

#ifndef __KVAUTOFIT_H
#define __KVAUTOFIT_H

#include "KVBase.h"

#include "TObject.h"
#include "TH1F.h"
#include "TF1.h"
#include "TF2.h"
#include "KVHashList.h"
#include "TCanvas.h"
#include "TString.h"
#include "KVList.h"
#include "TPaveLabel.h"


class KVAutoFit : public KVBase
{
	
	protected:
	KVHashList* lfunc;	//liste des fonctions de fit
	
	TF1* f1Dfit;			//fonction de fit une dimension
	TF2* f2Dfit;			//fonction de fit deux dimensions
	
	TH1* hfit;				//histogramme analyse
	TH1* hclone;			//clone de histogramme analyse
	TCanvas* c1;			//Canvas ou est trace l histo a analyse
	
	
	Bool_t XminSet,XmaxSet;	//Indique que l intervalle du fit est defini
	Double_t Xmin,Xmax;	//Borne en X du fit
	Double_t Ymin,Ymax;	//Borne en Y du fit
	
	KVHashList* lplabel;	//contient la liste des TPaveLabel constituant le menu contextuel
	
	KVHashList* lhisto;	//liste des histos initialise via SetHistos method
	Int_t nhisto;			//nombre d'histo traites
	/*
	TString spath,sdir,sfile;
	TString output_file;
	*/
	
	virtual void init();
	void ClearRange(void);

	Bool_t NewFunction();
	virtual Bool_t NewFunction_1D();
	virtual Bool_t NewFunction_2D();
	
	virtual Double_t f2D(Double_t *xx,Double_t *para);	
	virtual Double_t f1D(Double_t *xx,Double_t *para);

	
   public:
   Bool_t is2D;			//histogramme analyse herite de TH1 ou TH2
	
	KVAutoFit();
   KVAutoFit (const KVAutoFit&) ;
   virtual ~KVAutoFit();
   void Copy (TObject&) const;
	
	void Clear(Option_t* opt = "");
	void Print(Option_t* opt = "") const;
	
	virtual void SetHistos(KVHashList* lh);
	virtual void SetHisto(TH1* hh);
	void GetInterval();
	void Recommence();
	virtual void Gather();
	virtual void Save();
	
   ClassDef(KVAutoFit,1)//Manage SemiAutomatic Fits
};

#endif
