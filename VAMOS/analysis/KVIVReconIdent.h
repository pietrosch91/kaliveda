/*
$Id: KVIVReconIdent.h,v 1.2 2007/06/08 15:49:10 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/08 15:49:10 $
*/

//Created by KVClassFactory on Thu Jun  7 13:52:25 2007
//Author: e_ivamos

#ifndef __KVIVRECONIDENT_H
#define __KVIVRECONIDENT_H

//#include "KVReconIdent.h"
#include "KVIDGridManager.h"
#include <string>
#include <string.h>

#include "IonisationChamber.h"
#include "KVSiliconVamos.h"
#include "KVCsIVamos.h"
#include "KVDetector.h"

#define ID_SWITCH -1
#include "KVINDRAReconIdent.h"

class Analysisv;
class LogFile;

class KVIVReconIdent : public KVINDRAReconIdent
{
   Analysisv* fAnalyseV;//VAMOS calibration
   LogFile* fLogV;//VAMOS calibration log  
   
   public:

   KVIVReconIdent();
   virtual ~KVIVReconIdent();
   
   void InitAnalysis();
   void InitRun();
   Bool_t Analysis();
   void EndAnalysis();
   
   Int_t SetRunFlag(Int_t);
   Int_t GetRunFlag();   
   Int_t runFlag;
   Int_t ReadModuleMap();	//const Char_t *
   Bool_t LoadGrids();
   string module_map[18][80];   
   Int_t event;
   Int_t M_INDRA;
   Float_t  thetavam,brho;
   Double_t  brhorun;
   Double_t  thetavamrun;
   
   KVINDRAReconNuc *part;
   KVTelescope *kvt_sicsi;
   /*KVSiliconVamos *kvd_si;
   KVCsIVamos *kvd_csi;   
   KVDetector *gap; */
   KVDetector *kvd_si;
   KVDetector *kvd_csi;   
   KVDetector *gap; 	  	
   ClassDef(KVIVReconIdent,1)//Identification and reconstruction of VAMOS and INDRA events from recon data
};

#endif
