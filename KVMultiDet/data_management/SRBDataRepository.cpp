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
      AssignAndDelete(tmp, gSystem->ConcatFileName(path.Data(), GetDatatypeSubdir(datatype)));
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

//___________________________________________________________________________

void SRBDataRepository::CopyFileFromRepository(const Char_t * datasetdir,
                                              const Char_t * datatype,
                                              const Char_t * filename,
                                              const Char_t * destination)
{
   //Copy file [datasetdir]/[datatype]/[filename] from the repository to [destination]
   //We check if the file to copy exists.

   if (CheckFileStatus(datasetdir, datatype, filename)) {
      TString path, tmp;
      AssignAndDelete(path,
                      gSystem->ConcatFileName(fAccessroot.Data(),
                                              datasetdir));
      AssignAndDelete(tmp, gSystem->ConcatFileName(path.Data(), GetDatatypeSubdir(datatype)));
      AssignAndDelete(path, gSystem->ConcatFileName(tmp.Data(), filename));
      //copy file
      fSRB.Sget( path.Data(), destination, "-v -M" );
   }
}

//___________________________________________________________________________

void SRBDataRepository::CopyFileToRepository(const Char_t * source,
                                            const Char_t * datasetdir,
                                            const Char_t * datatype,
                                            const Char_t * filename)
{
   // Copy file [source] to [datasetdir]/[datatype]/[filename] in the repository

   TString path, tmp;
   AssignAndDelete(path,
                   gSystem->ConcatFileName(fAccessroot.Data(),
                                           datasetdir));
   AssignAndDelete(tmp, gSystem->ConcatFileName(path.Data(), GetDatatypeSubdir(datatype)));
   AssignAndDelete(path, gSystem->ConcatFileName(tmp.Data(), filename));
   
   //copy file
	Info("CopyFileToRepository","Sput -v -M %s %s",source,path.Data());
   fSRB.Sput(source,path.Data(),"-v -M");
}

//___________________________________________________________________________

Bool_t SRBDataRepository::CheckFileStatus(const Char_t * datasetdir,
                                         const Char_t * datatype,
                                         const Char_t * runfile)
{
   //Checks if the run file of given type is physically present in dataset subdirectory,
   //i.e. (schematically), if
   //
   //      /root_of_data_repository/[datasetdir]/[datatype]/[runfile]
   //
   //exists. If it does, the returned value is kTRUE (=1).

   TString path, tmp;
   AssignAndDelete(tmp,
                   gSystem->ConcatFileName(fAccessroot.Data(),
                                           datasetdir));
   AssignAndDelete(path, gSystem->ConcatFileName(tmp.Data(), GetDatatypeSubdir(datatype)));
   return fSRB.DirectoryContains(runfile,path.Data());
}

//___________________________________________________________________________

void SRBDataRepository::MakeSubdirectory(const Char_t * datasetdir,
                                        const Char_t * datatype)
{
	// Overrides KVDataRepository method.
   TString path, tmp;
   AssignAndDelete(tmp,
                   gSystem->ConcatFileName(fAccessroot.Data(),
                                           datasetdir));
   AssignAndDelete(path, gSystem->ConcatFileName(tmp.Data(), GetDatatypeSubdir(datatype)));
	fSRB.Smkdir(path.Data());
}

//___________________________________________________________________________

void SRBDataRepository::DeleteFile(const Char_t * datasetdir,
                                  const Char_t * datatype,
                                  const Char_t * filename, Bool_t confirm)
{
   //Delete repository file [datasetdir]/[datatype]/[filename]
   //
   //By default (confirm=kTRUE) we ask for confirmation before deleting.
   //Set confirm=kFALSE to delete without confirmation (DANGER!!!)

   TString path, tmp;
   AssignAndDelete(path,
                   gSystem->ConcatFileName(fAccessroot.Data(),
                                           datasetdir));
   AssignAndDelete(tmp, gSystem->ConcatFileName(path.Data(), GetDatatypeSubdir(datatype)));
   AssignAndDelete(path, gSystem->ConcatFileName(tmp.Data(), filename));
   TString cmd;
   cout << "Deleting file from repository: " << filename << endl;
   if (confirm) {
      cout <<
          "Are you sure you want to delete this file permanently ? (y/n)"
          << endl;
      TString answer;
      cin >> answer;
      answer.ToUpper();
      if (!answer.BeginsWith("Y")) {
         cout << "File not deleted" << endl;
         return;
      }
   }
   fSRB.Srm(path.Data(),"-f");
}

//___________________________________________________________________________

int SRBDataRepository::Chmod(const char *file, UInt_t mode)
{
	// Overrides KVDataRepository method.
	// Not used with SRB (???) - Method does nothing.
	return 0;
}
//___________________________________________________________________________

Bool_t SRBDataRepository::GetFileInfo(const Char_t * datasetdir,
                                     const Char_t * datatype,
                                     const Char_t * runfile,
                                     FileStat_t & fs)
{
   //Checks if the run file of given type is physically present in dataset subdirectory,
   //i.e. (schematically), if
   //
   //      /root_of_data_repository/[datasetdir]/[datatype]/[runfile]
   //
   //exists. If it does, the returned value is kTRUE (=1), in which case the FileStat_t object
   //contains information about the file:
	// WARNING:
	//   only  fs.fSize and fs.fMtime are used
	// in addition, the modification time corresponds to the last time that
	// the MCAT/SRB declaration of the file was changed, not the last physical
	// modification of the file, i.e. it will be the date at which the file
	// was imported into the SRB catalogue, the file may be much older.
	
   TString path, tmp;
   AssignAndDelete(path,
                   gSystem->ConcatFileName(fAccessroot.Data(),
                                           datasetdir));
   AssignAndDelete(tmp, gSystem->ConcatFileName(path.Data(), GetDatatypeSubdir(datatype)));
   AssignAndDelete(path, gSystem->ConcatFileName(tmp.Data(), runfile));
	
	SRBFile_t srbfile;
	if(fSRB.GetPathInfo(path.Data(), srbfile)){
		fs.fSize = srbfile.GetSize();
		fs.fMtime = srbfile.GetModTime().Convert();
		return kTRUE;
	}
   return kFALSE;
}

