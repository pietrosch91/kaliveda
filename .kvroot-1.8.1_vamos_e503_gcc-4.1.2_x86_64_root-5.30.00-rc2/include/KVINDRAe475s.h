/*
$Id: KVINDRAe475s.h,v 1.2 2006/10/19 14:32:43 franklan Exp $
$Revision: 1.2 $
$Date: 2006/10/19 14:32:43 $
*/

//Created by KVClassFactory on Wed May 17 12:26:57 2006
//Author: franklan

#ifndef __KVINDRAE475S_H
#define __KVINDRAE475S_H

#include "KVINDRA4.h"

class KVINDRAe475s:public KVINDRA4 {

 protected:

   virtual void BuildGeometry();
   virtual void MakeListOfDetectorTypes();
   virtual void PrototypeTelescopes();

 public:

    KVINDRAe475s();
    virtual ~ KVINDRAe475s();

   virtual void Build();

    ClassDef(KVINDRAe475s, 1)   //Experimental configuration for E475S
};

#endif
