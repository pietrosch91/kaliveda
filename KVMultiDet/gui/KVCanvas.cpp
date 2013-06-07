//Created by KVClassFactory on Fri Feb 17 17:47:35 2012
//Author: dgruyer

#include "KVCanvas.h"
#include "TROOT.h"
#include "TBox.h"
#include "TAxis.h"
#include "TContextMenu.h"
#include "TRootCanvas.h"
#include "TGWindow.h"
#include "KVKeyHandler.h"
#include <KeySymbols.h>

#include <Riostream.h>

ClassImp(KVCanvas)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVCanvas</h2>
TCanvas with mouse-controlled dynamic zoom and pan & scan.<br>

<img alt="KVCanvas" src="images/KVCanvas.png"><br><br>

Dynamic zoom: left-click to draw rectangle around region of interest, release mouse button to zoom in<br>
Zoom in: mouse wheel up<br>
Zoom out: mouse wheel down<br>
Pan: hold down mouse wheel (centre button) and move
<!-- */
// --> END_HTML
// KEYBOARD SHORTCUTS:
//  F9  -  X axis log/lin
//  F10 -  Y axis log/lin
//  F11 -  Z axis log/lin
//  F12 -  Zoom out (reset axes)
//  g   -  Show/hide grid on X and Y axes
//  l   -  Change log/lin mode for Y-axis (1D histo) or Z-axis (2D histo)
//  w   -  Age of Empires mode
//  v   -  Age of Empires vener mode
////////////////////////////////////////////////////////////////////////////////



//________________________________________________________________
KVCanvas::KVCanvas():TCanvas()
{
  fKeyHandler  = new KVKeyHandler(this);
  fAgeOfEmpire = false;
  fModeVener   = false;
  fHasDisabledClasses = false;
  fDisabledClasses = "";
  fFreezed = kFALSE;
   // Default constructor
}

//________________________________________________________________
KVCanvas::~KVCanvas()
{
   // Destructor
}

//________________________________________________________________
KVCanvas::KVCanvas(const char* name, const char* title, Int_t ww, Int_t wh, Bool_t keyHandler):TCanvas(name, title, ww, wh)
{
  if(keyHandler) fKeyHandler = new KVKeyHandler(this);
  fAgeOfEmpire = false;
  fModeVener   = false;
  fHasDisabledClasses = false;
  fDisabledClasses = "";
  fFreezed = kFALSE;
}

//________________________________________________________________
KVCanvas::KVCanvas(const char* name, Int_t ww, Int_t wh, Int_t winid):TCanvas(name, ww, wh, winid)
{
//  fKeyHandler = new KVKeyHandler(this);
  fAgeOfEmpire = false;
  fModeVener   = false;
  fHasDisabledClasses = false;
  fHasDisabledObject = false;
  fDisabledClasses = "";
  fFreezed = kFALSE;
}

//________________________________________________________________
bool KVCanvas::IsLogz()
{
  return fLogz;
}

//________________________________________________________________
bool KVCanvas::IsLogy()
{
  return fLogy;
}

//________________________________________________________________
bool KVCanvas::IsLogx()
{
  return fLogx;
}

//______________________________________________________________________________
void KVCanvas::RunAutoExec()
{
   // Execute the list of TExecs in the current pad.

   if (!TestBit(kAutoExec)) return;
   if (!gPad) return;
   ((TPad*)gPad)->AutoExec();
   
}

//______________________________________________________________________________
void KVCanvas::DisableClass(const char* className)
{
  fHasDisabledClasses = true;
  fDisabledClasses += className;
  
}

//______________________________________________________________________________
void KVCanvas::ResetDisabledClass()
{
  fHasDisabledClasses = false;
  fDisabledClasses = "";
}

//______________________________________________________________________________
void KVCanvas::ResetDisabledObject()
{
  fHasDisabledObject = true;
  fDisabledObjects.Clear();
}

//______________________________________________________________________________
void KVCanvas::DisableObject(TObject* obj)
{
  if(!obj) return;
  fHasDisabledObject = true;
  fDisabledObjects.AddLast(obj);
}

//______________________________________________________________________________
void KVCanvas::DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected)
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
void KVCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
   // Handle Input Events.
   //
   //  Handle input events, like button up/down in current canvas.
   
   if(fFreezed) return;

   TPad    *pad;
   TPad    *prevSelPad = (TPad*) fSelectedPad;
   TObject *prevSelObj = fSelected;
    
   fPadSave = (TPad*)gPad;
   cd();        // make sure this canvas is the current canvas

   fEvent  = event;
   fEventX = px;
   fEventY = py;
   
   Int_t sign = 0;
   Bool_t sendOrder = true;
   
   if(fHasDisabledClasses&&fSelected)
     {
     if(fDisabledClasses.Contains(fSelected->ClassName())) sendOrder = false;
     }
     
   if(fHasDisabledObject&&fSelected)
     {
     if(fDisabledObjects.Contains(fSelected)) sendOrder = false;
     }
   
   switch (event) {

   case kMouseMotion:
      // highlight object tracked over
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      EnterLeave(prevSelPad, prevSelObj);
	
      gPad = pad;   // don't use cd() we will use the current
                    // canvas via the GetCanvas member and not via
                    // gPad->GetCanvas

      if(sendOrder) fSelected->ExecuteEvent(event, px, py);

      RunAutoExec();
      
      if(fAgeOfEmpire&&(fSelected->InheritsFrom("TH2")))
        {
        TH2* TheHisto = (TH2*) fSelected;
	
        Double_t size = 0.4-0.35*fModeVener;
        
        Int_t dX = 0;
        Int_t dY = 0;
      
        px = AbsPixeltoX(px);
        py = AbsPixeltoY(py);
  
        TAxis* ax = TheHisto->GetXaxis();    
        Int_t X0 = ax->GetFirst();
        Int_t X1 = ax->GetLast();
        Int_t NbinsX = ax->GetNbins();
        px = ax->FindBin(px);
      
        Double_t ddX   = (X1+X0)*0.5 - px;
        Double_t distX = TMath::Abs(ddX)/(X1-X0);
        if(distX>=0.5) return;
        
        TAxis* ay = TheHisto->GetYaxis();    
        Int_t Y0 = ay->GetFirst();
        Int_t Y1 = ay->GetLast();
        Int_t NbinsY = ay->GetNbins();
        py = ay->FindBin(py);
     
        Double_t ddY   = (Y1+Y0)*0.5 - py;
        Double_t distY = TMath::Abs(ddY)/(Y1-Y0);
        if(distY>=0.5) return;
       
        if((distX<=size)&&(distY<=size)) return;
        dX = (Int_t)ddX*(0.05 + 0.05*fModeVener);
        dY = (Int_t)ddY*(0.05 + 0.05*fModeVener);
    
        Bool_t up = false;
        if((X0-dX>0)&&(X1-dX<NbinsX)) 
          {
          ax->SetRange(X0-dX,X1-dX);
          up = true;
          }
        if((Y0-dY>0)&&(Y1-dY<NbinsY)) 
          {
          ay->SetRange(Y0-dY,Y1-dY);
          up = true;
          }
        if(up) 
          {
  	  Modified();
	  Update();
	  }
        } 

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
         fSelected->ExecuteEvent(event, px, py);

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

         if(sendOrder) fSelected->ExecuteEvent(event, px, py);
         gVirtualX->Update();

         if (!fSelected->InheritsFrom(TAxis::Class())) {
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

         if(sendOrder) fSelected->ExecuteEvent(event, px, py);

         RunAutoExec();

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
      //was empty!
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
      
      
   case kButton2Up:
      if (fSelected) {
         gPad = fSelectedPad;

         if(sendOrder) fSelected->ExecuteEvent(event, px, py);
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

      if (fContextMenu && !fSelected->TestBit(kNoContextMenu) && !pad->TestBit(kNoContextMenu) && !TestBit(kNoContextMenu))
      {
          if(sendOrder) fContextMenu->Popup(px, py, fSelected, this, pad);
          else fSelected->ExecuteEvent(event, px, py);
      }

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

      sign = (event==kWheelUp ? 1:-1);
    
      gPad = pad;
//      if(!fSelected->InheritsFrom("TAxis")) fSelected->ExecuteEvent(event, px, py);
      if(fSelected->InheritsFrom("TH2")) DynamicZoom(sign,px,py);
      else if(fSelected->InheritsFrom("TH1")) DynamicZoomTH1(sign,px,py);
      
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

//________________________________________________________________
void KVCanvas::ZoomSelected(TH2* TheHisto)
{
  if(!TheHisto) return;
  TAxis* ax = TheHisto->GetXaxis();
  
  Double_t ratio1 = (xmin - GetUxmin())/(GetUxmax() - GetUxmin());
  Double_t ratio2 = (xmax - GetUxmin())/(GetUxmax() - GetUxmin());
  if((ratio2-ratio1 > 0.05)) ax->SetRangeUser(xmin, xmax);
  
  ax = TheHisto->GetYaxis();
  
  ratio1 = (ymin - GetUymin())/(GetUymax() - GetUymin());
  ratio2 = (ymax - GetUymin())/(GetUymax() - GetUymin());
  if((ratio2-ratio1 > 0.05)) ax->SetRangeUser(ymin, ymax);
  
  xmax = xmin = ymax = ymin = 0.;
  return;
}

//________________________________________________________________
void KVCanvas::DynamicZoomTH1(Int_t Sign, Int_t px, Int_t py)
{
   // Zoom in or out of histogram with mouse wheel
   
  if(!fSelected) return;
  TH1* TheHisto = (TH1*) fSelected;

  Double_t percent = 0.15-Sign*0.05;
  
  Int_t dX = 0;
  
  px = AbsPixeltoX(px);
    
  TAxis* ax = TheHisto->GetXaxis();
  Int_t NbinsXtmp = ax->GetNbins();
  Int_t X0tmp = ax->GetFirst();
  Int_t X1tmp = ax->GetLast();
  Int_t step = TMath::Min(TMath::Max(1, (Int_t)(percent*(X1tmp-X0tmp))),NbinsXtmp/2);
  step*=Sign;
  X0tmp = TMath::Min(TMath::Max(X0tmp+step,1),X1tmp-step);
  X1tmp = TMath::Max(TMath::Min(X1tmp-step,NbinsXtmp),X0tmp);
  if(X0tmp>=X1tmp) X0tmp=X1tmp-1;
  if(Sign>0) dX = (Int_t) (X0tmp + (X1tmp-X0tmp)*0.5 - ax->FindBin(px));
  if((X0tmp-dX)<0) ax->SetRange(0,X1tmp-X0tmp);
  else if((X1tmp-dX)>ax->GetNbins()) ax->SetRange(ax->GetNbins()-(X1tmp-X0tmp),ax->GetNbins());
  else ax->SetRange(X0tmp-dX,X1tmp-dX);
  
  
  Modified();
  Update();
  return;
}

//________________________________________________________________
void KVCanvas::DynamicZoom(Int_t Sign, Int_t px, Int_t py)
{
   // Zoom in or out of histogram with mouse wheel
   
  if(!fSelected) return;
  TH2* TheHisto = (TH2*) fSelected;

  Double_t percent = 0.15-Sign*0.05;
  
  Int_t dX = 0;
  Int_t dY = 0;
  
  px = AbsPixeltoX(px);
  py = AbsPixeltoY(py);
    
  TAxis* ax = TheHisto->GetXaxis();
  Int_t NbinsXtmp = ax->GetNbins();
  Int_t X0tmp = ax->GetFirst();
  Int_t X1tmp = ax->GetLast();
  Int_t step = TMath::Min(TMath::Max(1, (Int_t)(percent*(X1tmp-X0tmp))),NbinsXtmp/2);
  step*=Sign;
  X0tmp = TMath::Min(TMath::Max(X0tmp+step,1),X1tmp-step);
  X1tmp = TMath::Max(TMath::Min(X1tmp-step,NbinsXtmp),X0tmp);
  if(X0tmp>=X1tmp) X0tmp=X1tmp-1;
  if(Sign>0) dX = (Int_t) (X0tmp + (X1tmp-X0tmp)*0.5 - ax->FindBin(px));
  if((X0tmp-dX)<0) ax->SetRange(0,X1tmp-X0tmp);
  else if((X1tmp-dX)>ax->GetNbins()) ax->SetRange(ax->GetNbins()-(X1tmp-X0tmp),ax->GetNbins());
  else ax->SetRange(X0tmp-dX,X1tmp-dX);
  
  ax = TheHisto->GetYaxis();
  Int_t NbinsYtmp = ax->GetNbins();
  Int_t Y0tmp = ax->GetFirst();
  Int_t Y1tmp = ax->GetLast();  
  step = TMath::Min(TMath::Max(1, (Int_t)(percent*(Y1tmp-Y0tmp))),NbinsYtmp/2);
  step*=Sign;
  Y0tmp = TMath::Min(TMath::Max(Y0tmp+step,1),Y1tmp-step);
  Y1tmp = TMath::Max(TMath::Min(Y1tmp-step,NbinsYtmp),Y0tmp);
  if(Y0tmp>=Y1tmp) Y0tmp=Y1tmp-1;
  if(Sign>0) dY = (Int_t) (Y0tmp + (Y1tmp-Y0tmp)*0.5 - ax->FindBin(py));
  if((Y0tmp-dY)<0) ax->SetRange(0,Y1tmp-Y0tmp);
  else if((Y1tmp-dY)>ax->GetNbins()) ax->SetRange(ax->GetNbins()-(Y1tmp-Y0tmp),ax->GetNbins());
  else ax->SetRange(Y0tmp-dY,Y1tmp-dY);
  
  Modified();
  Update();
  return;
}

//________________________________________________________________
Bool_t KVCanvas::HandleKey(Event_t *event)
{
   // Handle keys
      
   char tmp[2];
   UInt_t keysym;

   gVirtualX->LookupString(event, tmp, sizeof(tmp), keysym);
   
   if (event->fType == kGKeyPress) {
      switch ((EKeySym)keysym) {
         case kKey_F1:
            break;    
	    
         case kKey_F2:
            break;    
	    
         case kKey_F3:
            break;    
	    
         case kKey_F4:
            break;    
	    
         case kKey_F5:
            break;
	    
         case kKey_F6:
            break;
	    
         case kKey_F7:
            break;
	    
         case kKey_F8:
            break;
	    
         case kKey_F9:
	    SetLogx(!fLogx);
	    Modified();
	    Update();
            break;
	    
         case kKey_F10:
	    SetLogy(!fLogy);
	    Modified();
	    Update();
            break;
	    
         case kKey_F11:
	    SetLogz(!fLogz);
	    Modified();
	    Update();
            break;
	    
         case kKey_F12:
	    if(fSelected->InheritsFrom("TH2"))
	      {
	      ((TH2*)fSelected)->GetXaxis()->UnZoom();
	      ((TH2*)fSelected)->GetYaxis()->UnZoom();
	      Modified();
	      Update();
	      }
            break;
	    
      case kKey_g:
          if(GetGridx()&&GetGridy())
              SetGrid(0,0);
          else
              SetGrid();
          Modified();
          Update();
          break;

      case kKey_l:
          if(fSelected){
              if(fSelected->InheritsFrom("TH2"))
                  SetLogz(!fLogz);
              else if(fSelected->InheritsFrom("TH1"))
                  SetLogy(!fLogy);
          }
          Modified();
          Update();
          break;

         case kKey_e:
            break;
	    
         case kKey_d:
            break;
	    
         case kKey_a:
            break;
	    
         case kKey_z:
            break;
	    
         case kKey_w:
	    fAgeOfEmpire = !fAgeOfEmpire;
            break;
	    
         case kKey_v:
	    fModeVener = !fModeVener;
            break;
	    
         case kKey_c:
            break;
	    
         case kKey_x:
            break;
	    
         case kKey_Left:
            break;
	    
         case kKey_Right:
            break;
	    
         case kKey_Down:
            break;
	    
         case kKey_Up:
            break;
	    
         case kKey_Space:
            break;
	    
         default:
            return kTRUE;
      }
   }
   return kTRUE;
}

