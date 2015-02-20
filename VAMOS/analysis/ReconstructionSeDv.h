#ifndef _RECONSTRUCTIONSED_CLASS
#define _RECONSTRUCTIONSED_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
#include"SeD12v.h"

class ReconstructionSeDv
{
   Bool_t Ready;
 public:
  ReconstructionSeDv(LogFile *Log, SeD12v *SeD12);
  virtual ~ReconstructionSeDv(void);
  
  LogFile *L;
  SeD12v *S12;

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
  Double_t Coef[4][330]; //D,T,P seventh order reconst in x,y,t,p
  
  Float_t Theta;
  Float_t Phi;
  Float_t Brho;
  Float_t Path;
  Float_t PathOffset;
  Float_t ThetaL;
  Float_t PhiL;


  //Counters
  Int_t Counter[6];

   ClassDef(ReconstructionSeDv,0)

};

#endif

