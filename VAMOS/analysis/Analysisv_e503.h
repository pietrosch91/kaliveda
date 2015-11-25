/*
$Id: Analysisv_e503.h,v 1.2 2007/06/08 15:49:10 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/08 15:49:10 $
*/

//Created by KVClassFactory on Fri Jun  8 15:26:12 2007
//Author: e_ivamos

#ifndef __ANALYSISV_e503_H
#define __ANALYSISV_e503_H

#include"Defines.h"

#include "Random.h"

#define DRIFT
#define IONCHAMBER
#define SI
#define CSI

#include"IonisationChamberv.h"
#include"Sive503.h"
#include"CsIv.h"
#include"DriftChamberv.h"
#include"Reconstructionv.h"
#include"Identificationv.h"
#include"CsICalib.h"

#include"Analysisv.h"

#include "KVTelescope.h"
#include "KVSiliconVamos.h"
#include "KVCsIVamos.h"
#include "PlaneAbsorber.h"
#include "KVDetector.h"
#include "KVMaterial.h"

#include "TTree.h"
#include "TList.h"

class Analysisv_e503 : public Analysisv {
public:

   DriftChamberv* Dr;
   Reconstructionv* RC;
   Identificationv* Id;
   IonisationChamberv* Ic;
   Sive503* Si;
   CsIv* CsI;
   CsICalib* energytree;

   KVDetector* si;
   KVDetector* csi;
   KVDetector* gap;

   KVTarget* tgt;
   KVDetector* dcv1;
   KVMaterial* sed;
   KVDetector* dcv2;
   KVDetector* ic;
   KVMaterial* isogap1;
   KVMaterial* ssi;
   KVMaterial* isogap2;
   KVMaterial* ccsi;

   TTree* t;

   UShort_t T_Raw[10];

   Analysisv_e503(LogFile* Log);
   virtual ~Analysisv_e503(void);

   void inAttach(); //Attaching the variables
   void outAttach(); //Attaching the variables
   void Treat(); // Treating data
   void CreateHistograms();
   void FillHistograms();

   void SetTel1(KVDetector* si);
   void SetTel2(KVDetector* gap);
   void SetTel3(KVDetector* csi);

   TList* fcoup;
   TList* fcoup2;
   void SetFileCut(TList* list);
   void SetFileCutChioSi(TList* list2);

   void SetTarget(KVTarget* tgt);
   void SetDC1(KVDetector* dcv1);
   void SetSed(KVMaterial* sed);
   void SetDC2(KVDetector* dcv2);
   void SetIC(KVDetector* ic);
   void SetGap1(KVMaterial* isogap1);
   void SetSi(KVMaterial* ssi);
   void SetGap2(KVMaterial* isogap2);
   void SetCsI(KVMaterial* ccsi);

   ClassDef(Analysisv_e503, 0) //VAMOS calibration for e503

};

#endif
