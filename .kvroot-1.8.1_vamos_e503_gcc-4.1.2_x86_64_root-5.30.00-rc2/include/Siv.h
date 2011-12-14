#ifndef _SI_CLASS
#define _SI_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
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

  //energy Raw
  UShort_t E_Raw[21];
  UShort_t E_Raw_Nr[21];
  Int_t E_RawM;
  UShort_t T_Raw[3];



  //Calibration coeff
  Float_t ECoef[21][3];
  Float_t TOff[21][2];
  Float_t TCoef[3][2];

  //

  //energy time Calibrated
  Int_t   EM;
  Float_t E[21];
  UShort_t ENr[21];
  Float_t T[3];
  Float_t Offset[2];
  Float_t ETotal;
  UShort_t Number;
 

  //Counters
  Int_t Counter[2];

  ClassDef(Siv,0)
};

#endif
