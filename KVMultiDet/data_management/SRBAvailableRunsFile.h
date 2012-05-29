//Created by KVClassFactory on Mon Jul 20 11:33:43 2009
//Author: John Frankland

#ifndef __SRBAVAILABLERUNSFILE_H
#define __SRBAVAILABLERUNSFILE_H

#include "KVAvailableRunsFile.h"

class SRBAvailableRunsFile : public KVAvailableRunsFile
{

   public:
   SRBAvailableRunsFile();
   SRBAvailableRunsFile(const Char_t*, KVDataSet*);
   virtual ~SRBAvailableRunsFile();

   virtual void Update(Bool_t no_existing_file = kFALSE);

   ClassDef(SRBAvailableRunsFile,1)//Handles list of available runs using SRB
};

#endif
