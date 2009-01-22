/*
$Id: KVRawDataReader.h,v 1.3 2007/06/08 15:49:10 franklan Exp $
$Revision: 1.3 $
$Date: 2007/06/08 15:49:10 $
*/

//Created by KVClassFactory on Wed May 16 15:52:21 2007
//Author: franklan

#ifndef __KVRAWDATAREADER_H
#define __KVRAWDATAREADER_H

#include "TNamed.h"

class KVRawDataReader : public TNamed
{
   public:

   KVRawDataReader();
   virtual ~KVRawDataReader();

   ClassDef(KVRawDataReader,0)//Base class for reading raw data
};

#endif
