/*
$Id: GetChIoSiMap.h,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Mon Jun 25 17:08:35 2007
//Author: franklan

#ifndef __GETCHIOSIMAP_H
#define __GETCHIOSIMAP_H

#include "KVINDRARawDataAnalyser.h"
#include "KVINDRAReconEvent.h"
#include "TTree.h"
#include "TFile.h"

class GetChIoSiMap : public KVINDRARawDataAnalyser
{
   TFile* file;
   TTree* tree;
   UChar_t ring, module;
   Short_t csir;
   UShort_t run;
   Bool_t si150;
   Float_t de_chio, e_si;
   KVINDRAReconEvent* rec_ev;
   
   public:

   GetChIoSiMap();
   virtual ~GetChIoSiMap();

   void InitAnalysis () ;
   void InitRun () ;
   Bool_t Analysis () ;
   void EndRun () ;
   void EndAnalysis () ;
   ClassDef(GetChIoSiMap,1)//Raw data analysis class for preparing ChIo-Si identification maps
};

#endif
