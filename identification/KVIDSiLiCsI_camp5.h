/*
$Id: KVIDSiLiCsI_camp5.h,v 1.2 2009/04/03 14:40:45 franklan Exp $
$Revision: 1.2 $
$Date: 2009/04/03 14:40:45 $
*/

//Created by KVClassFactory on Mon Mar 16 09:50:33 2009
//Author: John Frankland,,,

#ifndef __KVIDSILICSI_CAMP5_H
#define __KVIDSILICSI_CAMP5_H

#include "KVIDSiLiCsI.h"
#include "KVIDZAGrid.h"

class KVIDSiLiCsI_camp5 : public KVIDSiLiCsI
{

   KVIDZAGrid* fGGgrid;//! grid used for SiLi(GG)-CsI(R) charge & mass identification
   KVIDZAGrid* fPGgrid;//! grid used for SiLi(PG)-CsI(R) charge & mass identification
	
   public:
   KVIDSiLiCsI_camp5();
   virtual ~KVIDSiLiCsI_camp5();

   virtual Bool_t SetIDGrid(KVIDGraph*);

   virtual void Initialize(void);
   Bool_t Identify(KVReconstructedNucleus * nuc);
   
   virtual Double_t GetIDMapX(Option_t * opt = "");
   virtual Double_t GetIDMapY(Option_t * opt = "");
	
   ClassDef(KVIDSiLiCsI_camp5,1)//SiLi-CsI identification for INDRA_camp5 dataset
};

#endif
