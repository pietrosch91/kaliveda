#ifndef AnalyseCamp1_h
#define AnalyseCamp1_h

#include "KVSelector.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"

class AnalyseCamp1 : public KVSelector {

   TFile *tree_file;//!file containing results of analysis
	TTree *t__data;//!tree containing results of analysis
	Float_t Excit, VQProj, Theta_flot, Sphericity, Theta_QP;
	Int_t Mult, Zmax, ZQProj;
	TH2F *Valph_cm, *Valph_lab, *Valph_qp;

	public:
			
	AnalyseCamp1(TTree *tree=0):KVSelector(tree)
	{
		t__data=0;
		Valph_cm=(TH2F*)gROOT->FindObject("Valph_cm");
		Valph_lab=(TH2F*)gROOT->FindObject("Valph_lab");
		Valph_qp=(TH2F*)gROOT->FindObject("Valph_qp");
	};
	virtual ~AnalyseCamp1(){};
	
	virtual void InitAnalysis();
	virtual void InitRun();
	virtual Bool_t Analysis();
	virtual void EndRun();
	virtual void EndAnalysis();
	
	ClassDef(AnalyseCamp1,0)//Simple analysis class with a few global variables

};
#endif // #ifdef AnalyseCamp1_cxx

