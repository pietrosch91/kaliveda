//Created by KVClassFactory on Fri Feb 17 17:47:35 2012
//Author: dgruyer

#ifndef __KVCanvas_H
#define __KVCanvas_H

#include "TCanvas.h"
#include "TH2.h"
#include "TGFrame.h"


class KVCanvas : public TCanvas
{
friend class KVKeyHandler;

   protected:
   Double_t xmin,xmax,ymin,ymax;
   Double_t oldx, oldy;
      					// variables for pan & scan
   Int_t X0, Y0;  			// coordinates of initial click in pad pixels
   Int_t NdisXbins,NdisYbins;  		// number of displayed bins on X & Y
   Int_t NXbins, Xfirst0, Xlast0; 	// number of bins on x-axis, initial displayed bins
   Int_t NYbins, Yfirst0, Ylast0; 	// number of bins on y-axis, initial displayed bins
   TAxis *theXaxis, *theYaxis;  	// the axes of the histogram
   Double_t XbinPixel, YbinPixel; 	// size of bins in pixels
   Int_t Xf1,Xl1,Yf1,Yl1; 		// last modification to axis limits
   
   Bool_t   moved;
   Bool_t   fAgeOfEmpire;
   Bool_t   fModeVener;

   TGFrame* fKeyHandler;         // handler for arrow keys
   
   public:
   KVCanvas();
//   using TCanvas::TCanvas;
   KVCanvas(const char* name, const char* title, Int_t ww, Int_t wh);
   KVCanvas(const char* name, Int_t ww, Int_t wh, Int_t wimid);
   virtual ~KVCanvas();
   
   void HandleInput(EEventType event, Int_t px, Int_t py);
   Bool_t IsLogz();
   Bool_t IsLogy();
   Bool_t IsLogx();
   
   protected:
   
   virtual Bool_t HandleKey(Event_t *event);
   void DynamicZoom(Int_t Sign, Int_t px, Int_t py);
   void RunAutoExec();
   void DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);
   void ZoomSelected(TH2* TheHisto);

   ClassDef(KVCanvas,1)//bidouille
};

#endif
