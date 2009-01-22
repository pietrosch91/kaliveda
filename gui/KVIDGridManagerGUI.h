#include "TGFrame.h"
#include "TGMenu.h"
#include "TGListBox.h"
#include "TGButtonGroup.h"
#include "TGButton.h"
#include "KVIDGrid.h"
#include "RQ_OBJECT.h"
#include <TGComboBox.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGTextEntry.h>
#include <TGProgressBar.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// KVIDGridManagerGUI
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class KVIDGridManagerGUI:public TGMainFrame {

   //TGMainFrame* fMain;
   TGPopupMenu *fMenuFile;
   TGPopupMenu *fMenuHelp;
   enum {
      M_FILE_NEW,
      M_FILE_OPEN,
      M_FILE_SAVE,
      M_FILE_SAVEAS,
      M_FILE_CLOSE,
      M_FILE_REFRESH,
      M_FILE_CLEAR,
      M_FILE_EXIT,
      M_HELP_ABOUT,
      ID_LIST_BOX
   };
   TGLayoutHints *fMenuBarItemLayout;
   TGLayoutHints *fMenuBarHelpLayout;
   TGMenuBar *fMenuBar;
   TGHorizontalFrame *fHframe;
   TGVButtonGroup *fButtonsFrame;
   TGListBox *fIDGridList;
   TGTextButton *fNewGrid;
   TGTextButton *fDrawGrid;
   TGTextButton *fUnDrawGrid;
   TGTextButton *fClearGrid;
   TGTextButton *fCopyGrid;
   TGTextButton *fMergeGrids;
   TGTextButton *fCalculateGrid;
   TGTextButton *fModifyGrid;
   TGTextButton *fFitGrid;
   TGTextButton *fTestGrid;
   TGTextButton *fDeleteGrid;
   Int_t fFirstGrid;
   Int_t fLastGrid;
   KVIDGrid *fSelectedGrid;
   TString fFileName;
   TList *fSelectedEntries;

 public:

    KVIDGridManagerGUI();
    virtual ~ KVIDGridManagerGUI();

   void CloseWindow();

   void DisableGridButtons();
   void EnableGridButtons();

   void HandleFileMenu(Int_t id);

   void FillListOfGrids();
   void ClearListOfGrids();
   void UpdateListOfGrids();

   void SelectionChanged();
   Int_t GetNSelected();

   void DrawGrid();
   void UnDrawGrid();
   void NewGrid();
   void MergeGrids();
   void ModifyGrid();
   void FitGrid() {
   };
   void ClearGrid();
   void CalculateGrid();
   void CopyGrid();
   void TestGrid();
   void DeleteGrid();

   void SaveAs();

   ClassDef(KVIDGridManagerGUI, 0)      //A GUI for managing identification grids
};
