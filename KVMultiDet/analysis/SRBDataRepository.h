//Created by KVClassFactory on Sat Jul 18 16:54:29 2009
//Author: John Frankland

#ifndef __SRBDATAREPOSITORY_H
#define __SRBDATAREPOSITORY_H

#include "KVDataRepository.h"
#include "SRB.h"

class SRBDataRepository : public KVDataRepository
{
	SRB fSRB;//connection to SRB
	
   public:
   SRBDataRepository();
   virtual ~SRBDataRepository();

   virtual TList *GetDirectoryListing(const Char_t * datasetdir,
                                       const Char_t * datatype = "");
   virtual Bool_t CheckSubdirExists(const Char_t * dir,
                                    const Char_t * subdir = 0);
   virtual void CopyFileFromRepository(const Char_t * datasetdir,
                                       const Char_t * datatype,
                                       const Char_t * filename,
                                       const Char_t * destination);
   virtual void CopyFileToRepository(const Char_t * source,
                                     const Char_t * datasetdir,
                                     const Char_t * datatype,
                                     const Char_t * filename);
   virtual Bool_t CheckFileStatus(const Char_t * datasetdir,
                                  const Char_t * datatype,
                                  const Char_t * runfile);
	
   ClassDef(SRBDataRepository,1)//Remote data repository using SRB
};

#endif
