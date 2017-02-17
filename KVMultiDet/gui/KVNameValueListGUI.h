//Created by KVClassFactory on Fri Feb 17 09:32:40 2017
//Author: John Frankland,,,

#ifndef __KVNAMEVALUELISTGUI_H
#define __KVNAMEVALUELISTGUI_H

#include "TGFrame.h"
#include "RQ_OBJECT.h"
#include "KVNameValueList.h"

#include <TGButton.h>
#include <TObjArray.h>

class KVNameValueListGUI {
   RQ_OBJECT("KVNameValueListGUI")
protected:
   KVNameValueList* theList;

   TGTransientFrame* fMain;
   TGTextButton* fOKBut;        //OK button
   TGTextButton* fCancelBut;    //Cancel button
   Bool_t* fOK;                 //set to kTRUE if OK button is pressed
   UInt_t max_width;
   TObjArray fData;
   virtual TObject* AddAString(Int_t i, TGHorizontalFrame* hf);
   virtual TObject* AddABool(Int_t i, TGHorizontalFrame* hf);
   virtual TObject* AddADouble(Int_t i, TGHorizontalFrame* hf);
   virtual TObject* AddAInt(Int_t i, TGHorizontalFrame* hf);

public:
   KVNameValueListGUI(const TGWindow* main, KVNameValueList* params);

   virtual ~KVNameValueListGUI();

   void ReadData();
   void DoClose();
   void CloseWindow();

   ClassDef(KVNameValueListGUI, 1) //GUI for setting KVNameValueList parameters
};

#endif
