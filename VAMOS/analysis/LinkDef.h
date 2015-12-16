/***************************************************************************
$Id: VAMOSLinkDef.h,v 1.4 2009/01/14 15:59:49 franklan Exp $
 ***************************************************************************/

#ifdef __CINT__

#include "RVersion.h"
#include "KVConfig.h"
#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#ifdef WITH_BUILTIN_GRU
#pragma link C++ class KVIVRawDataAnalyser+;
#pragma link C++ class KVIVRawDataReconstructor+;
#endif

#ifdef __ENABLE_DEPRECATED_VAMOS__
#pragma link C++ class Analysisv+;
#pragma link C++ class Analysisv_e503+;
#pragma link C++ class Analysisv_e494s+;
#pragma link C++ class Reconstructionv+;
#pragma link C++ class ReconstructionSeDv+;
#pragma link C++ class KVIVReconIdent_e503+;
#endif

#pragma link C++ class KVIVReconDataAnalyser+;
#pragma link C++ class KVIVReconIdent+;
#pragma link C++ class KVIVSelector+;
#pragma link C++ class KVVAMOSExperimentalFilter+;

#endif
