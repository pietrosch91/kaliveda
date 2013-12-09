#ifndef _IDENTIFICATION_CLASS
#define _IDENTIFICATION_CLASS


#include "Rtypes.h"
#include "Defines.h"
#include "LogFile.h"
#include "Random.h"
#include "Reconstructionv.h"
#include "DriftChamberv.h"
#include "IonisationChamberv.h"
#include "Sive503.h"
#include "CsIv.h"
#include "TCutG.h"
#include "TGraph.h"
#include "TList.h"
#include "CsICalib.h"
#include "KVDetector.h"
#include "KVMaterial.h"
#include "KVNucleus.h"
#include "KV2Body.h"
#include "TString.h"

#include "KVReconstructedNucleus.h"
#include "KVINDRAReconNuc.h"
#include "KVIdentificationResult.h"

class Identificationv
{
 public:
  Identificationv(LogFile *Log, Reconstructionv *Recon,
		  DriftChamberv *Drift, IonisationChamberv *IonCh, Sive503 *SiD, CsIv *CsID, CsICalib *Ecsicalibrated);
  virtual ~Identificationv(void);
  
  Int_t NbCsI;
  Int_t MaxRun;
  Int_t NbSilicon;
 
 bool print;
 bool debug;
  
  LogFile *L;
  Reconstructionv *Rec;
  DriftChamberv   *Dr;
  IonisationChamberv *Ic;
  Sive503 *Si;
  CsIv *CsI;
  CsICalib *energytree;

	KVIdentificationResult*	id;
	KVIdentificationResult*	id_chiosi;
	KVIdentificationResult*	id_sitof;
	KVIdentificationResult*	id_chiov2;
	KVIdentificationResult*	id_qaq;	
	KVIdentificationResult*	id_qaq_chiosi;					
	KV2Body *kin;
		
	KVNucleus *kvn;	
	KVTarget *tgt;
        KVDetector *dcv1;
        KVMaterial *sed;
        KVDetector *dcv2;
        KVDetector *ic;
        KVMaterial *isogap1;
        KVMaterial *si;
        KVMaterial *isogap2;
        KVMaterial *csi;
			
	KVTarget *ttgt;
        KVDetector *ddcv1;
        KVMaterial *ssed;
        KVDetector *ddcv2;
        KVDetector *iic;
        KVMaterial *iisogap1;
        KVMaterial *ssi;	
        KVMaterial *iisogap2;
        KVMaterial *ccsi;
				
  UShort_t TFil1;
  UShort_t TFil2;
  UShort_t EFil1;
  UShort_t EFil2;

    Int_t runNumber;
    Bool_t grids_avail;
  
  Float_t *AA;
  Double_t *FragECsI;
  Double_t *FragEGap;  
  
  Double_t a_bisec;  
  Double_t e_bisec; 
    
  Double_t initThickness;
  Double_t ECsI_corr;  
  
  Double_t EEtot;
  Double_t NormVamos;
  Double_t DT;
  Double_t Brho_mag;
  Double_t FC_Indra;

	Float_t stat_tot;
	Float_t stat_indra;
	
	Float_t *Stat_Indra;  
  
Double_t *PID;
Double_t *Z_PID;
Double_t *A_PID;

Double_t *PID_sitof;
Double_t *Z_PID_sitof;

Double_t *PID_chiov2;
Double_t *Z_PID_chiov2;

  Double_t dif11[55];
  Double_t dif12[55];

  Double_t dif1[21];	//Z de 3 a 24
  Double_t dif2[21];
  

  Int_t **geom;		//Geometry Si-CsI
  Int_t **geom_cross;   
  Int_t **geomchiosi;	//Geometry Chio-Si
  Int_t **geomchiocsi;	//Geometry Chio-CsI
     

Int_t *FragDetSi;
Int_t *FragSignalSi;
Float_t *FragESi;
Double_t *FragTfinal;
Double_t *Off_chiosi;
Double_t *Off_dc1dc2;
Double_t *TOF_chiosi;

Int_t *FragSignalCsI; 
Int_t *FragDetCsI;

Int_t *FragDetChio; 
Int_t *FragSignalChio;
Float_t *FragEChio;

Long64_t event_number;
Int_t MaxM;
Int_t fMultiplicity_case;
Int_t Code_good_event;
KVList *grid_list;
KVIDGraph *grd;
    
  bool Present; //true if coordinates determined

  void Init(void); //Init for every event,  variables go to -500
  void InitSavedQuantities();
  void InitReadVariables();
  
  Int_t GetMaxMultiplicity(void);
  void FragPropertiesInOrder(void);
   
  void Calculate(); // Calulate  Initial coordinates
  void Show(void);
  void Treat(void);
  void inAttach(TTree *inT);
  void outAttach(TTree *outT);
  void CreateHistograms();
  void FillHistograms();
  //void PrintCounters(void);
//===================================================
// Q Regions  
/*  void SetFileCut(TList *list);
  void GetFileCut(); 
  void SetFileCutChioSi(TList *list2);
  void GetFileCutChioSi();
  void SetFileCutSiTof(TList *list3);
  void GetFileCutSiTof();      
  TList	*llist;
  TList	*llist2;
  TList	*llist3;  */  
//===================================================
// Total Energy Approximation
void SetTarget(KVTarget *tgt);
void SetDC1(KVDetector *dcv1);
void SetSed(KVMaterial *sed);
void SetDC2(KVDetector *dcv2);
void SetIC(KVDetector *ic);
void SetGap1(KVMaterial *isogap1);
void SetSi(KVMaterial *si);
void SetGap2(KVMaterial *isogap2);
void SetCsI(KVMaterial *csi);
//===================================================

//===================================================
KVTarget* GetTarget();
KVDetector* GetDC1();
KVMaterial* GetSed();
KVDetector* GetDC2();
KVDetector* GetIC();
KVMaterial* GetGap1();
KVMaterial* GetSi();
KVMaterial* GetGap2();
KVMaterial* GetCsI();
//===================================================

//===================================================
Double_t GetEnergyLossCsI(Int_t, Int_t, Double_t, Double_t);
Double_t GetEnergyLossGap2(Int_t, Int_t, Double_t, Double_t );
Double_t GetEnergyLossGap1(Int_t, Int_t, Double_t, Double_t );
Double_t GetEnergyLossChio(Int_t, Double_t);
Double_t GetEnergyLossDC2(Int_t, Double_t);
Double_t GetEnergyLossSed(Int_t, Double_t);
Double_t GetEnergyLossDC1(Int_t, Double_t);
Double_t GetEnergyLossTarget(Int_t, Double_t);
//===================================================
// Geometry Tests
Int_t *GetListOfChioFromSi(Int_t);
Int_t *GetListOfCsIFromSi(Int_t);
Int_t *GetListOfChioFromCsI(Int_t);
Int_t *GetListOfCsICrossFromSi(Int_t);
		
  void Geometry(); //method to reconstruct VAMOS telescopes
  void GeometryChioSi();  
  void GeometryChioCsI();
  void GeometrySiCsICross();
  
  Int_t IsChioCsITelescope(Int_t, Int_t);
  Int_t IsChioSiTelescope(Int_t, Int_t);
  Int_t IsSiCsITelescope(Int_t, Int_t);
  Int_t IsCsISiCrossTelescope(Int_t, Int_t);
//===================================================  
// Reading Files
void ReadQCorrection();
void ReadAQCorrection();
void ReadACorrection();
void ReadFlagVamos();
void ReadToFCorrectionChioSi();
void ReadToFCorrectionCode1();
void ReadStatIndra();
void ReadStraightAQ();
void ReadDoublingCorrection();

void ReadOffsetsChiosi();
Float_t *Offset_relativiste_chiosi;	//(MAXRUN)

void LoadGridForCode2Cuts();
  
//===================================================  
  
Double_t einc_csi;
Double_t eloss_csi;
Double_t einc_isogap2;
Double_t eloss_isogap2;
Double_t einc_si;
Double_t einc_isogap1;
Double_t eloss_isogap1;
Double_t einc_ic;
Double_t eloss_ic;
Double_t einc_dc2;
Double_t eloss_dc2;
Double_t einc_sed;
Double_t eloss_sed;
Double_t einc_dc1;
Double_t eloss_dc1;
Double_t einc_tgt;
Double_t eloss_tgt;

Double_t fELosLayer_dc1[3];
Double_t fELosLayer_dc2[3];
Double_t fELosLayer_ic[3];

Double_t E_tgt;
Double_t E_dc1;
Double_t E_dc2;
Double_t E_sed;
Double_t E_gap1;
Double_t E_chio;
Double_t E_gap2;
Double_t E_csi;

Float_t E_VAMOS;


  Random *Rnd;

  UShort_t T_Raw;


  Float_t D;
  Float_t dE;
  Float_t dE1;
  Float_t *Etot;
  Float_t E_corr;
  Float_t T;
  Float_t T_straightpaola;
        
  Float_t *V;
  Float_t *Vx;  
  Float_t *Vy;
  Float_t *Vz;
  Float_t VCMx;  
  Float_t VCMy;
  Float_t VCMz;
        
  Float_t V_Etot;
  Float_t T_FP;
  
  Float_t V2;
  Float_t *Beta;
  Float_t Gamma;
  Float_t *M_Q;
  Float_t *Q;  
  Float_t *M;
  Float_t *RealQ_straight;
  Float_t *M_straight;
  Int_t *Q_straight;
  
  Float_t *Beta_chiosi;
  Float_t *M_Q_chiosi;
  Float_t *Q_chiosi;  
  Float_t *M_chiosi;    
  
  Float_t Mr;
  Float_t M_Qr;
  Float_t Qr;
  Float_t Qc;
  Float_t Mc;
  Float_t Mass;
  Float_t M_simul;
  Float_t Z1;
  Float_t Z2;
  
  Float_t Z_tot;
  Float_t Z_si;  
  
   //Q Correction Function according to the CsI detector
   Float_t 	*P0;
   Float_t 	*P1;      
   Float_t 	*P2;
   Float_t 	*P3;      
   
   // Correction de M/Q
   Float_t 	*P0_mq;
   Float_t 	*P1_mq;
         
   // Correction de M
   Float_t 	**P0_m;
   Float_t 	**P1_m;
   
   // Correction de M/Q Chio-Si
   Float_t 	*P0_mq_chiosi;
   Float_t 	*P1_mq_chiosi;
         
   // Correction de M Chio-Si
   Float_t 	**P0_m_chiosi;
   Float_t 	**P1_m_chiosi; 
   
   // Correction de M/Q Si-Tof
   Float_t 	*P0_mq_sitof;
   Float_t 	*P1_mq_sitof;
         
   // Correction de M Si-Tof
   Float_t 	**P0_m_sitof;
   Float_t 	**P1_m_sitof;	   
   
   // Tag des events	
	Float_t *Brho_min;
	Float_t *Brho_max;		
	//Float_t ****Brho_min;
	//Float_t ****Brho_max;
	Int_t *Code_Vamos;
	Int_t *Code_Ident_Vamos;  
	
   // Correction de Tof pour identification Chio-Si
   	Float_t 	*Tof0;
   	Float_t 	*Esi0;   
	Float_t		*Brho0;
   // Correction de Tof pour identification Si-CsI
   	Float_t		*Tof0_code1;
	Float_t		*Deltat_code1;	      
	
	// Correction Double tof Pics
	Float_t ***Fit1;
	Float_t ***Fit2;	
	Float_t ***Fit3;	
	Float_t ***Fit4;
	Float_t ***Fit5;	
	Float_t ***Fit6;	
	Float_t ***Fit7;
		
	// Correction Offet relativiste
	Float_t *Offset_rela;
	
	// Redressement des distributions A/Q
	Float_t *P0_aq_straight;
	Float_t *P1_aq_straight;    
	Float_t *P2_aq_straight;
	
	Float_t *P0_aq_straight_chiosi;
	Float_t *P1_aq_straight_chiosi;    
	Float_t *P2_aq_straight_chiosi;	
	
/*TCutG *q21;
TCutG *q20;
TCutG *q19;
TCutG *q18;
TCutG *q17;
TCutG *q16;
TCutG *q15;
TCutG *q14;
TCutG *q13;
TCutG *q12;
TCutG *q11;
TCutG *q10;
TCutG *q9;
TCutG *q8;
TCutG *q7;
TCutG *q6;
TCutG *q5;

TCutG *q21cs;
TCutG *q20cs;
TCutG *q19cs;
TCutG *q18cs;
TCutG *q17cs;
TCutG *q16cs;
TCutG *q15cs;
TCutG *q14cs;
TCutG *q13cs;
TCutG *q12cs;
TCutG *q11cs;
TCutG *q10cs;
TCutG *q9cs;
TCutG *q8cs;
TCutG *q7cs;
TCutG *q6cs;
TCutG *q5cs;

TCutG *q21st;
TCutG *q20st;
TCutG *q19st;
TCutG *q18st;
TCutG *q17st;
TCutG *q16st;
TCutG *q15st;
TCutG *q14st;
TCutG *q13st;
TCutG *q12st;
TCutG *q11st;
TCutG *q10st;
TCutG *q9st;
TCutG *q8st;
TCutG *q7st;
TCutG *q6st;
TCutG *q5st;*/

   Double_t 	M_corr;
   Int_t 	Q_corr;
   Double_t	Q_corr_D;
   Double_t	M_corr_D; 
   Double_t	M_corr_D2;
   Double_t	Q_corr_D2;
   Int_t	Q2;
        
   Int_t	*Z_corr;
   Int_t	*Z_corr_sitof; 
   Int_t	*Z_corr_chiov2;     
   Double_t 	Delta;
   Double_t	M_realQ_D;
   Double_t	M_realQ;
   Double_t	realQ_D;
   Int_t	realQ;
   Int_t	Qid;
   Float_t	M_Qcorr; 
     
  //Counters
  Int_t Counter[6];

ClassDef(Identificationv,1)

};

#endif

