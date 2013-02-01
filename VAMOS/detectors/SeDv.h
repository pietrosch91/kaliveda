#ifndef _SED_CLASS
#define _SED_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
#include "TTree.h"

#define ONEOVERSQRTTWO	0.707106781

#define ORDRE_DU_FIT 4

#define Border 5
#define Nx 128
#define Ny 48

#define Borneminx_SED1 -158
#define Bornemaxx_SED1 180.
#define Borneminy_SED1 -46.
#define Bornemaxy_SED1 51.
#define Borneminx_SED2 -187.
#define Bornemaxx_SED2 184.
#define Borneminy_SED2 -43.0
#define Bornemaxy_SED2 55.0
/*
 *	Define the borders. readjust it according to a calibration on many physics runs
 */
#define Borneminx_SED -200.
#define Bornemaxx_SED 200.
#define Borneminy_SED -50.
#define Bornemaxy_SED 50.
#define LimitSED 180.

class SeDv
{
	Bool_t fReady;     //kTRUE when if the calibration files are well read
   
	public:
	SeDv(LogFile *Log, Short_t nr);
	virtual ~SeDv(void);
  
	LogFile *fLogFile;
	Short_t fSeDNum;   //SeD number locally

	Bool_t fPresent; //true if focal coordinates determined
	Bool_t fPresentWires; //true if Wires are present
	Bool_t fPresentStrips; //true if Strips are present and Wires are present
	
	void Init(void); //Init for every event,focal variables go to -500. 
	void InitRaw(void); 
	void Zero(void); //Zeroes initially, focal variables go to -500.0
	void Calibrate(void); 
	void FocalSubseqX(); //Subsequent X
	void Show_Raw(void);
	void Show(void);
	void Treat(void);
	void inAttach(TTree *inT);
	void outAttach(TTree *outT);
	void CreateHistograms();
	void FillHistograms();
	void PrintCounters(void);

/*
 *	Energy Wire
 */
	UShort_t fE_Raw[3];        // 3 energies
/*
 *	Time Wire
 */
	UShort_t fT_Raw[1];        // TSED1_HF, TSED2_HF
/*
 *	Calibration coeff
 */
	Float_t fECoef[3][2];      // 2 coef. for 3 energies
	Double_t fTSED_HF_Coef[7];
/*
 *	Calibrated Energy Wire
 */
	Float_t fE[3];
/*
 *	Calibrated Time Wire
 */
	Float_t fT[1];            // calibrated TSED1_HF, TSED2_HF
/*
 *	Charge Raw
 */
	UShort_t fQ_Raw[2*128];   // SED1_X_{1-128}, SED1_Y_{1-48}
	UShort_t fQ_Raw_Nr[2*128];
	Int_t    fQ_RawM[2];
/*
 *	Calibration coeff
 */
	Float_t fQCoef[128][2][3];
/*
 *	Calibrated Charge
 */
	UShort_t fNStrips; 				//Number of strips considered
	UShort_t fN[128][2];
	Float_t fQ[128][2],fQThresh[2]; 
/*
 *	Multiplicity
 */
	Int_t fMult[2];
/*
 *	Focal position Reference & Position
 */
	Float_t fXRef[2]; 
/*
 *	Coefficients de recadrage du SED1
 */
	Double_t fRecadrageX_SED[2][5][5], fRecadrageY_SED[2][5][5];
/*
 *	Focal position
 */
	Float_t fX[2]; //Subsequent X
	Float_t fXS[2]; //Subsequent XSech
	Float_t fXWA[2]; //Subsequent XWA
/*
 *	Counters
 */
	Int_t fCounter[14];
	Int_t fCounter1[4][5];

	ClassDef(SeDv,0)
};

#endif

