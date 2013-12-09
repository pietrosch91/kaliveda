#ifndef __KVIVRECONIDENT_E503_H
#define __KVIVRECONIDENT_E503_H

#include "KVIDGridManager.h"
#include <string>
#include <string.h>

#include "IonisationChamber.h"
#include "KVSiliconVamos.h"
#include "KVCsIVamos.h"
#include "KVDetector.h"

#include "KVIVReconIdent.h"

#include "KVCsI.h"
#include "KVSilicon.h"
#include "KVMaterial.h"
#include "KVTarget.h"

class Analysisv;
class LogFile;

class KVIVReconIdent_e503 : public KVIVReconIdent
{
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
   
   ClassDef(KVIVReconIdent_e503,1)//Identification of VAMOS and INDRA events from recon data for e503 experiment
};

#endif
