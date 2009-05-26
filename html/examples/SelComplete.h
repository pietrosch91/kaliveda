/*
$Id: SelComplete.h,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Tue Oct 24 16:53:38 2006
//Author: Daniel CUSSOL

#ifndef __SELCOMPLETE_H
#define __SELCOMPLETE_H

#include <KVDataSelector.h>


class SelComplete:public KVDataSelector
{
   private:
   Int_t zinf;
   Double_t pzinf;
   
   public:

   SelComplete();
   virtual ~SelComplete();
   
   virtual void Init(void);
   virtual Bool_t SelectCurrentEntry(void);

     ClassDef(SelComplete,1)//Example of KVDataSelector, selection of complete events
};

#endif
