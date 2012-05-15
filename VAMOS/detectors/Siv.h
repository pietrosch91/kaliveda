#ifndef _SI_CLASS
#define _SI_CLASS

#include "Rtypes.h"
#include "Defines.h"
#include "LogFile.h"
#include "Random.h"
#include "TTree.h"

class Siv
{
	Bool_t Ready;
   
	public:
	
	Siv(LogFile *Log);
	virtual ~Siv(void);
  
	LogFile *L;

	bool Present; 

	void Init(void); 
	void InitRaw(void); 
	void Calibrate(void); 
	void Show_Raw(void);
	void Show(void);
	void Treat(void);
	void inAttach(TTree *inT);
	void outAttach(TTree *outT);
	void CreateHistograms();
	void FillHistograms();
	void PrintCounters(void);

	Random *Rnd;
/*
 *	energy Raw
 */
	UShort_t E_Raw[18];
	UShort_t E_Raw_Nr[18];
	Int_t E_RawM;
	UShort_t T_Raw[3];
/*
 *	Calibration coeff
 */
	Double_t ECoef[18][8];
	Double_t TSi_HF_Coef[6];
	Double_t TSi_HF_t0[18];
	Double_t TSi_SED1_Coef[4];
	Double_t TSi_SED1_t0[18];
/*
 *	energy time Calibrated
 */
	Int_t EM;
	Float_t E[18];
	UShort_t ENr[18];
	Float_t T[3];
	Float_t ETotal;
	UShort_t Number; 
/*
 *	Counters	
 */
	Int_t Counter[2];

	ClassDef(Siv,0)
};

#endif
