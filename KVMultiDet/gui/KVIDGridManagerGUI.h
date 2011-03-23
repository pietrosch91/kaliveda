#include "TGFrame.h"
#include "TGMenu.h"
#include "KVListView.h"
#include "TGButtonGroup.h"
#include "TGButton.h"
#include "KVIDGrid.h"
#include "RQ_OBJECT.h"
#include <TGComboBox.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGTextEntry.h>
#include <TGProgressBar.h>
#include "TGTab.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// KVIDGridManagerGUI
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class KVIDGridManagerGUI:public TGMainFrame {

   //TGMainFrame* fMain;
   TGPopupMenu *fMenuFile;
   TGPopupMenu *fMenuHelp;
   enum {
		M_GRIDS_NEW,
		M_GRIDS_READ,
		M_GRIDS_SAVE_SEL,
		M_GRIDS_SAVE_TAB,
		M_GRIDS_SAVE_ALL,
		M_GRIDS_DEL_SEL,
		M_GRIDS_DEL_TAB,
		M_GRIDS_DEL_ALL,
		M_GRIDS_RUNLIST,
      ID_LIST_BOX
   };
   TGLayoutHints *fMenuBarItemLayout;
   TGLayoutHints *fMenuBarHelpLayout;
   TGMenuBar *fMenuBar;
   TGHorizontalFrame *fHframe;
	TGTab *fGridListTabs;//tabs with lists of grids
	KVListView *fIDGridList;//list of grids in current tab
	KVListView *fIDLineList;//list of lines in currently selected grid
	KVListView *fCUTLineList;//list of lines in currently selected grid
	KVListView *fCUTContourList;//list of lines in currently selected grid
   Int_t fFirstGrid;
   Int_t fLastGrid;
   KVIDGraph *fSelectedGrid;
   KVIDGraph *fLastSelectedGrid;
   TString fFileName;
   TList *fSelectedEntries;

	TList* GetAllGridsInTab()
	{
		// list of all grids (selected or not) in current tab
		if(fIDGridList) return fIDGridList->GetUserItems();
		return 0;
	};

 public:

    KVIDGridManagerGUI();
    virtual ~ KVIDGridManagerGUI();

   void CloseWindow();

   void HandleGridsMenu(Int_t id);

   void CreateAndFillTabs();
   void UpdateTabs();
   void RemoveEmptyTabs();
	void TabSelect(Int_t);
   void UpdateListOfGrids();
   
   void ShowListOfLines();
   void UpdateListOfLines();

   void SelectionChanged();
   Int_t GetNSelected();

   void NewGrid();
   void MergeGrids();
   void ClearGrid();
   void DeleteGrids();
   void DeleteAllGridsInTab();

   void SaveAs(const TList* /*selected*/ = 0);

   ClassDef(KVIDGridManagerGUI, 0)      //A GUI for managing identification grids
};
