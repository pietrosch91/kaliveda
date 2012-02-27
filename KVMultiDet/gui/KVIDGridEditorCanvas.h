//Created by KVClassFactory on Fri Feb 17 17:47:35 2012
//Author: dgruyer

#ifndef __KVIDGridEditorCanvas_H
#define __KVIDGridEditorCanvas_H

#include "TCanvas.h"
#include "KVCanvas.h"

class KVIDGridEditorCanvas : public KVCanvas
{

   public:
   KVIDGridEditorCanvas();
   KVIDGridEditorCanvas(const char* name, const char* title, Int_t ww, Int_t wh):KVCanvas(name,title,ww,wh){};
   virtual ~KVIDGridEditorCanvas();
   
   void HandleInput(EEventType event, Int_t px, Int_t py);

   ClassDef(KVIDGridEditorCanvas,1)//bidouille
};

#endif
