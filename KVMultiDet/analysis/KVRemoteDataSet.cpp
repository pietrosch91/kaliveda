/*
$Id: KVRemoteDataSet.cpp,v 1.4 2007/06/07 07:01:53 franklan Exp $
$Revision: 1.4 $
$Date: 2007/06/07 07:01:53 $
*/

//Created by KVClassFactory on Fri Jul 14 00:16:29 2006
//Author: John Frankland

#include "KVRemoteDataSet.h"
#include "KVRemoteAvailableRunsFile.h"
#include "KVDataRepository.h"

ClassImp(KVRemoteDataSet)
////////////////////////////////////////////////////////////////////////////////
// Class handling datasets stored in a remote data repository
////////////////////////////////////////////////////////////////////////////////
    KVRemoteDataSet::KVRemoteDataSet()
{
   //Default constructor
}

KVRemoteDataSet::~KVRemoteDataSet()
{
   //Destructor
}

KVAvailableRunsFile *KVRemoteDataSet::GetAvailableRunsFile(const Char_t *
                                                           type)
{
   //Returns available runs file object for given data 'type' (="raw", "recon", "ident", "root")
   //As the data set is stored in a remote data repository, this is a KVRemoteAvailableRunsFile object.
   //Object will be created and added to internal list if it does not exist

   KVAvailableRunsFile *avrf =
       (KVAvailableRunsFile *) fAvailableRuns.FindObjectByName(type);
   if (!avrf) {
      avrf = new KVRemoteAvailableRunsFile(type, this);
      fAvailableRuns.Add(avrf);
   }
   return avrf;
}

//__________________________________________________________________________________________________________________

TObject *KVRemoteDataSet::OpenRunfile(const Char_t * type, Int_t run)
{
   //Make sure remote data repository is accessible, then open file.
   //Returns 0 if repository is not accessible.
   TString fname = GetFullPathToRunfile(type, run);
   if (fname == "") return 0; //file not found/available
   if(!fRepository->IsConnected()) return 0;
   return KVDataSet::OpenRunfile(type,run);
}
