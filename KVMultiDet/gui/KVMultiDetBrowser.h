/***************************************************************************
                          kvmultidetbrowser.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVMultiDetBrowser.h,v 1.3 2006/10/19 14:32:43 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVB_H
#define KVB_H

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

class KVMultiDetArray;
class KVLayerBrowser;
class KVWidgetList;
class KVBrowserWidget;

class KVMultiDetBrowser: public TGMainFrame {

   friend class KVMultiDetArray;

private:

   KVMultiDetArray* fDetArray;         //! the multidetector array under study
   TGMenuBar* fMenuBar;         //!
   TGPopupMenu* fMenuFile, *fMenuHelp;  //!
   TGLayoutHints* fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;     //!
   TGLayoutHints* fLBLayout;    //!
   KVLayerBrowser* fLayerBrowser;       //! pointer to layer browser tab widget
   TList* fCleanup;             //! for keeping track of TGLayoutHints and deleting them at the end
   KVWidgetList* fWidgets;      //! list of widgets in mainwindow

public:

   KVMultiDetBrowser(KVMultiDetArray* detarray, const TGWindow* p,
                     UInt_t w, UInt_t h);
   virtual ~ KVMultiDetBrowser();
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void UpdateArray();
   void AddToWidgetList(KVBrowserWidget* widg);

   ClassDef(KVMultiDetBrowser, 0)      // KaliVeda Multidetector Array Browser and Configuration Tool
};

#endif
