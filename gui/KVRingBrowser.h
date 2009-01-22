/***************************************************************************
                          kvringbrowser.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRingBrowser.h,v 1.3 2006/10/19 14:32:43 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVRB_H
#define KVRB_H

#include "TROOT.h"
#include "TApplication.h"
#include "TVirtualX.h"

#include "TGClient.h"
#include "TGFrame.h"
#include "TGMenu.h"
#include "TGTab.h"
#include "TGLabel.h"
#include "TGFileDialog.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGTextEntry.h"
#include "TGTextBuffer.h"
#include "TList.h"

#include "KVMultiDetArray.h"
#include "KVWidget.h"
#include "KVWidgetList.h"
#include "KVBrowserWidget.h"

class KVRingBrowser:public TGTab {

 private:

   TList * fCleanup;            //for TGLayoutHints and TGFrame objects not used in a KVWidget, to delete at end
   KVWidgetList *fWidgets;      //! list of widgets

 public:

    KVRingBrowser(KVList * rings, const TGWindow * p, UInt_t w, UInt_t h);
    virtual ~ KVRingBrowser();
   void LayoutRing(KVRing * kvr);
   void LayoutTelescopes(KVRing * fRing, TGGroupFrame * fGrpFrame);
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void AddToWidgetList(KVBrowserWidget * widg) {
      if (!fWidgets)
         fWidgets = new KVWidgetList;
      fWidgets->Add(widg);
   };
   ClassDef(KVRingBrowser, 0)   // Ring browser
};

#endif
