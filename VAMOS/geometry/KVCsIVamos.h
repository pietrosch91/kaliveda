/*
$Id: KVClassFactory.cpp,v 1.19 2009/01/21 08:04:20 franklan Exp $
$Revision: 1.19 $
$Date: 2009/01/21 08:04:20 $
*/

//Created by KVClassFactory on Wed Jun 10 16:09:12 2009
//Author: Abdelouahao Chbihi

#ifndef __KVCSIVAMOS_H
#define __KVCSIVAMOS_H

#include "KVCsI.h"

class KVCsIVamos : public KVCsI {
   KVACQParam* fPar;//!

public:
   KVCsIVamos();
   KVCsIVamos(Float_t thick);
   virtual ~KVCsIVamos();
   const Char_t* GetName() const;
   void SetACQParams();
   KVACQParam* GetACQPar()
   {
      return fPar;
   };
   virtual Double_t GetLumiereTotale(Double_t  = -1.0, Double_t = -1.0)
   {
      return fPar->GetData();
   };
   virtual Double_t GetCorrectedLumiereTotale(Double_t = -1.0, Double_t  = -1.0)
   {
      return fPar->GetData();
   };


   ClassDef(KVCsIVamos, 1) //KVCsI for Vamos
};

#endif
