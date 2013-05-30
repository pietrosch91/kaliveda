//Created by KVClassFactory on Wed May 29 14:59:50 2013
//Author: Guilain ADEMARD

#ifndef __KVSPECTROGROUP_H
#define __KVSPECTROGROUP_H

#include "KVBase.h"
#include "KVList.h"

class KVSpectroDetector;

class KVSpectroGroup : public KVBase
{

	protected:

		KVList *fDetectors; //!list of all detectors in group

   	public:

   		KVSpectroGroup();
   		KVSpectroGroup(const KVSpectroGroup& obj);
   		KVSpectroGroup(const Char_t* name, const Char_t* title = "");
   		virtual ~KVSpectroGroup();
   		void Copy(TObject& obj) const;

   		void init();
		void Add( KVSpectroDetector *det );
		virtual const Char_t* GetTitle() const;
		void SetDetectors( KVList *list );
		virtual void SetNumber(UInt_t num);

		inline KVList *GetDetectors() const { return fDetectors; }
		void Sort(Bool_t order = kSortAscending){
			fDetectors->Sort(order);
		}

   		ClassDef(KVSpectroGroup,1)//Group of spectrometer detectors on a same trajectory
};

#endif
