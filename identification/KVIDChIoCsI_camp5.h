/*
$Id: KVIDChIoCsI_camp5.h,v 1.1 2009/04/01 09:30:41 franklan Exp $
$Revision: 1.1 $
$Date: 2009/04/01 09:30:41 $
*/

//Created by KVClassFactory on Mon Mar 30 16:44:34 2009
//Author: John Frankland,,,

#ifndef __KVIDCHIOCSI_CAMP5_H
#define __KVIDCHIOCSI_CAMP5_H

#include "KVIDChIoCsI.h"
#include "KVIDZAGrid.h"

class KVIDChIoCsI_camp5 : public KVIDChIoCsI
{

   KVIDZAGrid* fGGgrid;//! grid used for ChIo(GG)-CsI(R) charge identification
   KVIDZAGrid* fPGgrid;//! grid used for ChIo(PG)-CsI(R) charge identification
	
   public:
   KVIDChIoCsI_camp5();
   virtual ~KVIDChIoCsI_camp5();

   virtual Bool_t SetIDGrid(KVIDGraph*);
   virtual void Initialize(void);

   ClassDef(KVIDChIoCsI_camp5,1)//ChIo-CsI id with grids for INDRA_camp5
};

#endif
