/*
$Id: SelPtot.h,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Tue Oct 24 13:51:17 2006
//Author: Daniel CUSSOL

#ifndef __SELPTOT_H
#define __SELPTOT_H

#include <KVDataSelector.h>


class SelPtot:public KVDataSelector
{
   
   private:
   Double_t pzinf;
   
   public:

   SelPtot();
   virtual ~SelPtot();
   
   virtual void Init(void);
   virtual Bool_t SelectCurrentEntry(void);

     ClassDef(SelPtot,1)//Example of KVDataSelector, selection based on total parallel momentum
};

#endif
