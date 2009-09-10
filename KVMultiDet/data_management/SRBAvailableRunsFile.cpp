//Created by KVClassFactory on Mon Jul 20 11:33:43 2009
//Author: John Frankland

#include "SRBAvailableRunsFile.h"
#include "SRB.h"
#include "KVDataRepository.h"
#include "KVDataSet.h"
#include "KVDataBase.h"
#include "KVDBTable.h"
#include "KVDBRun.h"

//macro converting octal filemode to decimal value
//to convert e.g. 664 (=u+rw, g+rw, o+r) use CHMODE(6,6,4)
#define CHMODE(u,g,o) ((u << 6) + (g << 3) + o)

ClassImp(SRBAvailableRunsFile)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>SRBAvailableRunsFile</h2>
<h4>Handles list of available runs using SRB</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

SRBAvailableRunsFile::SRBAvailableRunsFile()
{
   // Default constructor
}

SRBAvailableRunsFile::SRBAvailableRunsFile(const Char_t* type)
		: KVAvailableRunsFile(type)
{
   // Constructor with data-type
}

SRBAvailableRunsFile::~SRBAvailableRunsFile()
{
   // Destructor
}


void SRBAvailableRunsFile::Update()
{
   //Examine the contents of the SRB container corresponding to this datatype
   //for parent dataset fDataSet.
   //For each file found which corresponds to a run in the database gDataBase,
   //we add an entry to the available runlist file:
   //      [run number]|[date of modification]|[name of file]

   //open temporary file
   TString tmp_file_path(GetFileName());
   ofstream tmp_file;
   KVBase::OpenTempFile(tmp_file_path, tmp_file);

   TString datapath_subdir = fDataSet->GetDatapathSubdir();
   KVDataRepository *repository = fDataSet->GetRepository();

   cout << endl << "Updating runlist : " << flush;
   //get directory listing from repository
   TList *dir_list =
       repository->GetDirectoryListing(datapath_subdir, GetDataType());
   if (!dir_list)
      return;

   TIter next(dir_list);
   SRBFile_t *objs;
   //progress bar
   Int_t ntot = dir_list->GetSize();
   Int_t n5pc = TMath::Max(ntot / 20, 1);
   Int_t ndone = 0;
   KVDBTable *run_table = fDataSet->GetDataBase()->GetTable("Runs");
   while ((objs = (SRBFile_t *) next())) {     // loop over all entries in directory

      Int_t run_num;
      //is this the correct name of a run in the repository ?
      if ((run_num = IsRunFileName(objs->GetName()))) {

         KVDBRun *run = (KVDBRun *) run_table->GetRecord(run_num);
         if (run) {
               //runfile exists in repository
               //write in temporary runlist file '[run number]|[date of modification]|[name of file]
               tmp_file << run->GetNumber() << '|' << objs->GetModTime().AsSQLString() << '|' << objs->GetName() << endl;
         }
      }

      ndone++;
      if (!(ndone % n5pc))
         cout << '>' << flush;
   }

   cout << " DONE" << endl;
   delete dir_list;
   //close temp file
   tmp_file.close();

   //copy temporary file to KVFiles directory, overwrite previous
   TString runlist;
   AssignAndDelete(runlist,
                   gSystem->ConcatFileName(fDataSet->GetDataSetDir(),
                                           GetFileName()));
   //use "lockfile" to make sure nobody else tries to write available_runs file
   //while we are copying it
   if(!runlist_lock.Lock(runlist.Data())) return;
   
   gSystem->CopyFile(tmp_file_path, runlist, kTRUE);
   //set access permissions to 664
   gSystem->Chmod(runlist.Data(), CHMODE(6,6,4));
   
   //remove lockfile
   runlist_lock.Release();
   
   //delete temp file
   gSystem->Unlink(tmp_file_path);
}
