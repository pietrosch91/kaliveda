#ifdef __CINT__
#include "RVersion.h"
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;
#pragma link C++ class KVRawDataReader+;
#ifdef ROOTGANILTAPE
#pragma link C++ class GTOneScaler+;
#pragma link C++ class GTScalers+;
#pragma link C++ class GTDataParameters+;
#pragma link C++ class GTGanilData+;
#endif
#ifdef WITH_GRULIB
//#pragma link C++ class KVGRUNetClientGanilReader+;
#pragma link C++ class KVGRUTapeGanilReader+;
#endif
#endif
