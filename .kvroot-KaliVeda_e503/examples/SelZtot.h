/*
$Id: SelZtot.h,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Mon Oct 23 17:58:28 2006
//Author: Daniel CUSSOL

#ifndef __SELZTOT_H
#define __SELZTOT_H

#include <KVDataSelector.h>

class SelZtot:public KVDataSelector
{
   
   private:
   Int_t zinf;
   
   public:

   SelZtot();
   virtual ~SelZtot();
   
   virtual void Init(void);
   virtual Bool_t SelectCurrentEntry(void);

     ClassDef(SelZtot,1)//Example of KVDataSelector, selection based on total charge measured
};

#endif
