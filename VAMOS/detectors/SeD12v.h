#ifndef _SED12_CLASS
#define _SED12_CLASS

#include"Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
#include"SeDv.h"

class SeD12v
{
	Bool_t fReady;
	public:
	SeD12v(LogFile *Log, SeDv *SeD1, SeDv *SeD2);
	virtual ~SeD12v(void);
  
	LogFile *fL;
	SeDv *fS1;
	SeDv *fS2;

	bool fPresent; //true if focal coordinates determined

	void Init(void); //Init for every event, focal variables go to -500. 
	void SetMatX(void); 
	void SetMatY(void); 
	void Calibrate(); //
	void Linearise(); //
	void Focal(); //
	void FocalX(); // X
	void FocalY(); // Y  
	void Show(void);
	void Treat(void);
	void inAttach(TTree *inT);
	void outAttach(TTree *outT);
	void CreateHistograms();
	void FillHistograms();
	void PrintCounters(void);

	Random *fRnd;
/*
 *	Focal position Reference & Position
 */
	Double_t fXRef[2];//X reference position for SED1,2 (read in calib. file)
	Double_t fYRef[2];//Y reference position for SED1,2 (read in calib. file)
	Double_t fFocalPos; //Focal Plane position (read in calib. file)
	Double_t fMatX[2][2];//2D Matrix for X (calculated in SetMatX)
	Double_t fMatY[2][2];//2D Matrix for Y (calculated in SetMatY)
	Double_t fAngleFocal[2];// Focal angle in X and Y direction in degree(read in calib file)
	Double_t fTanFocal[2];//tan( fAngleFocal ) (calculated ind SetMatX,Y)
/*
 *	Time TSED1-SED2
 */
	UShort_t fT_Raw;
/*
 *	Calibration coeff
 */
	Float_t fTCoef[6];
/*
 *	Linearisation coeff
 */
	Float_t fTlinearise[15];// (read in calib. file)
/*
 *	Calibrated Time TSED1-SED2
 */
	Float_t fT;
/*
 *	Velocity cm/ns
 */
	Float_t fV; // Velocity at the focal plan
/*
 *	Focal position
 */
	Float_t fX[2]; //Subsequent X of SED1,2 (see Focal())
	Float_t fY[2]; //Subsequent Y of SED1,2
	Float_t fXf;   //X at the focal plan
	Float_t fYf;   //Y at the focal plan
	Float_t fTf;   //Theta at the focal plan (mrad)
	Float_t fPf;   //Phi   at the focal plan (mrad)
/*
 *	Counters
 */
	Int_t fCounter[14];
/*
 *	Histograms
 */
	ClassDef(SeD12v,0)
};

#endif

