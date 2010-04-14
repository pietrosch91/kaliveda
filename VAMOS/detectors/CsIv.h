//Created by KVClassFactory on Thu Aug 27 10:54:29 2009
//Author: marini

#ifndef __CSIV_H
#define __CSIV_H

#include <Riostream.h>
#include <TString.h>
#include "TTree.h"
#include "./analysis/KVDataSet.h"

class CsIv
{

   public:
   CsIv();
   virtual ~CsIv();
   void Init(void);
   void InitRaw(void);
   void Calibrate(void);
   void Treat(void);
   void inAttach(TTree *inT);
   void outAttach(TTree *outT);
  
  Float_t  Ped[80][1];       //CsI pedestal
  Float_t  ECoef[80][3];  //CsI calibration coefficients

  //energy Raw
  UShort_t E_Raw[80];
  UShort_t E_Raw_Nr[80];
  Int_t E_RawM;
  
  //energy time Calibrated
  Int_t   EM;
  Float_t E[80];
  UShort_t ENr[80];
  Float_t ETotal;
  UShort_t Number;


   ClassDef(CsIv,1)//VAMOS CsI
};

#endif
