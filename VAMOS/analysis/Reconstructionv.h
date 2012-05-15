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

	void SetBrhoRef(Double_t);
	void SetAngleVamos(Double_t);
	Double_t GetBrhoRef(void);
	Double_t GetAngleVamos(void);
	Double_t B;
	Double_t theta;
	
  Random *Rnd;

  Float_t BrhoRef;
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

