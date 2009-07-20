/*
$Id: KVAvailableRunsFile.cpp,v 1.18 2009/03/12 14:01:02 franklan Exp $
$Revision: 1.18 $
$Date: 2009/03/12 14:01:02 $
*/

//Created by KVClassFactory on Fri May  5 10:46:40 2006
//Author: franklan

#include "KVAvailableRunsFile.h"
#include "KVDataSet.h"
#include "KVDataBase.h"
#include "KVDBRun.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "KVString.h"
#include "KVList.h"
#include "KVDataRepository.h"

//macro converting octal filemode to decimal value
//to convert e.g. 664 (=u+rw, g+rw, o+r) use CHMODE(6,6,4)
#define CHMODE(u,g,o) ((u << 6) + (g << 3) + o)

ClassImp(KVAvailableRunsFile)
////////////////////////////////////////////////////////////////////////////////
//Handles text files containing list of available runs for different datasets and types of data
//
//For each type of data associated with each dataset (type="raw", "recon", "ident" or "root")
//we maintain runlist files which contain the run numbers of datafiles which are physically
//present/available, along with the date/time of last modification of each file, and the filename.
//
//These files are kept in the dataset's KVFiles subdirectory, i.e. in $KVROOT/KVFiles/[name of dataset]
//
//The name of each file has the following format:
//
//      [repository].available_runs.[dataset subdir].[type of data]
//
////////////////////////////////////////////////////////////////////////////////
KVAvailableRunsFile::KVAvailableRunsFile(const Char_t * type,
                                             KVDataSet *
                                             parent):KVBase(type)
{
   //Constructor with name of datatype ("raw", "recon", "ident", "root") and pointer
   //to dataset to which this file belongs
   fDataSet = parent;
   //runlist lockfile
   runlist_lock.SetTimeout( 60 ); // 60-second timeout in case of problems
   runlist_lock.SetSuspend( 5 ); // suspension after timeout
   runlist_lock.SetSleeptime( 1 ); // try lock every second
}

KVAvailableRunsFile::KVAvailableRunsFile(const Char_t * type):KVBase(type)
{
   //Constructor with name of datatype ("raw", "recon", "ident", "root")
   fDataSet = 0;
   //runlist lockfile
   runlist_lock.SetTimeout( 60 ); // 60-second timeout in case of problems
   runlist_lock.SetSuspend( 5 ); // suspension after timeout
   runlist_lock.SetSleeptime( 1 ); // try lock every second
}

KVAvailableRunsFile::KVAvailableRunsFile()
{
   //Default ctor
   fDataSet = 0;
   //runlist lockfile
   runlist_lock.SetTimeout( 60 ); // 60-second timeout in case of problems
   runlist_lock.SetSuspend( 5 ); // suspension after timeout
   runlist_lock.SetSleeptime( 1 ); // try lock every second
}

KVAvailableRunsFile::~KVAvailableRunsFile()
{
   //Destructor
   //cout << "KVAvailableRunsFile::~KVAvailableRunsFile()" << endl;
}

//__________________________________________________________________________________________________________________

const Char_t *KVAvailableRunsFile::GetFileName()
{
   // Filename of text file containing information on available runs
   // i.e. [repository].available_runs.[dataset subdir].[type of data]
   
   static TString filename;
   filename.Form("%s.available_runs.%s.%s", fDataSet->GetRepository()->GetName(), fDataSet->GetDatapathSubdir(),
                 GetName());
   return filename.Data();
}

//__________________________________________________________________________________________________________________

Int_t KVAvailableRunsFile::IsRunFileName(const Char_t * filename)
{
   //This method tests the string given as 'filename' to see if it could be the name of a runfile
   //of the datatype of this available runs file (GetDataType()), for its parent dataset, fDataSet.
   //Any protocol and/or path information in the filename is first removed, i.e. if
   //e.g. "run8805.ident.root.2006-12-06_19:41:56" is a valid run filename, then the following
   //are also valid and will return the run number 8805:
   //
   //    "/hpss/in2p3.fr/group/indra/campagne5/ident/run8805.ident.root.2006-12-06_19:41:56"
   //    "root://ccxroot.in2p3.fr:1999//hpss/in2p3.fr/group/indra/campagne5/ident/run8805.ident.root.2006-12-06_19:41:56"
   //    "rfio:cchpssindra:/hpss/in2p3.fr/group/indra/campagne5/ident/run8805.ident.root.2006-12-06_19:41:56"
   //
   //The decision as to whether the filename is valid or not is based on whether it corresponds
   //to one of the formats defined in $KVROOT/KVFiles/.kvrootrc, e.g. :
   //
   //# Default formats for runfile names (run number is used to replace integer format)
   //DataSet.RunFileName.raw:    run%d.raw
   //DataSet.RunFileName.recon:    run%d.recon.root
   //DataSet.RunFileName.ident:    run%d.ident.root
   //DataSet.RunFileName.root:    run%d.root
   //
   //We also test, for each format, "format%*", in case the filename has the file creation date
   //string appended to it.
   //
   //If a working format is found, we return the run number deduced from the filename.
   //If not, we return 0.

   //get format string for current dataset
   TString fmt =
       fDataSet->
       GetDataSetEnv(Form("DataSet.RunFileName.%s", GetDataType()));
   if (fmt == "") {
      Error("IsRunFileName", "No default format set for datatype: %s",
            GetDataType());
      return 0;
   }
   Int_t run;
   //Test format of filename
   KVString _file( gSystem->BaseName(filename) );//Remove protocol and/or path
   if (_file.Sscanf(fmt.Data(), &run))
      return run;
   //wtih date at end ?
   fmt += "%*";
   if (_file.Sscanf(fmt.Data(), &run))
      return run;
   return 0;
}

//__________________________________________________________________________________________________________________

void KVAvailableRunsFile::Update()
{
   //Examine the contents of the repository directory corresponding to this datatype
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
   KVBase *objs;
   //progress bar
   Int_t ntot = dir_list->GetSize();
   Int_t n5pc = TMath::Max(ntot / 20, 1);
   Int_t ndone = 0;
   KVDBTable *run_table = fDataSet->GetDataBase()->GetTable("Runs");
   while ((objs = (KVBase *) next())) {     // loop over all entries in directory

      Int_t run_num;
      //is this the correct name of a run in the repository ?
      if ((run_num = IsRunFileName(objs->GetName()))) {

         KVDBRun *run = (KVDBRun *) run_table->GetRecord(run_num);
         if (run) {
            FileStat_t fs;
            //get file modification date
            if (repository->
                GetFileInfo(datapath_subdir, GetDataType(),
                            objs->GetName(), fs)) {
               //runfile exists in repository
               //write in temporary runlist file '[run number]|[date of modification]|[name of file]
               TDatime modt(fs.fMtime);
               tmp_file << run->GetNumber() << '|' << modt.
                   AsSQLString() << '|' << objs->GetName() << endl;
            }
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

//__________________________________________________________________________________________________________________

Bool_t KVAvailableRunsFile::GetRunInfo(Int_t run, TDatime & modtime,
                                       TString & filename)
{
   //Look for a given run number in the file, and read the file's modification date/time and filename
   //If run not found, returns kFALSE
   //If available runs file does not exist, Update() is called to create it.
   //If there are multiple versions of the same run (abnormal situation),
   //we print a warning and give info on the most recent file.

   KVList filenames, dates;
   //get infos for all versions of run
   GetRunInfos(run, &dates, &filenames);
   if (filenames.GetEntries()==1){//only one version
      filename = ((TObjString *) filenames.First())->String();
      modtime = ((TObjString *) dates.First())->String().Data();
      return kTRUE;
   } else if(filenames.GetEntries()>1){//several versions
      Warning("GetRunInfo",
            "Multiple versions of this runfile exist in the repository. Infos for most recent file will be returned.");
      Warning("GetRunInfo",
            "You should clean the repository using KVDataSet::CleanMultiRunfiles.");
      //find most recent version
      TDatime most_recent("1998-12-25 00:00:00");
      Int_t i_most_recent = 0;
      for (int i = 0; i < dates.GetEntries(); i++) {
         //check if run is most recent
         TDatime rundate(((TObjString *) dates.At(i))->String().Data());
         if (rundate > most_recent) {
            most_recent = rundate;
            i_most_recent = i;
         }
      }
      filename = ((TObjString *) filenames.At(i_most_recent))->String();
      modtime = ((TObjString *) dates.At(i_most_recent))->String().Data();
      return kTRUE;
   }
   return kFALSE;
}

//__________________________________________________________________________________________________________________

void KVAvailableRunsFile::GetRunInfos(Int_t run, KVList * dates,
                                      KVList * files)
{
   //Look for a given run number in the file, and read the file's modification date/time and filename
   //These informations are stored in the two TList as TObjString objects (these objects belong to the
   //lists and will be deleted by them).
   //We do not stop at the first run found, but continue until the end of the file, adding
   //informations for every occurence of the run in the file.
   //If available runs file does not exist, Update() is called to create it.

   //does runlist exist ?
   if (!OpenAvailableRunsFile()) {
      Error("GetRunInfos", "Error opening available runs file");
      return;
   }
   //clear lists - delete objects
   dates->Delete();
   files->Delete();

   //loop over lines in fRunlist file
   //look for line beginning with 'run|'
   TString line;
   line.ReadLine(fRunlist);
   while (fRunlist.good()) {
      if (line.BeginsWith(Form("%d|", run))) {

         //found it
         TObjArray *toks = line.Tokenize('|');  // split into fields
         //add date string
         dates->Add(toks->At(1)->Clone());

         //backwards compatibility
         //an old available_runs file will not have the filename field
         //in this case we assume that the name of the file is given by the
         //dataset's base file name (i.e. with no date/time suffix)
         if (toks->GetEntries() > 2) {
            files->Add(toks->At(2)->Clone());
         } else {
            files->
                Add(new
                    TObjString(fDataSet->
                               GetBaseFileName(GetDataType(), run)));
         }
         delete toks;

      }
      line.ReadLine(fRunlist);
   }
   CloseAvailableRunsFile();
}

//__________________________________________________________________________________________________________________

Bool_t KVAvailableRunsFile::CheckAvailable(Int_t run)
{
   //Look for a given run number in the file
   //If run not found, returns kFALSE
   //If available runs file does not exist, Update() is called to create it.

   //does runlist exist ?
   if (!OpenAvailableRunsFile()) {
      Error("CheckAvailable", "Error opening available runs file");
      return kFALSE;
   }
   //loop over lines in runlist file
   //look for line beginning with 'run|'
   Bool_t found = kFALSE;
   TString line;
   line.ReadLine(fRunlist);
   while (fRunlist.good()) {
      if (line.BeginsWith(Form("%d|", run))) {
         CloseAvailableRunsFile();
         return kTRUE;
      }
      line.ReadLine(fRunlist);
   }
   CloseAvailableRunsFile();
   return found;
}

//__________________________________________________________________________________________________________________

Int_t KVAvailableRunsFile::Count(Int_t run)
{
   //Count the number of times a given run number appears in the file
   //If available runs file does not exist, Update() is called to create it.

   //does runlist exist ?
   if (!OpenAvailableRunsFile()) {
      Error("Count", "Error opening available runs file");
      return 0;
   }
   //loop over lines in runlist file
   //look for line beginning with 'run|'
   TString line;
   Int_t occurs = 0;
   line.ReadLine(fRunlist);
   while (fRunlist.good()) {
      if (line.BeginsWith(Form("%d|", run))) {
         occurs++;
      }
      line.ReadLine(fRunlist);
   }
   CloseAvailableRunsFile();
   return occurs;
}

//__________________________________________________________________________________________________________________

const Char_t *KVAvailableRunsFile::GetFileName(Int_t run)
{
   //Read from available runs list the name of the file used for this run
   //If run is not available, filename will be empty
   
   static TString fname;
   static TDatime dtime;
   if(GetRunInfo(run, dtime, fname)){
      return fname.Data();
   }
   
   fname = "";
   return fname.Data();
}

//__________________________________________________________________________________________________________________

TList *KVAvailableRunsFile::GetListOfAvailableSystems(const KVDBSystem *
                                                      systol)
{
   //Create and fill a sorted list of available systems based on the runs in the available runs file.
   //If systol!=0 then create and fill a list of available runs for the given system
   //USER MUST DELETE THE LIST AFTER USE.
   //
   //For each system in the list we set the number of available runs : this number
   //can be retrieved with KVDBSystem::GetNumberRuns()
   //
   //If available runs file does not exist, Update() is called to create it.
   //
   //If no systems have been defined for the dataset, we return a list of available runs

   //does runlist exist ?
   if (!OpenAvailableRunsFile()) {
      Error("GetListOfAvailableSystems",
            "Error opening available runs file");
      return 0;
   }

   TString fLine;
   TList *sys_list = 0;
   Int_t good_lines = 0;
   fLine.ReadLine(fRunlist);

   Int_t fRunNumber;
   TDatime fDatime;
   KVDBTable *runs_table = fDataSet->GetDataBase()->GetTable("Runs");
   Bool_t noSystems = (!fDataSet->GetDataBase()->GetTable("Systems")->GetRecords()->GetSize());

   while (fRunlist.good()) {

     TObjArray *toks = fLine.Tokenize('|');    // split into fields
     if(toks->GetSize()){
        
      KVString kvs(((TObjString *) toks->At(0))->GetString());
      
   if(kvs.IsDigit()){
      
      good_lines++;
      
      fRunNumber = kvs.Atoi();

      if (!systol) {
         TString tmp = ((TObjString *) toks->At(1))->GetString();
         fDatime = TDatime(tmp.Data());
      }

      KVDBRun *a_run = (KVDBRun *) runs_table->GetRecord(fRunNumber);

      KVDBSystem *sys = 0;
      if (a_run && !noSystems)
         sys = a_run->GetSystem();
      if (!noSystems && !systol) {
         //making a systems list
         if (!sys_list)
            sys_list = new TList;
         if (sys) {
				

				/* Block Modified() signal being emitted by KVDBRun object
				   when we set its 'datime'. This is to avoid seg fault with
					KVDataAnalysisGUI, when we are changing system/display,
					and the KVLVEntry to which this signal was previously connected
					no longer exists */
            a_run->BlockSignals(kTRUE);
				a_run->SetDatime(fDatime);
				a_run->BlockSignals(kFALSE);

            if (!sys_list->Contains(sys)) {
               //new system
               sys_list->Add(sys);
               sys->SetNumberRuns(1);   //set run count to 1
            } else {
               //another run for this system
               sys->SetNumberRuns(sys->GetNumberRuns() + 1);
            }
         }
      } else {
         //making a runlist
         if (noSystems || (systol == sys)) {   //run belongs to same system
            if (!sys_list)
               sys_list = new TList;
            if(noSystems && a_run){
					/* Block Modified() signal being emitted by KVDBRun object
				   when we set its 'datime'. This is to avoid seg fault with
					KVDataAnalysisGUI, when we are changing system/display,
					and the KVLVEntry to which this signal was previously connected
					no longer exists */
            	a_run->BlockSignals(kTRUE);
					a_run->SetDatime(fDatime);
					a_run->BlockSignals(kFALSE);
				}
            if(a_run) sys_list->Add(a_run);
         }
      }
     }
     }
     delete toks;
      fLine.ReadLine(fRunlist);
   }

   //sort list of systems in order of increasing run number
   //sort list of runs in order of increasing run number
   if (sys_list && sys_list->GetSize()>1)
      sys_list->Sort();

   if(!good_lines){
      Error("GetListOfAvailableSystems",
            "Available runs file is empty or absent");
   }
   CloseAvailableRunsFile();
   return sys_list;

}

//__________________________________________________________________________________________________________________

void KVAvailableRunsFile::Remove(Int_t run, const Char_t * filename)
{
   //Remove from the file the entry corresponding to this run
   //By default, the first occurrence of the run number in the file will be removed.
   //If "filename" is given, we look for a line corresponding to both the run number
   //and the filename (important if run appears more than once !!!) ;-)

   //does runlist exist ?
   if (!OpenAvailableRunsFile()) {
      Error("Remove", "Error opening available runs file");
      return;
   }
   //open temporary file
   TString tmp_file_path(GetFileName());
   ofstream tmp_file;
   KVBase::OpenTempFile(tmp_file_path, tmp_file);

   TString FileName(filename);
   Bool_t withFileName = (FileName != "");
   //loop over lines in fRunlist file
   //all lines which do not begin with 'run'| are directly copied to temp file
   TString line;
   line.ReadLine(fRunlist);
   while (fRunlist.good()) {
      if (!withFileName) {
         //filename not specified : we copy everything up to the first line
         //which begins with the run number we want
         if (!line.BeginsWith(Form("%d|", run))) {
            //copy line
            tmp_file << line.Data() << endl;
         }
      } else {
         //filename was specified: we copy everything up to the line
         //with the right filename & number
         if (line.BeginsWith(Form("%d|", run))) {

            TObjArray *toks = line.Tokenize('|');       // split into fields
            TString ReadFileName;
            //backwards compatibility
            //an old available_runs file will not have the filename field
            //in this case we assume that the name of the file is given by the
            //dataset's base file name (i.e. with no date/time suffix)
            if (toks->GetEntries() > 2) {
               ReadFileName = ((TObjString *) toks->At(2))->String();
            } else {
               ReadFileName =
                   fDataSet->GetBaseFileName(GetDataType(), run);
            }
            delete toks;

            if (ReadFileName != FileName) {
               //copy line
               tmp_file << line.Data() << endl;
            }

         } else {
            //copy line
            tmp_file << line.Data() << endl;
         }
      }
      line.ReadLine(fRunlist);
   }

   CloseAvailableRunsFile();
   
   //close temp file
   tmp_file.close();

   //copy temporary file to KVFiles directory, overwrite previous
   TString fRunlist_path;
   AssignAndDelete(fRunlist_path,
                   gSystem->ConcatFileName(fDataSet->GetDataSetDir(),
                                           GetFileName()));
   //lock runsfile
   if( !runlist_lock.Lock( fRunlist_path.Data() ) ) return;
   
   gSystem->CopyFile(tmp_file_path, fRunlist_path, kTRUE);
   //set access permissions to 664
   gSystem->Chmod(fRunlist_path.Data(), CHMODE(6,6,4));
   //delete temp file
   gSystem->Unlink(tmp_file_path);
   //unlock runsfile
   runlist_lock.Release();
}

//__________________________________________________________________________________________________________________

void KVAvailableRunsFile::Add(Int_t run, const Char_t * filename)
{
   //Add to the file an entry corresponding to this run, assumed to be present in the repository
   //with the given filename.

   //does runlist exist ?
   if (!OpenAvailableRunsFile()) {
      Error("Add", "Error opening available runs file");
      return;
   }
   //open temporary file
   TString tmp_file_path(GetFileName());
   ofstream tmp_file;
   KVBase::OpenTempFile(tmp_file_path, tmp_file);

   //copy all lines in runlist file
   TString line;
   line.ReadLine(fRunlist);
   while (fRunlist.good()) {
      tmp_file << line.Data() << endl;
      line.ReadLine(fRunlist);
   }
   
   CloseAvailableRunsFile();
   
   //add entry for run
   FileStat_t fs;
   //get file modification date
   if (fDataSet->GetRepository()->
       GetFileInfo(fDataSet->GetDatapathSubdir(), GetDataType(), filename,
                   fs)) {
      //runfile exists in repository
      //write in temporary runlist file '[run number]|[date of modification]|[name of file]
      TDatime modt(fs.fMtime);
      tmp_file << run << '|' << modt.
          AsSQLString() << '|' << filename << endl;
   }
   //close temp file
   tmp_file.close();

   //copy temporary file to KVFiles directory, overwrite previous
   TString runlist_path;
   AssignAndDelete(runlist_path,
                   gSystem->ConcatFileName(fDataSet->GetDataSetDir(),
                                           GetFileName()));
   //lock runsfile
   if( !runlist_lock.Lock( runlist_path.Data() ) ) return;
   gSystem->CopyFile(tmp_file_path, runlist_path, kTRUE);
   //set access permissions to 664
   gSystem->Chmod(runlist_path.Data(), CHMODE(6,6,4));
   //delete temp file
   gSystem->Unlink(tmp_file_path);
   //unlock runsfile
   runlist_lock.Release();
}

//__________________________________________________________________________________________________________________

Bool_t KVAvailableRunsFile::OpenAvailableRunsFile()
{
   //Initialise fRunlist so that it can be used to read the available runs file,
   //opens the file containing the list of available runs for the current dataset.
   //If the file does not exist, call Update() to create it.
   //
   //Returns kFALSE in case of problems.

   TString runlist;             //absolute path to runs file
   AssignAndDelete(runlist,
                   gSystem->ConcatFileName(fDataSet->GetDataSetDir(),
                                           GetFileName()));
   fRunlist.clear();            // clear any error flags (EOF etc.) before trying to open file
   if (!SearchAndOpenKVFile(runlist, fRunlist, "", &runlist_lock)) {
      //no runlist exists. we therefore have to create it.
      Update();
      if (!SearchAndOpenKVFile(runlist, fRunlist, "", &runlist_lock)) {
         Error("OpenAvailableRunsFile",
               "Something weird: I just made the available runlist file, but I still can't open it!");
         return kFALSE;
      }
   }
      
   return kTRUE;
}

//__________________________________________________________________________________________________________________

void KVAvailableRunsFile::CloseAvailableRunsFile()
{
   //Must be called after each operation which calls OpenAvailableRunsFile
   //Not only do we close the file stream, we remove the file lock put in place
   //by OpenAvailableRunsFile in order to allow others to read/write the file.
   fRunlist.close();
   fRunlist.clear();
   runlist_lock.Release();
}

//__________________________________________________________________________________________________________________

KVDataSet *KVAvailableRunsFile::GetDataSet() const
{
   //Dataset to which this file belongs
   return fDataSet;
}


//__________________________________________________________________________________________________________________

void KVAvailableRunsFile::SetDataSet(KVDataSet* d)
{
   // Set dataset to which this file belongs
   fDataSet = d;
}

//__________________________________________________________________________________________________________________

KVNumberList KVAvailableRunsFile::CheckMultiRunfiles()
{
   //Returns a list with all runs which occur more than once in the available runs file.

   //does runlist exist ?
   if (!OpenAvailableRunsFile()) {
      Error("CheckMultiRunfiles", "Cannot open available runs file");
      return 0;
   }

   TString fLine;
   TList *run_list = new TList;
   fLine.ReadLine(fRunlist);

   KVNumberList multiruns;

   Int_t fRunNumber;
   KVDBTable *runs_table = fDataSet->GetDataBase()->GetTable("Runs");

   while (fRunlist.good()) {

      TObjArray *toks = fLine.Tokenize('|');    // split into fields
      KVString kvs(((TObjString *) toks->At(0))->GetString());
      fRunNumber = kvs.Atoi();
      delete toks;

      KVDBRun *a_run = (KVDBRun *) runs_table->GetRecord(fRunNumber);

      if (!run_list->Contains(a_run)) {
         //first time that run appears
         run_list->Add(a_run);
      } else {
         //run appears >1 times
         multiruns.Add(fRunNumber);
      }

      fLine.ReadLine(fRunlist);
   }
   delete run_list;
   CloseAvailableRunsFile();

   return multiruns;
}

//__________________________________________________________________________________________________________________

KVNumberList KVAvailableRunsFile::GetRunList(const KVDBSystem * sys)
{
   //Returns list of available run numbers for this data type.
   //If 'sys' gives the address of a valid database reaction system, only runs
   //corresponding to the system will be included.

   KVNumberList runs;

   //does runlist exist ?
   if (!OpenAvailableRunsFile()) {
      Error("GetRunList", "Cannot open available runs file");
      return runs;
   }

   TString fLine;
   fLine.ReadLine(fRunlist);

   Int_t fRunNumber;
   KVDBTable *runs_table = fDataSet->GetDataBase()->GetTable("Runs");

   while (fRunlist.good()) {

      TObjArray *toks = fLine.Tokenize('|');    // split into fields
      KVString kvs(((TObjString *) toks->At(0))->GetString());
      fRunNumber = kvs.Atoi();
      delete toks;

      if (sys) {
         // check run is from right system
         KVDBRun *a_run = (KVDBRun *) runs_table->GetRecord(fRunNumber);
         if (a_run) {
            if (a_run->GetSystem() == sys)
               runs.Add(fRunNumber);
         }
      } else {
         // add all runs to list
         runs.Add(fRunNumber);
      }

      fLine.ReadLine(fRunlist);
   }

   CloseAvailableRunsFile();
   return runs;
}
