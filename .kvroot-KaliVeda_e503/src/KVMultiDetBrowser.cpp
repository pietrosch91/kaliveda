//$Id: KVMultiDetBrowser.cpp,v 1.3 2003/08/23 09:17:06 franklan Exp $

#include "KVMultiDetBrowser.h"
#include "KVBrowserWidget.h"
#include "KVMultiDetArray.h"
#include "KVLayerBrowser.h"
#include "KVWidgetList.h"
#include "TObjectTable.h"

ClassImp(KVMultiDetBrowser)
///////////////////////////////////////////////////////////////////////////////////////////////////////
// KaliVeda Multidetector Array Browser and Configuration Tool
//////////////////////////////////////////////////////////////////////////////////////////////////////
enum ECommandIdentifiers {
   M_FILE_OPEN,
   M_FILE_SAVE,
   M_FILE_SAVEAS,
   M_FILE_PRINT,
   M_FILE_PRINTSETUP,
   M_FILE_EXIT,

   M_HELP_CONTENTS,
   M_HELP_SEARCH,
   M_HELP_ABOUT
};

const char *filetypes[] = { "All files", "*",
   "ROOT files", "*.root",
   "ROOT macros", "*.C",
   0, 0
};

//______________________________________________________________________________________                                                                                                                                                                                                                                                                                                                                                                                                                                      
KVMultiDetBrowser::KVMultiDetBrowser
    (KVMultiDetArray * detarray, const TGWindow * p, UInt_t w, UInt_t h)
:TGMainFrame(p, w, h)
{
   // Create main window

   // for deleting stray GUI objects
   fCleanup = new TList;
   // pointer to detector array under examination                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
   fDetArray = detarray;
   fWidgets = 0;

//_______________________________________________________________________________________
   // Create top level window.
//_______________________________________________________________________________________
   // Create menubar and popup menus. The hint objects are used to place
   // and group the different menu widgets with respect to eachother.
   fMenuBarLayout =
       new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0,
                         1, 1);
   fMenuBarItemLayout =
       new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   // File menu
   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
   fMenuFile->AddEntry("&Save", M_FILE_SAVE);
   fMenuFile->AddEntry("S&ave as...", M_FILE_SAVEAS);
   fMenuFile->AddEntry("&Close", -1);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Print", M_FILE_PRINT);
   fMenuFile->AddEntry("P&rint setup...", M_FILE_PRINTSETUP);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);

   fMenuFile->DisableEntry(M_FILE_SAVEAS);
   fMenuFile->HideEntry(M_FILE_PRINT);

   // Help menu
   fMenuHelp = new TGPopupMenu(fClient->GetRoot());
   fMenuHelp->AddEntry("&Contents", M_HELP_CONTENTS);
   fMenuHelp->AddEntry("&Search...", M_HELP_SEARCH);
   fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);

   // Menu button messages are handled by the main frame (i.e. "this")
   // ProcessMessage() method.
   fMenuFile->Associate(this);
   fMenuHelp->Associate(this);

   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   AddFrame(fMenuBar, fMenuBarLayout);

//______________________________________________________________________________________
   // Put "Add Layer", "Update" and "Quit" buttons at bottom of main interface

   // container frame
   // this will be deleted by the main window when it closes -
   // don't put it in another cleanup list or else nasty segmentation fault will arise
   TGHorizontalFrame *fFrame1 =
       new TGHorizontalFrame(this, 1000, 20, kFixedWidth);

   // button colours
   ULong_t red, green, white;
   fClient->GetColorByName("red", red);
   fClient->GetColorByName("white", white);
   fClient->GetColorByName("green", green);

   //_________________________________________________________________________
   // example of use of KVBrowserWidget class for interfacing between buttons etc. and KVMultiDetArray

   // create new widget associating the array referenced by fDetArray
   KVBrowserWidget *widg = new KVBrowserWidget(fDetArray, ADD_LAYER);
   TGTextButton *fAddLayer =
       new TGTextButton(fFrame1, "Add &Layer", widg->GetID());
   fAddLayer->Associate(this);
   widg->SetWidget(fAddLayer);
   AddToWidgetList(widg);
   fAddLayer->SetToolTipText("Add a layer of telescopes to the array");
   fAddLayer->ChangeBackground(green);

   widg = new KVBrowserWidget(fDetArray, UPDATE_ARRAY);
   TGTextButton *fUpdate =
       new TGTextButton(fFrame1, "&Update Array", widg->GetID());
   fUpdate->Associate(this);
   widg->SetWidget(fUpdate);
   AddToWidgetList(widg);
   fUpdate->SetToolTipText("Update array and display modifications");
   fUpdate->ChangeBackground(white);

   widg = new KVBrowserWidget(this, QUIT_KVBROWSER);
   TGTextButton *fQuit = new TGTextButton(fFrame1, "&Quit", widg->GetID());
   fQuit->Associate(this);
   widg->SetWidget(fQuit);
   AddToWidgetList(widg);
   fQuit->SetToolTipText("Quit KVBrowser without applying modifications");
   fQuit->ChangeBackground(red);

   TGLayoutHints *fL1 =
       new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2,
                         2, 2);
   TGLayoutHints *fL2 =
       new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 2, 2, 2, 2);
   fCleanup->Add(fL1);
   fCleanup->Add(fL2);

   fFrame1->AddFrame(fAddLayer, fL1);
   fFrame1->AddFrame(fUpdate, fL1);
   fFrame1->AddFrame(fQuit, fL1);

   fFrame1->Resize(1000, fAddLayer->GetDefaultHeight());
   AddFrame(fFrame1, fL2);
//______________________________________________________________________________________
   // Presentation of Layers as Tab widgets

   fLayerBrowser =
       new KVLayerBrowser(fDetArray->GetLayers(), this, 900, 600);
   fLBLayout =
       new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY,
                         5, 5, 5, 2);
   AddFrame(fLayerBrowser, fLBLayout);

//______________________________________________________________________________________
   // Display main window 
   MapSubwindows();
   Layout();
   Resize(1024, 768);

   SetWindowName("KVBrowser Main Window");
   SetIconName("KVBrowser");
   MapWindow();

}

//_________________________________________________________________________________________

void KVMultiDetBrowser::UpdateArray()
{

   // complete update
   // remove old presentation

   // total refresh of display to show new array structure
   this->HideFrame(fLayerBrowser);
   this->RemoveFrame(fLayerBrowser);
   delete fLayerBrowser;
   delete fLBLayout;
   // redraw all
   fLayerBrowser =
       new KVLayerBrowser(fDetArray->GetLayers(), this, 900, 600);
   fLBLayout =
       new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY,
                         5, 5, 5, 2);
   AddFrame(fLayerBrowser, fLBLayout);
   MapSubwindows();
   Layout();
   MapWindow();

   //gObjectTable->Print(); // ROOT memory leak trace
}

//__________________________________________________________________________________________
KVMultiDetBrowser::~KVMultiDetBrowser()
{
   if (fMenuBarLayout)
      delete fMenuBarLayout;
   if (fMenuBarItemLayout)
      delete fMenuBarItemLayout;
   if (fMenuBarHelpLayout)
      delete fMenuBarHelpLayout;

   if (fMenuBar)
      delete fMenuBar;
   if (fMenuFile)
      delete fMenuFile;
   if (fMenuHelp)
      delete fMenuHelp;
   if (fLayerBrowser)
      delete fLayerBrowser;
   if (fLBLayout)
      delete fLBLayout;
   //stray TGLayoutHints
   if (fCleanup) {
      fCleanup->Delete();
      delete fCleanup;
      fCleanup = 0;
   }
   // delete widgets in layer tab
   if (fWidgets) {
      //fWidgets->Delete();
      delete fWidgets;
      fWidgets = 0;
   }
}

//_________________________________________________________________________________
void KVMultiDetBrowser::CloseWindow()
{
   //Message to close the browser received from window manager

/*   gApplication->Terminate(0); */

   fDetArray->CloseBrowser();
}

//__________________________________________________________________________________
Bool_t KVMultiDetBrowser::ProcessMessage(Long_t msg, Long_t parm1,
                                         Long_t parm2)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.

   // get associated widget from widget list using calling widget ID (parm1)
   KVBrowserWidget *widg = 0;
   switch (GET_MSG(msg)) {

   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {

      case kCM_BUTTON:
         //printf("Button was pressed, id = %ld, widget action...\n", parm1);
         if (fWidgets)
            widg = (KVBrowserWidget *) fWidgets->GetWidget(parm1);
         if (widg)
            widg->Action();
         break;

      case kCM_TAB:
         printf("Layer Tab item %ld activated\n", parm1);
         break;

      case kCM_MENUSELECT:
         printf("Pointer over menu entry, id=%ld\n", parm1);
         break;

      case kCM_MENU:
         switch (parm1) {

         case M_FILE_OPEN:
            {
               static TString dir(".");
               TGFileInfo fi;
               fi.fFileTypes = filetypes;
               fi.fIniDir = StrDup(dir);
               new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
               printf("Open file: %s (dir: %s)\n", fi.fFilename,
                      fi.fIniDir);
               dir = fi.fIniDir;
            }
            break;

         case M_FILE_SAVE:
            printf("M_FILE_SAVE\n");
            break;

         case M_FILE_PRINT:
            printf("M_FILE_PRINT\n");
            printf
                ("Hiding itself, select \"Print Setup...\" to enable again\n");
            fMenuFile->HideEntry(M_FILE_PRINT);
            break;

         case M_FILE_PRINTSETUP:
            printf("M_FILE_PRINTSETUP\n");
            printf("Enabling \"Print\"\n");
            fMenuFile->EnableEntry(M_FILE_PRINT);
            break;

         case M_FILE_EXIT:
            CloseWindow();      // this also terminates theApp
            break;

         default:
            break;
         }
      default:
         break;
      }
   default:
      break;
   }
   return kTRUE;
}

//________________________________________________________________________________
void KVMultiDetBrowser::AddToWidgetList(KVBrowserWidget * widg)
{
   if (!fWidgets)
      fWidgets = new KVWidgetList;
   fWidgets->Add(widg);
}
