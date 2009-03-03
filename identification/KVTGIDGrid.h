/*
$Id: KVTGIDGrid.h,v 1.2 2009/03/03 14:27:15 franklan Exp $
$Revision: 1.2 $
$Date: 2009/03/03 14:27:15 $
*/

//Created by KVClassFactory on Fri Apr 25 15:55:08 2008
//Author: franklan

#ifndef __KVTGIDGRID_H
#define __KVTGIDGRID_H

#include "KVIDZAGrid.h"

class KVTGID;

class KVTGIDGrid : public KVIDZAGrid
{
	protected:
	KVTGID		*fTGID;		//KVTGID used to generate grid
	
   public:
   KVTGIDGrid();
   KVTGIDGrid(KVTGID*);
   virtual ~KVTGIDGrid();
	void WriteToAsciiFile(ofstream &);
	
	const KVTGID* GetTGID() const { return fTGID; };

   ClassDef(KVTGIDGrid,1)//Grid representing result of fit
};

#endif
