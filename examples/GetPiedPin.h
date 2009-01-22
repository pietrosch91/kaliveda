/*
$Id: GetPiedPin.cpp.h,v 1.2 2008/06/10
*/


#ifndef __GETPIEDPIN_H
#define __GETPIEDPIN_H

#include "KVINDRARawDataAnalyser.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

class GetPiedPin : public KVINDRARawDataAnalyser
{
   TTree* geneTree;
   TFile *geneFile;
   Int_t run, ring, module;
   Int_t pingg,pinpg;
   TH1F* geneHist;
   Int_t runEvents;
   
   public:

   GetPiedPin();
   virtual ~GetPiedPin();

   void InitAnalysis () ;
   void InitRun () ;
   Bool_t Analysis () ;
   void EndRun () ;
   void EndAnalysis () ;
   ClassDef(GetPiedPin,1)//Example gene data analysis class: extract mean values of all gene & laser
};

#endif
