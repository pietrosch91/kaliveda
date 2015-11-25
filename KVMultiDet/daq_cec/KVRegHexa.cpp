/***************************************************************************
                          KVRegHexa.cpp  -  description
                             -------------------
    begin                : ven déc 17 2004
    copyright            : (C) 2004 by J.D Frankland
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

#include "KVRegHexa.h"
#include "TMath.h"
#include "Hexa_t.h"

ClassImp(KVRegHexa)

////////////////////////////////////////////////////////////////////////////
//KVRegHexa
//
//A register holding hexadecimal values

KVRegHexa::KVRegHexa()
{
   //Default ctor

   SetName("KVRegHexa");
   SetLabel("Hexadecimal register");
}

//___________________________________________________________________________

KVRegHexa::KVRegHexa(const KVRegHexa& reg)
{
   //Copy ctor
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   reg.Copy(*this);
#else
   ((KVRegHexa&) reg).Copy(*this);
#endif
}

//___________________________________________________________________________

const Char_t* KVRegHexa::GetRealValue(const Long_t, const Long_t) const
{
   //Returns string with hexadecimal value of register

   Hexa_t tmp(GetContents());  //temporary hexa variable with decimal value of register
   return tmp.String();
}

//___________________________________________________________________________

void KVRegHexa::SetRealValue(const Double_t Clair, const Long_t, const Long_t)
{
   //Set the (decimal) value of the register.
   //If this value is less than 0 or greater than GetMaxCont(),
   //the contents of the register will be set to either the minimum or
   //maximum allowed value.

   Long_t Cont = (Long_t)TMath::Max(0., TMath::Min(Clair, GetMaxCont()));
   SetContents(Cont);
}

