#ifdef __CINT__
#include "RVersion.h"
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma link C++ class KVACQParam+;
#pragma link C++ class KVModule+;
#pragma link C++ class KVRawDataReader+;
#ifdef ROOTGANILTAPE
#pragma link C++ class GTOneScaler+;
#pragma link C++ class GTScalers+;
#pragma link C++ class GTDataParameters+;
#pragma link C++ class GTGanilData+;
#pragma link C++ class KVGANILDataReader+;
#endif
//#pragma link C++ class KVRegister+;
//#pragma link C++ class KVRegAnalogue+;
//#pragma link C++ class KVRegArray+;
//#pragma link C++ class KVRegBinary+;
//#pragma link C++ class KVRegHexa+;
//#pragma link C++ class KVRegShared+;
//#pragma link C++ class KVRBValAttribut+;
#ifdef WITH_GRULIB
#pragma link C++ class KVGRUNetClientGanilReader+;
#endif
#endif
