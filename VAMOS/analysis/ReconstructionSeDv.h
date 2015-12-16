#ifndef _RECONSTRUCTIONSED_CLASS

/**
   WARNING: This class has been deprecated and will eventually be removed.

   Deprecated by: Peter Wigg (peter.wigg.314159@gmail.com)
   Date:          Thu  8 Oct 13:56:43 BST 2015
*/

#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__
#ifdef __ENABLE_DEPRECATED_VAMOS__

// This class is only compiled if __ENABLE_DEPRECATED_VAMOS__ is set in
// VAMOS/analysis/Defines.h. If you enable the deprecated code using the default
// build options then a LARGE number of warnings will be printed to the
// terminal. To disable these warnings (not advised) compile VAMOS with
// -Wno-deprecated-declarations. Despite the warnings the code should compile
// just fine.

#define _RECONSTRUCTIONSED_CLASS

#include "Deprecation.h"
#include "KVDataSet.h"
#include "LogFile.h"
#include "Random.h"
#include "Rtypes.h"
#include "SeD12v.h"
#include "TVector3.h"

class ReconstructionSeDv {
   Bool_t Ready;
public:
   ReconstructionSeDv(LogFile* Log, SeD12v* SeD12);
   virtual ~ReconstructionSeDv(void);

   LogFile* L;
   SeD12v* S12;

   bool Present; //true if coordinates determined



   void Init(void); //Init for every event,  variables go to -500.
   void Calculate(); // Calulate  Initial coordinates
   void Show(void);
   void Treat(void);
   void outAttach(TTree* outT);
   void CreateHistograms();
   void FillHistograms();
   void PrintCounters(void);

   Random* Rnd;

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

   ClassDef(ReconstructionSeDv, 0)

};

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // _RECONSTRUCTIONSED_CLASS is not set

#ifdef _RECONSTRUCTIONSED_CLASS
DEPRECATED_CLASS(ReconstructionSeDv);
#endif

