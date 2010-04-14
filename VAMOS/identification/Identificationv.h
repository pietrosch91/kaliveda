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
#include "TCutG.h"
#include "EnergyTree.h"

//#include"CsIv.h" 

class Identificationv
{
 public:
  Identificationv(LogFile *Log, Reconstructionv *Recon,
		  DriftChamberv *Drift, IonisationChamberv *IonCh, Siv *SiD, CsIv *CsID);
  virtual ~Identificationv(void);
  
  LogFile *L;
  Reconstructionv *Rec;
  DriftChamberv   *Dr;
  IonisationChamberv *Ic;
  Siv *Si;
  CsIv *CsI;

  EnergyTree *energytree;

#ifdef FOLLOWPEAKS
  TCutG *myct[21];
  TCutG *myct1[21];
  Float_t CgM[21];
  Float_t CgM_v[21];
  Float_t CgM_Q[21];
  Float_t CgM_Q_v[21];
  Float_t M_QCoef[21][3];
  Float_t MCoef[21][3];
#endif
  
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
  


  //Counters
  Int_t Counter[6];

ClassDef(Identificationv,0)

};

#endif

