/*
$Id: KVIVReconIdent.h,v 1.2 2007/06/08 15:49:10 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/08 15:49:10 $
*/

//Created by KVClassFactory on Thu Jun  7 13:52:25 2007
//Author: e_ivamos

#ifndef __KVIVRECONIDENT_H
#define __KVIVRECONIDENT_H

#include "KVINDRAReconIdent.h"

class Analysisv;
class LogFile;

class KVIVReconIdent : public KVINDRAReconIdent
{
   Analysisv* fAnalyseV;//VAMOS calibration
   LogFile* fLogV;//VAMOS calibration log

   public:

   KVIVReconIdent();
   virtual ~KVIVReconIdent();
   
   void InitAnalysis();
   void InitRun();
   Bool_t Analysis();
   void EndAnalysis();

   ClassDef(KVIVReconIdent,1)//Identification and reconstruction of VAMOS and INDRA events from recon data
};

#endif
