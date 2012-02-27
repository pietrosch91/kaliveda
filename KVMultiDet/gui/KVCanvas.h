//Created by KVClassFactory on Fri Feb 17 17:47:35 2012
//Author: dgruyer

#ifndef __KVCanvas_H
#define __KVCanvas_H

#include "TCanvas.h"
#include "TH2.h"

class KVCanvas : public TCanvas
{

   protected:
   Double_t xmin,xmax,ymin,ymax;
   Double_t oldx, oldy;
   Bool_t   moved;

   public:
   KVCanvas();
   KVCanvas(const char* name, const char* title, Int_t ww, Int_t wh):TCanvas(name,title,ww,wh){};
   virtual ~KVCanvas();
   
   void HandleInput(EEventType event, Int_t px, Int_t py);
   Bool_t IsLogz();
   Bool_t IsLogy();
   Bool_t IsLogx();
   
   protected:
   void RunAutoExec();
   void DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);
   void ZoomSelected(TH2* TheHisto);

   ClassDef(KVCanvas,1)//bidouille
};

#endif
