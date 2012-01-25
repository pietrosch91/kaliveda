/***************************************************************************
                          kvbrowsernamedialog.h  -  description
                             -------------------
    begin                : Mon Dec 2 2002
    copyright            : (C) 2002 by J.D Frankland & Alexis Mignon
    email                : frankland@ganil.fr, mignon@ganil.fr

$Id: KVBrowserNameDialog.h,v 1.3 2006/10/19 14:32:43 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVBROWSERNAMEDIALOG_H
#define KVBROWSERNAMEDIALOG_H

#include "KVBrowser.h"
#include "TGWindow.h"
#include "TGFrame.h"

class KVBrowserNameDialog:public KVBrowser, public TGTransientFrame {

 public:

   KVBrowserNameDialog(const TGWindow *, const TGWindow *, const Char_t *,
                       Char_t *);
    virtual ~ KVBrowserNameDialog() {
   };

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(KVBrowserNameDialog, 0)     //A dialogue box for naming layers, rings, etc.
};

#endif
