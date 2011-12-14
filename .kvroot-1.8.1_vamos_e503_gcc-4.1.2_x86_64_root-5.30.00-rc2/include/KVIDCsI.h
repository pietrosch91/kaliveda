/***************************************************************************
                          KVIDCsI.h  -  description
                             -------------------
    begin                : Fri Feb 20 2004
    copyright            : (C) 2004 by J.D. Frankland
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

#ifndef KVIDCsI_H
#define KVIDCsI_H

#include "KVINDRAIDTelescope.h"
#include "KVIDGCsI.h"

class KVIDCsI:public KVINDRAIDTelescope {

   KVIDGCsI* CsIGrid;//! telescope's grid
	KVDetector* fCsI;//!

 public:

   KVIDCsI();
   virtual ~ KVIDCsI();

   const Char_t *GetArrayName();

   virtual Bool_t Identify(KVIdentificationResult*);

   Double_t GetIDMapX(Option_t * opt = "");
   Double_t GetIDMapY(Option_t * opt = "");

   const Char_t *GetIDSubCodeString(KVIDSubCode & concat) const;

   virtual void Initialize();

    ClassDef(KVIDCsI, 3)        //INDRA identification using CsI R-L matrices
};

#endif
