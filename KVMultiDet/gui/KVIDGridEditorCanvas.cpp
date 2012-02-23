//Created by KVClassFactory on Fri Feb 17 17:47:35 2012
//Author: dgruyer

#include "KVIDGridEditorCanvas.h"
#include "TROOT.h"
#include "TBox.h"
#include "TAxis.h"
#include "TContextMenu.h"

ClassImp(KVIDGridEditorCanvas)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDGridEditorCanvas</h2>
Classe basée sur TCanvas avec surcharge de la méthode 'HandleInput'
de manière à être sensible au mouvement de molette (kWheelUp/kWheelDown),
et à suprimer la sensibilité des TPavaLabel.
Cette Classe à été créée pour être urilisée par la classe KVIDGridEditor.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDGridEditorCanvas::KVIDGridEditorCanvas()
{
   // Default constructor
}

KVIDGridEditorCanvas::~KVIDGridEditorCanvas()
{
   // Destructor
}

//________________________________________________________________
bool KVIDGridEditorCanvas::IsLogz()
{
  return fLogz;
}

//________________________________________________________________
bool KVIDGridEditorCanvas::IsLogy()
{
  return fLogy;
}

//________________________________________________________________
bool KVIDGridEditorCanvas::IsLogx()
{
  return fLogx;
}

//______________________________________________________________________________
void KVIDGridEditorCanvas::RunAutoExec()
{
   // Execute the list of TExecs in the current pad.

   if (!TestBit(kAutoExec)) return;
   if (!gPad) return;
   ((TPad*)gPad)->AutoExec();
}

//______________________________________________________________________________
void KVIDGridEditorCanvas::DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected)
{
   // Report name and title of primitive below the cursor.
   //
   //    This function is called when the option "Event Status"
   //    in the canvas menu "Options" is selected.

   const Int_t kTMAX=256;
   static char atext[kTMAX];

   if (!TestBit(kShowEventStatus) || !selected) return;

   if (!fCanvasImp) return; //this may happen when closing a TAttCanvas

   TVirtualPad* savepad;
   savepad = gPad;
   gPad = GetSelectedPad();

   fCanvasImp->SetStatusText(selected->GetTitle(),0);
   fCanvasImp->SetStatusText(selected->GetName(),1);
   if (event == kKeyPress)
      snprintf(atext, kTMAX, "%c", (char) px);
   else
      snprintf(atext, kTMAX, "%d,%d", px, py);
   fCanvasImp->SetStatusText(atext,2);
   fCanvasImp->SetStatusText(selected->GetObjectInfo(px,py),3);
   gPad = savepad;
}

//______________________________________________________________________________
void KVIDGridEditorCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
   // Handle Input Events.
   //
   //  Handle input events, like button up/down in current canvas.

   TPad    *pad;
   TPad    *prevSelPad = (TPad*) fSelectedPad;
   TObject *prevSelObj = fSelected;

   fPadSave = (TPad*)gPad;
   cd();        // make sure this canvas is the current canvas

   fEvent  = event;
   fEventX = px;
   fEventY = py;

   switch (event) {

   case kMouseMotion:
      // highlight object tracked over
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      EnterLeave(prevSelPad, prevSelObj);

      gPad = pad;   // don't use cd() we will use the current
                    // canvas via the GetCanvas member and not via
                    // gPad->GetCanvas

      fSelected->ExecuteEvent(event, px, py);

      RunAutoExec();

      break;

   case kMouseEnter:
      // mouse enters canvas
      if (!fDoubleBuffer) FeedbackMode(kTRUE);
      break;

   case kMouseLeave:
      // mouse leaves canvas
      {
         // force popdown of tooltips
         TObject *sobj = fSelected;
         TPad    *spad = fSelectedPad;
         fSelected     = 0;
         fSelectedPad  = 0;
         EnterLeave(prevSelPad, prevSelObj);
         fSelected     = sobj;
         fSelectedPad  = spad;
         if (!fDoubleBuffer) FeedbackMode(kFALSE);
      }
      break;

   case kButton1Double:
      // triggered on the second button down within 350ms and within
      // 3x3 pixels of the first button down, button up finishes action

   case kButton1Down:
      // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system

      if (fSelected) {
         FeedbackMode(kTRUE);   // to draw in rubberband mode
         if(!fSelected->InheritsFrom("TPaveLabel")) fSelected->ExecuteEvent(event, px, py);

         RunAutoExec();
      }

      break;

   case kButton1Motion:
   case kButton1ShiftMotion: //8 == kButton1Motion + shift modifier
      if (fSelected) {
         gPad = fSelectedPad;

         if(!fSelected->InheritsFrom("TPaveLabel")) fSelected->ExecuteEvent(event, px, py);
         gVirtualX->Update();

         if ((!fSelected->InheritsFrom(TAxis::Class()))&&(!fSelected->InheritsFrom("TPaveLabel"))) {
            Bool_t resize = kFALSE;
            if (fSelected->InheritsFrom(TBox::Class()))
               resize = ((TBox*)fSelected)->IsBeingResized();
            if (fSelected->InheritsFrom(TVirtualPad::Class()))
               resize = ((TVirtualPad*)fSelected)->IsBeingResized();

            if ((!resize && TestBit(kMoveOpaque)) || (resize && TestBit(kResizeOpaque))) {
               gPad = fPadSave;
               Update();
               FeedbackMode(kTRUE);
            }
         }

         RunAutoExec();
      }

      break;

   case kButton1Up:

      if (fSelected) {
         gPad = fSelectedPad;

         if(!fSelected->InheritsFrom("TPaveLabel")) fSelected->ExecuteEvent(event, px, py);

         RunAutoExec();

         if (fPadSave)
            gPad = fPadSave;
         else {
            gPad     = this;
            fPadSave = this;
         }

         Update();    // before calling update make sure gPad is reset
      }
      break;

//*-*----------------------------------------------------------------------

   case kButton2Down:
      // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system

      FeedbackMode(kTRUE);

      if(!fSelected->InheritsFrom("TH1")) fSelected->Pop();           	// pop object to foreground
      pad->cd();                  					// and make its pad the current pad
      if (gDebug)
         printf("Current Pad: %s / %s\n", pad->GetName(), pad->GetTitle());

      // loop over all canvases to make sure that only one pad is highlighted
      {
         TIter next(gROOT->GetListOfCanvases());
         TCanvas *tc;
         while ((tc = (TCanvas *)next()))
            tc->Update();
      }

      /*if (pad->GetGLDevice() != -1 && fSelected)
         fSelected->ExecuteEvent(event, px, py);*/

      break;   // don't want fPadSave->cd() to be executed at the end

   case kButton2Motion:
      //was empty!
   case kButton2Up:
      if (fSelected) {
         gPad = fSelectedPad;

         fSelected->ExecuteEvent(event, px, py);
         RunAutoExec();
      }
      break;

   case kButton2Double:
      break;

//*-*----------------------------------------------------------------------

   case kButton3Down:
      // popup context menu
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      if (!fDoubleBuffer) FeedbackMode(kFALSE);

      if (fContextMenu && !fSelected->TestBit(kNoContextMenu) &&
         !pad->TestBit(kNoContextMenu) && !TestBit(kNoContextMenu) && (!fSelected->InheritsFrom("TPaveLabel")))
         fContextMenu->Popup(px, py, fSelected, this, pad);

      break;

   case kButton3Motion:
      break;

   case kButton3Up:
      if (!fDoubleBuffer) FeedbackMode(kTRUE);
      break;

   case kButton3Double:
      break;

   case kKeyPress:
      if (!fSelectedPad || !fSelected) return;
      gPad = fSelectedPad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system
      fSelected->ExecuteEvent(event, px, py);

      RunAutoExec();

      break;
   case kButton1Shift:
      // Try to select
      pad = Pick(px, py, prevSelObj);

      if (!pad) return;

      EnterLeave(prevSelPad, prevSelObj);

      gPad = pad;   // don't use cd() we will use the current
                    // canvas via the GetCanvas member and not via
                    // gPad->GetCanvas
      fSelected->ExecuteEvent(event, px, py);
      RunAutoExec();

      break;
   case kWheelUp:
   case kWheelDown:
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;
      if(!fSelected->InheritsFrom("TAxis")) fSelected->ExecuteEvent(event, px, py);
      
      RunAutoExec();
      
      break;
   default:
      break;
   }

   if (fPadSave && event != kButton2Down)
      fPadSave->cd();

   if (event != kMouseLeave) { // signal was already emitted for this event
      ProcessedEvent(event, px, py, fSelected);  // emit signal
      DrawEventStatus(event, px, py, fSelected);
   }
}
