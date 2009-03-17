/***************************************************************************
                          KVIDChIoSi.h  -  description
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

#ifndef KVIDChIoSi_H
#define KVIDChIoSi_H

#include "KVIDTelescope.h"
#include "KVIDGChIoSi.h"

class KVIDChIoSi:public KVIDTelescope {

   KVIDGChIoSi* ChIoSiGrid;//!
   
 public:

   enum {
      kID_OK,                              //normal successful identification
      kID_BelowPunchThrough,  //point to ID was below punch-through line (bruit)
      kID_BelowSeuilSi,               //point to ID was left of "Seuil Si" line (bruit/arret ChIo?)
      kID_LeftOfBragg,              //point to ID is between 'Seuil Si' and 'Bragg': Z given is a Zmin.
      kID_RightOfEmaxSi,          //point to ID has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable.
      kID_ZgtZmax,                 //point to ID is higher (in ChIo) than line with biggest Z. Z given is a Zmin.
      kID_nogrid                     //no grid for identification
   };

    KVIDChIoSi();
    virtual ~ KVIDChIoSi();

   virtual Bool_t Identify(KVReconstructedNucleus *);
   Bool_t SetIDGrid(KVIDGraph *);

   Double_t GetIDMapX(Option_t * opt = "");
   Double_t GetIDMapY(Option_t * opt = "");

   virtual void Initialize();
   
   const Char_t *GetIDSubCodeString(KVIDSubCode & concat) const;

    ClassDef(KVIDChIoSi, 1)     //INDRA identification using ChIo-Si matrices
};

#endif
