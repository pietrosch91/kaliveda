//Created by KVClassFactory on Fri Feb 17 17:47:35 2012
//Author: dgruyer

#include "KVIDGridEditorCanvas.h"
#include "KVIDGridEditor.h"
#include "TROOT.h"
#include "TBox.h"
#include "TAxis.h"
#include "TContextMenu.h"
#include "TRootCanvas.h"
#include "TGWindow.h"
#include <KeySymbols.h>

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

static KVIDGridEditorCanvas *gIDGridEditorCanvas;


KeyHandler::KeyHandler() : TGFrame(gClient->GetRoot(),0,0)
{
   // Key handler constructor.   
   
   TRootCanvas *main_frame = (TRootCanvas*)(gIDGridEditorCanvas->GetCanvasImp());

   // bind arrow keys and space-bar key
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_Up),    0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_Left),  0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_Right), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_Down),  0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_Space), 0);
   
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F1), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F2), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F3), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F4), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F5), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F6), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F7), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F8), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F9), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F10), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F11), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_F12), 0);
   
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_s), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_z), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_a), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_e), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_d), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_w), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_l), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_v), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_c), 0);
   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_x), 0);
   
   
   gVirtualX->SetKeyAutoRepeat(kTRUE);
   
//   main_frame->BindKey((const TGWindow*)this, gVirtualX->KeysymToKeycode(kKey_), 0);
   
}

KeyHandler::~KeyHandler()
{
   // Cleanup key handler.

   // get main frame of Tetris canvas
   TRootCanvas *main_frame = (TRootCanvas*)(gIDGridEditorCanvas->GetCanvasImp());

   // remove binding of arrow keys and space-bar key
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_Up),    0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_Left),  0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_Right), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_Down),  0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_Space), 0);
   
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F1), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F2), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F3), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F4), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F5), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F6), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F7), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F8), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F9), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F10), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F11), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_F12), 0);
   
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_s), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_z), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_a), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_e), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_d), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_w), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_l), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_v), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_c), 0);
   main_frame->RemoveBind(this, gVirtualX->KeysymToKeycode(kKey_x), 0);
   
   // restore key auto repeat functionality, was turned off in TGMainFrame::HandleKey()
   gVirtualX->SetKeyAutoRepeat(kTRUE);
   
}


Bool_t KeyHandler::HandleKey(Event_t *event)
{
   // Handle arrow and spacebar keys
   
   if(gIDGridEditor) gIDGridEditor->HandleKey(event);
   return kTRUE;
   
   char tmp[2];
   UInt_t keysym;

   gVirtualX->LookupString(event, tmp, sizeof(tmp), keysym);

   if (event->fType == kGKeyPress) {
      switch ((EKeySym)keysym) {
         case kKey_Left:
            cout << "gTetris->MoveLeft()" << endl;
            break;
         case kKey_Right:
            cout << "gTetris->MoveRight();" << endl;
            break;
         case kKey_Down:
            cout << "gTetris->RotateRight();" << endl;
            break;
         case kKey_Up:
            cout << "gTetris->RotateLeft();" << endl;
            break;
         case kKey_Space:
            cout << "gTetris->DropDown();" << endl;
            break;
         default:
            return kTRUE;
      }
   }
   return kTRUE;
}




//////////////////////////////////////////////////////////////////////////////


KVIDGridEditorCanvas::KVIDGridEditorCanvas(const char* name, const char* title, Int_t ww, Int_t wh):KVCanvas(name,title,ww,wh)
{
     gIDGridEditorCanvas = this;
     fKeyHandler = new KeyHandler();
}



KVIDGridEditorCanvas::KVIDGridEditorCanvas()
{
   // Default constructor
}

KVIDGridEditorCanvas::~KVIDGridEditorCanvas()
{
   // Destructor
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
	 
	 if(fSelected->InheritsFrom("TH2")){
            oldx = GetEventX();
            oldy = GetEventY();
            xmin = AbsPixeltoX(oldx);
            ymin = AbsPixeltoY(oldy);
            gVirtualX->DrawBox(XtoAbsPixel(xmin), YtoAbsPixel(ymin), oldx, oldy, TVirtualX::kHollow);
         }

      }

      break;

   case kButton1Motion:
      if (fSelected) {
          if(fSelected->InheritsFrom("TH2")){
             gVirtualX->DrawBox(XtoAbsPixel(xmin), YtoAbsPixel(ymin), oldx, oldy, TVirtualX::kHollow);
             oldx = GetEventX();
             oldy = GetEventY();
             gVirtualX->DrawBox(XtoAbsPixel(xmin), YtoAbsPixel(ymin), oldx, oldy, TVirtualX::kHollow);
	     moved = true;
          }
      }
   case kButton1ShiftMotion: //8 == kButton1Motion + shift modifier
      if (fSelected) {
         gPad = fSelectedPad;

         if(!fSelected->InheritsFrom("TPaveLabel")) fSelected->ExecuteEvent(event, px, py);
         gVirtualX->Update();

         if (!fSelected->InheritsFrom(TAxis::Class())&&(!fSelected->InheritsFrom("TPaveLabel")) ) {
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

         if (fPadSave)
            gPad = fPadSave;
         else {
            gPad     = this;
            fPadSave = this;
         }
         if(fSelected->InheritsFrom("TH2")&&moved){
            xmax = AbsPixeltoX(GetEventX());
            ymax = AbsPixeltoY(GetEventY());
            Double_t toto = 0;
            if(xmax<xmin){
	       toto = xmax;
	       xmax = xmin;
	       xmin = toto;
	    }
            if(ymax<ymin){
	       toto = ymax;
	       ymax = ymin;
	       ymin = toto;
	    }
            ZoomSelected((TH2*)fSelected);
            moved = false;
         }
         
         RunAutoExec();

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

      if(!fSelected->InheritsFrom("TH1")) {fSelected->Pop();           	// pop object to foreground
                                             pad->cd();                 // and make its pad the current pad
                                          }
      if(fSelected->InheritsFrom("TH2")){
         // implement pan & scan
         X0 = px; Y0 = py;  // u clikd here
         theXaxis = ((TH2*)fSelected)->GetXaxis();
         theYaxis = ((TH2*)fSelected)->GetYaxis();
         NXbins = theXaxis->GetNbins();  // maximum bin number in X
         NYbins = theYaxis->GetNbins();  // maximum bin number in Y
         Xf1 = Xfirst0 = theXaxis->GetFirst(); // initial displayed bin range in X
         Xl1 = Xlast0 = theXaxis->GetLast();
         Yf1 = Yfirst0 = theYaxis->GetFirst(); // initial displayed bin range in Y
         Yl1 = Ylast0 = theYaxis->GetLast();
         // size of axes in pixels
         Int_t pixelWidthX = gPad->XtoAbsPixel(gPad->GetUxmax()) - gPad->XtoAbsPixel(gPad->GetUxmin());
         Int_t pixelWidthY = gPad->YtoAbsPixel(gPad->GetUymax()) - gPad->YtoAbsPixel(gPad->GetUymin());
         // sizes of bins in pixels
         NdisXbins = Xlast0 - Xfirst0 + 1;
         NdisYbins = Ylast0 - Yfirst0 + 1;
         XbinPixel = pixelWidthX / (1.0 * NdisXbins); 
         YbinPixel = pixelWidthY / (1.0 * NdisYbins); 
      }
      
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
      if(fSelected && fSelected->InheritsFrom("TH2")){
         // implement pan & scan
         Int_t dX = px - X0; // how far have i moved ?
         Int_t dY = py - Y0;
         Int_t dXbins = dX / XbinPixel;
         Int_t dYbins = dY / YbinPixel;
         Bool_t changed=kFALSE;
         Int_t newXfirst = Xfirst0-dXbins;
         Int_t newXlast;
         if(newXfirst<1){
            newXfirst = 1;
            newXlast = NdisXbins;
         }
         else
         {
            newXlast = Xlast0-dXbins;
            if(newXlast>NXbins){
               newXlast = NXbins;
               newXfirst = newXlast - NdisXbins + 1;
            }
         }
         if(newXfirst!=Xf1){
            Xf1 = newXfirst;
            Xl1 = newXlast;
            theXaxis->SetRange(Xf1,Xl1);
            changed=kTRUE;
         }
         Int_t newYfirst = Yfirst0-dYbins;
         Int_t newYlast;
         if(newYfirst<1){
            newYfirst = 1;
            newYlast = NdisYbins;
         }
         else
         {
            newYlast = Ylast0-dYbins;
            if(newYlast>NYbins){
               newYlast = NYbins;
               newYfirst = newYlast - NdisYbins + 1;
            }
         }
         if(newYfirst!=Yf1){
            Yf1 = newYfirst;
            Yl1 = newYlast;
            theYaxis->SetRange(Yf1,Yl1);
            changed=kTRUE;
         }
         if(changed){Modified();Update();}
      }
      break;
      
   case kButton2Up:
      if (fSelected) {
         gPad = fSelectedPad;

         if(!fSelected->InheritsFrom("TPaveLabel")) fSelected->ExecuteEvent(event, px, py);
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
         !pad->TestBit(kNoContextMenu) && !TestBit(kNoContextMenu)&&(!fSelected->InheritsFrom("TPaveLabel")) )
         fContextMenu->Popup(px, py, fSelected, this, pad);

      break;

   case kButton3Motion:
      break;

   case kButton3Up:
      if (!fDoubleBuffer) FeedbackMode(kTRUE);
      break;

   case kButton3Double:
      break;
      
   case kESC:
       RunAutoExec();
      break;

   case kKeyPress:
      if (!fSelectedPad || !fSelected) return;
      gPad = fSelectedPad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system
      if(!fSelected->InheritsFrom("TPaveLabel")) fSelected->ExecuteEvent(event, px, py);

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
      if(!fSelected->InheritsFrom("TPaveLabel")) fSelected->ExecuteEvent(event, px, py);
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
