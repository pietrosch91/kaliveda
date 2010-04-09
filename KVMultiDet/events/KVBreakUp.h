//Created by KVClassFactory on Thu Mar 25 11:25:27 2010
//Author: bonnet

#ifndef __KVBREAKUP_H
#define __KVBREAKUP_H

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
#include "KVPartition.h"
#include "KVPartitionManager.h"
#include "TDatime.h"
#include <cstdlib>


class KVBreakUp:public TObject
{
	protected:
	
	enum {
      kStorePartitions = BIT(14),	//Variables has to be recalculated
      kFillTree = BIT(15)			//Variables has to be recalculated
  	};
	
	void Reset_ForNextTime();
	
	void SetZtot(Int_t zt) {
		if (zt!=Ztotal){
			Ztotal=zt; 
			if (bound) delete bound;	bound = new Int_t[Ztotal];
			for (Int_t nn=0;nn<Ztotal;nn+=1) bound[nn]=1;
			nbre_nuc = Ztotal-Mtotal*Zmin;
		}
	}
	
	void SetMtot(Int_t mt) { 
		if (mt>size_max) {
			Warning("SetMtot","%d -> La multiplicite max (%d) est depassee",mt,size_max);
			exit(EXIT_FAILURE);
		}
		Mtotal=mt; 
		nbre_nuc = Ztotal-Mtotal*Zmin;
	}
	
	void SetZmin(Int_t zlim) { Zmin=zlim; nbre_nuc = Ztotal-Mtotal*Zmin;}
	void SetBreakUpMethod(KVString bup_method) { 
		BreakUpMethod=bup_method;
	}

	Int_t Ztotal;
	Int_t Mtotal;
	Int_t Zmin;
	Int_t *bound;	//[Ztotal]
	KVString BreakUpMethod,TRandom_Method;
	TRandom* alea;
	
	KVNumberList nl;
	Int_t nbre_nuc;

	Int_t size_max;
	Int_t size[500];

	Int_t iterations_total;
	
	Int_t tstart,tstop,tellapsed;

	TTree* tt;
	KVPartitionManager* parman;
	
	public:

	Int_t nraffine;

	KVPartition* partition;

	TH1F* hzz;
	TH1F* hzt;
	TH1F* hmt;
	
	KVList* lobjects;
	KVList* lhisto;
	
	virtual void init(void);
	virtual void DefineHistos();
	virtual void StoreEntriesInTree();
	
	KVBreakUp() { init(); }
	
   virtual ~KVBreakUp(){
	
		delete alea;
		delete lhisto;
		TCanvas* c1 = 0;
		if ( (c1 = (TCanvas* )gROOT->GetListOfCanvases()->FindObject("BreakUp_Control")) ) delete c1;
		if (bound) delete [] bound;
		
		lobjects->Clear();
		delete lobjects;
			
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
	
	void DefineBreakUpMethod(KVString bup_method=""){
		if (bup_method==""){
			Info("DefineBreakUpMethod", "Available methods are");
			cout << "BreakUsingChain" << endl;
			cout << "BreakUsingPile" << endl;
			cout << "BreakUsingIndividual" << endl;
			cout << "BreakUsingLine" << endl;
			cout << "Make your choice" << endl;
		}
		else {
			SetBreakUpMethod(bup_method);
		}
	}
	KVString GetBreakUpMethod(void) const {return BreakUpMethod;}
	
	void StorePartition(Bool_t choix=kTRUE);
	
	Int_t BreakUsingChain();
	Int_t BreakUsingPile();
	Int_t BreakUsingIndividual();
	Int_t BreakUsingLine();
		
//	virtual void OrdonneCharge();
	virtual void TreatePartition();
	void TransfertFromTree(void);
	
	
	void BreakNtimes(Int_t times=1000);
	void BreakNtimesOnGaussian(Int_t times,Double_t Ztot_moy,Double_t Ztot_rms,Double_t Mtot_moy,Double_t Mtot_rms,Int_t zmin=1);
	void BreakFromHisto(TH2F* hh_zt_VS_mt,Int_t zmin=1);
	
	virtual void DrawPanel();
	
	Int_t TotalIterations(void) { return iterations_total; }
	
	KVList* GetHistos() {return lhisto;}
	KVList* GetObjects() {return lobjects; }
	
	virtual void ResetHistos();
	virtual void ResetTree();
	void ResetManager();
	KVPartitionManager* GetManager() {return parman;}
	TTree* GetTree() {return tt;}
	
	virtual void ResetTotalIterations() {iterations_total=0;}
	
	virtual void SaveHistos(KVString filename="",KVString suff="",Bool_t update=kTRUE);
	virtual void SaveTree(KVString filename,KVString suff,Bool_t update=kTRUE);
   
	void	Print(Option_t* option = "") const {
		Info("Print","Configuration for the break up");
		printf(" Ztot=%d - Mtot=%d - Zmin=%d\n",GetZtot(),GetMtot(),GetZmin());
		printf(" Charge to be distributed %d - Biggest possible charge %d\n",nbre_nuc,Zmin+nbre_nuc);
		printf(" Methode de cassage aleatoire %s\n",GetBreakUpMethod().Data());
		alea->Print();
		printf(" Partition are stored via KVPartitionManager : %d\n",Int_t(TestBit(kStorePartitions)));
		printf("------------------------------------------------------");
	}
	
	void Start(){
		TDatime time;
		tstart = time.GetHour()*3600+time.GetMinute()*60+time.GetSecond();
	}
	void Stop(){
		TDatime time;
		tstop = time.GetHour()*3600+time.GetMinute()*60+time.GetSecond();
		tellapsed = tstop-tstart;
	}
	Int_t GetDeltaTime() { return tellapsed; }
	
	
	ClassDef(KVBreakUp,1)//Partitioning of nuclei
};

#endif
