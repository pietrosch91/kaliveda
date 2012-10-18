//Created by KVClassFactory on Thu Oct 11 11:51:14 2012
//Author: Guilain Ademard

#ifndef __KVRTGIDMANAGER_H
#define __KVRTGIDMANAGER_H

#include "KVTGIDManager.h"

class KVRTGIDManager : public KVTGIDManager
{
	protected:

		static KVList *fIDGlobalList;  //! Global list of TGID of all the KVRTGIDManager
		
   		virtual void  SetTGID(KVTGID *);

   	public:
   		KVRTGIDManager();
   		virtual ~KVRTGIDManager();

   		virtual void   AddTGID(KVTGID *);
   		virtual void   RemoveAllTGID();
   		virtual void   RemoveTGID(const Char_t * name);

		//------- static methods --------------------------//

		static void    Clear();
		static void    DeleteTGID(const Char_t * name);
   		static KVList *GetGlobalListOfIDFunctions(){return fIDGlobalList; };   
   		static void    SetIDFuncInTelescopes(UInt_t run);

   		ClassDef(KVRTGIDManager,1)//Run-dependant KVTGIDManager
};

#endif
