/***************************************************************************
                          kvbrowserwidget.h  -  description
                             -------------------
    begin                : Mon Nov 18 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVBrowserWidget.h,v 1.3 2006/10/19 14:32:43 franklan Exp $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVBROWSERWIDGET_H
#define KVBROWSERWIDGET_H

#include "KVWidget.h"

class KVMultiDetArray;

class KVBrowserWidget:public KVWidget {

   static KVMultiDetArray *fMDA;        //! pointer to multidet array shared by all widgets

 public:

    KVBrowserWidget();
    KVBrowserWidget(TObject * obj, Long_t func);
    virtual ~ KVBrowserWidget();
   virtual void Action(Long_t parm = 0) const;

    ClassDef(KVBrowserWidget, 0)        //Extension of KVWidget for KVBrowser
};

#endif
