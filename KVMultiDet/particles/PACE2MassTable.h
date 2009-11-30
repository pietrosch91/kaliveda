//Created by KVClassFactory on Mon Jul 27 11:49:41 2009
//Author: John Frankland,,,

#ifndef __PACE2MASSTABLE_H
#define __PACE2MASSTABLE_H

#include "KVMassTable.h"
#include "TMap.h"

class PACE2MassTable : public KVMassTable
{
	Double_t *theTable;//
	TMap *nucMap;//mapping (Z,A) -> nucleus index
	
   public:
   PACE2MassTable();
   virtual ~PACE2MassTable();

   virtual void Initialize();
   virtual Double_t GetMassExcess(Int_t /*Z*/, Int_t /*A*/);
   virtual Bool_t IsKnown(Int_t /*Z*/, Int_t /*A*/);

   ClassDef(PACE2MassTable,1)//The pace2_2001.data mass table used in the SIMON code
};

namespace PACE2Map
{
	class key : public TNamed
	{
		public:
		key(int z,int a) {
			SetName(Form("%d:%d",z,a));
		};
		virtual ~key(){};
		ClassDef(key,0)
	};
	class value : public TObject
	{
		int idx;
		public:
		value(int i) : idx(i) {};
		virtual ~value(){};
		int Index() const
		{
			return idx;
		};
		ClassDef(value,0)
	};
};
#endif
