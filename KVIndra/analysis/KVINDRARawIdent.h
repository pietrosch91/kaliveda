/*
$Id: KVINDRARawIdent.h,v 1.2 2008/11/12 16:15:49 franklan Exp $
$Revision: 1.2 $
$Date: 2008/11/12 16:15:49 $
*/

//Created by KVClassFactory on Wed Feb 20 10:52:20 2008
//Author: franklan

#ifndef __KVINDRARAWIDENT_H
#define __KVINDRARAWIDENT_H

#include "KVINDRARawDataReconstructor.h"

class KVINDRARawIdent : public KVINDRARawDataReconstructor
{

   public:
   KVINDRARawIdent();
   virtual ~KVINDRARawIdent(){};

   void InitRun () ;
   virtual void ExtraProcessing();

   ClassDef(KVINDRARawIdent,3)//User raw data analysis class
};

#endif
