#ifndef _IONCHAMBER_CLASS
#define _IONCHAMBER_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include"LogFile.h"
#include"Random.h"
#include "TTree.h"

//const int MAXIC=7;

class IonisationChamberv {
   Bool_t Ready;
   int logLevel;

public:
   IonisationChamberv(LogFile* LOg);
   virtual ~IonisationChamberv(void);

   LogFile* L;

   Int_t NbChio;
   //bool Present;

   void Init(void);
   void InitSavedQuantities(void);
   void Calibrate(void);
   void Show_Raw(void);
   void Show(void);
   void Treat(void);
   void inAttach(TTree* inT);
   void outAttach(TTree* outT);
   void CreateHistograms();
   void FillHistograms();
   void PrintCounters(void);

   Random* Rnd;

   //energy Raw
   UShort_t* E_Raw;
   UShort_t* E_Raw_Nr;
   Int_t* IcRaw;
   Int_t E_RawM;

   Int_t* DetChio;

   //Calibration coeff
   Float_t* a;
   Float_t* b;
   Float_t* Vnorm;

   //energy Calibrated
   Float_t* E;
   Int_t EMIC;

   Float_t eloss;

   //Counters
   Int_t Counter[5];

   ClassDef(IonisationChamberv, 0)

   void SetLogLevel(int level)
   {
      logLevel = level;
   };
   int GetLogLevel()
   {
      return logLevel;
   };
};

#endif
