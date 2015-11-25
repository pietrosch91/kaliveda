/***************************************************************************
$Id: KVModule.cpp,v 1.6 2006/10/19 14:32:43 franklan Exp $
                          kvmodule.cpp  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
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
#include "KVModule.h"
#include "KVRList.h"
#include "KVDetector.h"

ClassImp(KVModule);
//__________________________________________________________________________
//class for all electronic coder,
//ampli, power supply, discri, etc. modules.
//This should use/interface to the Vintache/LeSaec classes.
//

//__________________________________________________________________________
KVModule::KVModule()
{
   fDetectors = 0;
   fRegisters = new KVList;
   SetName("KVModule");
}

//__________________________________________________________________________
KVModule::~KVModule()
{
   if (fDetectors) {
      delete fDetectors;
      fDetectors = 0;
   }
   delete fRegisters;
   fRegisters = 0;
}

//__________________________________________________________________________
void KVModule::ConnectDetector(KVDetector* d, const int fcon)
{
//   make connection between detector and module. this is a twin of the
//       KVDetector::ConnectModule method. whichever is called first (with only
//       one argument, the pointer) will call the other to set up the "reciprocal
//       connexion".

   // called by detector to connect itself to the module
   if (!fDetectors)
      fDetectors = new KVRList;
   fDetectors->Add(d);          // add to list of connected detectors
   if (fcon == KVD_RECPRC_CNXN)
      d->ConnectModule(this, KVD_NORECPRC_CNXN);
}
