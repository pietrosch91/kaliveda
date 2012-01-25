//Created by KVClassFactory on Sat Jul 18 16:54:29 2009
//Author: John Frankland

#ifndef __SRBDATAREPOSITORY_H
#define __SRBDATAREPOSITORY_H

#include "KVDataRepository.h"
#include "SRB.h"

class SRBDataRepository : public KVDataRepository
{
	protected:
	SRB fSRB;//connection to SRB
	
   virtual int             Chmod(const char *file, UInt_t mode);
   public:
   SRBDataRepository();
   virtual ~SRBDataRepository();

   virtual TList *GetDirectoryListing(KVDataSet*,
                                       const Char_t * datatype = "");
   virtual Bool_t CheckSubdirExists(const Char_t * dir,
                                    const Char_t * subdir = 0);
   virtual void CopyFileFromRepository(KVDataSet*,
                                       const Char_t * datatype,
                                       const Char_t * filename,
                                       const Char_t * destination);
   virtual void CopyFileToRepository(const Char_t * source,
                                     KVDataSet*,
                                     const Char_t * datatype,
                                     const Char_t * filename);
   virtual Bool_t CheckFileStatus(KVDataSet*,
                                  const Char_t * datatype,
                                  const Char_t * runfile);
   virtual void MakeSubdirectory(KVDataSet*,
                                 const Char_t * datatype = "");
   virtual void DeleteFile(KVDataSet*,
                           const Char_t * datatype,
                           const Char_t * filename, Bool_t confirm =
                           kTRUE);
   virtual Bool_t GetFileInfo(KVDataSet*,
                              const Char_t * datatype,
                              const Char_t * runfile, FileStat_t & fs);
	
   ClassDef(SRBDataRepository,1)//Remote data repository using SRB
};

#endif
