#include "KVIDGridManagerGUI.h"
#include "Riostream.h"
#include "KVIDGridManager.h"
#include "KVIDGrid.h"
#include "TGFileDialog.h"
#include "TPad.h"
#include <TGMsgBox.h>
#include <TTimer.h>
#include <TClass.h>
#include <TROOT.h>
#include <TGLayout.h>
#include <TCanvas.h>
#include <KVNewGridDialog.h>
#include <KVTestIDGridDialog.h>
#include <KVCalculateChIoSiGridDialog.h>
#include <KVMergeGridsDialog.h>
#include "KVConfig.h"
#include <KVIDGUITelescopeChooserDialog.h>
#include <KVDropDownDialog.h>
#include "TEnv.h"
#include "KVInputDialog.h"
#include "KVIDCutLine.h"
#include "KVIDCutContour.h"
#include "KVVirtualIDFitter.h"

ClassImp(KVIDGridManagerGUI)
//////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDGridManagerGUI, KVNewGridDialog, KVCalculateChIoSiGridDialog, KVTestIDGridDialog</h2>

<h4>Graphical interface tool for managing, creating, testing and fitting identification grids.</h4>

 <h2>KVIDGridManagerGUI</h2>
 <h4><a name="fig1"></a></h4>
 <img alt="" src="images/KVIDGridManagerGUI.gif"
 style="width: 538px; height: 327px;"><br>
 <h4>Fig 1 : the main window of the ID Grid Manager</h4>
 <p>Launch the ID Grid Manager in an interactive ROOT session using the
 command<br>
 </p>
 <div style="margin-left: 40px;"><code>gIDGridManager-&gt;StartViewer()</code><br>
 <br>
 </div>
 The window shown in <a href="#fig1">Fig 1</a> will appear. <br>
 The list on the right gives the list of all identification grids
 currently in memory.<br>
 Click on the name of a grid to select it.&nbsp; Click again and it is
 deselected. Click on several grid names to make a multiple selection.<br>
 <br>
 The buttons in the "Grid" frame on the left of the window allow to
 perform different actions on the selected grid or grids.<br>
 When no grids are selected, only the "New..." button is active.<br>
 Selecting one or more grids will activate/deactivate other buttons.<br>
 <br>
 The "File" menu allows to open a grid file, save the current grids in a
 file, refresh the list of current grids, delete all the current grids
 from memory, or close the ID Grid Manager window.<br>
 <br>
 <h2>Creating a new grid</h2>
 Clicking the "New..." button opens the following dialogue box shown in
 Fig 2:<br>
 <br>
 <h4><a name="fig2"></a></h4>
 <img alt="" src="images/NewGridDialog.gif"
 style="width: 487px; height: 276px;"><br>
 <h4>Fig 2 : the grid creation dialogue box</h4>
 At the top is a drop-down list allowing to select the type of
 identification grid you want to create.<br>
 The possible types are:<br>
 <ul>
 <li>KVIDZGrid - a grid for identifying particles solely according to
 their atomic number, Z</li>
 <li>KVIDZAGrid - a grid for identifying particles according to Z and
 their mass number, A</li>
 <li>KVIDGChIoSi - a Z identification grid specifically used with
 Ionisation chamber (dE) - Silicon (E) telescopes</li>
 <li>KVIDGCsI - a Z and A identification grid specifically used for
 Caesium Iodide fast-slow identification maps<br>
 </li>
 </ul>
 You must choose the correct type of grid: this is the only grid
 characteristic which cannot be changed later.<br>
 <br>
 The other grid parameters are optional. Clicking a selection box next
 to one of them will activate/deactivate it.<br>
 <ul>
 <li>Set runs - use this to specify for which data the grid is
 applicable. This information will be used in order to set the correct
 grids for each identification telescope on a run by run basis when data
 is analysed.</li>
 <li>Set rings - specify a range of rings for which the grid is valid.
 This allows to associate the grid with the correct identification
 telescopes.</li>
 <li>Set mods - specify a range of module numbers for which the grid
 is valid. This allows to associate the grid with the correct
 identification telescopes.</li>
 <li>Scaling - specify scaling factors for the X- and Y-axes of the
 grid. These can be set later (see <a href="#modify_grid">Modifying a
 grid</a>) in order to fit the grid to data.</li>
 </ul>
 Once you have made your choice, click the "Create" button to create the
 grid. It will be added at the end of the list in the main window.<br>
 <br>
 <h2>Drawing a grid</h2>
 Select a grid from the list in the main window (<a href="#fig1">Fig 1</a>).
 Click the "Draw" button and it will appear in the currently active pad
 or canvas. If this pad contains a data histogram, the grid will be
 superimposed on the data. If no canvas exists, a new one will be
 created and the axes of the picture will be adjusted to display the
 whole grid.<br>
 <br>
 Identification lines are drawn in black. "OK" lines (used to decide if
 a point in an ID map is identifiable or not) are drawn in red. Here is
 an example for a CsI R-L grid:<br>
 <a name="fig2b"></a><br>
 <img alt="" src="images/CsIGrid.gif"
 style="width: 740px; height: 686px;"><br>
 <h4>Fig 2b: example of a CsI R-L grid superimposed on data from the
 corresponding detector</h4>
 <br>
 Note that selecting several grids deactivates the "Draw" button. Only
 one grid can be visualised at a time.<br>
 <br>
 To remove a grid from the current display (active pad or canvas), click
 the "Undraw" button in the main window. You must first select the pad
 or canvas in which the grid is displayed.<br>
 <br>
 <h2>Copying a grid</h2>
 To make a new grid by copying an existing grid, select a grid from the
 list in the main window (<a href="#fig1">Fig 1</a>) and click the
 "Copy..." button.<br>
 <a name="fig3"></a><br>
 <img alt="" src="images/CopyGridDialog.gif"
 style="width: 487px; height: 237px;"><br>
 <h4>Fig 3 : copy grid dialogue box</h4>
 The grid properties are displayed in the copy grid dialogue box (<a
 href="#fig3">Fig 3</a>). Modify them as desired, and then click the
 "Copy" button to create the new grid. It will be added at the end of
 the list in the main window.<br>
 <br>
 <h2><a name="modify_grid"></a>Modifying a grid</h2>
 To modify an existing grid, select a grid from the list in the main
 window (<a href="#fig1">Fig 1</a>) and click the "Edit..." button.<br>
 <a name="fig4"></a><br>
 <img alt="" src="images/EditGridDialog.gif"
 style="width: 487px; height: 529px;"><br>
 <h4>Fig 4 : grid editing dialogue box</h4>
 The grid properties are displayed in the upper half of the edit grid
 dialogue box (<a href="#fig4">Fig 4</a>). Modify them as desired and
 then click the "Apply" button to change the grid. If the grid is
 currently displayed in a canvas or pad, the change will be visible
 straight away. You can continue changing properties until you are
 satisfied with the result, and then click "OK" or "Cancel" to close the
 editing window.<br>
 <br>
 In the lower half of the window are the lists of all lines contained in
 the grid, separated between the "OK" lines (used to decide if a point
 in an ID map is identifiable or not) and the "ID" lines (actual
 identification lines). Selecting a line will make it change colour if
 the grid is currently displayed. You can then use the mouse to modify the
 line. You can modify line properties such as Z, A, name, etc. using its context menu.<br>
 <br>
 <h2>Adding a line to a grid</h2>
 In order to add lines to a grid, select the grid in the <a href="#fig1">main
 window</a> and click the "Edit" button. The grid editing dialog box (<a
 href="#fig4">Fig 4</a>) opens. In the lower half of this window is a
 zone entitled "New line". <br>
 <ul>
 <li>Choose which type of line you want to add: "ID" for an
 identification line, "OK" for lines used to decide if a point in an ID
 map is identifiable or not. <br>
 </li>
 <li>Choose the class of line you want to add in the pull-down list: <a
 href="KVIDLine.html">KVIDLine</a> (only used for "OK" lines), <a
 href="KVIDZLine.html">KVIDZLine</a> (Z-identification line), <a
 href="KVIDZALine.html">KVIDZALine</a> (Z &amp; A identification), or <a
 href="KVIDCsIRLLine.html">KVIDCsIRLLine</a> (special Z &amp; A
 identification line for CsI R-L grids).</li>
 <li>Click the "Add" button and then draw a new line in the currently
 active pad/canvas: click with the left mouse button to set each point
 of the line, then double-click the left button to set the last point.
 After you double-click the last point, the line should change thickness
 and small open circles should appear at each point.</li>
 <li>Right-click on the line you just drew to open its contextual
 menu. Use the menu to modify the line's Z and/or A (identification
 lines), or its name ("OK" lines).</li>
 <li>Click the "Update" button in order to see your new line appear in
 the "ID" or "OK" list.<br>
 </li>
 </ul>
 <br>
 <h2>Deleting all lines in a grid</h2>
 To delete all existing lines in a grid, select a grid from the list in
 the main window (<a href="#fig1">Fig 1</a>) and click the "Clear"
 button. Note that this will also reset any scaling factors associated
 with the grid.<br>
 <br>
 <h2>Generating a grid from calculated energy losses</h2>
 To fill a grid with lines generated using calculated energy losses,
 select a grid from the list in the main window (<a href="#fig1">Fig 1</a>)
 and click the "Calculate..." button.<br>
 Note that this option is only available for KVIDGChIoSi grids. The
 values of "ring min" and "mod min" (see Figs <a href="#fig2">2</a>, <a
 href="#fig3">3</a>, <a href="#fig4">4</a>) you set for the grid will
 be used to determine which ChIo-Si telescope of INDRA will be used to
 calculate the energy losses. Any existing lines in the grid will be
 destroyed.<br>
 <a name="fig5"></a><br>
 <img alt="" src="images/CalculateGridDialog.gif"
 style="width: 290px; height: 238px;"><br>
 <h4>Fig 5 : grid generation using energy losses dialogue box</h4>
 The dialogue box shown in <a href="#fig5">Fig 5</a> appears.<br>
 You can choose the maximum atomic number Zmax for which a line will be
 calculated.<br>
 The three following parameters are optional :<br>
 <ul>
 <li>Change ChIo pressure - by default the current gas pressure of the
 ionisation chamber is used. By selecting this option, you can adjust
 the pressure to best fit the data.</li>
 <li>Simulate Silicon PHD - if ticked, the pulse height deficit of
 the first silicon detector among the ranges of ring/mod you set for the grid will be taken into account in the
 calculation of the lines, the effective energy loss is reduced
 according to the Z and energy for each calculated point.</li>
 <li>Mass formula - you can choose which formula is used to calculate the A of each
 ion used to calculate the energy loss</li>
 <li>Number of points/line - the default number of points calculated
 per line is 20. Note that points are more closely spaced at low energy
 (where the form of the lines evolves fastest) than at high energy.</li>
 </ul>
 Choose the parameters you require and then click the "Apply" button to
 generate the lines. The grid will be drawn in the currently active
 canvas or pad. You can modify the parameters and recalculate the lines
 until you are satisfied with the result, and then click "OK" or
 "Cancel" to close the window.<br>
 <br>
 <h2>Testing an identification grid</h2>
 Select a grid from the list in the main window and click the "Test..."
 button (<a href="#fig1">Fig 1</a>). The following dialogue box will
 open:<br>
 <br>
 <img alt="" src="images/TestGridDialog.gif"
 style="width: 357px; height: 388px;"><br>
 <h4>Fig 6: testing an identification grid</h4>
 You can use this feature to rapidly test the identification capacity of
 a grid. A 2D-histogram containing data corresponding to an
 identification map is scanned and the identification corresponding to
 each point is used to fill some control histograms.<br>
 <br>
 The first requirement is a 2D histogram containing the data you want to
 use for the test. The X- and Y-axes of this histogram should correspond
 to the axes of the identification grid. By default, we take data from a
 histogram called "htemp" (this is the default name for histograms
 generated by the TTreeViewer). Replace "htemp" by the name of your data
 histogram if necessary.<br>
 <br>
 In the "ID test histo" box you define the characteristics of a one
 dimensional histogram which will be filled with the distribution of the
 PID variable obtained from the data. For Z-only identification, the PID
 is the "real" Z value. For A &amp; Z identification, the PID is
 Z+0.2(A-2Z). You can keep the default values or change them if required.<br>
 <br>
 In the "ID vs. E histo" box you define a 2D histogram which will be
 filled with PID vs. residual energy (the value plotted on the X-axis of
 your data histogram). <br>
 <br>
 Click the "Test" button to start the identification. For each bin in
 the data histogram, we generate random (x,y) coordinates within the
 limits of the cell, as many as the bin contents. For each coordinate
 pair, the IsIdentifiable(x,y) method of the grid is used to decide
 whether or not to call the Identify(x,y) method. The histograms are
 filled with results. The progress bar at the bottom of the dialogue box
 indicates the status of the test. While the test is progressing, you
 can use a TBrowser to locate the test histograms (they are created in
 the current directory) and plot them. Here is an example of the two
 resulting histograms for the CsI R-L data shown in <a href="#fig2b">Fig
 2b</a>:<br>
 <br>
 <img alt="" src="images/TestIDExample.gif"
 style="width: 677px; height: 684px;"><br>
<!-- */
// --> END_HTML
//////////////////////////////////////////////////////////
KVIDGridManagerGUI::KVIDGridManagerGUI(): TGMainFrame(gClient->GetRoot(), 500,
                                                         300)
{
   fFirstGrid = 0;
   fLastGrid = -1;
   fSelectedGrid = 0;
   fSelectedEntries = 0;
   fLastSelectedGrid = 0;

   //to have access to online KaliVeda documentation via context menus
   //and dialog box "Online Help" buttons
   gEnv->SetValue("Browser.StartUrl", Form("http://indra.in2p3.fr/KaliVedaDoc/%s/", KVBase::GetKVVersion()));

   //any change of ID grid manager causes UpdateListOfGrids to be called
   gIDGridManager->Connect("Modified()", "KVIDGridManagerGUI", this,
                           "UpdateListOfGrids()");

   //create new manager GUI

   /**************** GRIDS popup menu *****************/
   fMenuFile = new TGPopupMenu(gClient->GetRoot());
   //fMenuFile->AddEntry("&New grid...", M_GRIDS_NEW);
   //fMenuFile->AddSeparator();
   //fMenuFile->AddEntry("&Read grids...", M_GRIDS_READ);

   /* cascading "Set..." menu... */
   TGPopupMenu * sgm = new TGPopupMenu(gClient->GetRoot());
   sgm->AddEntry("List of Runs", M_GRIDS_RUNLIST);
// sgm->AddEntry("...in tab", M_GRIDS_DEL_TAB);
// sgm->AddEntry("...all", M_GRIDS_DEL_ALL);
   fMenuFile->AddPopup("Set...", sgm);
   /* cascading "Save grids" menu... */
   sgm = new TGPopupMenu(gClient->GetRoot());
   sgm->AddEntry("Selected", M_GRIDS_SAVE_SEL);
   sgm->AddEntry("Tab", M_GRIDS_SAVE_TAB);
   sgm->AddEntry("All", M_GRIDS_SAVE_ALL);
   fMenuFile->AddPopup("Save...", sgm);

   fMenuFile->AddSeparator();
   /* cascading "Delete grids" menu... */
   sgm = new TGPopupMenu(gClient->GetRoot());
   sgm->AddEntry("Selected", M_GRIDS_DEL_SEL);
   sgm->AddEntry("Tab", M_GRIDS_DEL_TAB);
   sgm->AddEntry("All", M_GRIDS_DEL_ALL);
   fMenuFile->AddPopup("Delete...", sgm);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Quit", M_QUIT);

   fMenuFile->Connect("Activated(Int_t)", "KVIDGridManagerGUI", this,
                      "HandleGridsMenu(Int_t)");
   /**************** HELP popup menu *****************/
//   fMenuHelp = new TGPopupMenu(gClient->GetRoot());
//   fMenuHelp->AddEntry("About...", M_HELP_ABOUT);
   /******************MENUBAR*********************/
   fMenuBarItemLayout =
      new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
//   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
//   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   //add to main window
   AddFrame(fMenuBar,
            new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0,
                              0, 1, 1));
// adding a horizontal line as a separator
   TGHorizontal3DLine *lh = new TGHorizontal3DLine(this);
   AddFrame(lh, new TGLayoutHints(kLHintsTop | kLHintsExpandX));

///////////////////////////*   TOOLBAR *////////////////////////////////
// toolbar icon files
   const char *xpms[] = {
      "ofolder_t.xpm", // open
      "filesaveas.xpm",  // save
      "profile_t.xpm",
      "line.xpm",
      "ellipse.xpm",
      "cut.xpm",
      "selection_t.xpm",
      "h1_t.xpm",
      "branch_t.xpm",
      "sm_delete.xpm",
//      "quit.xpm",
      0
   };
// toolbar tool tip text
   const char *tips[] = {
      "Open file containing grids",
      "Save all grids in current file",
      "New grid",
      "Add identification line",
      "Add identification contour",
      "Add cut",
      "Fit grid",
      "Test grid identification",
      "Test grid (TTree)",
      "Delete selected grid(s)",
//      "Quit",
      0
   };
   int spacing[] = {
      5,
      0,
      20,
      0,
      0,
      0,
      10,
      0,
      0,
      10,
 //     1000,
      0
   };
   TGButton** buttons[] = {
      &fTBOpen,
      &fTBSave,
      &fTBNewG,
      &fTBNewIDL,
      &fTBNewIDC,
      &fTBNewCut,
      &fTBFit,
      &fTBTest,
      &fTBTestTree,
      &fTBDelG,
//      &fTBQuit,
      0
   };
   const char* method[] = {
      "OpenFile()",
      "SaveCurrent()",
      "NewGrid()",
      "NewIDLine()",
      "NewIDContour()",
      "NewCut()",
      "FitGrid()",
      "TestGrid()",
      "TestTreeGrid()",
      "DeleteSelectedGrids()",
 //     "Quit()",
      0
   };
   fNbButtons = 0;
// structure containing toolbar button information
   ToolBarData_t t[50];
// creation of a toolbar object as a child of main frame
   fToolBar = new TGToolBar(this, 520, 80);
   int i = 0;
   while (xpms[i]) {
      t[i].fPixmap = xpms[i];
      t[i].fTipText = tips[i];
      t[i].fStayDown = kFALSE;
      t[i].fId = i + 1;
      t[i].fButton = NULL;
      *buttons[i] = fToolBar->AddButton(this, &t[i], spacing[i]);
      (*buttons[i])->Connect("Clicked()", "KVIDGridManagerGUI", this, method[i]);
      fTBbuttons[i] = *buttons[i];
      fNbButtons++;
      i++;
   }
   // 'new id line'  & 'new cut' button stays down until line is drawn
   fTBNewIDL->AllowStayDown(kTRUE);
   fTBNewCut->AllowStayDown(kTRUE);

// adding the tool bar to the main frame
   AddFrame(fToolBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
///////////////////////////*  END TOOLBAR *////////////////////////////////

// adding a horizontal line as a separator
   lh = new TGHorizontal3DLine(this);
   AddFrame(lh, new TGLayoutHints(kLHintsTop | kLHintsExpandX));

//////////////////////////*STATUS BAR*////////////////////////////////:
   fStatusBar = new TGStatusBar(this);
   AddFrame(fStatusBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
//////////////////////////*STATUS BAR*////////////////////////////////:

   fHframe = new TGHorizontalFrame(this, 10, 10);

   // Tabs for lists of grids. Each tab holds list of grids for a given
   // type of ID telescope.
   fGridListTabs = new TGTab(fHframe, 550, 400);
   fGridListTabs->Connect("Selected(Int_t)", "KVIDGridManagerGUI", this, "TabSelect(Int_t)");
   fIDGridList = 0;
   //initialise tabs with lists of grids
   CreateAndFillTabs();

   fHframe->AddFrame(fGridListTabs,
                     new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10));

   TGVerticalFrame* line_frame = new TGVerticalFrame(fHframe, 350, 400);

   TGLabel* lab1 = new TGLabel(line_frame, "CURRENT GRID IDENTIFIERS");
   line_frame->AddFrame(lab1, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 2, 2, 10, 2));

   // list view for lines in current grid
   fIDLineList = new KVListView(KVIDentifier::Class(), line_frame, 350, 400);
   fIDLineList->SetDataColumns(5);
   fIDLineList->SetDataColumn(0, "Name", "", kTextLeft);
   fIDLineList->SetDataColumn(1, "Z", "", kTextCenterX);
   fIDLineList->SetDataColumn(2, "A", "", kTextCenterX);
   fIDLineList->SetDataColumn(3, "OnlyZId", "OnlyZId", kTextCenterX);
   fIDLineList->SetDataColumn(4, "MassFormula", "", kTextCenterX);
   fIDLineList->GetDataColumn(3)->SetIsBoolean();
   fIDLineList->ActivateSortButtons();
   fIDLineList->AllowBrowse(kFALSE);
   //fIDLineList->Connect("SelectionChanged()", "KVIDGridManagerGUI", this,
   //                    "SelectionChanged()");
   line_frame->AddFrame(fIDLineList, new TGLayoutHints(kLHintsTop | kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 10));

   lab1 = new TGLabel(line_frame, "CUT LINES");
   line_frame->AddFrame(lab1, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 2, 2, 2, 2));

   fCUTLineList = new KVListView(KVIDCutLine::Class(), line_frame, 350, 150);
   fCUTLineList->SetDataColumns(3);
   fCUTLineList->SetDataColumn(0, "Name", "", kTextLeft);
   fCUTLineList->SetDataColumn(1, "# Points", "GetN", kTextCenterX);
   fCUTLineList->SetDataColumn(2, "Direction", "GetAcceptedDirection", kTextCenterX);
   fCUTLineList->ActivateSortButtons();
   fCUTLineList->AllowBrowse(kFALSE);
   //fIDLineList->Connect("SelectionChanged()", "KVIDGridManagerGUI", this,
   //                    "SelectionChanged()");
   line_frame->AddFrame(fCUTLineList, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

   lab1 = new TGLabel(line_frame, "CUT CONTOURS");
   line_frame->AddFrame(lab1, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 2, 2, 2, 2));

   fCUTContourList = new KVListView(KVIDCutContour::Class(), line_frame, 350, 150);
   fCUTContourList->SetDataColumns(3);
   fCUTContourList->SetDataColumn(0, "Name", "", kTextLeft);
   fCUTContourList->SetDataColumn(1, "# Points", "GetN", kTextCenterX);
   fCUTContourList->SetDataColumn(2, "Exclusive", "IsExclusive", kTextCenterX);
   fCUTContourList->GetDataColumn(2)->SetIsBoolean();
   fCUTContourList->ActivateSortButtons();
   fCUTContourList->AllowBrowse(kFALSE);
   //fIDLineList->Connect("SelectionChanged()", "KVIDGridManagerGUI", this,
   //                    "SelectionChanged()");
   line_frame->AddFrame(fCUTContourList, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 10));

   fHframe->AddFrame(line_frame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandY, 20, 20, 20, 20));

   AddFrame(fHframe,
            new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0,
                              0));

   SetWindowName("ID Grid Manager");

   //layout & draw window
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
   SetWMSize(1200, 600);

   // first tab is visible, but TabSelect(0) is not called automatically
   TabSelect(0);

}

KVIDGridManagerGUI::~KVIDGridManagerGUI()
{
   //close window
   UnmapWindow();
   gIDGridManager->Disconnect("Modified()", this, "UpdateListOfGrids()");
   if (fSelectedEntries) delete fSelectedEntries;
   fSelectedEntries = 0;
}

void KVIDGridManagerGUI::CloseWindow()
{
   //close viewer
   DeleteWindow();
}


void KVIDGridManagerGUI::HandleGridsMenu(Int_t id)
{
   //Receive signals emitted by items selected in Grids menu
   switch (id) {

      case M_QUIT:

         Quit();
         break;

      case M_GRIDS_NEW:

         cout << "Add new grid" << endl;
         NewGrid();
         break;
      case M_GRIDS_READ:
         OpenFile();
         break;

      case M_GRIDS_SAVE_SEL:
         // save current selection of grids in file
         SaveAs(fSelectedEntries);
         break;

      case M_GRIDS_SAVE_TAB:
         // save all grids in current tab in file
         SaveAs(GetAllGridsInTab());
         break;

      case M_GRIDS_SAVE_ALL:

         //save all grids in file - ask user to confirm or change filename/path
         SaveAs();
         break;

      case M_GRIDS_DEL_SEL:
         DeleteSelectedGrids();
         break;

      case M_GRIDS_DEL_TAB:
         //warning message and confirmation
      {
         Int_t ret_val;
         new TGMsgBox(fClient->GetDefaultRoot(), this, "ID Grid Manager",
                      "This will delete all grids in the current tab. Are you sure ?",
                      kMBIconExclamation, kMBOk | kMBCancel, &ret_val);
         if (ret_val & kMBOk) {
            DeleteAllGridsInTab();
         }
      }
      break;
      case M_GRIDS_DEL_ALL:

         //warning message and confirmation
      {
         Int_t ret_val;
         new TGMsgBox(fClient->GetDefaultRoot(), this, "ID Grid Manager",
                      "This will delete all grids. Are you sure ?",
                      kMBIconExclamation, kMBOk | kMBCancel, &ret_val);
         if (ret_val & kMBOk) {
            gIDGridManager->Clear();
            //UpdateListOfGrids();
            ///reset filename to avoid hitting Save and overwriting previous file
            //with an empty file !!
            fFileName = "";
         }
      }
      break;

      case M_GRIDS_RUNLIST:
         // set runlist for all selected grids in tab
      {
         if (!fSelectedGrid) break; // must have selected at least one grid
         TString runs = fSelectedGrid->GetRunList(); // fill dialog box with current runlist of selected grid
         Bool_t ok_pressed = kFALSE;
         new KVInputDialog(this, "Enter list of runs for grid(s):", &runs, &ok_pressed,
                           "Example: 1-10, 13, 22-657");
         if (!ok_pressed) break; // user pressed 'cancel' or otherwise closed the dialog
         TIter next(fSelectedEntries);
         KVIDGraph* entry;
         while ((entry = (KVIDGraph *) next())) {
            entry->SetRunList(runs.Data());
         }
      }
      break;

      default:

         break;
   }
}

void KVIDGridManagerGUI::DeleteSelectedGrids()
{
   //warning message and confirmation
   Int_t ret_val;
   new TGMsgBox(fClient->GetDefaultRoot(), this, "ID Grid Manager",
                "This will delete the selected grids. Are you sure ?",
                kMBIconExclamation, kMBOk | kMBCancel, &ret_val);
   if (ret_val & kMBOk) {
      DeleteGrids();
   }
}

void KVIDGridManagerGUI::OpenFile()
{
   static TString dir(".");
   const char *filetypes[] = {
      "ID Grid files", "*.dat",
      "All files", "*",
      0, 0
   };
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   fi.fIniDir = StrDup(dir);
   //printf("fIniDir = %s\n", fi.fIniDir);
   new TGFileDialog(fClient->GetDefaultRoot(), this, kFDOpen, &fi);
   if (fi.fFilename) {
      int ngri=gIDGridManager->GetGrids()->GetEntries();
      if (gIDGridManager->ReadAsciiFile(fi.fFilename)) {
         //read file ok no problem.
         int ngriread=gIDGridManager->GetGrids()->GetEntries()-ngri;
         SetStatus(Form("Read %d grids from file %s", ngriread, fi.fFilename));
         //set filename for Save
         fFileName = fi.fFilename;
      } else {
         new TGMsgBox(fClient->GetDefaultRoot(), this, "ID Grid Manager",
                      Form("Could not read file %s", fi.fFilename),
                      0, kMBOk);
      }
   }
   dir = fi.fIniDir;
}

void KVIDGridManagerGUI::UpdateListOfGrids()
{
   //remove all existing entries, then fill list from gIDGridManager
   //we update the current pad, in case the displayed grid no longer exists
   //grid buttons are disabled, as any selected grid is deselected

   UpdateTabs();
   //update all canvases
   TSeqCollection *Clist = gROOT->GetListOfCanvases();
   if (Clist && Clist->GetEntries()) {
      Clist->R__FOR_EACH(TCanvas, Modified)();
      Clist->R__FOR_EACH(TCanvas, Update)();
   }
}

void KVIDGridManagerGUI::SelectionChanged()
{
   //called any time the selection of grids changes
   //each time, we update:
   //fSelectedEntries : the list of all selected entries
   //GetNSelected() : the number of selected entries
   //fSelectedGrid : the last selected grid (=the only grid selected if GeTNSelected==1)

   //get number of selected items
   if (fSelectedEntries)
      delete fSelectedEntries;
   fSelectedEntries = fIDGridList->GetSelectedObjects();
   fSelectedGrid = (KVIDGraph*)fIDGridList->GetLastSelectedObject();
   if(!GetNSelected())fSelectedGrid=0x0;
   ShowListOfLines();
   //(de)activate toolbar buttons
   ActivateToolbarButtons();
   if(!GetNSelected()) SetStatus();
   else if(GetNSelected()==1) SetStatus(Form("Selected grid %s (%s)", fSelectedGrid->GetName(), fSelectedGrid->ClassName()));
   else SetStatus(Form("Selected %d grids, last selected grid %s (%s)", GetNSelected(), fSelectedGrid->GetName(), fSelectedGrid->ClassName()));
}

Int_t KVIDGridManagerGUI::GetNSelected()
{
   //returns current number of selected items in grid list
   return (fSelectedEntries ? fSelectedEntries->GetSize() : 0);
}

void KVIDGridManagerGUI::DeleteGrids()
{
   //delete the current selected grid, or all currently selected grids if there are more than one
   
   if (!GetNSelected()) return;
   
   TIter next(fSelectedEntries, kIterBackward);
   KVIDGraph* entry;
   while ((entry = (KVIDGraph *) next())) {
         gIDGridManager->DeleteGrid(entry, kFALSE);   //no update
   }
   if(fSelectedEntries) delete fSelectedEntries;
   fSelectedEntries = 0;
   fSelectedGrid = 0;
   UpdateListOfGrids();
}

void KVIDGridManagerGUI::DeleteAllGridsInTab()
{
   //delete the all grids in currently selected tab

   TIter next(GetAllGridsInTab(), kIterBackward);
   KVIDGraph* entry;
   while ((entry = (KVIDGraph *) next())) {
      gIDGridManager->DeleteGrid(entry, kFALSE);   //no update
   }
   if (fSelectedEntries) delete fSelectedEntries;
   fSelectedEntries = 0;
   fSelectedGrid = 0;
   UpdateListOfGrids();
}

void KVIDGridManagerGUI::ClearGrid()
{
   //deletes all lines in currently selected grid(s)

   if (GetNSelected() == 1)
      fSelectedGrid->Clear();
   else if (GetNSelected() > 1) {
      //multiselection
      TIter next(fSelectedEntries);
      KVIDGraph *grid;
      while ((grid = (KVIDGraph *) next())) {
         grid->Clear();
      }
   }
   //update all canvases
   TSeqCollection *Clist = gROOT->GetListOfCanvases();
   if (Clist->GetSize() > 0) {
      Clist->R__FOR_EACH(TCanvas, Modified)();
      Clist->R__FOR_EACH(TCanvas, Update)();
   }
}

void KVIDGridManagerGUI::NewGrid()
{
   // Create a new identification grid.
   // First we ask the user to select the identification telescope(s)
   // for which this grid will be used.
   // For a given type of ID telescope, several types of grid may be
   // applicable. If so, we ask the user to choose one.

   TList * telescopes = new TList;
   Bool_t cancel;
   new KVIDGUITelescopeChooserDialog(gMultiDetArray, telescopes, &cancel,
                                     fClient->GetDefaultRoot(), this);
   if (cancel || !telescopes->At(0)) {
      Info("NewGrid", "No ID telescopes chosen. Grid creation cancelled.");
      return;
   }
   // get default ID grid class of first ID telescope
   TString default_class = ((KVIDTelescope*)telescopes->At(0))->GetDefaultIDGridClass();
   // get list of possible choices of grid class = list of all plugin classes
   // defined for KVIDGraph
   TString choice = KVBase::GetListOfPlugins("KVIDGraph");
   // open dialog to choose ID grid class
   Bool_t ok_pressed = kFALSE;
   TString id_grid_class;
   new KVDropDownDialog(this, "Choose class for new grid:",
                        choice.Data(), default_class.Data(), &id_grid_class, &ok_pressed);
   if (!ok_pressed) {
      Info("NewGrid", "No ID grid class chosen. Grid creation cancelled.");
      return;
   }
   cout << "Make grid with class " << id_grid_class.Data() << endl;
   gIDGridManager->Disconnect("Modified()", this, "UpdateListOfGrids()");
   KVIDGraph *new_gr = KVIDGraph::MakeIDGraph(id_grid_class.Data());
   new_gr->AddIDTelescopes(telescopes);
   UpdateTabs();
   gIDGridManager->Connect("Modified()", "KVIDGridManagerGUI", this, "UpdateListOfGrids()");
   delete telescopes;
}

void KVIDGridManagerGUI::MergeGrids()
{
   //merge 2 grids
   //KVIDGraph *g1 = (KVIDGraph *) fSelectedEntries->At(1);
   //KVIDGraph *g2 = (KVIDGraph *) fSelectedEntries->First();
   //new KVMergeGridsDialog(g1, g2, fClient->GetDefaultRoot(), this, 10, 10);
}

void KVIDGridManagerGUI::SaveAs(const TList *selection)
{
   // Opens dialog to choose filename in which to save grids.
   // If selection=0 (default), all grids are saved
   // If selection!=0 only grids in list are saved

   static TString dir(".");
   const char *filetypes[] = {
      "ID Grid files", "*.dat",
      "All files", "*",
      0, 0
   };
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   fi.fIniDir = StrDup(dir);
   new TGFileDialog(fClient->GetDefaultRoot(), this, kFDSave, &fi);
   if (fi.fFilename) {
      //if no ".xxx" ending given, we add ".dat"
      TString filenam(fi.fFilename);
      if (!filenam.Contains('.'))
         filenam += ".dat";
      Int_t n_saved = gIDGridManager->WriteAsciiFile(filenam.Data(), selection);
      if (n_saved) {
         //wrote file no problem
         SetStatus( Form("Saved %d grids in file %s", n_saved, filenam.Data()) );
         //set file name for Save
         fFileName = filenam;
      } else {
         new TGMsgBox(fClient->GetDefaultRoot(), this, "ID Grid Manager",
                      Form("Could not write file %s", filenam.Data()), 0,
                      kMBOk);
      }
   }
   dir = fi.fIniDir;
}

void KVIDGridManagerGUI::CreateAndFillTabs()
{
   // create a tab for each type of ID telescope
   // put a list box for ID grid names on each tab

   KVString labels;
   if (gIDGridManager->GetGrids()->GetSize()) gIDGridManager->GetListOfIDTelescopeLabels(labels);
	if(labels==""){
		// no known idtelescopes referenced by grids (maybe we don't have a KVMultiDetArray?)
		// make 1 tab "Grids" and put them all in
      KVString lab = "Grids";
      TGCompositeFrame*cf = fGridListTabs->AddTab(lab.Data());
      cf->ChangeOptions(kVerticalFrame);
      fIDGridList = new KVListView(KVIDGraph::Class(), cf, 600, 400);
      fIDGridList->SetDataColumns(10);
      fIDGridList->SetDataColumn(0, "Name", "", kTextLeft);
      fIDGridList->SetDataColumn(1, "VarX", "", kTextLeft);
      fIDGridList->SetDataColumn(2, "VarY", "", kTextLeft);
      fIDGridList->SetDataColumn(3, "ID Telescopes", "GetNamesOfIDTelescopes", kTextLeft);
      fIDGridList->SetDataColumn(4, "RunList", "", kTextLeft);
      fIDGridList->SetDataColumn(5, "OnlyZId", "IsOnlyZId", kTextCenterX);
      fIDGridList->GetDataColumn(5)->SetIsBoolean();
      fIDGridList->SetDataColumn(6, "# Ident.", "GetNumberOfIdentifiers", kTextRight);
      fIDGridList->SetDataColumn(7, "# Cuts", "GetNumberOfCuts", kTextRight);
      fIDGridList->SetDataColumn(8, "X scaling", "GetXScaleFactor", kTextRight);
      fIDGridList->SetDataColumn(9, "Y scaling", "GetYScaleFactor", kTextRight);
      fIDGridList->ActivateSortButtons();
      fIDGridList->Connect("SelectionChanged()", "KVIDGridManagerGUI", this,
                           "SelectionChanged()");
      cf->AddFrame(fIDGridList, new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                                  kLHintsExpandX | kLHintsExpandY, 30,
                                                  10, 10, 10));
      KVList* grids = gIDGridManager->GetGrids();
      fIDGridList->Display(grids);
      return;
	}
   //loop over labels
   labels.Begin(",");
   while (! labels.End()) {
      KVString lab = labels.Next();
      TGCompositeFrame*cf = fGridListTabs->AddTab(lab.Data());
      cf->ChangeOptions(kVerticalFrame);
      fIDGridList = new KVListView(KVIDGraph::Class(), cf, 600, 400);
      fIDGridList->SetDataColumns(10);
      fIDGridList->SetDataColumn(0, "Name", "", kTextLeft);
      fIDGridList->SetDataColumn(1, "VarX", "", kTextLeft);
      fIDGridList->SetDataColumn(2, "VarY", "", kTextLeft);
      fIDGridList->SetDataColumn(3, "ID Telescopes", "GetNamesOfIDTelescopes", kTextLeft);
      fIDGridList->SetDataColumn(4, "RunList", "", kTextLeft);
      fIDGridList->SetDataColumn(5, "OnlyZId", "IsOnlyZId", kTextCenterX);
      fIDGridList->GetDataColumn(5)->SetIsBoolean();
      fIDGridList->SetDataColumn(6, "# Ident.", "GetNumberOfIdentifiers", kTextRight);
      fIDGridList->SetDataColumn(7, "# Cuts", "GetNumberOfCuts", kTextRight);
      fIDGridList->SetDataColumn(8, "X scaling", "GetXScaleFactor", kTextRight);
      fIDGridList->SetDataColumn(9, "Y scaling", "GetYScaleFactor", kTextRight);
      fIDGridList->ActivateSortButtons();
      fIDGridList->Connect("SelectionChanged()", "KVIDGridManagerGUI", this,
                           "SelectionChanged()");
      cf->AddFrame(fIDGridList, new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                                  kLHintsExpandX | kLHintsExpandY, 30,
                                                  10, 10, 10));
      KVList* grids = gIDGridManager->GetGridsForIDTelescope(lab);
      fIDGridList->Display(grids);
      delete grids;
   }

}

void KVIDGridManagerGUI::TabSelect(Int_t tab)
{
   //called when a new tab is selected

   TGCompositeFrame *cf = fGridListTabs->GetCurrentContainer();
   if (!cf) return;//there are no tabs
   TGFrameElement *el = (TGFrameElement*)cf->GetList()->At(0);
   fIDGridList = (KVListView*)el->fFrame;
   fIDGridList->SelectionChanged();
}

void KVIDGridManagerGUI::UpdateTabs()
{
   // create a tab for each type of ID telescope
   // put a list box for ID grid names on each tab

   KVString labels("[unknown]");
   if (gIDGridManager->GetGrids()->GetSize()) gIDGridManager->GetListOfIDTelescopeLabels(labels);
	if(labels==""){
		// no known idtelescopes referenced by grids (maybe we don't have a KVMultiDetArray?)
		// update "Grids" tab
         TGCompositeFrame*cf = fGridListTabs->GetTabContainer("Grids");
         if (!cf) {
            cout << "cf = 0x0 : label=Grids tab name=" <<
                 fGridListTabs->GetTabTab("Grids")->GetText()->GetString() << endl;
         } else {
            TGFrameElement *el = (TGFrameElement*)cf->GetList()->At(0);
            fIDGridList = (KVListView*)el->fFrame;
            KVList* grids = gIDGridManager->GetGrids();
            fIDGridList->Display(grids);
         }
   //make sure we are on the right tab
   Int_t ntabs = fGridListTabs->GetCurrent();
   TabSelect(ntabs);
         return;
	}
   //add any missing labels, update existing ones
   labels.Begin(",");
   while (! labels.End()) {
      KVString lab = labels.Next();

      if (!fGridListTabs->GetTabContainer(lab.Data())) { // new tab
         TGCompositeFrame*cf = fGridListTabs->AddTab(lab.Data());
         cf->ChangeOptions(kVerticalFrame);
         fIDGridList = new KVListView(KVIDGraph::Class(), cf, 600, 400);
         fIDGridList->SetDataColumns(10);
         fIDGridList->SetDataColumn(0, "Name", "", kTextLeft);
         fIDGridList->SetDataColumn(1, "VarX", "", kTextLeft);
         fIDGridList->SetDataColumn(2, "VarY", "", kTextLeft);
         fIDGridList->SetDataColumn(3, "ID Telescopes", "GetNamesOfIDTelescopes", kTextLeft);
         fIDGridList->SetDataColumn(4, "RunList", "", kTextLeft);
         fIDGridList->SetDataColumn(5, "OnlyZId", "IsOnlyZId", kTextCenterX);
         fIDGridList->GetDataColumn(5)->SetIsBoolean();
         fIDGridList->SetDataColumn(6, "# Ident.", "GetNumberOfIdentifiers", kTextRight);
         fIDGridList->SetDataColumn(7, "# Cuts", "GetNumberOfCuts", kTextRight);
         fIDGridList->SetDataColumn(8, "X scaling", "GetXScaleFactor", kTextRight);
         fIDGridList->SetDataColumn(9, "Y scaling", "GetYScaleFactor", kTextRight);
         fIDGridList->ActivateSortButtons();
         fIDGridList->Connect("SelectionChanged()", "KVIDGridManagerGUI", this,
                              "SelectionChanged()");
         cf->AddFrame(fIDGridList, new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                                     kLHintsExpandX | kLHintsExpandY, 30,
                                                     10, 10, 10));
         KVList* grids = gIDGridManager->GetGridsForIDTelescope(lab);
         fIDGridList->Display(grids);
         delete grids;
         fGridListTabs->MapSubwindows();
         fGridListTabs->Layout();
         fGridListTabs->SetTab(fGridListTabs->GetNumberOfTabs() - 1, kTRUE);
      } else { //existing tab
         TGCompositeFrame*cf = fGridListTabs->GetTabContainer(lab.Data());
         if (!cf) {
            cout << "cf = 0x0 : label=" << lab.Data() << " tab name=" <<
                 fGridListTabs->GetTabTab(lab.Data())->GetText()->GetString() << endl;
         } else {
            TGFrameElement *el = (TGFrameElement*)cf->GetList()->At(0);
            fIDGridList = (KVListView*)el->fFrame;
            KVList* grids = gIDGridManager->GetGridsForIDTelescope(lab);
            fIDGridList->Display(grids);
            delete grids;
         }
      }
   }
   //now check that none of the remaining tabs are empty & should be removed
   RemoveEmptyTabs();
   //make sure we are on the right tab
   Int_t ntabs = fGridListTabs->GetCurrent();
   TabSelect(ntabs);
}

void KVIDGridManagerGUI::RemoveEmptyTabs()
{
   // Recursively remove any empty tabs

   Int_t ntabs = fGridListTabs->GetNumberOfTabs();
   Bool_t recursive = kFALSE;
   for (Int_t itab = 0; itab < ntabs; itab++) {

      //get name of tab
      KVString lab = fGridListTabs->GetTabTab(itab)->GetString();
      //get grids for this tab (if any)
      KVList* grids = gIDGridManager->GetGridsForIDTelescope(lab);
      Int_t ngrids = grids->GetEntries();
      delete grids;
      if (!ngrids) {
         //empty tab! remove it!
         //delete the KVListView
         TGCompositeFrame*cf = fGridListTabs->GetTabContainer(itab);
         TGFrameElement *el = (TGFrameElement*)cf->GetList()->At(0);
         KVListView* lv = (KVListView*)el->fFrame;
         delete lv;
         //remove tab
         fGridListTabs->RemoveTab(itab, kFALSE);
         recursive = kTRUE; // call recursively
         break;//stop loop - tab numbers have changed
      }
   }
   if (recursive) RemoveEmptyTabs();
   fGridListTabs->MapSubwindows();
   fGridListTabs->Layout();
}

void KVIDGridManagerGUI::UpdateListOfLines()
{
   // called when a previously selected grid is modified
   // updates lists of lines

    if (!fLastSelectedGrid) return;

   KVList* ids = fLastSelectedGrid->GetIdentifiers();
   // sort lines in order of increasing Z
   ids->Sort();
   fIDLineList->Display(ids);
   KVSeqCollection* cutlines = fSelectedGrid->GetCuts()->GetSubListWithClass("KVIDCutLine");
   fCUTLineList->Display(cutlines);
   delete cutlines;
   cutlines = fSelectedGrid->GetCuts()->GetSubListWithClass("KVIDCutContour");
   fCUTContourList->Display(cutlines);
   delete cutlines;
}

void KVIDGridManagerGUI::ShowListOfLines()
{
   // Called when a grid is selected in list of grids
   // We fill list of all lines in grid
   //If only one grid is selected, we display its lines in the line list
   //If more than one grid is selected, we clear the line list
   
   if (GetNSelected() == 1) {
      KVList* ids = fSelectedGrid->GetIdentifiers();
      // sort lines in order of increasing Z
      ids->Sort();
      fIDLineList->Display(ids);
      KVSeqCollection* cutlines = fSelectedGrid->GetCuts()->GetSubListWithClass("KVIDCutLine");
      fCUTLineList->Display(cutlines);
      delete cutlines;
      cutlines = fSelectedGrid->GetCuts()->GetSubListWithClass("KVIDCutContour");
      fCUTContourList->Display(cutlines);
      delete cutlines;
      if (fLastSelectedGrid) {
         fLastSelectedGrid->Disconnect("Modified()", this, "UpdateListOfLines()");
      }
      fSelectedGrid->Connect("Modified()", "KVIDGridManagerGUI", this, "UpdateListOfLines()");
      fLastSelectedGrid = fSelectedGrid;
   } else {
      fIDLineList->RemoveAll();
      fCUTLineList->RemoveAll();
      fCUTContourList->RemoveAll();
      if (fLastSelectedGrid) {
         fLastSelectedGrid->Disconnect("Modified()", this, "UpdateListOfLines()");
         fLastSelectedGrid = 0;
      }
   }
}

void KVIDGridManagerGUI::ActivateToolbarButtons()
{
   // disable all buttons
   for (int i = 0; i < fNbButtons; i++) fTBbuttons[i]->SetEnabled(kFALSE);
   // enable 'open' & 'quit' & 'new grid"
   fTBOpen->SetEnabled();
   //fTBQuit->SetEnabled();
   fTBNewG->SetEnabled();
   // enable 'save' if there are grids
   if (gIDGridManager->GetGrids()->GetEntries()) fTBSave->SetEnabled();

   if (!GetNSelected()) return; // no grids selected

   //enable delete selected grid(s)
   fTBDelG->SetEnabled();

   if (GetNSelected() == 1) {
      // only one grid selected
      fTBNewIDL->SetEnabled();
      fTBFit->SetEnabled();
      fTBNewCut->SetEnabled();
      fTBTest->SetEnabled();
      fTBTestTree->SetEnabled();
   }
}

void KVIDGridManagerGUI::NewIDLine()
{
   // GUI method to draw a new identifier and add it to graph.
   // A dialog box with drop-down list pops up for the user to choose the class of the
   // new identifier, unless only one choice is possible, in which case it is used automatically.
   // For each KVIDGraph-derived class, the list of possible identifier classes and the
   // default class are define in .kvrootrc by the variables:
   //
   // [class_name].IDClass:  [id class 1]
   // +[class_name].IDClass:  [id class 2]
   // + ...
   // [class_name].DefaultIDClass:  [id class]

   if (!fSelectedGrid || GetNSelected() != 1) return;
   TString resname;
   resname.Form("%s.IDClass", fSelectedGrid->ClassName());
   TString cut_choices = gEnv->GetValue(resname.Data(), "");
   resname.Form("%s.DefaultIDClass", fSelectedGrid->ClassName());
   TString cut_default = gEnv->GetValue(resname.Data(), "");
   TString cut_class;
   Bool_t okpressed;
   if (cut_choices.Contains(" ")) {
      new KVDropDownDialog(this,
                           "Choose class of new identifier :",
                           cut_choices.Data(),
                           cut_default.Data(),
                           &cut_class,
                           &okpressed);
      if (!okpressed) return;
   } else
      cut_class = cut_choices;
   SetStatus(Form("Draw ID line (%s) in current pad", cut_class.Data()));
   fSelectedGrid->DrawAndAdd("ID", cut_class.Data());
   fTBNewIDL->SetDown(kFALSE, kFALSE);
   SetStatus();
}

void KVIDGridManagerGUI::NewCut()
{
   // GUI method to draw a new cut and add it to graph.
   // A dialog box with drop-down list pops up for the user to choose the class of the
   // new cut, unless only one choice is possible, in which case it is used automatically.
   // For each KVIDGraph-derived class, the list of possible cut classes and the
   // default class are define in .kvrootrc by the variables:
   //
   // [class_name].CutClass:  [cut class 1]
   // +[class_name].CutClass:  [cut class 2]
   // + ...
   // [class_name].DefaultCutClass:  [cut class]

   if (!fSelectedGrid || GetNSelected() != 1) return;
   TString resname;
   resname.Form("%s.CutClass", fSelectedGrid->ClassName());
   TString cut_choices = gEnv->GetValue(resname.Data(), "");
   resname.Form("%s.DefaultCutClass", fSelectedGrid->ClassName());
   TString cut_default = gEnv->GetValue(resname.Data(), "");
   TString cut_class;
   TString cut_types = cut_choices;
   cut_types.ReplaceAll("KVIDCut", "");
   Bool_t okpressed;
   if (cut_choices.Contains(" ")) {
      new KVDropDownDialog(this,
                           "Choose class of new cut :",
                           cut_types.Data(),
                           cut_default.Data(),
                           &cut_class,
                           &okpressed);
      if (!okpressed) return;
   } else
      cut_class = cut_types;
   SetStatus(Form("Draw cut %s in current pad", cut_class.Data()));
   cut_class.Prepend("KVIDCut");
   fSelectedGrid->DrawAndAdd("CUT", cut_class.Data());
   fTBNewCut->SetDown(kFALSE, kFALSE);
   SetStatus();
}

//___________________________________________________________________________________

void KVIDGridManagerGUI::TestGrid()
{
   // test the identification with selected grid
   // we search in current pad for the data histogram
   
   	SetStatus("");
   if(GetNSelected()!=1) return;
   if(!fSelectedGrid) return;
   // look for data histogram in current pad
   TH2* histo=0;
   if(gPad){
   	TIter next(gPad->GetListOfPrimitives());
   	TObject* o; 
   	while( (o = next()) ){
   		if(o->InheritsFrom("TH2")) { histo = (TH2*)o; break; }
   	}
   }
   if(!histo){
   	SetStatus("No TH2 found in current pad. Select pad containing 2D histo with data to identify.");
   	return;
   }
   else
   {
   	SetStatus(Form("Test identification of data in current pad %s.",histo->GetName()));
   }
   new KVTestIDGridDialog(fClient->GetDefaultRoot(), this, 10, 10, fSelectedGrid, histo);
   	SetStatus("");
}

void KVIDGridManagerGUI::TestTreeGrid()
{
   // test the identification with selected grid
   // we search in current pad for the data histogram
   	SetStatus("");
   if(GetNSelected()!=1) return;
   if(!fSelectedGrid) return;
   // look for data histogram in current pad
   TH2* histo=0;
   if(gPad){
   	TIter next(gPad->GetListOfPrimitives());
   	TObject* o; 
   	while( (o = next()) ){
   		if(o->InheritsFrom("TH2")) { histo = (TH2*)o; break; }
   	}
   }
   if(!histo){
   	SetStatus("No TH2 found in current pad. Select pad containing 2D histo with data to identify.");
   	return;
   }
   else
   {
   	SetStatus(Form("Test identification of data in current pad %s.",histo->GetName()));
   }
   TFile* tmpfiles = fSelectedGrid->TestIdentificationWithTree(histo->GetName());
   if(!tmpfiles) {
   	SetStatus("There was a problem with the test ???");
   	return;
   }
   new TCanvas;
   TH2* id = (TH2*)tmpfiles->Get("idcode_map");
   id->SetStats(kFALSE);
   id->Draw("zcol");
   TTree* t = (TTree*)tmpfiles->Get("tree_idresults");
   t->StartViewer();
   	SetStatus("");
}

void KVIDGridManagerGUI::FitGrid()
{
   	SetStatus("");
   if(GetNSelected()!=1) return;
   if(!fSelectedGrid) return;
   SetStatus(Form("Fitting grid %s",fSelectedGrid->GetName()));
	 KVVirtualIDFitter * fitter = KVVirtualIDFitter::GetDefaultFitter();
    fitter->SetGrid(fSelectedGrid);
    fitter->SetPad(fSelectedGrid->GetPad());
    TMethod * m = fitter->IsA()->GetMethodAny("FitPanel");
    TContextMenu * cm = new TContextMenu("FitPanel", "Context menu for KVVirtualIDFitter::FitPanel");
    cm->Action(fitter,m);
    delete cm;
}
