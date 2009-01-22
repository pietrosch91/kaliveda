/*
$Id: KVLVEntry.h,v 1.1 2008/04/10 15:43:45 franklan Exp $
$Revision: 1.1 $
$Date: 2008/04/10 15:43:45 $
*/

//Created by KVClassFactory on Wed Apr  9 13:55:03 2008
//Author: franklan

#ifndef __KVLVENTRY_H
#define __KVLVENTRY_H

#include "TGListView.h"
#include "KVLVContainer.h"

class KVLVEntry : public TGLVEntry
{

   public:
   KVLVEntry(TObject* obj, const KVLVContainer *cnt,
			UInt_t ncols, KVLVColumnData **coldata);
   virtual ~KVLVEntry(){};

   ClassDef(KVLVEntry,0)//List view item class
};

#endif
