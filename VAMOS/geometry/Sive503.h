#ifndef _SIVE503_CLASS

/**
   WARNING: This class has been deprecated and will eventually be removed.

   Deprecated by: Peter Wigg (peter.wigg.314159@gmail.com)
   Date:          Thu  8 Oct 13:11:29 BST 2015
*/

#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__
#ifdef __ENABLE_DEPRECATED_VAMOS__

// This class is only compiled if __ENABLE_DEPRECATED_VAMOS__ is set in
// VAMOS/analysis/Defines.h. If you enable the deprecated code using the default
// build options then a LARGE number of warnings will be printed to the
// terminal. To disable these warnings (not advised) compile VAMOS with
// -Wno-deprecated-declarations. Despite the warnings the code should compile
// just fine.

#define _SIVE503_CLASS

#include "Defines.h"
#include "Deprecation.h"
#include "KVDataSet.h"
#include "KVINDRAe503.h"
#include "LogFile.h"
#include "Random.h"
#include "Rtypes.h"
#include "TRandom3.h"
#include "TTree.h"
#include <TString.h>
#include <cmath>

//const int MAX=21;  //Maximum of silicon detector in VAMOS
//const int MAXRUN=635;

class Sive503 {
   Bool_t Ready;

private:
   Float_t fTofOffset;

public:
   Sive503(LogFile* Log);
   virtual ~Sive503(void);

   LogFile* L;

   bool Present;

   void Init(void);
   void InitSavedQuantities(void);
   void SetOffsetRelativisteRun(Int_t);
   void Calibrate(void);
   void Show_Raw(void);
   void Show(void);
   void Treat(void);
   void inAttach(TTree* inT);
   void outAttach(TTree* outT);
   void CreateHistograms();
   void FillHistograms();
   void PrintCounters(void);

   TRandom3* Rnd;

   Int_t NbSilicon;
   Int_t MaxRun;

   //energy Raw
   UShort_t* E_Raw;
   Int_t* SiERaw;

   UShort_t* E_Raw_Nr;

   //energy Raw
   Int_t E_RawM;
   UShort_t T_Raw[3];

   //Calibration coeff
   Float_t* si_ped;
   Float_t** ECoef;
   Float_t** TOff;

   //Float_t ECoef[MAX][3];
   //Float_t TOff[MAX][2];
   Float_t TCoef[3][5]; //for the new time calibration with 4 parameters



   Double_t* TRef;
   Double_t* Tpropre_el;
   Double_t* Tfrag;
   Double_t* T_final;
   Float_t* Offset_relativiste;   //(MAXRUN)

   Double_t* si_thick;

   //energy time Calibrated
   Int_t   EMSI;
   Float_t* E;
   Float_t T[3];
   Int_t* DetSi;

   //Counters
   Int_t Counter[2];

   //ClassDef(Sive503,2)
};

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // _SIVE503_CLASS is not set

#ifdef _SIVE503_CLASS
DEPRECATED_CLASS(Sive503);
#endif

