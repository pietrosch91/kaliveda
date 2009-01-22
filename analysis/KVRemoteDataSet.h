/*
$Id: KVRemoteDataSet.h,v 1.3 2007/05/31 09:59:22 franklan Exp $
$Revision: 1.3 $
$Date: 2007/05/31 09:59:22 $
*/

//Created by KVClassFactory on Fri Jul 14 00:16:29 2006
//Author: John Frankland

#ifndef __KVREMOTEDATASET_H
#define __KVREMOTEDATASET_H

#include <KVDataSet.h>

class KVRemoteDataSet:public KVDataSet {
 protected:

   virtual KVAvailableRunsFile * GetAvailableRunsFile(const Char_t * type);

 public:

    KVRemoteDataSet();
    virtual ~ KVRemoteDataSet();
    virtual TObject *OpenRunfile(const Char_t * type, Int_t run);

    ClassDef(KVRemoteDataSet, 1)        //Class handling datasets stored in a remote data repository
};

#endif
