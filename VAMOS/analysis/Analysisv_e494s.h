#ifndef __ANALYSISV_E494S_H

/**
   WARNING: This class has been deprecated and will eventually be removed.

   Deprecated by: Peter Wigg (peter.wigg.314159@gmail.com)
   Date:          Thu  8 Oct 13:31:07 BST 2015
*/

#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__
#ifdef __ENABLE_DEPRECATED_VAMOS__

// This class is only compiled if __ENABLE_DEPRECATED_VAMOS__ is set in
// VAMOS/analysis/Defines.h. If you enable the deprecated code using the default
// build options then a LARGE number of warnings will be printed to the
// terminal. To disable these warnings (not advised) compile VAMOS with
// -Wno-deprecated-declarations. Despite the warnings the code should compile
// just fine.

#define __ANALYSISV_E494S_H

#define SED1
#define SED2
#define IONCHAMBER
#define SI

#ifdef SED1
#ifdef SED2
#define SED12
#endif
#endif

#include "Analysisv.h"
#include "Deprecation.h"
#include "IonisationChamberv.h"
#include "Random.h"
#include "ReconstructionSeDv.h"
#include "SeD12v.h"
#include "SeDv.h"
#include "Siv.h"
#include <cstdlib>

class Analysisv_e494s : public Analysisv {
public:

   SeDv* SeD1;
   SeDv* SeD2;
   SeD12v* SeD12;
   ReconstructionSeDv* RC;
   IonisationChamberv* Ic;
   Siv* Si;

   UShort_t T_Raw[10];

   Analysisv_e494s(LogFile* Log);
   virtual ~Analysisv_e494s(void);

   void inAttach(); //Attaching the variables
   void outAttach(); //Attaching the variables
   void Treat(); // Treating data
   void CreateHistograms();
   void FillHistograms();

   ClassDef(Analysisv_e494s, 0); //VAMOS calibration for E494S

};

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // __ANALYSISV_E494S_H is not set

#ifdef __ANALYSISV_E494S_H
DEPRECATED_CLASS(Analysisv_e494s);
#endif

