/*
$Id: KVLVContainer.cpp,v 1.8 2009/04/28 09:11:29 franklan Exp $
$Revision: 1.8 $
$Date: 2009/04/28 09:11:29 $
*/

//Created by KVClassFactory on Wed Apr  9 13:54:31 2008
//Author: franklan

#include "KVListView.h"
#include "KVLVContainer.h"
#include "KVLVEntry.h"
#include "TFunction.h"
#include "TRootContextMenu.h"
#include "TSystem.h"

class KVLVFrameElement : public TGFrameElement {
	public:
	KVLVContainer *fContainer; // object container

	Bool_t IsSortable() const { return kTRUE; }
	Int_t  Compare(const TObject* obj) const;
};

Int_t KVLVFrameElement::Compare(const TObject* obj) const
{
	// Method responsible for sorting the objects in the GUI list

	KVLVEntry *f1 = (KVLVEntry *) fFrame;
   KVLVEntry *f2 = (KVLVEntry *) ((TGFrameElement *) obj)->fFrame;
	TObject* r1 = (TObject*)f1->GetUserData();
	TObject* r2 = (TObject*)f2->GetUserData();
	return fContainer->fSortType * fContainer->fSortData->Compare(r1,r2);
}

//____________________________________________________________________________//

ClassImp(KVLVColumnData)

void KVLVColumnData::GetData(TObject* obj, Long_t &i)
{
	fMethCall->Execute(obj,"",i);
}

void KVLVColumnData::GetData(TObject* obj,Double_t &i)
{
	fMethCall->Execute(obj,"",i);
}

void KVLVColumnData::GetData(TObject* obj,TString &i)
{
	Char_t *cret;
	fMethCall->Execute(obj,"",&cret);
	i.Form("%s",cret);
}

void KVLVColumnData::GetData(TObject* obj,KVDatime &i)
{
	Long_t lret;
	fMethCall->Execute(obj,"",lret);
	switch(fRetType){
		case kDatimeRef:
			((TDatime*)lret)->Copy(i);
			break;

		case kDatimeInt:
			i.Set((UInt_t)lret);
	}
}

const Char_t* KVLVColumnData::GetDataString(TObject* obj)
{
	//  Format string with column data for object

	result = "";
	Long_t lret;
	Double_t dret;
	KVDatime dtret;
	switch(fRetType){

		case TMethodCall::kString :
			GetData(obj,result);
			break;

		case TMethodCall::kLong :
	   	GetData(obj,lret);
	   	result.Form("%ld", lret);
			break;

		case TMethodCall::kDouble :
	   	GetData(obj,dret);
	   	result.Form("%f", dret);
			break;

		case kDatimeRef:
		case kDatimeInt:
			GetData(obj,dtret);
			result.Form("%s", dtret.String(fFmt));
			break;

		default:
			cout << "Error in <KVLVColumnData::GetData> : this type is not supported "
	   		<< fMethCall->ReturnType() << endl;
			break;
	}
	return result.Data();
}

void KVLVColumnData::SetIsDateTime(KVDatime::EKVDateFormat fmt, Bool_t with_reference)
{
	// If data in column is date & time info, use this method to
	//
	//    - set the format for presenting the date & time. These are the formats
	//      available for KVDatime objects, i.e.
	//       KVDatime::kCTIME       - ctime format (default)   e.g. Thu Apr 10 10:48:34 2008
	//       KVDatime::kSQL         - SQL format               e.g. 1997-01-15 20:16:28
	//       KVDatime::kGANACQ      - GANIL acquisition format e.g. 29-SEP-2005 09:42:17.00
	//
	//    - define how the information is obtained from the object. The method
	//      given to the constructor must return either
	//        a reference/pointer to TDatime/KVDatime :                      with_reference=kTRUE (default)
	//        a UInt_t/time_t value (such as returned by TDatime::Convert) : with_reference=kFALSE

	fFmt = fmt;
	if( with_reference ){
		fRetType = kDatimeRef;
		// determine class of returned pointer/reference
		TString ptr_type = fMethCall->GetMethod()->GetReturnTypeName();
		fIsKVDatime = ptr_type.Contains("KVDatime");
	}
	else
	{
		fRetType = kDatimeInt;
	}
}

Int_t KVLVColumnData::Compare_date(TObject* o1, TObject* o2)
{
	KVDatime d1, d2;
	GetData(o1,d1); GetData(o2,d2);
	return (d1<d2 ? -1 : (d1>d2 ? 1 : 0));
}

Int_t KVLVColumnData::Compare_string(TObject* o1, TObject *o2)
{
	TString s1, s2;
	GetData(o1,s1); GetData(o2,s2);
	return s2.CompareTo(s1);
}

Int_t KVLVColumnData::Compare_long(TObject* o1, TObject *o2)
{
	Long_t l1, l2;
	GetData(o1,l1); GetData(o2,l2);
	return (l1<l2 ? -1 : (l1>l2 ? 1 : 0));
}

Int_t KVLVColumnData::Compare_double(TObject* o1, TObject *o2)
{
	Double_t l1, l2;
	GetData(o1,l1); GetData(o2,l2);
	return (l1<l2 ? -1 : (l1>l2 ? 1 : 0));
}


Int_t KVLVColumnData::Compare(TObject* ob1, TObject* ob2)
{
	// Compare (for sorting) the two objects based on the type of data stored
	// in this column. The sorting depends on the return type of the data.
	//  for integers:  return 1 if ob1->data < ob2->data etc.
	//  for floats:    return 1 if ob1->data < ob2->data etc.
	//  for strings:   alphabetical sorting using TString::CompareTo
	//  for dates:     chronological order (earliest first)

	switch(fRetType){

		case TMethodCall::kString :
			return Compare_string(ob1,ob2);
			break;

		case TMethodCall::kLong :
	   	return Compare_long(ob1,ob2);
			break;

		case TMethodCall::kDouble :
	   	return Compare_double(ob1,ob2);
			break;

		case kDatimeRef:
		case kDatimeInt:
			return Compare_date(ob1,ob2);
			break;

		default:
			cout << "Error in <KVLVColumnData::Compare> : this type is not supported "
	   		<< fMethCall->ReturnType() << endl;
			break;
	}
	return 0;
}

//____________________________________________________________________________//

ClassImp(KVLVContainer)
////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVLVContainer</h2>
<h4>Utility class for KVListView</h4>

<img src="images/KVLVContainer.png"><br>

<h3>Example of use</h3>
KVListView takes a list of objects (TObject or derived class) and presents them
as a list of items with different data for each object presented in separate columns.
The user specifies the base-class of the objects to display, the number of columns,
and the data to be presented in each column. The example in the figure shown above
was generated by the following:
<pre>
	lvRuns = new KVListView(<a href="KVDBRun.html">KVDBRun</a>::Class(), cfRuns, 500, 250); //create list view for KVDBRun objects. 'cfRuns' is pointer to GUI frame containing the list.
	lvRuns->SetDataColumns(4);	//set number of columns to 4

	lvRuns->SetDataColumn(0, "Run", "GetNumber");       //by default, the data presented in a column with title "Toto" will be retrieved
	lvRuns->SetDataColumn(1, "Events", "", kTextRight); //from each object by calling the "GetToto" method of the class given to the KVListView ctor.
	lvRuns->SetDataColumn(2, "Date", "GetDatime");      //in case the "getter" method has a non-standard name, it can be given explicitly.

	lvRuns->GetDataColumn(2)-><a href="KVLVColumnData.html#KVLVColumnData:SetIsDateTime">SetIsDateTime()</a>;//some special treatment is afforded to date/time data. see method doc.
	lvRuns->SetDataColumn(3, "Comments", "", kTextLeft);// specify text alignment for data


	lvRuns->ActivateSortButtons();//when clicking a column header, the list is sorted according to that column's data.
	...                           //clicking a second time the same column sorts objects in the opposite sense.
</pre>
To display objects, put them in a TList and call
<pre>
   lvRuns->Display( pointer_to_TList );
</pre>
Items in the list can be selected using either:
		<ul>
		<li>single left click (single object selection),</li>
		<li>CTRL+left-click (selection of multiple non-neighbouring objects),</li>
		<li>or SHIFT+left-click (selection of multiple & neighbouring objects)</li>
		</ul>
When the selection changes, the KVListView emits the <pre>SelectionChanged()</pre> signal.<br>
The list of the currently selected objects can be retrieved with the method <pre>GetSelectedItems()</pre>,
		which returns a TList of the currently selected <a href="KVLVEntry.html">KVLVEntry</a> objects,
		each one's name corresponds to the value displayed in the first column of the list view.<br>
Double-left-clicking an object will execute the <pre>Browse()</pre> method of the object. This method is defined for all TObjects,
it can be overridden in derived classes in order to do something interesting and/or useful.<br>
Right-clicking an object opens the context menu of the object, allowing the usual interaction with objects as in TBrower, TCanvas, etc.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////


KVLVContainer::KVLVContainer(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options, Pixel_t back)
	: TGLVContainer(p, w, h, options, back)
{
	default_init();
}

KVLVContainer::KVLVContainer(TGCanvas *p, UInt_t options, Pixel_t back)
	: TGLVContainer(p, options, back)
{
	default_init();
}

void KVLVContainer::default_init()
{
	// Default initialisation of list view container
	// Multiple selection is enabled
	// The parent list view widget handles messages generated by container.
	// By default, we allow objects' context menu to be opened with right-click,
	// and objects' Browse() method to be executed by double-clicking.
	// This can be modified using AllowContextMenu(kFALSE) and AllowDoubleClick(kFALSE).
	
	SetMultipleSelection(kTRUE);
	fIsResized=kFALSE;
	fSort=kFALSE;
	fSortDir=0;
	fColData=0;
	if(fListView) Associate(fListView);
	fContextMenu = new TContextMenu("KVListViewContextMenu");
	Connect("Clicked(TGFrame*,Int_t,Int_t,Int_t)",
			"KVLVContainer", this, "OpenContextMenu(TGFrame*,Int_t,Int_t,Int_t)");
	Connect("DoubleClicked(TGFrame*,Int_t,Int_t,Int_t)",
			"KVLVContainer", this, "DoDoubleClick(TGFrame*,Int_t,Int_t,Int_t)");
	fUserItems = new KVList;
   fUserItems->SetOwner(kFALSE);
   fUserItems->SetCleanup();
	
	fAllowContextMenu = kTRUE;
	fAllowDoubleClick = kTRUE;
	fUserDoubleClickAction = kFALSE;
	fKeepUserItems = kFALSE;
}

KVLVContainer::~KVLVContainer()
{
   // Destructor
	if(fSortDir) delete [] fSortDir;
	DeleteColData();
	delete fContextMenu;
	fUserItems->Clear();
	delete fUserItems;
}

//____________________________________________________________________________//

void KVLVContainer::DeleteColData()
{
	if(fColData){
		for(int i=0; i<fNcols; i++){
			if(fColData[i]) delete fColData[i];
		}
		delete [] fColData;
		fColData=0;
	}
}

//____________________________________________________________________________//

void KVLVContainer::Sort(int column)
{
   // Sort objects in container according to contents of given column.

   fSortData = fColData[column];
	fSortType = fSortDir[column];
	fSort = kTRUE;

   fList->Sort();
	//invert sort direction for this column for next time
	fSortDir[column]=-fSortDir[column];

   TGCanvas *canvas = (TGCanvas *) this->GetParent()->GetParent();
   canvas->Layout();

	fSort = kFALSE;
}

//____________________________________________________________________________//

void KVLVContainer::AddFrame(TGFrame *f, TGLayoutHints *l)
{
   // Add an item to the list

   KVLVFrameElement *nw;

   nw = new KVLVFrameElement;
   nw->fFrame     = f;
   nw->fLayout    = l ? l : fgDefaultHints;
   nw->fState     = 1;
   nw->fContainer = this;
   fList->Add(nw);
}

//______________________________________________________________________________

void KVLVContainer::Display(const TCollection* list_of_objects)
{
   // Display the list of objects in the container.

	/* remove all items from display, but keep list in memory */
   fKeepUserItems = kTRUE;
	RemoveAll();
	fKeepUserItems = kFALSE;
	
   FillList(list_of_objects);
	if(!fIsResized){
		GetListView()->ResizeColumns();
		fIsResized=kTRUE;
	}
   TGCanvas *canvas = (TGCanvas *) this->GetParent()->GetParent();
   canvas->Layout();

   MapSubwindows();
}

//______________________________________________________________________________

void KVLVContainer::Refresh()
{
   // Redisplay the list of objects in the container.
   // This can be used to refresh the contents of the window.

	Display();
}

//______________________________________________________________________________

void KVLVContainer::RemoveAll()
{
	// When the graphical list is emptied we need to empty the list
	// of user objects also.
	
	if( !fKeepUserItems ) fUserItems->Clear();
	TGLVContainer::RemoveAll();
}

//______________________________________________________________________________

void KVLVContainer::FillList(const TCollection* l)
{
	// Fill list from list
	// Pointers to objects are stored in internal list fUserItems for Refresh()

	if(l){
      fUserItems->Clear();
   }

	if(l && !l->GetSize()) return;

	TCollection* theList = (TCollection*)(l ? l : fUserItems);
	TIter nxt(theList);
	TObject* obj = 0;
	while( (obj = nxt()) ){
		if(l) fUserItems->Add(obj);
		KVLVEntry* ent =
				new KVLVEntry(obj, this, fNcols, fColData);
		AddItem( ent );
	}
}

//______________________________________________________________________________

void KVLVContainer::SetDataColumns(Int_t ncols)
{
	DeleteColData();
	if(fSortDir) delete [] fSortDir;
	fNcols = ncols;
	fSortDir = new int [ncols];
	fColData = new KVLVColumnData* [ncols];
	for(int i=0;i<ncols;i++){
		fSortDir[i]=1;
		fColData[i]=0;
	}
}

//______________________________________________________________________________

void KVLVContainer::SetDataColumn(Int_t index, TClass *cl, const Char_t* name, const Char_t* method)
{
	fColData[index] = new KVLVColumnData(cl, name, method);
}

//______________________________________________________________________________

TGLVEntry* KVLVContainer::FindItemWithData(void *userData)
{
   // Find item with fUserData == userData in container.

   TGFrameElement *el;
   TIter next(fList); TGLVEntry *f=0;
   while ((el = (TGFrameElement *) next())) {
      f = (TGLVEntry *) el->fFrame;
      if (f->GetUserData() == userData) {
         return f;
      }
   }
	return 0;
}

//______________________________________________________________________________

void KVLVContainer::ActivateItemWithData(void *userData, Bool_t activate)
{
   // Find item with fUserData == userData in container and make it active
	// (inactive if activate=kFALSE).

   TGFrameElement *el;
   TIter next(fList);
   while ((el = (TGFrameElement *) next())) {
      TGLVEntry *f = (TGLVEntry *) el->fFrame;
      if (f->GetUserData() == userData) {
         if(activate) ActivateItem(el);
			else DeActivateItem(el);
			break;
      }
   }
	fClient->NeedRedraw(this);
}

//______________________________________________________________________________

TGLVEntry* KVLVContainer::FindItemWithColumnData(const Char_t* colname, const Char_t* data)
{
	KVLVColumnData* CD = fColData[((KVListView*)GetListView())->GetColumnNumber(colname)];
	TString val;
   TGFrameElement *el;
   TIter next(fList);TGLVEntry *f=0;
   while ((el = (TGFrameElement *) next())) {
      f = (TGLVEntry *) el->fFrame;
		CD->GetData( (TObject*)f->GetUserData(), val );
		if(val==data) return f;
   }
	return 0;
}

//______________________________________________________________________________

TGLVEntry* KVLVContainer::FindItemWithColumnData(const Char_t* colname, Long_t data)
{
	KVLVColumnData* CD = fColData[((KVListView*)GetListView())->GetColumnNumber(colname)];
	Long_t val;
   TGFrameElement *el;
   TIter next(fList);TGLVEntry *f=0;
   while ((el = (TGFrameElement *) next())) {
      f = (TGLVEntry *) el->fFrame;
		CD->GetData( (TObject*)f->GetUserData(), val );
		if(val==data) return f;
   }
	return 0;
}

//______________________________________________________________________________

TGLVEntry* KVLVContainer::FindItemWithColumnData(const Char_t* colname, Double_t data)
{
	KVLVColumnData* CD = fColData[((KVListView*)GetListView())->GetColumnNumber(colname)];
	Double_t val;
   TGFrameElement *el;
   TIter next(fList);TGLVEntry *f=0;
   while ((el = (TGFrameElement *) next())) {
      f = (TGLVEntry *) el->fFrame;
		CD->GetData( (TObject*)f->GetUserData(), val );
		if(val==data) return f;
   }
	return 0;
}

//______________________________________________________________________________

void KVLVContainer::ActivateItemWithColumnData(const Char_t* colname, const Char_t* data, Bool_t activ)
{
	KVLVColumnData* CD = fColData[((KVListView*)GetListView())->GetColumnNumber(colname)];
	TString val;
   TGFrameElement *el;
   TIter next(fList);TGLVEntry *f=0;
   while ((el = (TGFrameElement *) next())) {
      f = (TGLVEntry *) el->fFrame;
		CD->GetData( (TObject*)f->GetUserData(), val );
		if(val==data) {
         if(activ) ActivateItem(el);
			else DeActivateItem(el);
			break;
		}
   }
	fClient->NeedRedraw(this);
}

//______________________________________________________________________________

void KVLVContainer::ActivateItemWithColumnData(const Char_t* colname, Long_t data, Bool_t activ)
{
	KVLVColumnData* CD = fColData[((KVListView*)GetListView())->GetColumnNumber(colname)];
	Long_t val;
   TGFrameElement *el;
   TIter next(fList);TGLVEntry *f=0;
   while ((el = (TGFrameElement *) next())) {
      f = (TGLVEntry *) el->fFrame;
		CD->GetData( (TObject*)f->GetUserData(), val );
		if(val==data) {
         if(activ) ActivateItem(el);
			else DeActivateItem(el);
			break;
		}
   }
	fClient->NeedRedraw(this);
}

//______________________________________________________________________________

void KVLVContainer::ActivateItemWithColumnData(const Char_t* colname, Double_t data, Bool_t activ)
{
	KVLVColumnData* CD = fColData[((KVListView*)GetListView())->GetColumnNumber(colname)];
	Double_t val;
   TGFrameElement *el;
   TIter next(fList);TGLVEntry *f=0;
   while ((el = (TGFrameElement *) next())) {
      f = (TGLVEntry *) el->fFrame;
		CD->GetData( (TObject*)f->GetUserData(), val );
		if(val==data) {
         if(activ) ActivateItem(el);
			else DeActivateItem(el);
			break;
		}
   }
	fClient->NeedRedraw(this);
}

//______________________________________________________________________________

void KVLVContainer::OpenContextMenu(TGFrame* f,Int_t but,Int_t x,Int_t y)
{
	// Open context menu when user right-clicks an object in the list.
	// Calling AllowContextMenu(kFALSE) will disable this.

	if(!fAllowContextMenu) return;

	if(but == kButton3){
		TGLVEntry *el = (TGLVEntry*)f;
		TObject *ob = (TObject*)el->GetUserData();
		if(ob) {
			fContextMenu->Popup(x,y,ob);
		}
	}
}


//______________________________________________________________________________

void KVLVContainer::DoDoubleClick(TGFrame* f,Int_t but,Int_t x,Int_t y)
{
	// Perform 'default' action when user double-left-clicks an object in the list.
	// By default, this calls the Browse(TBrowser*) method of the object (defined for TObject,
	// overridden in child classes).
   // If SetDoubleClickAction() was called, the DoubleClickAction(TObject*) signal
   // will be emitted with the address of the selected object.
	// Calling AllowDoubleClick(kFALSE) will disable this.

	if(!fAllowDoubleClick) return;
	
	if(but == kButton1){
		TGLVEntry *el = (TGLVEntry*)f;
		TObject *ob = (TObject*)el->GetUserData();
		if(ob) {
         if(fUserDoubleClickAction) DoubleClickAction(ob);
         else ob->Browse(0);
      }
	}
}

//______________________________________________________________________________

TObject* KVLVContainer::GetFirstInList()
{
    // Returns first object in currently displayed list

    TGFrameElement* f = (TGFrameElement*)fList->First();
    if(f){
        KVLVEntry* l = (KVLVEntry*)f->fFrame;
        if(l){
            return (TObject*)l->GetUserData();
        }
    }
    return 0;
}

//______________________________________________________________________________

TObject* KVLVContainer::GetLastInList()
{
    // Returns last object in currently displayed list

    TGFrameElement* f = (TGFrameElement*)fList->Last();
    if(f){
        KVLVEntry* l = (KVLVEntry*)f->fFrame;
        if(l){
            return (TObject*)l->GetUserData();
        }
    }
    return 0;
}

//______________________________________________________________________________

TList* KVLVContainer::GetSelectedItems()
{
	// Create and fill list with all currently selected items (KVLVEntry objects)
	// USER MUST DELETE TLIST AFTER USE!!!

   TGFrameElement *el;
   TIter next(fList);
   TList *ret = new TList();

   while ((el = (TGFrameElement *) next())) {
      if (el->fFrame->IsActive()) {
         ret->Add(el->fFrame);
      }
   }
   return ret;
}

//______________________________________________________________________________


TList* KVLVContainer::GetSelectedObjects()
{
	// Create and fill list with all currently selected objects (derived from TObject)
	// USER MUST DELETE TLIST AFTER USE!!!

   TGFrameElement *el;
   TIter next(fList);
   TList *ret = new TList();

   while ((el = (TGFrameElement *) next())) {
      if (el->fFrame->IsActive()) {
         ret->Add( (TObject*)((KVLVEntry*)el->fFrame)->GetUserData() );
      }
   }
   return ret;
}

//______________________________________________________________________________

void KVLVContainer::SetDoubleClickAction(const char* receiver_class, void* receiver, const char* slot)
{
   // Overrides the default 'double-click' action.
   // By default, double-clicking on an object in the list will call the Browse(TBrowser*)
   // method of the selected object.
   // Use this method to override this behaviour.
   // When an object is double-clicked the method 'slot' of the object 'receiver' of class
   // 'receiver_class' will be called. The method in question must have the signature
   //       receiver_class::slot(TObject*)
   // The address of the selected (T)object is passed as argument.
   
   Connect("DoubleClickAction(TObject*)", receiver_class, receiver, slot);
   fUserDoubleClickAction = kTRUE;
}
   
void KVLVContainer::DoubleClickAction(TObject*obj)
{
   Emit("DoubleClickAction(TObject*)", (Long_t)obj);
}
