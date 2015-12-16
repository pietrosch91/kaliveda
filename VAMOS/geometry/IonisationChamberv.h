#ifndef _IONCHAMBER_CLASS

/**
   WARNING: This class has been deprecated and will eventually be removed.

   Deprecated by: Peter Wigg (peter.wigg.314159@gmail.com)
   Date:          Thu  8 Oct 13:19:33 BST 2015
*/

#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__
#ifdef __ENABLE_DEPRECATED_VAMOS__

// This class is only compiled if __ENABLE_DEPRECATED_VAMOS__ is set in
// VAMOS/analysis/Defines.h. If you enable the deprecated code using the default
// build options then a LARGE number of warnings will be printed to the
// terminal. To disable these warnings (not advised) compile VAMOS with
// -Wno-deprecated-declarations. Despite the warnings the code should compile
// just fine.

#define _IONCHAMBER_CLASS

#include "Deprecation.h"
#include "KVDataSet.h"
#include "LogFile.h"
#include "Random.h"
#include "Rtypes.h"
#include "TTree.h"
#include <cmath>

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

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // _IONCHAMBER_CLASS is not set

#ifdef _IONCHAMBER_CLASS
DEPRECATED_CLASS(IonisationChamberv);
#endif

