/*
$Id: GetGeneMean.h,v 1.2 2007/06/29 10:46:46 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/29 10:46:46 $
*/

//Created by KVClassFactory on Thu Jun 28 12:08:25 2007
//Author: franklan

#ifndef __GETGENEMEAN_H
#define __GETGENEMEAN_H

#include "KVINDRAGeneDataSelector.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

class GetGeneMean : public KVINDRAGeneDataSelector
{
   TTree* geneTree;
   TFile *geneFile;
   Int_t run, ring, module, cigg, cipg, sipg, sigg, si75gg, si75pg, siligg, silipg, csir, csil;
   TH1F* geneHist;
   Int_t runEvents;
   
   public:

   GetGeneMean();
   virtual ~GetGeneMean();

   void InitAnalysis () ;
   void InitRun () ;
   Bool_t Analysis () ;
   void EndRun () ;
   void EndAnalysis () ;
   ClassDef(GetGeneMean,1)//Example gene data analysis class: extract mean values of all gene & laser
};

#endif
