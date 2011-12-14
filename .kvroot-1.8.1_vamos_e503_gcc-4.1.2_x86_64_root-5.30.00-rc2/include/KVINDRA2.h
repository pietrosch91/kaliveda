/***************************************************************************
                          KVINDRA2.h  -  description
                             -------------------
    begin                : 25th October 2003
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVINDRA2.h,v 1.6 2006/10/19 14:32:43 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVINDRA2_H
#define KVINDRA2_H

#include "KVINDRA.h"

class KVINDRA2:public KVINDRA {

 public:

   KVINDRA2();
   virtual ~ KVINDRA2();

   virtual void Build();

    ClassDef(KVINDRA2, 1)       //the INDRA multidetector array in its 2nd campaign configuration
};

#endif
