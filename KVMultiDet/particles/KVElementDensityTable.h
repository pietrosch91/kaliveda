//Created by KVClassFactory on Thu Sep 27 17:23:50 2012
//Author: John Frankland,,,

#ifndef __KVElementDensityTable_H
#define __KVElementDensityTable_H

#include "KVNuclDataTable.h"

class KVElementDensityTable : public KVNuclDataTable
{

   virtual NDT::value* getNDTvalue(Int_t zz, Int_t aa) const;
   
   public:
   KVElementDensityTable();
   KVElementDensityTable (const KVElementDensityTable&) ;
   virtual ~KVElementDensityTable();
   void Copy (TObject&) const;
	
   virtual void GiveIndexToNucleus(Int_t zz,Int_t aa,Int_t ntot);

	virtual void Initialize();
	
   ClassDef(KVElementDensityTable,1)//Table of atomic elements
};

#endif
