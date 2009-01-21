/***************************************************************************
                          KVIDChIoCsI5.h  -  description
                             -------------------
    begin                : 17/05/2004
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

#ifndef KVIDChIoCsI5_H
#define KVIDChIoCsI5_H

#include "KVIDChIoCsI.h"
#include "KVTGIDManager.h"

class KVMultiDetArray;

class KVIDChIoCsI5:public KVIDChIoCsI, public KVTGIDManager {

   Int_t fWhichGrid;            //!internal IdentifyZ status code, tells us which Z grid was last used

   enum {                       //grid identifiers
      k_GG, k_PG1
   };

 public:

   enum {                       //identification status subcodes
      k_OK_GG = KVTGIDManager::kStatus_OK,      //identification OK GG
      k_noTGID_GG,              //no KVTGIDZ GG for Z identification
      k_OutOfIDRange_GG,        //point to identify outside of Z identification range of KVTGIDZ GG
      k_OK_PG1,                 //identification OK PG1
      k_noTGID_PG1,             //no KVTGIDZ PG1 for Z identification
      k_OutOfIDRange_PG1,       //point to identify outside of Z identification range of KVTGIDZ PG1
   };
      
    KVIDChIoCsI5() {
   };
   virtual ~ KVIDChIoCsI5() {
   };

   Double_t IdentifyZ(Double_t & funLTG);

   virtual Bool_t Identify(KVReconstructedNucleus *);

   void Print(Option_t * opt = "") const;

   virtual Double_t GetIDMapX(Option_t * opt = "");
   virtual Double_t GetIDMapY(Option_t * opt = "");

   const Char_t *GetIDSubCodeString(KVIDSubCode & concat) const;
   virtual Bool_t SetIdentificationParameters(const KVMultiDetArray*);
   virtual void RemoveIdentificationParameters();

   virtual void Initialize();
   
   ClassDef(KVIDChIoCsI5, 2)    //5th campaign INDRA identification using ChIo-CsI matrices
};

#endif
