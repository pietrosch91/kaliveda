/***************************************************************************
$Id: VAMOSLinkDef.h,v 1.4 2009/01/14 15:59:49 franklan Exp $
 ***************************************************************************/

#ifdef __CINT__
#include "RVersion.h"
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#ifdef ROOTGANILTAPE
#pragma link C++ class KVIVRawDataAnalyser+;
#pragma link C++ class KVIVRawDataReconstructor+;
#endif
#pragma link C++ class KVIVReconIdent+;
#pragma link C++ class Analysisv+;
#pragma link C++ class Analysisv_e494s+;
#pragma link C++ class Analysisv_e503+;
#pragma link C++ class Reconstructionv+;
#pragma link C++ class ReconstructionSeDv+;
#pragma link C++ class KVIVSelector+;
#endif
