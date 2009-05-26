#ifndef INDRAnalysis_h
#define INDRAnalysis_h

#include "KVSelector.h"

class INDRAnalysis : public KVSelector {

	TTree *t__chiosi;
	Float_t E_si;
	Float_t E_chio;
	Int_t Ring_si;

	public:
			
	INDRAnalysis(TTree *chain=0):KVSelector(chain)
	{
		t__chiosi=0;
		E_si=0;
		E_chio=0;
		Ring_si=0;
	};
	virtual ~INDRAnalysis(){};
	
	virtual void InitAnalysis();
	virtual void InitRun();
	virtual Bool_t Analysis();
	virtual void EndRun();
	virtual void EndAnalysis();
		
	ClassDef(INDRAnalysis,0)//Example analysis class for reconstructed (recon) data

};
#endif // #ifdef INDRAnalysis_cxx

