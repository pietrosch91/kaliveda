#ifndef _DRIFT_CLASS
#define _DRIFT_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
 #include "Sive503.h"
#include "TH2.h"

class DriftChamberv
{
   Bool_t Ready;
 public:
  DriftChamberv(LogFile *Log, Sive503 *SiD);
  virtual ~DriftChamberv(void);
  
  LogFile *L;
  Sive503 *Si;

  bool Present; //true if focal coordinates determined
  bool PresentWires; //true if Wires are present
  bool PresentStrips; //true if Strips are present and Wires are present
  bool PresentSubseqX;
  bool PresentSubseqY;



  void Init(void); //Init for every event, focal variables go to -500. 
  void InitRaw(void); 
  void SetMatX(void); 
  void SetMatY(void); 
  void Zero(void); //Zeroes initially, focal variables go to -500.0
  void Calibrate(void); 
  void Focal(); // Calulate focal coordinates
  void FocalSubseqX(); //Subsequent X
  void FocalSubseqY(); //Subsequent Y
  void FocalX(); // X
  void FocalY(); // Y  
  void Show_Raw(void);
  void Show(void);
  void Treat(void);
  void inAttach(TTree *inT);
  void outAttach(TTree *outT);
  void CreateHistograms();
  void FillHistograms();
  void PrintCounters(void);

  Random *Rnd;

  //Energy Wire
  UShort_t E_Raw[2];
 
  //Time Wire
  UShort_t T_Raw[2];

 //Calibration coeff
  Float_t ECoef[2][2];
  Float_t TCoef[2][2];

 //Calibrated Energy Wire
  Float_t E[2];
 
  //Calibrated Time Wire
  Float_t T[2];
  Float_t T_DCRef[2];
  Float_t T_DCpropre_el[2];
  Float_t T_DCfrag[2];
   
  //Charge Raw
  UShort_t Q_Raw[4*64];
  UShort_t Q_Raw_Nr[4*64];
  Int_t    Q_RawM[4];

  //Calibration coeff
  Float_t QCoef[64][4][3];

 
  //Calibrated Charge
  Float_t Q[64][4]; 
  UShort_t N[64][4];
  UShort_t NStrips; //Number of strips considered
  Float_t QThresh;

  //Multiplicity
  UShort_t Mult[4];

 //Focal position Reference & Position
  Double_t XRef[4][2]; 
  Double_t YRef[2][2];
  Double_t FocalPos; //Focal Plane position
  Double_t MatX[2][2];
  Double_t MatY[2][2];
  Float_t DriftVelocity;
  Double_t AngleFocal[2];
  Double_t AngleFocal1[1];
  Double_t TanFocal[2];
  Double_t TanFocal1[1];


  //Focal position
  Float_t X[4]; //Subsequent X
  Float_t Y[2]; //Subsequent Y
  Float_t XS[4]; //Subsequent XSech
  Float_t XWA[4]; //Subsequent XWA
  Float_t Xf;
  Float_t Xf1;
  Float_t Yf;
  Float_t Tf;
  Float_t Pf;

  //Counters
  Int_t Counter[14];
  Int_t Counter1[4][5];

  //Histograms
  TH2S *Dr_ST_Q_11;
  TH2S *Dr_ST_Q_12;
  TH2S *Dr_ST_Q_21;
  TH2S *Dr_ST_Q_22;

  ClassDef(DriftChamberv,0)

};

#endif

