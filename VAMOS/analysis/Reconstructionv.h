#ifndef _RECONSTRUCTION_CLASS
#define _RECONSTRUCTION_CLASS

#include "Rtypes.h"
#include "Defines.h"
#include "LogFile.h"
#include "Random.h"
#include "DriftChamberv.h"

class Reconstructionv
{
   Bool_t Ready;
 public:
  Reconstructionv(LogFile *Log, DriftChamberv *Drift);
  virtual ~Reconstructionv(void);
  
  LogFile *L;
  DriftChamberv *Dr;

  bool Present; //true if coordinates determined



  void Init(void); //Init for every event,  variables go to -500. 
  void Calculate(); // Calulate  Initial coordinates
  void Show(void);
  void Treat(void);
  void outAttach(TTree *outT);
  void CreateHistograms();
  void FillHistograms();
  void PrintCounters(void);
	
  Random *Rnd;

  Float_t BrhoRef;
  Float_t DDC1;   
  Float_t DSED1; 
  Float_t DDC2;
  Float_t DCHIO;
  Float_t DSI;
  Float_t DCSI;
      
  Double_t Coef[4][330]; //D,T,P,Path seventh order reconst in x,y,t,p
  
  Float_t Theta;
  Float_t Phi;
  Float_t Brho;
  Float_t Path;
  Float_t PathOffset;
  Float_t ThetaL;
  Float_t PhiL;


  //Counters
  Int_t Counter[6];

ClassDef(Reconstructionv,0)

};

#endif

