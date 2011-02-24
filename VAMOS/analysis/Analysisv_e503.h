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
#include"Siv.h"
#include"CsIv.h"
#include"DriftChamberv.h"
#include"Reconstructionv.h"
#include"Identificationv.h"
#include"EnergyTree.h"

#include"Analysisv.h"

#include"TTree.h"
class Analysisv_e503 : public Analysisv
{
 public:
  
  DriftChamberv *Dr;
  Reconstructionv *RC;
  Identificationv *Id;
  IonisationChamberv *Ic;
  Siv *Si;
  CsIv *CsI;
  EnergyTree *energytree;
  
   TTree* t;
   
  UShort_t T_Raw[10];
  
  Analysisv_e503(LogFile *Log);
  virtual ~Analysisv_e503(void);
  
  void inAttach(); //Attaching the variables 
  void outAttach(); //Attaching the variables 
  void Treat(); // Treating data
  void CreateHistograms();
  void FillHistograms();
  
  void SetBrhoRef(Double_t);
  void SetAngleVamos(Double_t); 
  Double_t GetBrhoRef(void);
  Double_t GetAngleVamos(void);
    
  Double_t BB;
  Double_t ttheta;

  ClassDef(Analysisv_e503,0)//VAMOS calibration for e503

};

#endif
