//Created by KVClassFactory on Thu Aug 27 10:54:29 2009
//Author: marini

#ifndef __CSIV_H

/**
   WARNING: This class has been deprecated and will eventually be removed.

   Deprecated by: Peter Wigg (peter.wigg.314159@gmail.com)
   Date:          Thu  8 Oct 13:14:41 BST 2015
*/

#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__
#ifdef __ENABLE_DEPRECATED_VAMOS__

// This class is only compiled if __ENABLE_DEPRECATED_VAMOS__ is set in
// VAMOS/analysis/Defines.h. If you enable the deprecated code using the default
// build options then a LARGE number of warnings will be printed to the
// terminal. To disable these warnings (not advised) compile VAMOS with
// -Wno-deprecated-declarations. Despite the warnings the code should compile
// just fine.

#define __CSIV_H

#include "Deprecation.h"
#include "KVDataSet.h"
#include "LogFile.h"
#include "Random.h"
#include "TRandom3.h"
#include "TTree.h"
#include <Riostream.h>
#include <TString.h>
#include <cmath>

//const int MAX_CSI=80;

class CsIv {

public:
   CsIv(LogFile* Log);
   virtual ~CsIv();

   LogFile* L;

   Int_t NbCsI;

   void Init(void);
   void InitSavedQuantities(void);
   void Treat(void);
   void inAttach(TTree* inT);
   void outAttach(TTree* outT);
   void CreateHistograms();
   void FillHistograms();

   Float_t*  Ped;       //CsI pedestal
   Float_t**  ECoef;  //CsI calibration coefficients

   TRandom3* Rnd;

   //energy Raw
   UShort_t* E_Raw;
   Int_t* CsIERaw;
   UShort_t* E_Raw_Nr;
   Int_t E_RawM;

   //energy time Calibrated
   Int_t   EMCSI;
   Int_t* DetCsI;

   ClassDef(CsIv, 2) //VAMOS CsI

};

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // __CSIV_H is not set

#ifdef __CSIV_H
DEPRECATED_CLASS(CsIv);
#endif

