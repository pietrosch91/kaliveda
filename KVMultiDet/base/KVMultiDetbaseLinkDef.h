#ifdef __CINT__
#include "RVersion.h"
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ global gHistoManipulator;
#pragma link C++ enum KVBase::EKaliVedaBits;
#pragma link C++ function SearchFile(const Char_t*, TString&, int);
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma extra_include "Rtypes.h";
#pragma link C++ class Binary_t<UChar_t>+;
#pragma link C++ class Binary_t<UShort_t>+;
#pragma link C++ class Binary_t<UInt_t>+;
#pragma link C++ class Binary_t<Long64_t>+;
#pragma link C++ class KVString+;
#ifdef __WITHOUT_TPARAMETER
#pragma link C++ class KVParameter<Int_t>+;
#pragma link C++ class KVParameter<Double_t>+;
#elif ! defined ( __WITH_TPARAMETER_INT )
#pragma link C++ class KVParameter<Int_t>+;
#endif
#ifdef __WITHOUT_TPARAMETER
#pragma link C++ class KVParameter<KVString>+;
#pragma link C++ class KVParameter<TString>+;
#else
#pragma link C++ class TParameter<KVString>+;
#pragma link C++ class TParameter<TString>+;
#endif
#pragma link C++ class KVParameterList<Int_t>+;
#pragma link C++ class KVParameterList<Double_t>+;
#pragma link C++ class KVParameterList<TString>+;
#pragma link C++ class KVParameterList<KVString>+;
#pragma link C++ class KVRungeKutta+;
#pragma link C++ class Hexa_t+;
#pragma link C++ class KVNameValueList+;
#pragma link C++ class KVBase-;//customised streamer
#pragma link C++ class KVClassFactory+;
#pragma link C++ class KVClassMethod+;
#pragma link C++ class KVClassMonitor+;
#pragma link C++ class KVSeqCollection-;
#pragma link C++ class KVDataBranchHandler+;
#pragma link C++ class KVDatime+;
#pragma link C++ class KVDatedFileManager+;
#pragma link C++ class KVGenParList+;
#pragma link C++ class KVHashList+;
#pragma link C++ class KVUniqueNameList+;
#pragma link C++ class KVPartitionList+;
#pragma link C++ class KVList-;
#pragma link C++ class KVLockfile+;
#pragma link C++ class KVNamedParameter+;
#pragma link C++ class KVLogReader+;
#pragma link C++ class KVBQSLogReader+;
#pragma link C++ class KVGELogReader+;
#pragma link C++ class KVMemoryChunk+;
#pragma link C++ class KVMemoryPool+;
#pragma link C++ class KVNumberList+;
#pragma link C++ class KVFileReader;
#pragma link C++ class KVValues;
#pragma link C++ class KVRList+;
#pragma link C++ class KVSortableDatedFile+;
#pragma link C++ class KVTarArchive+;
#pragma link C++ class KVHistoManipulator;
#pragma link C++ class KVEventListMaker;
#pragma link C++ class KVIntegerList-;
#pragma link C++ class KVUpdateChecker;
#pragma link C++ class KVCVSUpdateChecker;
#pragma link C++ class KVSystemFile+;
#pragma link C++ class KVSystemDirectory+;
#pragma link C++ class KVEnv+;
#endif
