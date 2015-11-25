/***************************************************************************
                          kvdetectorbrowser.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVDetectorBrowser.h,v 1.3 2006/10/19 14:32:43 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVDB_H
#define KVDB_H

#include "KVBrowser.h"
#include "TGFrame.h"

class KVDetector;

class KVDetectorBrowser: public KVBrowser, public TGMainFrame {

   friend class KVDetector;

private:

   KVDetector* fDet;           //! the detector under study

public:

   KVDetectorBrowser(const TGWindow* p, KVDetector* d, UInt_t w,
                     UInt_t h);
   virtual ~ KVDetectorBrowser()
   {
   };
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);

   ClassDef(KVDetectorBrowser, 0)       // KaliVeda detector Browser and Configuration Tool
};

#endif
