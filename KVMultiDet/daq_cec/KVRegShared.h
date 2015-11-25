/***************************************************************************
                          KVTableau.h  -  description
                             -------------------
    begin                : 29/07/2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRegShared.h,v 1.2 2005/09/28 17:17:01 franklan Exp $
**************************************************************************/
/* Definition de la classe registre tableau                               */
/* Premier test                                                           */
/* D.Cussol                                                               */
/* 14/02/97                                                               */
/**************************************************************************/

#ifndef KVREGSHARED__H
#define KVREGSHARED__H

#include "KVRegister.h"
#include "KVList.h"

class KVRegShared : public KVRegister {
private:

   KVList* fRegList;//list of registers

public:
   KVRegShared();

   virtual ~KVRegShared();

   KVList* GetListRegs() const
   {
      return fRegList;
   };

   Long64_t GetContents(const Long64_t = 0, const Long64_t = 0) const;
   const Char_t* GetRealValue(const Long64_t = 0, const Long64_t = 0) const;

   void Print(Option_t* opt = "") const;

   void SetContents(const Long64_t, const Long64_t = 0, const Long64_t = 0);

   ClassDef(KVRegShared, 1)//Shared register
};

#endif
