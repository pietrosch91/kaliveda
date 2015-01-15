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
#include "KVTarget.h"
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
		  DriftChamberv *Drift, IonisationChamberv *IonCh, Sive503 *SiD, CsIv *CsID, CsICalib *E);
  virtual ~Identificationv(void);
  
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
  
  Int_t ZZ;
  Float_t AA;
  Int_t CsIRaw;
  Int_t DetCsI;
  Int_t SiRaw;
  
  Double_t a_bisec;  
  Double_t e_bisec; 
    
  Double_t initThickness;
  Double_t EChio;
  Double_t EGap;
  Double_t ECsI;
  Double_t ECsI_corr;  
  Double_t ESi;
  Double_t EEtot;
  Double_t NormVamos;
  Double_t DT;
  Double_t Brho_mag;
  Double_t FC_Indra;

	Float_t stat_tot;
	Float_t stat_indra;
	Float_t Stat_Indra[600];  
  
Double_t PID;
Double_t PID_chiosi;
Double_t Z_PID;
Double_t A_PID;

  Double_t dif11[55];
  Double_t dif12[55];

  Double_t dif1[21];	//Z de 3 a 24
  Double_t dif2[21];
  

  Int_t geom[18][6];
  Int_t geomchiosi[8][6];   
  Int_t i;  
  Int_t zt;
  Int_t aa;
    
  bool Present; //true if coordinates determined



  void Init(void); //Init for every event,  variables go to -500. 
  void Calculate(); // Calulate  Initial coordinates
  void Show(void);
  void Treat(void);
  void inAttach(TTree *inT);
  void outAttach(TTree *outT);
  void CreateHistograms();
  void FillHistograms();
  //void PrintCounters(void);
  
  void SetFileCut(TList *list);
  void GetFileCut(); 
  void SetFileCutChioSi(TList *list2);
  void GetFileCutChioSi();    
  TList	*llist;
  TList	*llist2;  
//===================================================
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

Double_t GetEnergyLossCsI(Int_t);
Double_t GetEnergyLossGap2(Int_t);
Double_t GetEnergyLossGap1(Int_t);
Double_t GetEnergyLossChio();
Double_t GetEnergyLossDC2();
Double_t GetEnergyLossSed();
Double_t GetEnergyLossDC1();
Double_t GetEnergyLossTarget();

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
//===================================================
	
  int Geometry(UShort_t, UShort_t); //method to reconstruct VAMOS telescopes
  int GeometryChioSi(UShort_t, UShort_t);   
  Random *Rnd;

  UShort_t T_Raw;


  Float_t D;
  Float_t dE;
  Float_t dE1;
  Float_t E;
  Float_t E_corr;
  Float_t T;
  Float_t V;
  Float_t Vx;  
  Float_t Vy;
  Float_t Vz;
  Float_t VCMx;  
  Float_t VCMy;
  Float_t VCMz;
        
  Float_t V_Etot;
  Float_t T_FP;
  
  Float_t V2;
  Float_t Beta;
  Float_t Gamma;
  Float_t M_Q;
  Float_t Q;
  Float_t Mr;
  Float_t M_Qr;
  Float_t Qr;
  Float_t Qc;
  Float_t Mc;
  Float_t M;
  Float_t Mass;
  Float_t M_simul;
  Float_t Z1;
  Float_t Z2;
  
  Float_t Z_tot;
  Float_t Z_si;  
  Double_t ZR;
  
//Int_t csi;
//Float_t p0;
//Float_t p1;
//Float_t p2;
  
   //Q Correction Function according to the CsI detector
   Float_t 	P0[80];
   Float_t 	P1[80];      
   Float_t 	P2[80];
   Float_t 	P3[80];      
   
   // Correction de M/Q
   Float_t 	P0_mq[600];
   Float_t 	P1_mq[600];
         
   // Correction de M
   Float_t 	P0_m[600][25];
   Float_t 	P1_m[600][25];
   
   // Correction de M/Q Chio-Si
   Float_t 	P0_mq_chiosi[600];
   Float_t 	P1_mq_chiosi[600];
         
   // Correction de M Chio-Si
   Float_t 	P0_m_chiosi[600][25];
   Float_t 	P1_m_chiosi[600][25];   
   
   // Tag des events	
	Float_t Brho_min[25][60][10][600];
	Float_t Brho_max[25][60][10][600];
	Int_t Code_Vamos;
	Int_t Code_Ident_Vamos;  
	
   // Correction de Tof pour identification Chio-Si
   	Int_t 	Tof0[600];
   	Int_t 	Esi0[600];	        

TCutG *q21;
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


   Double_t 	M_corr;
   Int_t 	Q_corr;
   Double_t	Q_corr_D;
   Double_t	M_corr_D; 
   Double_t	M_corr_D2;
   Double_t	Q_corr_D2;
   Int_t	Q2;
        
   Int_t	Z_corr;
   Double_t 	Delta;
   Double_t	M_realQ_D;
   Double_t	M_realQ;
   Double_t	realQ_D;
   Int_t	realQ;
   Int_t	Qid;
   Float_t	M_Qcorr; 
     
  //Counters
  Int_t Counter[6];

ClassDef(Identificationv,0)

};

#endif

