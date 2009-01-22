/*
$Id: SelMult.h,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Fri Oct 20 17:07:39 2006
//Author: Daniel CUSSOL

#ifndef __SELMULT_H
#define __SELMULT_H

#include "KVDataSelector.h"

class SelMult:public KVDataSelector
{
   public:

   SelMult();
   virtual ~SelMult();
   
   virtual void Init(void);
   virtual Bool_t SelectCurrentEntry(void);

   ClassDef(SelMult,1)//Example of KVDataSelector, selection based on multiplicity of events
};

#endif
