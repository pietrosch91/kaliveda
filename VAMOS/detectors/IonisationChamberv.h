#ifndef _IONCHAMBER_CLASS
#define _IONCHAMBER_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
#include "TTree.h"

class IonisationChamberv
{
   Bool_t Ready;
   int logLevel;

 public:
  IonisationChamberv(LogFile *LOg);
  virtual ~IonisationChamberv(void);
  
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
  UShort_t E_Raw[7];		//UShort_t E_Raw[3*8]
  UShort_t E_Raw_Nr[7];		//UShort_t E_Raw_Nr[3*8]
  Int_t E_RawM;
  UShort_t IC_Raw;
  
  //Calibration coeff
  Float_t a[7];
  Float_t b[7];
  Float_t Vnorm[7];
  
  //Float_t ECoef[3][7][3];

  //energy Calibrated
  Float_t E[7];			//Float_t E[3*8]
  UShort_t ENr[3*8];		//UShort_t ENr[3*8]
  Int_t EM;
  
  //Float_t ERef[3][2]; 	//Position & energy
  //Float_t ES[3]; 		//Energy sum per row
  
  Float_t ETotal;	//Multiplicity
  
  //Float_t ETresh[3];
  
  Int_t Number;	//Multiplicity
  Float_t eloss;

  //Counters
  Int_t Counter[5];

  ClassDef(IonisationChamberv,0)

    void SetLogLevel(int level){logLevel = level;};
    int GetLogLevel(){return logLevel;};
};

#endif
