/***************************************************************************
                          KVINDRA4.h  -  description
                             -------------------
    begin                : 25th October 2003
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVINDRA4.h,v 1.6 2006/10/19 14:32:43 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVINDRA4_H
#define KVINDRA4_H

#include "KVINDRA.h"


class KVINDRA4:public KVINDRA {

 protected:

   virtual void BuildGeometry();

 public:

    KVINDRA4();
    virtual ~ KVINDRA4();

   virtual void Build();

    ClassDef(KVINDRA4, 1)       //the INDRA multidetector array in its 4th campaign configuration
};

#endif
