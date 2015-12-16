#ifndef __KVIVRECONIDENT_E503_H

/**
   WARNING: This class has been deprecated and will eventually be removed.

   Deprecated by: Peter Wigg (peter.wigg.314159@gmail.com)
   Date:          Thu  8 Oct 14:00:13 BST 2015
*/

#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__
#ifdef __ENABLE_DEPRECATED_VAMOS__

// This class is only compiled if __ENABLE_DEPRECATED_VAMOS__ is set in
// VAMOS/analysis/Defines.h. If you enable the deprecated code using the default
// build options then a LARGE number of warnings will be printed to the
// terminal. To disable these warnings (not advised) compile VAMOS with
// -Wno-deprecated-declarations. Despite the warnings the code should compile
// just fine.

#define __KVIVRECONIDENT_E503_H

#include "Analysisv.h"
#include "Deprecation.h"
#include "KVBatchSystem.h"
#include "KVCsI.h"
#include "KVCsIVamos.h"
#include "KVDataSet.h"
#include "KVDetector.h"
#include "KVIDGridManager.h"
#include "KVINDRAReconNuc.h"
#include "KVIVReconIdent.h"
#include "KVIdentificationResult.h"
#include "KVMaterial.h"
#include "KVReconstructedNucleus.h"
#include "KVSilicon.h"
#include "KVSiliconVamos.h"
#include "KVTarget.h"
#include "KVVAMOS.h"
#include "LogFile.h"
#include "Riostream.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>

class Analysisv;
class LogFile;

class KVIVReconIdent_e503 : public KVIVReconIdent {
   Analysisv* fAnalyseV;//VAMOS calibration
   LogFile* fLogV;//VAMOS calibration log

public:

   KVIVReconIdent_e503();
   virtual ~KVIVReconIdent_e503();

   void InitAnalysis();
   void InitRun();
   Bool_t Analysis();
   void EndAnalysis();

   //Grilles - E503
   Bool_t LoadGrids_e503();

   //Init - E503
   void Init_e503();

   ClassDef(KVIVReconIdent_e503, 1) //Identification of VAMOS and INDRA events from recon data for e503 experiment
};

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // __KVIVRECONIDENT_E503_H is not set

#ifdef __KVIVRECONIDENT_E503_H
DEPRECATED_CLASS(KVIVReconIdent_e503);
#endif

