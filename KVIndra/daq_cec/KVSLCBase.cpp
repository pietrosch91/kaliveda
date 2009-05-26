/***************************************************************************
                          KVSLCBase.cpp  -  description
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

#include "KVSLCBase.h"

ClassImp(KVSLCBase)

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

KVSLCBase::KVSLCBase(){
	//default constructor
	//set all arrays to zero

	for(UInt_t r=0;r<MAX_RINGS;r++)
	{
		for(UInt_t m=0;m<MAX_MODS;m++)
		{
			for(UInt_t s=0;s<MAX_SIGS;s++)
			{
				fTotEv[r][m][s]=0;
				fPhyEv[r][m][s]=0;
				fNPhys[r][m][s]=0;
				fAvPhys[r][m][s]=0.0;
				fSigPhys[r][m][s]=0.0;
				fNGen[r][m][s]=0;
				fAvGen[r][m][s]=0.0;
				fSigGen[r][m][s]=0.0;
			}
		}
	}
}

KVSLCBase::~KVSLCBase(){
}
