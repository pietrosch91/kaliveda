#ifndef _SIVE503_CLASS
#define _SIVE503_CLASS

#include <TString.h>
#include "Rtypes.h"
#include "Defines.h"
#include "LogFile.h"
#include "Random.h"
#include "TRandom3.h"
#include "TTree.h"
#include "KVDataSet.h"

class Sive503
{
   Bool_t Ready;
   
 public:
  Sive503(LogFile *Log);
  virtual ~Sive503(void);
  
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

  TRandom3 *Rnd;

  //energy Raw
  UShort_t E_Raw[21];
    UShort_t SiRaw[21];
  //Float_t E_Raw[21];
  UShort_t E_Raw_Nr[21];
  Int_t E_RawM;
  UShort_t T_Raw[3];

  //Calibration coeff
  Float_t ECoef[21][3];
  Float_t TOff[21][2];
  Float_t TCoef[3][5]; //for the new time calibration with 4 parameters

  Float_t TOffset[21];
  Double_t TRef[19];
  Double_t Tpropre_el[19];
  Double_t Tfrag;
  Double_t si_thick[19];   
  //

  //energy time Calibrated
  Int_t   EM;
  Float_t E[21];
  UShort_t ENr[21];
  Float_t T[3];
  Float_t Offset[2];
  Float_t ETotal;

    Float_t timeRefCheck;

  UShort_t Number;
  UShort_t GetNumber(){return Number;};


  //Counters
  Int_t Counter[2];

  //ClassDef(Sive503,1)
};

#endif
