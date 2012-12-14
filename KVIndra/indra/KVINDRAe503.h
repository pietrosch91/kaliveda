/*
$Id: KVINDRAe503.h,v 1.2 2007/10/01 15:03:38 franklan Exp $
$Revision: 1.2 $
$Date: 2007/10/01 15:03:38 $
*/

//Created by KVClassFactory on Wed May 17 12:26:57 2006
//Author: franklan

#ifndef __KVINDRAE503_H
#define __KVINDRAE503_H

#include "KVINDRA4.h"

class KVINDRAe503:public KVINDRA4 {

 protected:

   virtual void BuildGeometry();
   virtual void MakeListOfDetectorTypes();
   virtual void PrototypeTelescopes();
   virtual void SetGroupsAndIDTelescopes();

 public:

    KVINDRAe503();
    virtual ~ KVINDRAe503();

   virtual void Build();

    ClassDef(KVINDRAe503, 1)   //Experimental configuration for INDRA-VAMOS experiments
};

#endif
