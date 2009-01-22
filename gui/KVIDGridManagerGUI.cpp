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
KVIDGridManagerGUI::KVIDGridManagerGUI():TGMainFrame(gClient->GetRoot(), 500,
            300)
{
   fFirstGrid = 0;
   fLastGrid = -1;
   fSelectedGrid = 0;
   fSelectedEntries = new TList;

   //any change of ID grid manager causes UpdateListOfGrids to be called
   gIDGridManager->Connect("Modified()", "KVIDGridManagerGUI", this,
                           "UpdateListOfGrids()");

   //create new manager GUI

        /**************** FILE popup menu *****************/
   fMenuFile = new TGPopupMenu(gClient->GetRoot());
   fMenuFile->AddEntry("&New", M_FILE_NEW);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
   fMenuFile->AddEntry("&Save", M_FILE_SAVE);
   fMenuFile->AddEntry("S&ave as...", M_FILE_SAVEAS);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Clear", M_FILE_CLEAR);
   fMenuFile->AddEntry("&Refresh", M_FILE_REFRESH);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Close", M_FILE_CLOSE);
   fMenuFile->Connect("Activated(Int_t)", "KVIDGridManagerGUI", this,
                      "HandleFileMenu(Int_t)");
        /**************** HELP popup menu *****************/
   fMenuHelp = new TGPopupMenu(gClient->GetRoot());
   fMenuHelp->AddEntry("About...", M_HELP_ABOUT);
        /******************MENUBAR*********************/
   fMenuBarItemLayout =
       new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   //add to main window
   AddFrame(fMenuBar,
            new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0,
                              0, 1, 1));

   fHframe = new TGHorizontalFrame(this, 10, 10);
   //vertical frame for buttons
   fButtonsFrame = new TGVButtonGroup(fHframe, "Grid");
   fNewGrid = new TGTextButton(fButtonsFrame, "&New...");
   fNewGrid->SetToolTipText("Create a new grid");
   fDrawGrid = new TGTextButton(fButtonsFrame, "&Draw");
   fDrawGrid->SetToolTipText("Draw selected grid");
   fUnDrawGrid = new TGTextButton(fButtonsFrame, "&Undraw");
   fUnDrawGrid->SetToolTipText("Remove selected grid from current pad");
   fCopyGrid = new TGTextButton(fButtonsFrame, "&Copy...");
   fCopyGrid->SetToolTipText("Copy the selected grid");
   fMergeGrids = new TGTextButton(fButtonsFrame, "&Merge...");
   fMergeGrids->SetToolTipText("Merge lines from two grids in a new grid");
   fModifyGrid = new TGTextButton(fButtonsFrame, "&Edit...");
   fModifyGrid->SetToolTipText("Edit properties of selected grid");
   fCalculateGrid = new TGTextButton(fButtonsFrame, "C&alculate...");
   fCalculateGrid->
       SetToolTipText
       ("Replace grid with lines calculate from energy loss tables");
   fFitGrid = new TGTextButton(fButtonsFrame, "&Fit...");
   fFitGrid->SetToolTipText("Fit the selected grid with a KVTGID");
   fTestGrid = new TGTextButton(fButtonsFrame, "&Test...");
   fTestGrid->
       SetToolTipText("Perform an identification test with this grid");
   fClearGrid = new TGTextButton(fButtonsFrame, "C&lear");
   fClearGrid->SetToolTipText("Delete all lines in selected grid(s)");
   fDeleteGrid = new TGTextButton(fButtonsFrame, "D&elete");
   fDeleteGrid->SetToolTipText("Delete the selected grid(s)");
   //make all buttons same width
   fButtonsFrame->
       SetLayoutHints(new
                      TGLayoutHints(kLHintsExpandX | kLHintsCenterX |
                                    kLHintsTop, 2, 2, 0, 0));
   fButtonsFrame->Show();
   fButtonsFrame->Resize(fButtonsFrame->GetDefaultWidth(),
                         fButtonsFrame->GetDefaultHeight());
   fHframe->AddFrame(fButtonsFrame,
                     new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 20,
                                       10, 2, 2));

   fDrawGrid->Connect("Clicked()", "KVIDGridManagerGUI", this,
                      "DrawGrid()");
   fUnDrawGrid->Connect("Clicked()", "KVIDGridManagerGUI", this,
                        "UnDrawGrid()");
   fNewGrid->Connect("Clicked()", "KVIDGridManagerGUI", this, "NewGrid()");
   fFitGrid->Connect("Clicked()", "KVIDGridManagerGUI", this, "FitGrid()");
   fClearGrid->Connect("Clicked()", "KVIDGridManagerGUI", this,
                       "ClearGrid()");
   fCopyGrid->Connect("Clicked()", "KVIDGridManagerGUI", this,
                      "CopyGrid()");
   fMergeGrids->Connect("Clicked()", "KVIDGridManagerGUI", this,
                        "MergeGrids()");
   fCalculateGrid->Connect("Clicked()", "KVIDGridManagerGUI", this,
                           "CalculateGrid()");
   fModifyGrid->Connect("Clicked()", "KVIDGridManagerGUI", this,
                        "ModifyGrid()");
   fDeleteGrid->Connect("Clicked()", "KVIDGridManagerGUI", this,
                        "DeleteGrid()");
   fTestGrid->Connect("Clicked()", "KVIDGridManagerGUI", this,
                      "TestGrid()");

   //disable grid-related buttons
   DisableGridButtons();

   //list box for ID grid names
   fIDGridList = new TGListBox(fHframe, ID_LIST_BOX);
   fIDGridList->SetMultipleSelections(kTRUE);
   fIDGridList->Resize(350, 250);

   fIDGridList->Connect("SelectionChanged()", "KVIDGridManagerGUI", this,
                        "SelectionChanged()");

   fHframe->AddFrame(fIDGridList,
                     new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                       kLHintsExpandX | kLHintsExpandY, 30,
                                       10, 10, 10));

   AddFrame(fHframe,
            new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0,
                              0));

   SetWindowName("ID Grid Manager");

   //layout & draw window
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
   SetWMSize(500, 300);

   //initialise list of grids
   FillListOfGrids();
}

KVIDGridManagerGUI::~KVIDGridManagerGUI()
{
   //close window
   UnmapWindow();
   gIDGridManager->Disconnect("Modified()", this, "UpdateListOfGrids()");
   delete fSelectedEntries;
   fSelectedEntries = 0;
}

void KVIDGridManagerGUI::CloseWindow()
{
   //close viewer
   DeleteWindow();
}

void KVIDGridManagerGUI::DisableGridButtons()
{
   //Disable "Draw", "Modify", "Fit", "Delete" grid buttons
   //This is the state when no grid is selected
   
#ifdef __WITHOUT_TGBUTTON_SETENABLED
#define SetEnabled(flag) \
   SetState( flag ? kButtonUp : kButtonDisabled )
#endif
   fDrawGrid->SetEnabled(kFALSE);
   fUnDrawGrid->SetEnabled(kFALSE);
   fModifyGrid->SetEnabled(kFALSE);
   fCopyGrid->SetEnabled(kFALSE);
   fMergeGrids->SetEnabled(kFALSE);
   fClearGrid->SetEnabled(kFALSE);
   fCalculateGrid->SetEnabled(kFALSE);
   fFitGrid->SetEnabled(kFALSE);
   fDeleteGrid->SetEnabled(kFALSE);
   fTestGrid->SetEnabled(kFALSE);
}

void KVIDGridManagerGUI::EnableGridButtons()
{
   //Enable buttons when a grid is selected, depending on which type of
   //grid and how many are selected
   fClearGrid->SetEnabled(kTRUE);
   fDeleteGrid->SetEnabled(kTRUE);
   if (GetNSelected() == 1) {
      fDrawGrid->SetEnabled(kTRUE);
      fUnDrawGrid->SetEnabled(kTRUE);
      fModifyGrid->SetEnabled(kTRUE);
      fCopyGrid->SetEnabled(kTRUE);
      fTestGrid->SetEnabled(kTRUE);
      //fFitGrid->SetEnabled(kTRUE);
      if (fSelectedGrid)
         if (fSelectedGrid->InheritsFrom("KVIDGChIoSi"))
            fCalculateGrid->SetEnabled(kTRUE);
   } else if (GetNSelected() == 2) {    //two grids: merge is possible
      fMergeGrids->SetEnabled(kTRUE);
   } else {                     //multi-selection : only clear and delete active
      fDrawGrid->SetEnabled(kFALSE);
      fUnDrawGrid->SetEnabled(kFALSE);
      fModifyGrid->SetEnabled(kFALSE);
      fCopyGrid->SetEnabled(kFALSE);
      fMergeGrids->SetEnabled(kFALSE);
      fCalculateGrid->SetEnabled(kFALSE);
      fFitGrid->SetEnabled(kFALSE);
      fTestGrid->SetEnabled(kFALSE);
   }
#ifdef __WITHOUT_TGBUTTON_SETENABLED
#undef SetEnabled
#endif
}

void KVIDGridManagerGUI::HandleFileMenu(Int_t id)
{
   //Receive signals emitted by items selected in File menu
   switch (id) {

   case M_FILE_NEW:

      cout << "New" << endl;
      break;

   case M_FILE_OPEN:
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
         new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);
         if (fi.fFilename) {
            if (gIDGridManager->ReadAsciiFile(fi.fFilename)) {
               //read file ok no problem. update list of grids.
               //UpdateListOfGrids();
               //set filename for Save
               fFileName = fi.fFilename;
            } else {
               new TGMsgBox(gClient->GetRoot(), this, "ID Grid Manager",
                            Form("Could not read file %s", fi.fFilename),
                            0, kMBOk);
            }
         }
         dir = fi.fIniDir;
      }
      break;

   case M_FILE_SAVE:

      //if filename has not been set, do SaveAs... dialog
      if (fFileName == "")
         SaveAs();
      else {
         gIDGridManager->WriteAsciiFile(fFileName.Data());
         new TGMsgBox(gClient->GetRoot(), this, "ID Grid Manager",
                      Form("Saved grids in %s", fFileName.Data()), 0,
                      kMBOk);
      }
      break;

   case M_FILE_SAVEAS:

      SaveAs();
      break;

   case M_FILE_CLOSE:

      SendCloseMessage();
      break;

   case M_FILE_REFRESH:

      //update list of grids
      UpdateListOfGrids();
      break;

   case M_FILE_CLEAR:

      //warning message and confirmation
      {
         Int_t ret_val;
         new TGMsgBox(gClient->GetRoot(), this, "ID Grid Manager",
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

   default:

      break;
   }
}

void KVIDGridManagerGUI::FillListOfGrids()
{
   //if gIDGridManager exists, we use it to fill fIDGridList with the names of all existing
   //identification grids

   if (gIDGridManager && gIDGridManager->GetGrids()->GetSize()) {

      TIter nextgrid(gIDGridManager->GetGrids());
      KVIDGrid *grid;
      Int_t id = 0;
      while ((grid = (KVIDGrid *) nextgrid())) {
         fIDGridList->AddEntry(grid->GetName(), id++);
      }
      fFirstGrid = 0;
      fLastGrid = id - 1;
      fIDGridList->MapSubwindows();
      fIDGridList->Layout();
   }
}

void KVIDGridManagerGUI::ClearListOfGrids()
{
   //remove all entries from grid list box

   if (fFirstGrid <= fLastGrid) {
      fIDGridList->RemoveEntries(fFirstGrid, fLastGrid);
      fIDGridList->Layout();
      fFirstGrid = 0;
      fLastGrid = -1;
   }
}

void KVIDGridManagerGUI::UpdateListOfGrids()
{
   //remove all existing entries, then fill list from gIDGridManager
   //we update the current pad, in case the displayed grid no longer exists
   //grid buttons are disabled, as any selected grid is deselected
   ClearListOfGrids();
   FillListOfGrids();
   DisableGridButtons();
   //update all canvases
   TSeqCollection *Clist = gROOT->GetListOfCanvases();
   if (Clist->GetSize() > 0) {
      Clist->R__FOR_EACH(TCanvas, Modified) ();
      Clist->R__FOR_EACH(TCanvas, Update) ();
   }
}

void KVIDGridManagerGUI::SelectionChanged()
{
   //called any time the selection of grids changes
   //each time, we update:
   //fSelectedEntries : the list of all selected entries
   //GetNSelected() : the number of selected entries
   //fSelectedGrid : the last selected grid (=the only grid selected if GeTNSelected==1)
   //The grid buttons' state is modified according to the number of selected grids

   //get number of selected items
   if (fSelectedEntries)
      fSelectedEntries->Clear();
   else
      fSelectedEntries = new TList;
   fIDGridList->GetSelectedEntries(fSelectedEntries);
   Int_t id = fIDGridList->GetSelected();       //last selected i.e. clicked
   fSelectedGrid = 0;
   Int_t idlist = (GetNSelected()? ((TGLBEntry *) fSelectedEntries->First())->EntryId() : -1);  //id of first entry in list
   if (id > -1) {
      if (GetNSelected() == 1)
         fSelectedGrid =
             (KVIDGrid *) gIDGridManager->GetGrids()->At(idlist);
      else if (GetNSelected() > 1)
         fSelectedGrid = (KVIDGrid *) gIDGridManager->GetGrids()->At(id);
   } else fSelectedGrid = 0;
   DisableGridButtons();
   if (GetNSelected()) EnableGridButtons();
}

Int_t KVIDGridManagerGUI::GetNSelected()
{
   //returns current number of selected items in grid list
   return (fSelectedEntries ? fSelectedEntries->GetSize() : 0);
}

void KVIDGridManagerGUI::DrawGrid()
{
   //draw last selected grid in active canvas
   //if no canvas exists, we create one with axes adapted to the grid

   if (fSelectedGrid) {
      fSelectedGrid->Draw();
      gPad->Update();
   }
}

void KVIDGridManagerGUI::UnDrawGrid()
{
   //remove selected grid from active pad
   if (fSelectedGrid) {
      fSelectedGrid->UnDraw();
   }
}

void KVIDGridManagerGUI::DeleteGrid()
{
   //delete the current selected grid, or all currently selected grids if there are more than one

   if (GetNSelected() == 1)
      gIDGridManager->DeleteGrid(fSelectedGrid);
   else if (GetNSelected() > 1) {
      //multiselection
      TIter next(fSelectedEntries, kIterBackward);
      TGLBEntry *entry;
      while ((entry = (TGLBEntry *) next())) {
         Int_t index = entry->EntryId();
         KVIDGrid *grid =
             (KVIDGrid *) gIDGridManager->GetGrids()->At(index);
         if (grid)
            gIDGridManager->DeleteGrid(grid, kFALSE);   //no update
      }
   }
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
      TGLBEntry *entry;
      while ((entry = (TGLBEntry *) next())) {
         Int_t index = entry->EntryId();
         KVIDGrid *grid =
             (KVIDGrid *) gIDGridManager->GetGrids()->At(index);
         grid->Clear();
      }
   }
   //update all canvases
   TSeqCollection *Clist = gROOT->GetListOfCanvases();
   if (Clist->GetSize() > 0) {
      Clist->R__FOR_EACH(TCanvas, Modified) ();
      Clist->R__FOR_EACH(TCanvas, Update) ();
   }
}

void KVIDGridManagerGUI::NewGrid()
{
   //add a new grid to the manager
   new KVNewGridDialog(gClient->GetRoot(), this, 400, 200);
}

void KVIDGridManagerGUI::ModifyGrid()
{
   //modify an existing grid
//      new KVNewGridDialog(gClient->GetRoot(), this, 400, 200, "Edit", fSelectedGrid); 
   fSelectedGrid->NewGridDialog(gClient->GetRoot(), this, 400, 200,
                                "Edit");
}

void KVIDGridManagerGUI::TestGrid()
{
   //test the identification with this grid
   new KVTestIDGridDialog(gClient->GetRoot(), this, 10, 10, fSelectedGrid);
}

void KVIDGridManagerGUI::MergeGrids()
{
   //merge 2 grids
   Int_t id1 = ((TGLBEntry *) fSelectedEntries->First())->EntryId();
   Int_t id2 = ((TGLBEntry *) fSelectedEntries->At(1))->EntryId();
   KVIDGrid *g1 = (KVIDGrid *) gIDGridManager->GetGrids()->At(id1);
   KVIDGrid *g2 = (KVIDGrid *) gIDGridManager->GetGrids()->At(id2);
   new KVMergeGridsDialog(g1, g2, gClient->GetRoot(), this, 10, 10);
}

void KVIDGridManagerGUI::CalculateGrid()
{
   //replace lines in grid with lines calculated from energy loss tables
   new KVCalculateChIoSiGridDialog(gClient->GetRoot(), this, 10, 10,
                                   fSelectedGrid);
}

void KVIDGridManagerGUI::CopyGrid()
{
   //copy the selected grid, then open edit grid dialog to modify its properties
//      new KVNewGridDialog(gClient->GetRoot(), this, 400, 200, "Copy", fSelectedGrid); 
   fSelectedGrid->NewGridDialog(gClient->GetRoot(), this, 400, 200,
                                "Copy");
}

void KVIDGridManagerGUI::SaveAs()
{
   //Opens dialog to choose filename in which to save grid manager
   static TString dir(".");
   const char *filetypes[] = {
      "ID Grid files", "*.dat",
      "All files", "*",
      0, 0
   };
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   fi.fIniDir = StrDup(dir);
   new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fi);
   if (fi.fFilename) {
      //if no ".xxx" ending given, we add ".dat"
      TString filenam(fi.fFilename);
      if (!filenam.Contains('.'))
         filenam += ".dat";
      if (gIDGridManager->WriteAsciiFile(filenam.Data())) {
         //wrote file no problem
         new TGMsgBox(gClient->GetRoot(), this, "ID Grid Manager",
                      Form("Saved grids in %s", filenam.Data()), 0, kMBOk);
         //set file name for Save
         fFileName = filenam;
      } else {
         new TGMsgBox(gClient->GetRoot(), this, "ID Grid Manager",
                      Form("Could not write file %s", filenam.Data()), 0,
                      kMBOk);
      }
   }
   dir = fi.fIniDir;
}
