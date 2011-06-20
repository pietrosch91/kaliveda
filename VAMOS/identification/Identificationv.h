#ifndef _IDENTIFICATION_CLASS
#define _IDENTIFICATION_CLASS


#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
#include"Reconstructionv.h"
#include"DriftChamberv.h"
#include"IonisationChamberv.h"
#include"Siv.h"
#include"CsIv.h"
#include "TCutG.h"
#include "EnergyTree.h"

#include "KVSiliconVamos.h"
#include "KVCsIVamos.h"
#include "KVIDSiCsIVamos.h"
#include "KVReconstructedNucleus.h"
#include "KVINDRAReconNuc.h"
#include "KVIdentificationResult.h"

class Identificationv
{
 public:
  Identificationv(LogFile *Log, Reconstructionv *Recon,
		  DriftChamberv *Drift, IonisationChamberv *IonCh, Siv *SiD, CsIv *CsID, EnergyTree *E);
  virtual ~Identificationv(void);
  
  LogFile *L;
  Reconstructionv *Rec;
  DriftChamberv   *Dr;
  IonisationChamberv *Ic;
  Siv *Si;
  CsIv *CsI;
  EnergyTree *energytree;

        KVSiliconVamos *si;
        PlaneAbsorber *gap;
        KVCsIVamos *csi;
	KVIdentificationResult*	id;
		
  UShort_t TFil1;
  UShort_t TFil2;
  UShort_t EFil1;
  UShort_t EFil2;
  
  Int_t ZZ;
  Int_t AA;
  Float_t AA2;
  Int_t DetCsI;
  Int_t DetSi;
  Int_t CsIRaw;
  Int_t SiRaw;
  
  Double_t initThickness;
  Double_t ECsI;
  Double_t ESi;
  Double_t EEtot;
  Double_t NormVamos;

Double_t PID;
Double_t Z_PID;
Double_t A_PID;

  Double_t dif11[55];
  Double_t dif12[55];
  
  Float_t As11[55];
  Float_t ARetreive11[55];
  Double_t CsIsRef11[55];
  Double_t SiRef11[55];
        

  Double_t dif1[21];	//Z de 3 a 24
  Double_t dif2[21];
    
  Float_t As[21]; 
  Float_t ARetreive[21];
  Double_t CsIsRef[21];
  Double_t SiRef[21];
  

  Int_t geom[18][6]; 
  Int_t i;  
  Int_t zt;
  Int_t aa;

Double_t brho;
Int_t runFlag;
    
  bool Present; //true if coordinates determined



  void Init(void); //Init for every event,  variables go to -500. 
  void Calculate(); // Calulate  Initial coordinates
  void Show(void);
  void Treat(void);
  void inAttach(TTree *inT);
  void outAttach(TTree *outT);
  void CreateHistograms();
  void FillHistograms();
  void PrintCounters(void);
  
  void SetBrho(Double_t);
  Double_t GetBrho(void);
  


	void SetRunFlag(Int_t);
	Int_t GetRunFlag(void);
	
  int Geometry(UShort_t, UShort_t);//temporary method to reconstruct VAMOS telescopes

  Random *Rnd;

  UShort_t T_Raw;


  Float_t D;
  Float_t dE;
  Float_t dE1;
  Float_t E;
  Float_t T;
  Float_t V;
  Float_t Beta;
  Float_t Gamma;
  Float_t M_Q;
  Float_t M_Qcorr;
  Float_t M_Qcorr1;
  Float_t Q;
  Float_t Mr;
  Float_t M_Qr;
  Float_t Qr;
  Float_t Qc;
  Float_t Mc;
  Float_t M;
  Float_t Mcorr;
  Float_t Mcorr1;
  Float_t Z1;
  Float_t Z2;
  
  Float_t Z_tot;
  Float_t Z_si;  
  Double_t ZR;

  //Counters
  Int_t Counter[6];

ClassDef(Identificationv,0)

};

#endif

