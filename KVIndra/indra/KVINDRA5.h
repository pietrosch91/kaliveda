/***************************************************************************
                          kvindra5.h  -  description
                             -------------------
    begin                : Mon May 20 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVINDRA5.h,v 1.25 2007/12/10 11:37:40 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVINDRA5_H
#define KVINDRA5_H

#include "KVINDRA4.h"

class KVINDRA5:public KVINDRA4 {

 protected:
   virtual void BuildGeometry();
   virtual void MakeListOfDetectorTypes();
   virtual void PrototypeTelescopes();

 public:
    KVINDRA5();
    virtual ~ KVINDRA5();

   virtual void Build();

   virtual void SetPGtoGGFactors(Option_t * type);

    ClassDef(KVINDRA5, 1)       //the INDRA multidetector array in its 5th campaign configuration
};
#endif
