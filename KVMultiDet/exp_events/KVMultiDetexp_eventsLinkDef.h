#ifdef __CINT__
#include "RVersion.h"
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ enum KVMultiDetArray::EFilterType;
#pragma link C++ global gMultiDetArray;

#pragma link C++ class KVEventFiltering+;
#pragma link C++ class KVReconstructedNucleus-;//customised streamer
#pragma link C++ class KVDetectorEvent+;
#pragma link C++ class KVElasticScatter+;
#pragma link C++ class KVElasticScatterEvent+;
#pragma link C++ class KVReconstructedEvent-;//customised streamer
#pragma link C++ class KVIDTelescope+;
#pragma link C++ class KVRTGIDManager+;
#pragma link C++ class KVGANILDataReader+;
#pragma link C++ class KVUpDater;
#ifdef ROOTGANILTAPE
#pragma link C++ class KVRawDataAnalyser+;
#endif
#pragma link C++ class KVMultiDetArray+;
#pragma link C++ class KVASMultiDetArray+;
#pragma link C++ class KVGeoImport+;
#endif
