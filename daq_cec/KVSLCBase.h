/***************************************************************************
                          KVSLCBase.h  -  description
                             -------------------
    begin                : Thu Apr 3 2003
    copyright            : (C) 2003 by J.D. Frankland
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

#ifndef KVSLCBASE_H
#define KVSLCBASE_H

#define MAX_RINGS 17
#define MAX_MODS 24
#define MAX_SIGS 15

#include "KVBase.h"

class KVSLCBase : public KVBase  {

	UInt_t fTotEv[MAX_RINGS][MAX_MODS][MAX_SIGS];
	UInt_t fPhyEv[MAX_RINGS][MAX_MODS][MAX_SIGS];
	UInt_t fNPhys[MAX_RINGS][MAX_MODS][MAX_SIGS];
	Float_t fAvPhys[MAX_RINGS][MAX_MODS][MAX_SIGS];
	Float_t fSigPhys[MAX_RINGS][MAX_MODS][MAX_SIGS];
	UInt_t fNGen[MAX_RINGS][MAX_MODS][MAX_SIGS];
	Float_t fAvGen[MAX_RINGS][MAX_MODS][MAX_SIGS];
	Float_t fSigGen[MAX_RINGS][MAX_MODS][MAX_SIGS];

public: 
	KVSLCBase();
	virtual ~KVSLCBase();

	ClassDef(KVSLCBase,0)//SlowControl database class
};

#endif
