#ifndef _SED12_CLASS
#define _SED12_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
#include"SeDv.h"

class SeD12v
{
   Bool_t Ready;
 public:
  SeD12v(LogFile *Log, SeDv *SeD1, SeDv *SeD2);
  virtual ~SeD12v(void);
  
  LogFile *L;
  SeDv *S1;
  SeDv *S2;


  bool Present; //true if focal coordinates determined


  void Init(void); //Init for every event, focal variables go to -500. 
  void SetMatX(void); 
  void SetMatY(void); 
  void Calibrate(); //
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

  Random *Rnd;

 //Focal position Reference & Position
  Double_t XRef[2]; 
  Double_t YRef[2];
  Double_t FocalPos; //Focal Plane position
  Double_t MatX[2][2];
  Double_t MatY[2][2];
  Double_t AngleFocal[2];
  Double_t TanFocal[2];

  //Time Wire
  UShort_t T_Raw[1];

 //Calibration coeff
  Float_t TCoef[1][2];

  //Calibrated Time Wire
  Float_t T[1];

  //Velocity cm/ns
  Float_t V;


  //Focal position
  Float_t X[2]; //Subsequent X
  Float_t Y[2]; //Subsequent Y
  Float_t Xf;
  Float_t Yf;
  Float_t Tf;
  Float_t Pf;

  //Counters
  Int_t Counter[14];


  //Histograms


  ClassDef(SeD12v,0)
};

#endif

