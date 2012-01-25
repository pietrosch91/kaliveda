/*
$Id: SelZVtot.h,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Mon Oct 30 09:52:31 2006
//Author: Daniel CUSSOL

#ifndef __SELZVTOT_H
#define __SELZVTOT_H

#include <KVDataSelector.h>


class SelZVtot:public KVDataSelector
{
   private:
   Double_t zvinf;
   Int_t nsel;
   
   public:

   SelZVtot();
   virtual ~SelZVtot();
   
   virtual void Init(void);
   virtual Bool_t SelectCurrentEntry(void);

     ClassDef(SelZVtot,1)//Example of KVDataSelector, selection ZVtot >= 80% ZVInit
};

#endif
