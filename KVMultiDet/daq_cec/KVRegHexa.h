/***************************************************************************
                          KVRegHexa.h  -  description
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

#ifndef KVRegHexa_H
#define KVRegHexa_H

#include "KVRegister.h"

class KVRegHexa : public KVRegister  {

public:

   KVRegHexa();
   KVRegHexa(const KVRegHexa&);
   virtual ~KVRegHexa() {};

   const Char_t* GetRealValue(const Long_t = 0, const Long_t = 0) const;
   void SetRealValue(const Double_t, const Long_t = 0, const Long_t = 0);

   ClassDef(KVRegHexa, 1) //Hexadecimal register in module
};

#endif
