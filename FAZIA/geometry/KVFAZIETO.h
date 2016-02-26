//Created by KVClassFactory on Fri Feb 26 14:22:01 2016
//Author: bonnet,,,

#ifndef __KVFAZIETO_H
#define __KVFAZIETO_H

#include "KVFAZIA.h"

class KVFAZIETO : public KVFAZIA {

   virtual void BuildFAZIA();
   virtual void GetGeometryParameters();

public:
   KVFAZIETO();
   virtual ~KVFAZIETO();

   ClassDef(KVFAZIETO, 1) //description of the FAZIA-12B demonstrator
};

#endif
