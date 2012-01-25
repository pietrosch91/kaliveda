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
//#include "KVSelector.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

//class GetGeneMean : public KVSelector
class GetGeneMean : public KVINDRAGeneDataSelector
{
   TTree* geneTree;
   TFile *geneFile;
   TH1F* geneHist;
   Int_t run, ring, module, cigg, cipg, sipg, sigg, si75gg, si75pg;
   Int_t siligg, silipg, csir, csil;
   Int_t bininf, binsup, binmax;
   Int_t runEvents;
   Double_t moy;
   
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
