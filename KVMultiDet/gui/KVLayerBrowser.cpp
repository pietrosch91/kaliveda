//$Id: KVLayerBrowser.cpp,v 1.4 2006/10/19 14:32:43 franklan Exp $
#include "KVLayerBrowser.h"
#include "KVLayer.h"
#include "KVSeqCollection.h"

ClassImp(KVLayerBrowser)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Layer browser
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KVLayerBrowser::KVLayerBrowser(KVSeqCollection * layers, const TGWindow * p,
                                   UInt_t w, UInt_t h)
:TGTab(p, w, h)
{
// KVLayerBrowser is a TGTab widget with a tab for each layer in the structure.
// On each tab is a KVRingBrowser for the rings in the layer, and a set of buttons at the bottom of the
// tab to perform tasks such as "Remove Layer", "Add Ring", etc.

   fCleanup = new TList;
   fWidgets = 0;
   fRingBrowsers = 0;
   TIter next_lay(layers);
   KVBase *lobj;

   while ((lobj = (KVBase *) next_lay())) {     // loop over layers
      LayoutLayer((KVLayer *) lobj);    // display info for layer on a tab
   }
}

//______________________________________________________________________________________

void KVLayerBrowser::LayoutLayer(KVLayer * layer)
{
   // set up tab for each layer

   TGCompositeFrame *tf, *fF1;
   TGTextButton *fTB1, *fTB2;
   ULong_t red, green;
   fClient->GetColorByName("red", red);
   fClient->GetColorByName("green", green);
   TGLayoutHints *fTGL;
   KVBrowserWidget *widg;

   tf = AddTab(layer->GetName());       // new tab with name of layer

   // display ringbrowser i.e. tabs for rings
   KVRingBrowser *fRB = new KVRingBrowser(layer->GetRings(), tf, 800, 500);
   AddRingBrowser(fRB);         // add to list of ring browsers
   fTGL =
       new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5,
                         5, 10, 5);
   tf->AddFrame(fRB, fTGL);
   fCleanup->Add(fTGL);

   // Put "Add Ring" and "Remove Layer" buttons at bottom of each Layer tab
   // frame to hold the buttons
   fF1 = new TGCompositeFrame(tf, 400, 20, kHorizontalFrame | kFixedWidth);

   // button "Add Ring"
   // create a new KVWidget associating this layer with the function "add ring"
   widg = new KVBrowserWidget(layer, ADD_RING);
   // add to list of widgets
   AddToWidgetList(widg);
   // create button using this widget's ID
   fTB1 = new TGTextButton(fF1, "&Add Ring", widg->GetID());
   fTB1->Associate(this);
   widg->SetWidget(fTB1);
   fTB1->SetToolTipText("Add a ring of telescopes to this layer");
   fTB1->ChangeBackground(green);

   // button "Remove Layer"
   widg = new KVBrowserWidget(layer, REMOVE_LAYER);
   AddToWidgetList(widg);
   fTB2 = new TGTextButton(fF1, "&Remove Layer", widg->GetID());
   fTB2->Associate(this);
   widg->SetWidget(fTB2);
   fTB2->SetToolTipText("Remove this layer from the array");
   fTB2->ChangeBackground(red);

   fTGL =
       new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2,
                         2, 2);
   fCleanup->Add(fTGL);
   fF1->AddFrame(fTB1, fTGL);
   fF1->AddFrame(fTB2, fTGL);

   fF1->Resize(400, fTB1->GetDefaultHeight());
   fTGL = new TGLayoutHints(kLHintsBottom | kLHintsLeft, 5, 5, 1, 5);
   fCleanup->Add(fTGL);
   tf->AddFrame(fF1, fTGL);

   //set pointer in layer to its layer-browser
   layer->fBrowser = this;
}

//______________________________________________________________________________________
KVLayerBrowser::~KVLayerBrowser()
{
   if (fRingBrowsers) {
      fRingBrowsers->Delete();
      delete fRingBrowsers;
      fRingBrowsers = 0;
   }
   //stray TGLayoutHints
   if (fCleanup) {
      fCleanup->Delete();
      delete fCleanup;
      fCleanup = 0;
   }
   if (fWidgets) {
      //fWidgets->Delete();
      delete fWidgets;
      fWidgets = 0;
   }
}

void KVLayerBrowser::CloseWindow()
{
   // Called when window is closed via the window manager.

   delete this;
}

Bool_t KVLayerBrowser::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.

   KVBrowserWidget *widg = (KVBrowserWidget *) fWidgets->GetWidget(parm1);

   switch (GET_MSG(msg)) {

   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {

      case kCM_BUTTON:
         printf("Button was pressed, id = %ld, widget action...\n", parm1);
         if (widg)
            widg->Action();
         break;

      case kCM_TAB:
         printf("Ring Tab item %ld activated\n", parm1);
         break;


      default:
         break;
      }

   case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) {

      case kTE_TEXTCHANGED:
         printf("Text changed, Wid = %ld\n", parm1);
         if (widg)
            widg->Action();
         break;

      case kTE_ENTER:
         printf("Enter pressed, Wid = %ld\n", parm1);
         if (widg)
            widg->Action();
         break;

      default:
         break;

      }
   default:
      break;
   }
   return kTRUE;
}
