/*
$Id: KVLVContainer.h,v 1.6 2009/04/10 13:48:07 franklan Exp $
$Revision: 1.6 $
$Date: 2009/04/10 13:48:07 $
*/

//Created by KVClassFactory on Wed Apr  9 13:54:31 2008
//Author: franklan

#ifndef __KVLVCONTAINER_H
#define __KVLVCONTAINER_H

#include "TGListView.h"
#include "TString.h"
#include "TMethodCall.h"
#include "TClass.h"
#include "TTimer.h"
#include "Riostream.h"
#include "KVDatime.h"
#include "TContextMenu.h"

class KVLVColumnData
{
	// Utility class describing the data used to fill each
	// column of the list view container

	TString			fName; 		// name used on button at top of column
	TString			fMethod; 	// method used to retrieve data from objects
	TMethodCall 	*fMethCall; // method call object
	Int_t 			fRetType;	// return type of data retrieval method
	TString			result;		// string used to store object data
	Bool_t			fDate;		// kTRUE if column contains TDatime date & time info
	KVDatime::EKVDateFormat  fFmt; 		// format for presenting date & time
	Bool_t			fIsKVDatime;	// kTRUE if date & time is in KVDatime object, TDatime if not

	enum {
		kDatimeRef = TMethodCall::kNone+1,
		kDatimeInt
	};
	Int_t Compare_date(TObject* o1, TObject* o2);
	Int_t Compare_string(TObject* o1, TObject *o2);
	Int_t Compare_long(TObject* o1, TObject *o2);
	Int_t Compare_double(TObject* o1, TObject *o2);

	public:
	KVLVColumnData(TClass* cl, const Char_t* name, const Char_t* method="")
			: fName(name), fMethod(method), result(""), fDate(kFALSE), fFmt(KVDatime::kCTIME), fIsKVDatime(kFALSE)
	{
		if(fMethod=="") fMethod.Form("Get%s", name);
		fMethCall = new TMethodCall(cl, fMethod.Data(), "");
		if( !fMethCall->IsValid() ){
			cout << "Error in <KVLVColumnData::KVLVColumnData> : method " << fMethod.Data()
				<< " is not valid" << endl;
		}
		fRetType = fMethCall->ReturnType();
	};
	virtual ~KVLVColumnData()
	{
		delete fMethCall;
	};
	virtual void SetIsDateTime(KVDatime::EKVDateFormat fmt=KVDatime::kCTIME, Bool_t with_reference=kTRUE);
	const Char_t* GetDataString(TObject*);
	void GetData(TObject*,Long_t&);
	void GetData(TObject*,Double_t&);
	void GetData(TObject*,TString&);
	void GetData(TObject*,KVDatime&);
	Int_t Compare(TObject* ob1, TObject* ob2);

	ClassDef(KVLVColumnData,0)//column data handler
};

class KVLVContainer : public TGLVContainer
{

	friend class KVLVFrameElement;
	friend class KVLVEntry;

	Bool_t 		fIsResized;		// used to resize columns exactly once
	Bool_t 		fSort;

	Bool_t		fAllowContextMenu;	// can objects' context menu be opened with right-click ?
	Bool_t		fAllowBrowse;			// can objects' Browse() method be executed by double-clicking ?
	
	Bool_t      fKeepUserItems;      // internal use only, do not clear list of user items in RemoveAll()
	protected:

	KVLVColumnData 	**fColData;		// description of column data
	Int_t 				fSortType;		// current sorting mode of contents (ascending or descending)
	KVLVColumnData		*fSortData;		// name of column (i.e. type of data) currently used to sort objects
	Int_t 				*fSortDir;		// direction of sorting for each column
	Int_t 				fNcols;			// number of data columns
	TContextMenu		*fContextMenu; // used to display popup context menu for items
	TList 				*fUserItems;	// list of currently displayed items, used by Refresh()

	virtual void FillList(const TList* = 0);
	void DeleteColData();
	void default_init();

   public:

	KVLVContainer(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1,
                   UInt_t options = kSunkenFrame,
                   Pixel_t back = GetDefaultFrameBackground());
   KVLVContainer(TGCanvas *p, UInt_t options = kSunkenFrame,
                   Pixel_t back = GetDefaultFrameBackground());
   virtual ~KVLVContainer();

				void  	AddFrame 		(TGFrame *f, TGLayoutHints *l=0);
				void  	Sort				(int column);
	virtual  void  	Display			(const TList* = 0);
	virtual  void  	Refresh			();
	virtual  void  	SetDataColumns (Int_t ncols);
	virtual  void  	SetDataColumn	(Int_t index, TClass *cl, const Char_t* name, const Char_t* method="");
	virtual	KVLVColumnData*		GetDataColumn	(Int_t index) const
	{
		return (fColData ? (index<fNcols ? fColData[index] : 0) : 0);
	};
		 TGLVEntry*  	FindItemWithData(void *userData);
				void  	ActivateItemWithData(void *userData, Bool_t activate=kTRUE);
		 TGLVEntry*  	FindItemWithColumnData(const Char_t* colname, const Char_t* data);
		 TGLVEntry*  	FindItemWithColumnData(const Char_t* colname, Long_t data);
		 TGLVEntry*  	FindItemWithColumnData(const Char_t* colname, Double_t data);
				void  	ActivateItemWithColumnData(const Char_t* colname, const Char_t* data, Bool_t activate=kTRUE);
				void  	ActivateItemWithColumnData(const Char_t* colname, Long_t data, Bool_t activate=kTRUE);
				void  	ActivateItemWithColumnData(const Char_t* colname, Double_t data, Bool_t activate=kTRUE);

				void		OpenContextMenu(TGFrame*,Int_t,Int_t,Int_t);
				void		HandleDoubleClick(TGFrame*,Int_t,Int_t,Int_t);

	TObject* GetLastSelectedObject() const
	{
		// Returns object corresponding to last clicked item in list
		return GetLastSelectedItem() ? (TObject*)GetLastSelectedItem()->GetUserData() : 0;
	};
	TGLVEntry* GetLastSelectedItem() const
	{
		// Returns last clicked item in list
		return fLastActive;
	};
	TObject* GetFirstInList();
	TObject* GetLastInList();
	TList* GetSelectedItems();
	TList* GetSelectedObjects();
	TList* GetUserItems()
	{
		// return list of all objects in list (regardless of selection)
		return fUserItems;
	};
	Bool_t IsBeingSorted() const
	{
		return fSort;
	};
	void AllowContextMenu(Bool_t on=kTRUE)
	{
		// Call with on=kFALSE to disable objects' context menus opening with mouse right-click
		fAllowContextMenu=on;
	};
	void AllowBrowse(Bool_t on=kTRUE)
	{
		// Call with on=kFALSE to disable objects' Browse() method being called with mouse double-click
		fAllowBrowse=on;
	};
	virtual void RemoveAll();

   ClassDef(KVLVContainer,0)//List view container class
};

#endif
