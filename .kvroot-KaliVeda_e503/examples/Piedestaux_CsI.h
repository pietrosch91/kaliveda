/*
$Id: Piedestaux_CsI.h,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Mon Jun 25 17:09:12 2007
//Author: franklan

#ifndef __PIEDESTAUX_CSI_H
#define __PIEDESTAUX_CSI_H

#include "KVINDRARawDataAnalyser.h"
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

class Piedestaux_CsI : public KVINDRARawDataAnalyser
{
   TTree *t;
   TCanvas* can;
   TFile* file;

   public:

   Piedestaux_CsI();
   virtual ~Piedestaux_CsI();

   void InitAnalysis () ;
   void InitRun () ;
   Bool_t Analysis () ;
   void EndRun () ;
   void EndAnalysis () ;
   ClassDef(Piedestaux_CsI,1)//Example of raw data analysis class, for extracting & fitting pedestals
};

#endif
