/*
$Id: KVINDRARawIdent.h,v 1.2 2008/11/12 16:15:49 franklan Exp $
$Revision: 1.2 $
$Date: 2008/11/12 16:15:49 $
*/

//Created by KVClassFactory on Wed Feb 20 10:52:20 2008
//Author: franklan

#ifndef __KVINDRARAWIDENT_H
#define __KVINDRARAWIDENT_H

#include "KVINDRARawDataAnalyser.h"
#include "KVINDRAReconEvent.h"
#include "TFile.h"
#include "TTree.h"

class KVINDRARawIdent : public KVINDRARawDataAnalyser
{

   protected:
         
   TFile* file;
   TTree* tree;
   TTree* genetree;
   TTree* rawtree;
   KVINDRAReconEvent* recev;
   Int_t nb_recon;//number of reconstructed INDRA events
   
   public:
   KVINDRARawIdent();
   virtual ~KVINDRARawIdent();

   void InitAnalysis () ;
   void InitRun () ;
   Bool_t Analysis () ;
   void EndRun () ;
   void EndAnalysis () ;

   ClassDef(KVINDRARawIdent,2)//User raw data analysis class
};

#endif
