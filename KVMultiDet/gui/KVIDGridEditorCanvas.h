//Created by KVClassFactory on Fri Feb 17 17:47:35 2012
//Author: dgruyer

#ifndef __KVIDGridEditorCanvas_H
#define __KVIDGridEditorCanvas_H

#include "TCanvas.h"

class KVIDGridEditorCanvas : public TCanvas
{

   public:
   KVIDGridEditorCanvas();
   KVIDGridEditorCanvas(const char* name, const char* title, Int_t ww, Int_t wh):TCanvas(name,title,ww,wh){};
   virtual ~KVIDGridEditorCanvas();
   
   void HandleInput(EEventType event, Int_t px, Int_t py);
   bool IsLogz();
   bool IsLogy();
   bool IsLogx();
   
   protected:
   void RunAutoExec();
   void DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);

   ClassDef(KVIDGridEditorCanvas,1)//bidouille
};

#endif
