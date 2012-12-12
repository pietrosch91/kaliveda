/*
$Id: KVIVReconIdent.h,v 1.2 2007/06/08 15:49:10 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/08 15:49:10 $
*/

//Created by KVClassFactory on Thu Jun  7 13:52:25 2007
//Author: e_ivamos

#ifndef __KVIVRECONIDENT_H
#define __KVIVRECONIDENT_H

#include "KVIDGridManager.h"
#include <string>
#include <string.h>

#include "IonisationChamber.h"
#include "KVSiliconVamos.h"
#include "KVCsIVamos.h"
#include "KVDetector.h"

#include "KVINDRAReconIdent.h"

#include "KVCsI.h"
#include "KVSilicon.h"
#include "KVMaterial.h"
#include "KVTarget.h"

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
   
   //Grilles - E503
   Bool_t LoadGrids_e503();
   
   //Init - E503
   void Init_e503();
   
	TFile*	fcoup;
	TList* list;
	TFile*	fcoup2;
	TList* list2;
	TFile*	fcoup3;
	TList* list3;
			
TCutG *q21;
TCutG *q20;
TCutG *q19;
TCutG *q18;
TCutG *q17;
TCutG *q16;
TCutG *q15;
TCutG *q14;
TCutG *q13;
TCutG *q12;
TCutG *q11;
TCutG *q10;
TCutG *q9;
TCutG *q8;
TCutG *q7;
TCutG *q6;
TCutG *q5;

TCutG *q21cs;
TCutG *q20cs;
TCutG *q19cs;
TCutG *q18cs;
TCutG *q17cs;
TCutG *q16cs;
TCutG *q15cs;
TCutG *q14cs;
TCutG *q13cs;
TCutG *q12cs;
TCutG *q11cs;
TCutG *q10cs;
TCutG *q9cs;
TCutG *q8cs;
TCutG *q7cs;
TCutG *q6cs;
TCutG *q5cs;

TCutG *q21st;
TCutG *q20st;
TCutG *q19st;
TCutG *q18st;
TCutG *q17st;
TCutG *q16st;
TCutG *q15st;
TCutG *q14st;
TCutG *q13st;
TCutG *q12st;
TCutG *q11st;
TCutG *q10st;
TCutG *q9st;
TCutG *q8st;
TCutG *q7st;
TCutG *q6st;
TCutG *q5st;
	
	KVNucleus *kvn;
	KVTarget *tgt;
        KVDetector *dcv1;
        KVMaterial *sed;
        KVDetector *dcv2;
        KVDetector *ic;
        KVMaterial *isogap1;
        KVMaterial *ssi;
        KVMaterial *isogap2;
        KVMaterial *ccsi;
			
	KVTelescope *kvt_sicsi;
	KVDetector *kvd_si;
   	KVDetector *kvd_csi;   
   	KVDetector *gap;    
    	  	
   ClassDef(KVIVReconIdent,1)//Identification and reconstruction of VAMOS and INDRA events from recon data
};

#endif
