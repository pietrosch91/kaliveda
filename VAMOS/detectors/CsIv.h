//Created by KVClassFactory on Thu Aug 27 10:54:29 2009
//Author: marini

#ifndef __CSIV_H
#define __CSIV_H

#include <Riostream.h>
#include <TString.h>
#include "TTree.h"
#include "KVDataSet.h"
#include"LogFile.h"
#include"Random.h"
#include"TRandom3.h"

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

#endif
