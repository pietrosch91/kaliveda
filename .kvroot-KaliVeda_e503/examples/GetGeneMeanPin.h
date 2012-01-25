/*
$Id: GetGeneMeanPin.h,v 1.1 2009/01/21 11:15:08 franklan Exp $
$Revision: 1.1 $
$Date: 2009/01/21 11:15:08 $
*/

//Created by KVClassFactory on Thu Jun 28 12:08:25 2007
//Author: franklan

#ifndef __GETGENEMEANPIN_H
#define __GETGENEMEANPIN_H

#include "KVINDRAGeneDataSelector.h"
//#include "KVINDRARawDataAnalyser.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

class GetGeneMeanPin : public KVINDRAGeneDataSelector
//class GetGeneMeanPin : public KVINDRARawDataAnalyser
{
   TTree* geneTree;
   TFile *geneFile;
   Int_t run, ring, module;
   Int_t pingg_laser,pinpg_laser,pingg_gene,pinpg_gene;
   TH1F* geneHist;
   Int_t runEvents,nlaser,ngene;
   
   public:

   GetGeneMeanPin();
   virtual ~GetGeneMeanPin();

   void InitAnalysis () ;
   void InitRun () ;
   Bool_t Analysis () ;
   void EndRun () ;
   void EndAnalysis () ;
   ClassDef(GetGeneMeanPin,1)//Example gene data analysis class: extract mean values of all gene & laser
};

#endif
