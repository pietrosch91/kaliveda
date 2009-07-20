//Created by KVClassFactory on Sat Jul 18 16:54:29 2009
//Author: John Frankland

#include "SRBDataRepository.h"

ClassImp(SRBDataRepository)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>SRBDataRepository</h2>
<h4>Remote data repository using SRB</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

SRBDataRepository::SRBDataRepository()
{
   // Default constructor
}

SRBDataRepository::~SRBDataRepository()
{
   // Destructor
}

TList *SRBDataRepository::GetDirectoryListing(const Char_t * datasetdir,
                                              const Char_t * datatype)
{
   //Use the SRB catalogue in order to examine the directory
   //
   //      /root_of_data_repository/[datasetdir]/[datatype]
   //      /root_of_data_repository/[datasetdir]                    (if datatype="", default value)
   //
   //and fill a TList with one SRBFile_t object for each entry in the directory,
   //User must delete the TList after use (list will delete its members)

   TString path, tmp;
   AssignAndDelete(path,
                   gSystem->ConcatFileName(fAccessroot.Data(),
                                           datasetdir));
   if (datatype) {
      AssignAndDelete(tmp, gSystem->ConcatFileName(path.Data(), datatype));
      path = tmp;
   }
	
	return fSRB.GetFullListing(path.Data());
}

//___________________________________________________________________________

Bool_t SRBDataRepository::CheckSubdirExists(const Char_t * dir,
                                           const Char_t * subdir)
{
   //Returns kTRUE if the following path is valid
   //      /root_of_data_repository/dir/[subdir]

   TString dirname, path;
   if (subdir){
   	AssignAndDelete(path, gSystem->ConcatFileName(fAccessroot.Data(), dir));
		dirname=subdir;
	}
   else {
      path = fAccessroot.Data();
		dirname=dir;
	}
   return fSRB.DirectoryContains(dirname.Data(),path.Data());
}
