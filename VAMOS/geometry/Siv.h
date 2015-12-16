#ifndef _SI_CLASS

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

#define _SI_CLASS

#include "Deprecation.h"
#include "KVDataSet.h"
#include "LogFile.h"
#include "Random.h"
#include "Rtypes.h"
#include "TTree.h"

class Siv {
   Bool_t Ready;

public:
   Siv(LogFile* Log);
   virtual ~Siv(void);

   LogFile* L;

   bool Present;

   void Init(void);
   void InitRaw(void);
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
   UShort_t E_Raw[21];
   UShort_t E_Raw_Nr[21];
   Int_t E_RawM;
   UShort_t T_Raw[3];



   //Calibration coeff
   Float_t ECoef[21][3];
   Float_t TOff[21][2];
   Float_t TCoef[3][2];

   //

   //energy time Calibrated
   Int_t   EM;
   Float_t E[21];
   UShort_t ENr[21];
   Float_t T[3];
   Float_t Offset[2];
   Float_t ETotal;
   UShort_t Number;


   //Counters
   Int_t Counter[2];

   ClassDef(Siv, 0)
};

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // _SI_CLASS is not set

#ifdef _SI_CLASS
DEPRECATED_CLASS(Siv);
#endif

