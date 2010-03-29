//Created by KVClassFactory on Thu Mar 25 11:25:27 2010
//Author: bonnet

#ifndef __KVKVBreakUp_H
#define __KVKVBreakUp_H

#include "TRandom.h"
#include "KVNumberList.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "KVList.h"
#include "KVString.h"
#include "TTree.h"
#include "TH2F.h"
#include "TProfile.h"

class KVBreakUp:public TObject
{
	protected:
	
	void Reset_ForNextTime();
	
	void SetZtot(Int_t zt) {
		if (zt!=Ztotal){
			Ztotal=zt; 
			if (bound) delete bound;	bound = new Int_t[Ztotal];
			for (Int_t nn=0;nn<Ztotal;nn+=1) bound[nn]=1;
			nbre_nuc = Ztotal-Mtotal*Zmin;
		}
	}
	
	void SetMtot(Int_t mt) { Mtotal=mt; nbre_nuc = Ztotal-Mtotal*Zmin;}
	void SetZmin(Int_t zlim) { Zmin=zlim; nbre_nuc = Ztotal-Mtotal*Zmin;}
	void SetKVBreakUpMethod(KVString bup_method) { 
		KVBreakUpMethod=bup_method;
	}

	Int_t Ztotal;
	Int_t Mtotal;
	Int_t Zmin;
	Int_t *bound;
	KVString KVBreakUpMethod,TRandom_Method;
	TRandom* alea;
	
	KVNumberList nl;
	Int_t nbre_nuc;
	Int_t size[400];
	
   public:

	Int_t nraffine;
	KVList* lhisto;
	
	TH1F* hzz;
	TH1F* hzt;
	TH1F* hmt;
	TProfile* hite,*hess;
	
	TTree* tt;
	
	virtual void init(void);
	virtual void DefineHistos();
	virtual void DefineTree();
	
	KVBreakUp() { init(); }
	
   virtual ~KVBreakUp(){
	
		delete alea;
		delete lhisto;
		TCanvas* c1 = 0;
		if ( (c1 = (TCanvas* )gROOT->GetListOfCanvases()->FindObject("KVBreakUp_Control")) ) delete c1;
		if (bound) delete bound;
		if (tt) delete tt;
		
	}
	
	void RedefineTRandom(KVString TRandom_Method){
	
		if (alea) delete alea;
		TClass *cl = new TClass(TRandom_Method.Data());
		alea = (TRandom* )cl->New();
		delete cl;
	}
	
	void SetConditions(Int_t zt,Int_t mt,Int_t zmin=1){
		SetZtot(zt);
		SetMtot(mt);
		SetZmin(zmin);
	}
	
	Int_t GetZtot(void) const {return Ztotal;}
	Int_t GetMtot(void) const {return Mtotal;}
	Int_t GetZmin(void) const {return Zmin;}
	
	void DefineKVBreakUpMethod(KVString bup_method=""){
		if (bup_method==""){
			Info("DefineKVBreakUpMethod", "Available methods are");
			cout << "BreakUsingChain" << endl;
			cout << "BreakUsingPile" << endl;
			cout << "BreakUsingIndividual" << endl;
			cout << "Make your choice" << endl;
		}
		else {
			SetKVBreakUpMethod(bup_method);
		}
	}
	KVString GetKVBreakUpMethod(void) const {return KVBreakUpMethod;}

	Int_t BreakUsingChain();
	Int_t BreakUsingPile();
	Int_t BreakUsingIndividual();
		
	virtual void OrdonneCharge();
	virtual void TreatePartition();
	
	void BreakNtimes(Int_t times=1000);
	void BreakNtimesOnGaussian(Int_t times,Double_t Ztot_moy,Double_t Ztot_rms,Double_t Mtot_moy,Double_t Mtot_rms,Int_t zmin=1);
	void BreakNtimesFromHisto(TH2F* hh_zt_VS_mt,Int_t zmin=1);
	
	virtual void DrawPanel();
	virtual void ResetHistos();
	virtual void SaveHistos(KVString filename="",KVString suff="",Bool_t update=kTRUE);
	virtual void SaveTree(KVString filename,KVString suff,Bool_t update=kTRUE);
   
	void	Print(Option_t* option = "") const {
		Info("Print","Configuration for the break up");
		printf(" Ztot=%d - Mtot=%d - Zmin=%d\n",GetZtot(),GetMtot(),GetZmin());
		printf(" Charge to be distributed %d - Biggest possible charge %d\n",nbre_nuc,Zmin+nbre_nuc);
		printf(" Methode de cassage aleatoire %s\n",GetKVBreakUpMethod().Data());
		alea->Print();
		printf("------------------------------------------------------");
	}
	
	ClassDef(KVBreakUp,1)//Partitioning of nuclei
};

#endif
