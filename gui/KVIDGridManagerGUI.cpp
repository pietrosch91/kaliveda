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
#include "TGMimeTypes.h"

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
   fSelectedEntries = 0;
	
	//add new mime-types for id-graphs
	if( !gClient->GetMimeTypeList()->GetIcon("KVIDMap",kFALSE) ){
		ReadGraphMimeTypes();
	}

   //any change of ID grid manager causes UpdateListOfGrids to be called
   gIDGridManager->Connect("Modified()", "KVIDGridManagerGUI", this,
                           "UpdateListOfGrids()");

   //create new manager GUI

        /**************** GRIDS popup menu *****************/
   fMenuFile = new TGPopupMenu(gClient->GetRoot());
   fMenuFile->AddEntry("&New grid...", M_GRIDS_NEW);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Read grids...", M_GRIDS_READ);
	
	/* cascading "Save grids" menu... */
	TGPopupMenu * sgm = new TGPopupMenu(gClient->GetRoot());
	sgm->AddEntry("...selection", M_GRIDS_SAVE_SEL);
	sgm->AddEntry("...in tab", M_GRIDS_SAVE_TAB);
	sgm->AddEntry("...all", M_GRIDS_SAVE_ALL);
   fMenuFile->AddPopup("Save grids", sgm);
	
   fMenuFile->AddSeparator();
	
	/* cascading "Delete grids" menu... */
	sgm = new TGPopupMenu(gClient->GetRoot());
	sgm->AddEntry("...selection", M_GRIDS_DEL_SEL);
	sgm->AddEntry("...in tab", M_GRIDS_DEL_TAB);
	sgm->AddEntry("...all", M_GRIDS_DEL_ALL);
   fMenuFile->AddPopup("Delete grids", sgm);
	
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
   fMenuBar->AddPopup("&Grids", fMenuFile, fMenuBarItemLayout);
//   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   //add to main window
   AddFrame(fMenuBar,
            new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0,
                              0, 1, 1));

   fHframe = new TGHorizontalFrame(this, 10, 10);

	// Tabs for lists of grids. Each tab holds list of grids for a given
	// type of ID telescope.
	fGridListTabs = new TGTab(fHframe, 600, 400);
	fGridListTabs->Connect("Selected(Int_t)","KVIDGridManagerGUI",this,"TabSelect(Int_t)");
	fIDGridList = 0;
   //initialise tabs with lists of grids
   CreateAndFillTabs();

   fHframe->AddFrame(fGridListTabs,
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
   SetWMSize(750, 500);
	// first tab is visible, but TabSelect(0) is not called automatically
	TabSelect(0);
}

KVIDGridManagerGUI::~KVIDGridManagerGUI()
{
   //close window
   UnmapWindow();
   gIDGridManager->Disconnect("Modified()", this, "UpdateListOfGrids()");
   if(fSelectedEntries) delete fSelectedEntries;
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

   case M_GRIDS_NEW:

      cout << "Add new grid" << endl;
		NewGrid();
      break;

   case M_GRIDS_READ:
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
      //warning message and confirmation
      {
         Int_t ret_val;
         new TGMsgBox(gClient->GetRoot(), this, "ID Grid Manager",
                      "This will delete the selected grids. Are you sure ?",
                      kMBIconExclamation, kMBOk | kMBCancel, &ret_val);
         if (ret_val & kMBOk) {
            DeleteGrids();
         }
      }
      break;
   case M_GRIDS_DEL_TAB:
      //warning message and confirmation
      {
         Int_t ret_val;
         new TGMsgBox(gClient->GetRoot(), this, "ID Grid Manager",
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

void KVIDGridManagerGUI::UpdateListOfGrids()
{
   //remove all existing entries, then fill list from gIDGridManager
   //we update the current pad, in case the displayed grid no longer exists
   //grid buttons are disabled, as any selected grid is deselected
	
 	printf("UpdateListOfGrids called\n");
	UpdateTabs();	
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
      delete fSelectedEntries;
   fSelectedEntries = fIDGridList->GetSelectedObjects();
   fSelectedGrid = (KVIDGraph*)fIDGridList->GetLastSelectedObject();
}

Int_t KVIDGridManagerGUI::GetNSelected()
{
   //returns current number of selected items in grid list
   return (fSelectedEntries ? fSelectedEntries->GetSize() : 0);
}

void KVIDGridManagerGUI::DeleteGrids()
{
   //delete the current selected grid, or all currently selected grids if there are more than one

	if(fSelectedEntries){
		cout << "DeleteGrids(): selected entries are " << endl;
		fSelectedEntries->ls();
	}
	cout << "GetNSelected() = " << GetNSelected() << endl;
   if (GetNSelected() == 1)
      gIDGridManager->DeleteGrid(fSelectedGrid);
   else if (GetNSelected() > 1) {
      //multiselection
      TIter next(fSelectedEntries, kIterBackward); KVIDGraph* entry;
      while ((entry = (KVIDGraph *) next())) {
            gIDGridManager->DeleteGrid(entry, kFALSE);   //no update
      }
   }
	delete fSelectedEntries; fSelectedEntries = 0; fSelectedGrid = 0;
   UpdateListOfGrids();
}

void KVIDGridManagerGUI::DeleteAllGridsInTab()
{
   //delete the all grids in currently selected tab

   TIter next(GetAllGridsInTab(), kIterBackward); KVIDGraph* entry;
   while ((entry = (KVIDGraph *) next())) {
		gIDGridManager->DeleteGrid(entry, kFALSE);   //no update
	}
	if(fSelectedEntries) delete fSelectedEntries;
	fSelectedEntries = 0; fSelectedGrid = 0;
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
      Clist->R__FOR_EACH(TCanvas, Modified) ();
      Clist->R__FOR_EACH(TCanvas, Update) ();
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
			gClient->GetRoot(), this);
	if(cancel || !telescopes->At(0)){
		Info("NewGrid","No ID telescopes chosen. Grid creation cancelled.");
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
	if(!ok_pressed){
		Info("NewGrid","No ID grid class chosen. Grid creation cancelled.");
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
   KVIDGraph *g1 = (KVIDGraph *) fSelectedEntries->At(1);
   KVIDGraph *g2 = (KVIDGraph *) fSelectedEntries->First();
   //new KVMergeGridsDialog(g1, g2, gClient->GetRoot(), this, 10, 10);
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
   new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fi);
   if (fi.fFilename) {
      //if no ".xxx" ending given, we add ".dat"
      TString filenam(fi.fFilename);
      if (!filenam.Contains('.'))
         filenam += ".dat";
		Int_t n_saved=gIDGridManager->WriteAsciiFile(filenam.Data(),selection);
      if (n_saved) {
         //wrote file no problem
         new TGMsgBox(gClient->GetRoot(), this, "ID Grid Manager",
                      Form("Saved %d grids in %s", n_saved, filenam.Data()), 0, kMBOk);
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

void KVIDGridManagerGUI::CreateAndFillTabs()
{
	// create a tab for each type of ID telescope
   // put a list box for ID grid names on each tab
	
	KVString labels("[unknown]");
	if(gIDGridManager->GetGrids()->GetSize()) gIDGridManager->GetListOfIDTelescopeLabels(labels);
	
	//loop over labels
	labels.Begin(",");
	while( ! labels.End() ){
		KVString lab = labels.Next();
		TGCompositeFrame*cf = fGridListTabs->AddTab(lab.Data());
		cf->ChangeOptions(kVerticalFrame);
		fIDGridList = new KVListView(KVIDGraph::Class(), cf, 600, 400);
			fIDGridList->SetDataColumns(8);
			fIDGridList->SetDataColumn(0, "Name", "", kTextLeft);
			fIDGridList->SetDataColumn(1, "VarX", "", kTextLeft);
			fIDGridList->SetDataColumn(2, "VarY", "", kTextLeft);
			fIDGridList->SetDataColumn(3, "RunList", "", kTextLeft);
			fIDGridList->SetDataColumn(4, "# Ident.", "GetNumberOfIdentifiers", kTextRight);
			fIDGridList->SetDataColumn(5, "# Cuts", "GetNumberOfCuts", kTextRight);
			fIDGridList->SetDataColumn(6, "X scaling", "GetXScaleFactor", kTextRight);
			fIDGridList->SetDataColumn(7, "Y scaling", "GetYScaleFactor", kTextRight);
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
	cout << "TabSelect("<<tab<<")"<<endl;
	TGCompositeFrame *cf = fGridListTabs->GetCurrentContainer();
	if(!cf) return;//there are no tabs
	TGFrameElement *el = (TGFrameElement*)cf->GetList()->At(0);
 	fIDGridList = (KVListView*)el->fFrame;
 	fIDGridList->SelectionChanged();
}

void KVIDGridManagerGUI::UpdateTabs()
{
	// create a tab for each type of ID telescope
   // put a list box for ID grid names on each tab
	
	cout << "UpdateTabs called" << endl;
	KVString labels("[unknown]");
	if(gIDGridManager->GetGrids()->GetSize()) gIDGridManager->GetListOfIDTelescopeLabels(labels);
	cout << "labels = " << labels.Data() << endl;
	//add any missing labels, update existing ones
	labels.Begin(",");
	while( ! labels.End() ){
		KVString lab = labels.Next();
		
		if( !fGridListTabs->GetTabContainer(lab.Data()) ){// new tab
			cout << "making new tab : " <<lab.Data()<<endl;
			TGCompositeFrame*cf = fGridListTabs->AddTab(lab.Data());
			cf->ChangeOptions(kVerticalFrame);
			fIDGridList = new KVListView(KVIDGraph::Class(), cf, 600, 400);
			fIDGridList->SetDataColumns(8);
			fIDGridList->SetDataColumn(0, "Name", "", kTextLeft);
			fIDGridList->SetDataColumn(1, "VarX", "", kTextLeft);
			fIDGridList->SetDataColumn(2, "VarY", "", kTextLeft);
			fIDGridList->SetDataColumn(3, "RunList", "", kTextLeft);
			fIDGridList->SetDataColumn(4, "# Ident.", "GetNumberOfIdentifiers", kTextRight);
			fIDGridList->SetDataColumn(5, "# Cuts", "GetNumberOfCuts", kTextRight);
			fIDGridList->SetDataColumn(6, "XScaleFactor", "", kTextRight);
			fIDGridList->SetDataColumn(7, "YScaleFactor", "", kTextRight);
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
			fGridListTabs->SetTab(fGridListTabs->GetNumberOfTabs()-1,kTRUE);
		}
		else {//existing tab
			cout << "adding to tab : " <<lab.Data()<<endl;
			TGCompositeFrame*cf = fGridListTabs->GetTabContainer(lab.Data());
			if(!cf){
				cout << "cf = 0x0 : label="<<lab.Data()<<" tab name="<<
						fGridListTabs->GetTabTab(lab.Data())->GetText()->GetString()<<endl;
			}
			else{
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
	
	cout << "RemoveEmptyTabs()" << endl;
	Int_t ntabs = fGridListTabs->GetNumberOfTabs();
	Bool_t recursive = kFALSE;
	for(Int_t itab=0; itab<ntabs; itab++){
		
		//get name of tab
		KVString lab = fGridListTabs->GetTabTab(itab)->GetString();
		//get grids for this tab (if any)
		KVList* grids = gIDGridManager->GetGridsForIDTelescope(lab);
		Int_t ngrids = grids->GetEntries();
		delete grids;
		if(!ngrids){
			//empty tab! remove it!
			cout << "Removing tab : " << lab.Data() << endl;
			//delete the KVListView
			TGCompositeFrame*cf =fGridListTabs->GetTabContainer(itab);
			TGFrameElement *el = (TGFrameElement*)cf->GetList()->At(0);
			KVListView* lv = (KVListView*)el->fFrame;
			delete lv;
			//remove tab
			fGridListTabs->RemoveTab(itab,kFALSE);
			recursive = kTRUE; // call recursively
			break;//stop loop - tab numbers have changed
		}
	}
	if(recursive) RemoveEmptyTabs();
	fGridListTabs->MapSubwindows();
	fGridListTabs->Layout();
}

void KVIDGridManagerGUI::ReadGraphMimeTypes()
{
	// Add to standard ROOT mime types some new ones defined in .kvrootrc
	// for icons associated with graphs etc. by lines such as:
	//
	//  IDGridManagerGUI.MimeTypes :   KVIDMap
	//  IDGridManagerGUI.MimeTypes.KVIDMap.Icon :   rootdb_t.xpm
	//  +IDGridManagerGUI.MimeTypes :   KVIDZAGrid
	//  IDGridManagerGUI.MimeTypes.KVIDZAGrid.Icon :   draw_t.xpm
	//
	// etc.
	
	KVString mimetypes = gEnv->GetValue("IDGridManagerGUI.MimeTypes","");
	if(mimetypes!=""){
		
		mimetypes.Begin(" ");
		while( !mimetypes.End() ){
			
			KVString classname = mimetypes.Next(kTRUE);
			KVString icon = gEnv->GetValue( Form("IDGridManagerGUI.MimeTypes.%s.Icon", classname.Data()), "draw_t.xpm");
			
			gClient->GetMimeTypeList()->AddType("[kaliveda/idgraph]", classname.Data(), icon.Data(), icon.Data(), "");
			
		}
	}
}
