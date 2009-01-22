//$Id: KVRingBrowser.cpp,v 1.4 2006/10/19 14:32:43 franklan Exp $

#include "KVRingBrowser.h"
#include "KVList.h"
#include "KVTelescope.h"
#include "KVRing.h"

ClassImp(KVRingBrowser)
///////////////////////////////////////////////////////////////////////////////////////
// Ring browser
///////////////////////////////////////////////////////////////////////////////////////
    KVRingBrowser::KVRingBrowser(KVList * rings, const TGWindow * p,
                                 UInt_t w, UInt_t h)
:TGTab(p, w, h)
{
   // Displays all information on rings in a given layer, the ring list of the layer is passed as the argument
   // KVList *rings. a ring browser is basically a TGTab widget with a tab for each ring. the ring name is
   // written on the tab.
   // each tab contains info on the ring (number of telescopes, angular range etc.), buttons for adding a
   // telescope or removing the ring, and a list of all the telescopes in the ring represented by buttons for
   // launching the associated KVTelescopeBrowser.
   // all TGFrame-derived objects which are associated with a KVWidget in the interface will be deleted in the
   // dtor via the fWidgets list. any object not associated with a KVWidget
   // should be added to the fCleanup list otherwise used for deleting TGLayoutHints.

   fWidgets = 0;
   fCleanup = new TList;
   TIter next_ring(rings);
   KVBase *robj;

   while ((robj = (KVBase *) next_ring())) {    // loop over rings
      LayoutRing((KVRing *) robj);      // display info for ring on a tab
   }
}

//______________________________________________________________________________________
KVRingBrowser::~KVRingBrowser()
{
   if (fCleanup) {
      fCleanup->Delete();       // garbage collection
      delete fCleanup;
      fCleanup = 0;
   }
   if (fWidgets) {
      //fWidgets->Delete();  // delete all KVWidget objects of interface i.e. all TGFrame objects in list
      delete fWidgets;
      fWidgets = 0;
   }
}

//_______________________________________________________________________________________
void KVRingBrowser::LayoutRing(KVRing * kvr)
{

   TGCompositeFrame *tf, *fF1;
   TGLayoutHints *fTGL;

   ULong_t red, green;
   fClient->GetColorByName("red", red);
   fClient->GetColorByName("green", green);
   TGNumberEntry *nent;
   TGLabel *lab;
   KVBrowserWidget *widg;

   // Add a new tab for this ring
   tf = AddTab(kvr->GetName());
//_______________________________________________________________________________________
// TOP ROW OF RING TAB - NB OF TELESCOPES, THETA_MIN, ETC ETC
//_______________________________________________________________________________________
   // Container for ring properties shown at top of tab
   fF1 = new TGCompositeFrame(tf, 900, 20, kHorizontalFrame | kFixedWidth);
   fTGL = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 1, 5);
   fCleanup->Add(fTGL);
   fCleanup->Add(fF1);
   tf->AddFrame(fF1, fTGL);

   // set up fields for ring properties   
   //nb_telescopes
   lab = new TGLabel(fF1, "Nb_telescopes=");
   fF1->AddFrame(lab, fTGL);
   fCleanup->Add(lab);
   // new widget associating this ring with this function
   widg = new KVBrowserWidget(kvr, SET_NUMBER_TELESCOPES_RING);
   nent =
       new TGNumberEntry(fF1, kvr->GetTelescopes()->GetSize(), 5,
                         widg->GetID(), TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEAPositive,
                         TGNumberFormat::kNELLimitMinMax, 0, 1000);
   // number entries for all Ring properties will be dealt with by KVRingBrowser
   nent->Associate(this);
   // give pointer to number entry field to the KVWidget for this widget
   // will be deleted with the widget when dtor is called
   widg->SetWidget(nent);
   // add widget to widget list in order to keep track
   AddToWidgetList(widg);
   fF1->AddFrame(nent, fTGL);

   //theta_min
   lab = new TGLabel(fF1, "Theta_min=");
   fF1->AddFrame(lab, fTGL);
   fCleanup->Add(lab);
   widg = new KVBrowserWidget(kvr, SET_THETA_MIN_RING);
   nent =
       new TGNumberEntry(fF1, kvr->GetThetaMin(), 5,
                         widg->GetID(), TGNumberFormat::kNESReal,
                         TGNumberFormat::kNEAPositive,
                         TGNumberFormat::kNELLimitMinMax, 0.0, 180.0);
   nent->Associate(this);
   widg->SetWidget(nent);
   AddToWidgetList(widg);
   fF1->AddFrame(nent, fTGL);

   //theta_max
   lab = new TGLabel(fF1, "Theta_max=");
   fF1->AddFrame(lab, fTGL);
   fCleanup->Add(lab);
   widg = new KVBrowserWidget(kvr, SET_THETA_MAX_RING);
   nent =
       new TGNumberEntry(fF1, kvr->GetThetaMax(), 5,
                         widg->GetID(), TGNumberFormat::kNESReal,
                         TGNumberFormat::kNEAPositive,
                         TGNumberFormat::kNELLimitMinMax, 0.0, 180.0);
   nent->Associate(this);
   widg->SetWidget(nent);
   AddToWidgetList(widg);
   fF1->AddFrame(nent, fTGL);

   //phi_min
   lab = new TGLabel(fF1, "Phi_min=");
   fF1->AddFrame(lab, fTGL);
   fCleanup->Add(lab);
   widg = new KVBrowserWidget(kvr, SET_PHI_MIN_RING);
   nent =
       new TGNumberEntry(fF1, kvr->GetPhiMin(), 5,
                         widg->GetID(), TGNumberFormat::kNESReal,
                         TGNumberFormat::kNEAPositive,
                         TGNumberFormat::kNELLimitMinMax, 0.0, 360.0);
   nent->Associate(this);
   widg->SetWidget(nent);
   AddToWidgetList(widg);
   fF1->AddFrame(nent, fTGL);

   //phi_max
   lab = new TGLabel(fF1, "Phi_max=");
   fF1->AddFrame(lab, fTGL);
   fCleanup->Add(lab);
   widg = new KVBrowserWidget(kvr, SET_PHI_MAX_RING);
   nent =
       new TGNumberEntry(fF1, kvr->GetPhiMax(), 5,
                         widg->GetID(), TGNumberFormat::kNESReal,
                         TGNumberFormat::kNEAPositive,
                         TGNumberFormat::kNELLimitMinMax, 0.0, 360.0);
   nent->Associate(this);
   widg->SetWidget(nent);
   AddToWidgetList(widg);
   fF1->AddFrame(nent, fTGL);

//____________________________________________________________________________________
//              DESCRIPTION OF TELESCOPES IN THE RING
//____________________________________________________________________________________          
   // set out description of ring telescopes in a TGGroupFrame
   // attached to the current ring tab
   TGGroupFrame *fGrpFrame;
   fGrpFrame = new TGGroupFrame(tf, "Telescopes");
   fTGL =
       new TGLayoutHints(kLHintsNormal | kLHintsExpandX | kLHintsExpandY,
                         2, 2, 0, 0);
   fCleanup->Add(fTGL);
   fCleanup->Add(fGrpFrame);
   tf->AddFrame(fGrpFrame, fTGL);
   LayoutTelescopes(kvr, fGrpFrame);
//____________________________________________________________________________________
//              BOTTOM ROW OF RING TAB - ADD TELESCOPE, REMOVE RING BUTTONS
//____________________________________________________________________________________
   // Put "Add Telescope" and "Remove Ring" buttons at bottom of each Ring tab
   fF1 = new TGCompositeFrame(tf, 400, 20, kHorizontalFrame | kFixedWidth);
   widg = new KVBrowserWidget(kvr, ADD_TELESCOPE);
   TGTextButton *fTB1 =
       new TGTextButton(fF1, "&Add Telescope", widg->GetID());
   fTB1->Associate(this);
   widg->SetWidget(fTB1);
   AddToWidgetList(widg);
   fTB1->SetToolTipText("Add a telescope to this ring");
   fTB1->ChangeBackground(green);

   widg = new KVBrowserWidget(kvr, REMOVE_RING);
   TGTextButton *fTB2 =
       new TGTextButton(fF1, "&Remove Ring", widg->GetID());
   fTB2->Associate(this);
   widg->SetWidget(fTB2);
   AddToWidgetList(widg);
   fTB2->SetToolTipText("Remove this ring from the array");
   fTB2->ChangeBackground(red);

   fTGL =
       new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2,
                         2, 2);
   fCleanup->Add(fTGL);
   fF1->AddFrame(fTB1, fTGL);
   fF1->AddFrame(fTB2, fTGL);

   fF1->Resize(400, fTB1->GetDefaultHeight());
   fTGL = new TGLayoutHints(kLHintsBottom | kLHintsLeft, 5, 5, 1, 1);
   fCleanup->Add(fTGL);
   tf->AddFrame(fF1, fTGL);
}

//________________________________________________________________________________________

void KVRingBrowser::LayoutTelescopes(KVRing * fRing,
                                     TGGroupFrame * fGrpFrame)
{

   // displays the list of all the telescopes in the current ring

   TGCompositeFrame *fHFrame = new TGCompositeFrame(fGrpFrame, 900, 600,
                                                    kHorizontalFrame |
                                                    kFixedWidth);
   //horizontal frame to lay out vertical frames containing telescopes side by side
   TGLayoutHints *fL1 =
       new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX |
                         kLHintsExpandY,
                         0, 0, 5, 0);
   fCleanup->Add(fL1);
   fCleanup->Add(fHFrame);
   fGrpFrame->AddFrame(fHFrame, fL1);

   ULong_t yellow;
   fClient->GetColorByName("yellow", yellow);

   TGCompositeFrame *TF1 = 0, *TF2 = 0;
   TGTextButton *TGTB;

   TGLayoutHints *fL2 =
       new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 1, 1,
                         1, 1);
   fCleanup->Add(fL2);
   TGLayoutHints *fL3 =
       new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,
                         0, 15, 5, 0);
   fCleanup->Add(fL3);
   TGLayoutHints *fL4 =
       new TGLayoutHints(kLHintsTop | kLHintsLeft, 1, 1, 1, 1);
   fCleanup->Add(fL4);

   TGNumberEntry *nent;
   TGLabel *lab;
   KVBrowserWidget *widg;
   TIter nxt_tel(fRing->GetTelescopes());
   KVTelescope *tele;
   UInt_t count = 0;

   while ((tele = (KVTelescope *) nxt_tel())) { // loop over telescopes

      if (!count) {
         TF1 =
             new TGCompositeFrame(fHFrame, 450, 600,
                                  kVerticalFrame | kFixedWidth);
         fHFrame->AddFrame(TF1, fL3);
         fCleanup->Add(TF1);
      }
      //a horizontal frame to layout the telescope button and angles
      TF2 =
          new TGCompositeFrame(TF1, 450, 20,
                               kHorizontalFrame | kFixedWidth);
      TF1->AddFrame(TF2, fL2);
      fCleanup->Add(TF2);
      //add button to open telescope browser
      widg = new KVBrowserWidget(tele, LAUNCH_BROWSER_TELESCOPE);
      TGTB = new TGTextButton(TF2, tele->GetName(), widg->GetID());
      TGTB->Associate(this);
      AddToWidgetList(widg);
      widg->SetWidget(TGTB);
      TGTB->ChangeBackground(yellow);
      TGTB->SetToolTipText("Edit telescope detectors etc.");
      TF2->AddFrame(TGTB, fL4);
      //phi min
      lab = new TGLabel(TF2, "Phi_min=");
      fCleanup->Add(lab);
      TF2->AddFrame(lab, fL2);
      widg = new KVBrowserWidget(tele, SET_PHI_MIN_TELESCOPE);
      nent =
          new TGNumberEntry(TF2, tele->GetPhiMin(), 5,
                            widg->GetID(), TGNumberFormat::kNESReal,
                            TGNumberFormat::kNEAPositive,
                            TGNumberFormat::kNELLimitMinMax, 0.0, 360.0);
      nent->Associate(this);
      widg->SetWidget(nent);
      AddToWidgetList(widg);
      TF2->AddFrame(nent, fL2);
      //phi max
      lab = new TGLabel(TF2, "Phi_max=");
      TF2->AddFrame(lab, fL2);
      fCleanup->Add(lab);
      widg = new KVBrowserWidget(tele, SET_PHI_MAX_TELESCOPE);
      nent =
          new TGNumberEntry(TF2, tele->GetPhiMax(), 5,
                            widg->GetID(), TGNumberFormat::kNESReal,
                            TGNumberFormat::kNEAPositive,
                            TGNumberFormat::kNELLimitMinMax, 0.0, 360.0);
      nent->Associate(this);
      widg->SetWidget(nent);
      AddToWidgetList(widg);
      TF2->AddFrame(nent, fL2);
      count++;
      if (count == 16)
         count = 0;
   }
}

void KVRingBrowser::CloseWindow()
{
   // Called when window is closed via the window manager.

   delete this;
}

Bool_t KVRingBrowser::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.

   KVBrowserWidget *widg = (KVBrowserWidget *) fWidgets->GetWidget(parm1);

   switch (GET_MSG(msg)) {

   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {

      case kCM_BUTTON:
         // printf("Button was pressed, id = %ld, widget action...\n", parm1);
         if (widg)
            widg->Action();
         break;

      default:
         break;
      }
   case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) {

      case kTE_TEXTCHANGED:
         // printf("Text changed, Wid = %ld\n", parm1);
         if (widg)
            widg->Action();
         break;

         /*                              case kTE_ENTER:
            printf("Enter pressed, Wid = %ld\n", parm1);
            if(widg) widg->Action();
            break;
          */
      default:
         break;

      }
   default:
      break;
   }
   return kTRUE;
}
