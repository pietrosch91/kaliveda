//$Id: KVDetectorBrowser.cpp,v 1.8 2006/10/19 14:32:43 franklan Exp $
#include "Riostream.h"
#include "KVDetectorBrowser.h"
#include "KVMaterial.h"
#include "KVWidget.h"
#include "KVDetector.h"
#include "KVWidgetList.h"

ClassImp(KVDetectorBrowser)
///////////////////////////////////////////////////////////////////////////////////////////////////////
// KaliVeda detector Browser and Configuration Tool
//////////////////////////////////////////////////////////////////////////////////////////////////////
//______________________________________________________________________________________                                                                                                                                                                                                                                                                                                                                                                                                                                      
KVDetectorBrowser::KVDetectorBrowser(const TGWindow * p,
                                         KVDetector * d, UInt_t w,
                                         UInt_t h):TGMainFrame(p, w, h)
{

   // pointer to detector under examination                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
   fDet = d;
//_______________________________________________________________________________________
// Create top level window.
//______________________________________________________________________________________

   //combo box for setting detector material
   KVWidget *widg = new KVWidget(fDet, kDET_CHOOSE_MATERIAL);
   TGComboBox *fCombo = new TGComboBox(this, widg->GetID());
   fCombo->Associate(this);
   widg->SetWidget(fCombo);
   AddToWidgetList(widg);
   // fill list with all available materials
   KVList *mat_list = fDet->GetListOfMaterials();
   TIter next_mat(mat_list);
   KVMaterial *mat;
   Int_t i = 0, iselect = -1;
   while ((mat = (KVMaterial *) next_mat())) {
      if (!strcmp(mat->GetName(), fDet->GetMaterialName()))
         iselect = i;
      fCombo->AddEntry(mat->GetName(), i++);
   }
   //highlight the material from which detector is made
   if (iselect >= 0)
      fCombo->Select(iselect);
   delete mat_list;             //temporary list of materials

   TGLayoutHints *lay =
       new TGLayoutHints(kLHintsCenterX | kLHintsTop, 2, 2, 2, 2);
   AddFrame(fCombo, lay);
   fCombo->Resize(150, 20);
   AddToCleanupList(lay);

//display thickness of detector with appropriate units
   widg = new KVWidget(fDet, kDET_SET_THICKNESS);
   TGNumberEntry *fNumEnt =
       new TGNumberEntry(this, fDet->GetThickness(), 5, widg->GetID(),
                         TGNumberFormat::kNESRealOne);
   fNumEnt->Associate(this);
   widg->SetWidget(fNumEnt);
   AddToWidgetList(widg);
   AddFrame(fNumEnt, lay);

//______________________________________________________________________________________
   // Display main window 
   MapSubwindows();
   Layout();
   Resize(200, 200);

   SetWindowName(fDet->GetName());
   SetIconName("KVBrowser");
   MapWindow();
}

//_________________________________________________________________________________________

void KVDetectorBrowser::CloseWindow()
{
   fDet->CloseBrowser();
}

//________________________________________________________________________________

Bool_t KVDetectorBrowser::ProcessMessage(Long_t msg, Long_t parm1,
                                         Long_t parm2)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.

   // get associated widget from widget list using calling widget ID (parm1)
   KVWidget *widg = 0;
   switch (GET_MSG(msg)) {

   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {

      case kCM_BUTTON:
         //printf("Button was pressed, id = %ld, widget action...\n", parm1);
         if (GetWidgetList())
            widg = GetWidgetList()->GetWidget(parm1);
         if (widg)
            widg->Action();
         break;

      case kCM_COMBOBOX:
         if (GetWidgetList())
            widg = GetWidgetList()->GetWidget(parm1);
         if (widg)
            widg->Action(parm2);

      default:
         break;
      }

   case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) {

      case kTE_TEXTCHANGED:
         cout << "TEXTCHANGED" << endl;
         if (GetWidgetList())
            widg = GetWidgetList()->GetWidget(parm1);
         if (widg)
            widg->Action();
      }

   default:
      break;
   }
   return kTRUE;
}
