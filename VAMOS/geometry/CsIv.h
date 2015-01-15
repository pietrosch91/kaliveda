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

class CsIv
{

   public:
   CsIv(LogFile *Log);
   virtual ~CsIv();
   
   LogFile *L;
   
   void Init(void);
   void InitRaw(void);
   void Calibrate(void);
   void Treat(void);
   void inAttach(TTree *inT);
   void outAttach(TTree *outT);
   void CreateHistograms();
   void FillHistograms();
  
  Float_t  Ped[80][1];       //CsI pedestal
  Float_t  ECoef[80][3];  //CsI calibration coefficients

  TRandom3 *Rnd;
  
  
  //energy Raw
  UShort_t E_Raw[80];
  UShort_t CsIRaw[80];
  //Float_t E_Raw[80];
  UShort_t E_Raw_Nr[80];
  Int_t E_RawM;
  
  UShort_t CSI_Raw;
  
  //energy time Calibrated
  Int_t   EM;
  //Float_t E[80];
  UShort_t ENr[80];
  Float_t ETotal;
  UShort_t Number;
  Int_t DetCsI;
  



   ClassDef(CsIv,1)//VAMOS CsI
};

#endif
