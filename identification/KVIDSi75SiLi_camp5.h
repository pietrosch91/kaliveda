/*
$Id: KVIDSi75SiLi_camp5.h,v 1.2 2009/03/02 16:48:17 franklan Exp $
$Revision: 1.2 $
$Date: 2009/03/02 16:48:17 $
*/

//Created by KVClassFactory on Mon Oct 29 16:45:49 2007
//Author: franklan

#ifndef __KVIDSI75SILI_CAMP5_H
#define __KVIDSI75SILI_CAMP5_H

#include "KVIDSi75SiLi.h"
#include "KVIDZAGrid.h"

class KVIDSi75SiLi_camp5 : public KVIDSi75SiLi
{
   KVIDZAGrid* fGGgrid;//! grid used for Si75(GG)-SiLi(PG) charge & mass identification
   KVIDZAGrid* fPGgrid;//! grid used for Si75(PG)-SiLi(PG) charge & mass identification
   KVIDZGrid* fPGZgrid;//! grid used for Si75(PG)-SiLi(PG) charge identification
   
   public:

   KVIDSi75SiLi_camp5();
   virtual ~KVIDSi75SiLi_camp5();

   virtual Bool_t SetIdentificationParameters(const KVMultiDetArray*);
   virtual Bool_t SetIDGrid(KVIDGrid *);

   virtual void Initialise(void);
   Bool_t Identify(KVReconstructedNucleus * nuc);
   
   virtual Double_t GetIDMapX(Option_t * opt = "");
   virtual Double_t GetIDMapY(Option_t * opt = "");
   
   ClassDef(KVIDSi75SiLi_camp5,1)//Identification in Si75-SiLi telescopes for INDRA_camp5 dataset
};

#endif
