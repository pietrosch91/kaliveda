/*
$Id: KVLVEntry.h,v 1.3 2009/04/28 09:11:29 franklan Exp $
$Revision: 1.3 $
$Date: 2009/04/28 09:11:29 $
*/

//Created by KVClassFactory on Wed Apr  9 13:55:03 2008
//Author: franklan

#ifndef __KVLVENTRY_H
#define __KVLVENTRY_H

#include "TGListView.h"
#include "KVLVContainer.h"

class KVLVEntry : public TGLVEntry
{

	protected:
			
	static Pixel_t fgBGColor;// current background color for items
	static Pixel_t fgGreyPixel;// light grey color used to distinguish adjacent list items
	Pixel_t fBGColor;// background color of this item
	
	Bool_t *fBoolean;//'IsBoolean' attribute of each column
	
   public:
   KVLVEntry(TObject* obj, const KVLVContainer *cnt,
			UInt_t ncols, KVLVColumnData **coldata);
   virtual ~KVLVEntry(){
		delete [] fBoolean;
	};
	
	virtual void Refresh();
	virtual void	DrawCopy(Handle_t id, Int_t x, Int_t y);
	
   ClassDef(KVLVEntry,0)//List view item class
};

#endif
