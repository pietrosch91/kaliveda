/*
$Id: KVClassFactory.cpp,v 1.19 2009/01/21 08:04:20 franklan Exp $
$Revision: 1.19 $
$Date: 2009/01/21 08:04:20 $
*/

//Created by KVClassFactory on Fri Jun 12 11:34:53 2009
//Author: Abdelouahao Chbihi

#ifndef __KVIDSICSIVAMOS_H
#define __KVIDSICSIVAMOS_H

#include "KVIDSiCsI.h"
#include "KVCsIVamos.h"
#include "KVSiliconVamos.h"
#include "KVIDZAGrid.h"

class KVIDSiCsIVamos : public KVIDSiCsI
{
   
   KVIDZAGrid* fgrid;//! grid used for Si-CsI charge identification of Vamos focal plan
	KVSiliconVamos* fSi;//!
	KVCsIVamos* fCsI;//!
   
   public:
   KVIDSiCsIVamos();
   virtual ~KVIDSiCsIVamos();
   
   Double_t esi;
   Double_t ecsi;
   
   virtual void Initialize(void);
   Bool_t Identify(Double_t, Double_t, KVIdentificationResult*);

   const Char_t * GetName() const;
	virtual Double_t GetIDMapX(Double_t, Option_t * opt = "");
	virtual Double_t GetIDMapY(Double_t, Option_t * opt = "");
   ClassDef(KVIDSiCsIVamos,1)//Identification map SIE_xx-CSIyy of Vamos
};

#endif
