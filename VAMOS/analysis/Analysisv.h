#ifndef __ANALYSISV_H__

/**
   WARNING: This class has been deprecated and will eventually be removed.

   Deprecated by: Peter Wigg (peter.wigg.314159@gmail.com)
   Date:          Thu  8 Oct 13:56:06 BST 2015
*/

#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__
#ifdef __ENABLE_DEPRECATED_VAMOS__

// This class is only compiled if __ENABLE_DEPRECATED_VAMOS__ is set in
// VAMOS/analysis/Defines.h. If you enable the deprecated code using the default
// build options then a LARGE number of warnings will be printed to the
// terminal. To disable these warnings (not advised) compile VAMOS with
// -Wno-deprecated-declarations. Despite the warnings the code should compile
// just fine.

#define __ANALYSISV_H__

#include <cstdlib>

#include "Deprecation.h"
#include "KVBase.h"
#include "KVCsIVamos.h"
#include "KVDetector.h"
#include "KVMaterial.h"
#include "KVSiliconVamos.h"
#include "KVTarget.h"
#include "KVTelescope.h"
#include "LogFile.h"
#include "Rtypes.h"
#include "TFile.h"
#include "TList.h"
#include "TPluginManager.h"
#include "TTree.h"

class Analysisv {

public:

   LogFile* L;

   TFile* inTree;
   TTree* inT;
   Int_t innEntries;
   Int_t totalEntries;
   TFile* outTree;
   TTree* outT;

   Analysisv();
   Analysisv(LogFile* Log);
   virtual ~Analysisv();

   virtual void OpenInputTree(TTree*);
   virtual void CloseInputTree(const char* inTreeName);
   virtual void OpenOutputTree(TTree*);
   virtual void CloseOutputTree(const char* outTreeName);
   static Analysisv* NewAnalyser(const Char_t* dataset, LogFile* Log);

   virtual void inAttach() = 0; //Attaching the variables
   virtual void outAttach() = 0; //Attaching the variables
   virtual void Treat() = 0; // Treating data
   virtual void CreateHistograms() = 0;
   virtual void FillHistograms() = 0;

   virtual void SetTel1(KVDetector* si) = 0;
   virtual void SetTel2(KVDetector* gap) = 0;
   virtual void SetTel3(KVDetector* csi) = 0;

   virtual void SetTarget(KVTarget* tgt) = 0;
   virtual void SetDC1(KVDetector* dcv1) = 0;
   virtual void SetSed(KVMaterial* sed) = 0;
   virtual void SetDC2(KVDetector* dcv2) = 0;
   virtual void SetIC(KVDetector* ic) = 0;
   virtual void SetGap1(KVMaterial* isogap1) = 0;
   virtual void SetSi(KVMaterial* si) = 0;
   virtual void SetGap2(KVMaterial* isogap2) = 0;
   virtual void SetCsI(KVMaterial* ccsi) = 0;

   virtual void SetFileCut(TList* list) = 0;
   virtual void SetFileCutChioSi(TList* list2) = 0;
   virtual void SetFileCutSiTof(TList* list3) = 0;

   virtual void SetAngleVamos(Float_t angle) = 0;
   virtual void SetBrhoRef(Float_t brho) = 0;
   virtual void SetCurrentRun(Int_t run) = 0;

   ClassDef(Analysisv, 0)

};

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // __ANALYSISV_H__ is not set

#ifdef __ANALYSISV_H__
DEPRECATED_CLASS(Analysisv);
#endif

