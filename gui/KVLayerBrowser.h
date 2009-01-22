/***************************************************************************
                          kvlayerbrowser.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVLayerBrowser.h,v 1.4 2006/11/03 08:01:32 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVLB_H
#define KVLB_H

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
#include "TList.h"

#include "KVWidgetList.h"
#include "KVBrowserWidget.h"
#include "KVRingBrowser.h"

class KVList;
class KVLayer;

class KVLayerBrowser:public TGTab {

   friend class KVLayer;

 private:

    TList * fCleanup;           //! for TGLayoutHints objects, delete at end
   KVWidgetList *fWidgets;      //! list of all widgets in frame
   TList *fRingBrowsers;        //! put ring browser of each layer in this list

 public:

    KVLayerBrowser(KVList * layers, const TGWindow * p, UInt_t w,
                   UInt_t h);
    virtual ~ KVLayerBrowser();
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void LayoutLayer(KVLayer * layer);
   void AddToWidgetList(KVBrowserWidget * widg) {
      if (!fWidgets)
         fWidgets = new KVWidgetList;
      fWidgets->Add(widg);
   } void AddRingBrowser(KVRingBrowser * w) {
      if (!fRingBrowsers)
         fRingBrowsers = new TList;
      fRingBrowsers->Add(w);
   }

   ClassDef(KVLayerBrowser, 0)  // Layer browser
};

#endif
