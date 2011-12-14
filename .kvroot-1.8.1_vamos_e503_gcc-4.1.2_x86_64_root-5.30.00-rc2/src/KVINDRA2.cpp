/***************************************************************************
$Id: KVINDRA2.cpp,v 1.6 2006/10/19 14:32:43 franklan Exp $
                          KVINDRA2.cpp  -  description
                             -------------------
    begin                : 25th October 2003
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

#include "Riostream.h"
#include <TF1.h>
#include <TGraph.h>
#include "KVINDRA2.h"
#include "KVTelescope.h"
#include "KVLayer.h"
#include "KVList.h"
#include "KVSilicon.h"
#include "KVChIo.h"
#include "KVCsI.h"
#include "KVNucleus.h"
#include "KVGroup.h"
#include "KVRing.h"

ClassImp(KVINDRA2);
//____________________________________________________________________________________
// class describing the INDRA multidetector array in its 2nd campaign configuration
//____________________________________________________________________________________
KVINDRA2::KVINDRA2()
{
}

//____________________________________________________________________________________
KVINDRA2::~KVINDRA2()
{
}

//____________________________________________________________________________________

void KVINDRA2::Build()
{
   //construct specific realisation of INDRA array for 2nd campaign

   KVINDRA::Build();
   SetName("INDRA");
   SetTitle("2nd campaign INDRA detector");
}
