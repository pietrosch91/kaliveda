/*
$Id: KVElasticScatterEvent_e475s.h,v 1.1 2009/01/23 08:30:05 ebonnet Exp $
$Revision: 1.1 $
$Date: 2009/01/23 08:30:05 $
*/

//Created by KVClassFactory on Wed Dec 17 17:04:49 2008
//Author: eric bonnet,,,

#ifndef __KVELASTICSCATTEREVENT_E475S_H
#define __KVELASTICSCATTEREVENT_E475S_H

#include "KVElasticScatterEvent.h"
#include "KVGenParList.h"

class KVElasticScatterEvent_e475s : public KVElasticScatterEvent
{
	protected:
	Int_t b_noyau,b_det,b_evt;
	Float_t b_einc,b_theta,b_ruth,b_de,b_eres;
	KVGenParList lgen;
	
   public:
   KVElasticScatterEvent_e475s();
   virtual ~KVElasticScatterEvent_e475s();

	void AddHistoTree();
	void Process(KVMultiDetArray* mdet);

   ClassDef(KVElasticScatterEvent_e475s,1)//improvments
};

#endif
