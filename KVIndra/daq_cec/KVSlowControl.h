/***************************************************************************
                          KVSlowControl.h  -  description
                             -------------------
    begin                : Wed Apr 2 2003
    copyright            : (C) 2003 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVSLOWCONTROL_H
#define KVSLOWCONTROL_H

#include "KVBase.h"

class TFile;
class TTree;
class KVSLCBase;

class KVSlowControl : public KVBase  {

   TFile* fFile;//file containing SLC database tree
   TTree* fTree;//tree containing SLC database
   KVSLCBase* fBase;//SLC database for 1 run read from tree

public:
   KVSlowControl();
   virtual ~KVSlowControl();

   ClassDef(KVSlowControl, 0) //Access to Slow Control data
};

#endif
