//Created by KVClassFactory on Mon Sep  3 11:28:25 2012
//Author: Guilain ADEMARD

#ifndef __KVSED_H
#define __KVSED_H

#include "KVVAMOSDetector.h"

class KVSeD : public KVVAMOSDetector
{

	protected:
		void init();
   public:
   KVSeD();
   KVSeD (const KVSeD&) ;
   virtual ~KVSeD();
   void Copy (TObject&) const;

   virtual const Char_t* GetArrayName();
   virtual void SetACQParams();
   virtual void SetCalibrators();
//   virtual void SetCalibrators(KVDBParameterSet *kvdbps);

   ClassDef(KVSeD,1)//Secondary Electron Detector, used at the focal plan of VAMOS
};

#endif
