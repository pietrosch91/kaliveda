/*
$Id: KVListView.h,v 1.1 2008/04/10 15:43:45 franklan Exp $
$Revision: 1.1 $
$Date: 2008/04/10 15:43:45 $
*/

//Created by KVClassFactory on Wed Apr  9 11:51:38 2008
//Author: franklan

#ifndef __KVLISTVIEW_H
#define __KVLISTVIEW_H

#include "TGListView.h"
#include "KVLVContainer.h"

class KVListView : public TGListView
{
	int 		nselected;		//number of selected items
	TClass 	*fObjClass; 	//class of objects in list

   public:
   KVListView(TClass* obj_class, const TGWindow *p, UInt_t w, UInt_t h,
              UInt_t options = kSunkenFrame | kDoubleBorder,
              Pixel_t back = GetDefaultFrameBackground());
   virtual ~KVListView(){};

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
	virtual void 	ActivateSortButtons();
   virtual void   SetDataColumns(Int_t ncolumns);
	virtual void 	SetDataColumn	(Int_t index, const Char_t* name, const Char_t* method="",
			Int_t mode = kTextCenterX);
	virtual	KVLVColumnData*		GetDataColumn	(Int_t index) const
	{
		return ((KVLVContainer*)GetContainer())->GetDataColumn(index);
	};
	
	virtual void Display(const TList *l) { ((KVLVContainer*)GetContainer())->Display(l); };
	virtual void SelectAll() { ((KVLVContainer*)GetContainer())->SelectAll(); };
	virtual void UnSelectAll() { ((KVLVContainer*)GetContainer())->UnSelectAll(); };
	virtual void RemoveAll() { ((KVLVContainer*)GetContainer())->RemoveAll(); };
	virtual TList* GetSelectedItems() { return ((KVLVContainer*)GetContainer())->GetSelectedItems(); };
	virtual Int_t GetColumnNumber(const Char_t* colname);
		 TGLVEntry*  	FindItemWithData(void *userData) { return ((KVLVContainer*)GetContainer())->FindItemWithData(userData);};
				void  	ActivateItemWithData(void *userData, Bool_t activate=kTRUE) {((KVLVContainer*)GetContainer())->ActivateItemWithData(userData,activate);};
		 TGLVEntry*  	FindItemWithColumnData(const Char_t* colname, const Char_t* data) 
				 { return ((KVLVContainer*)GetContainer())->FindItemWithColumnData(colname, data);};
		 TGLVEntry*  	FindItemWithColumnData(const Char_t* colname, Long_t data) 
				 { return ((KVLVContainer*)GetContainer())->FindItemWithColumnData(colname, data);};
		 TGLVEntry*  	FindItemWithColumnData(const Char_t* colname, Double_t data) 
				 { return ((KVLVContainer*)GetContainer())->FindItemWithColumnData(colname, data);};
				void  	ActivateItemWithColumnData(const Char_t* colname, const Char_t* data, Bool_t activate=kTRUE)
						{((KVLVContainer*)GetContainer())->ActivateItemWithColumnData(colname, data, activate);};
				void  	ActivateItemWithColumnData(const Char_t* colname, Long_t data, Bool_t activate=kTRUE)
						{((KVLVContainer*)GetContainer())->ActivateItemWithColumnData(colname, data, activate);};
				void  	ActivateItemWithColumnData(const Char_t* colname, Double_t data, Bool_t activate=kTRUE)
						{((KVLVContainer*)GetContainer())->ActivateItemWithColumnData(colname, data, activate);};

   ClassDef(KVListView,0)//Slightly modified TGListView
};

#endif
