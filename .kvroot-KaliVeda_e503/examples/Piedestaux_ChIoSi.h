/*
$Id: Piedestaux_ChIoSi.h,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Mon Jun 25 17:09:04 2007
//Author: franklan

#ifndef __PIEDESTAUX_CHIOSI_H
#define __PIEDESTAUX_CHIOSI_H

#include "KVINDRARawDataAnalyser.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TFile.h"

class Piedestaux_ChIoSi : public KVINDRARawDataAnalyser
{
   TTree *t;
   TCanvas* can;
   TFile* file;

   public:

   Piedestaux_ChIoSi();
   virtual ~Piedestaux_ChIoSi();

   void InitAnalysis () ;
   void InitRun () ;
   Bool_t Analysis () ;
   void EndRun () ;
   void EndAnalysis () ;
   ClassDef(Piedestaux_ChIoSi,1)//Example of raw data analysis class, for extracting & fitting pedestals
};

#endif
