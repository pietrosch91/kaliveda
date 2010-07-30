#ifndef _SED_CLASS
#define _SED_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
#include "TTree.h"

#define ONEOVERSQRTTWO	0.707106781

#define ORDRE_DU_FIT 4

#define Borneminx_SED1 -158
#define Bornemaxx_SED1 180.
#define Borneminy_SED1 -46.
#define Bornemaxy_SED1 51.
#define Borneminx_SED2 -187.
#define Bornemaxx_SED2 184.
#define Borneminy_SED2 -43.0
#define Bornemaxy_SED2 55.0

#define Borneminx_SED -200.
#define Bornemaxx_SED 200.
#define Borneminy_SED -50.
#define Bornemaxy_SED 50.


class SeDv
{
   Bool_t Ready;
   
 public:
  SeDv(LogFile *Log, short nr);
  virtual ~SeDv(void);
  
  LogFile *L;
  short lnr;   //SeD number locally

  bool Present; //true if focal coordinates determined
  bool PresentWires; //true if Wires are present
  bool PresentStrips; //true if Strips are present and Wires are present
 



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

  Random *Rnd;

  //Energy Wire
  UShort_t E_Raw[3];
 
  //Time Wire
  UShort_t T_Raw[1];

 //Calibration coeff
  Float_t ECoef[3][2];
  Float_t TCoef[1][2];

 //Calibrated Energy Wire
  Float_t E[3];
 
  //Calibrated Time Wire
  Float_t T[1];
 
  //Charge Raw
  UShort_t Q_Raw[2*128];
  UShort_t Q_Raw_Nr[2*128];
  Int_t    Q_RawM[2];

  //Calibration coeff
  Float_t QCoef[128][2][3];

 
  //Calibrated Charge
  Float_t Q[128][2]; 
  UShort_t N[128][2];
  UShort_t NStrips; //Number of strips considered
  Float_t QThresh;

  //Multiplicity
  Int_t Mult[2];

 //Focal position Reference & Position
  Float_t XRef[2]; 
 //Coefficients de recadrage du SED1
  Double_t RecadrageX_SED[2][5][5], RecadrageY_SED[2][5][5];

  //Focal position
  Float_t X[2]; //Subsequent X
  Float_t XS[2]; //Subsequent XSech
  Float_t XWA[2]; //Subsequent XWA

  //Counters
  Int_t Counter[14];
  Int_t Counter1[4][5];

ClassDef(SeDv,0)
};

#endif

