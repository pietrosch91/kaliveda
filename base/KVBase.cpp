/***************************************************************************
$Id: KVBase.cpp,v 1.54 2009/01/16 14:55:20 franklan Exp $
                          kvbase.cpp  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Riostream.h"
#include "TMath.h"
#include "KVBase.h"
#include "TClass.h"
#include "KVString.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TPluginManager.h"
#include "KVParameterList.h"
#include "TSystemDirectory.h"
#include "../../KVVersion.h"
#include "TROOT.h"
#include "TDatime.h"
#include "THashList.h"
#include "TError.h"
#include "KVCVSUpdateChecker.h"
#include "KVConfig.h"

ClassImp(KVBase)
//////////////////////////////////////////////////////////////////////////////////
//          *****       Base class for all KaliVeda objects.      ******
//
// All objects in the KaliVeda environment inherit from KVBase,
// which itself inherits from the ROOT class of named objects, TNamed.
// The fTitle member of TNamed is used to hold the "type" of the object,
// vias SetType (=SetTitle) and GetType(=GetTitle).
// We also have a "label" via SetLabel and GetLabel.
//
// The total number of KVBase (and derived) objects in existence can be obtained
// from GetNumberOfObjects().
// The first KVBase object created will:
//  - check that the KVROOT environment variable has been set
//  - reset the gRandom random number sequence using a clock-based seed (i.e. sequences do not repeat).
//
//Global string pointers gKVRootDir, gKVBinDir and gKVFilesDir hold full paths to $KVROOT, $KVROOT/bin
//and $KVROOT/KVFiles directories, respectively.
//
//A global "file search" function is also defined in this file (it is used by SearchKVFile()):
//
//Bool_t SearchFile(const Char_t* name, TString& fullpath, int ndirs, ...)
//{
//      Search for file in an arbitrary number of locations, return kTRUE if file found and put full path to file in 'fullpath"
//
//      'name' is a filename (not an absolute pathname) i.e. "toto.dat"
//      'fullpath" will contain the full path to the file if it is found (if file not found, fullpath="")
//      'ndirs" is the number of directories to search in
//      the remaining arguments are the names of 'ndirs' paths to search in, i.e.
//
//              SearchFile("toto.dat", fullpath, 2, gSystem->pwd(), gSystem->HomeDirectory());
//
//      means: search for a file 'toto.dat' in current working directory, then user's home directory.
//
//              SearchFile("toto.dat", fullpath, 3, gKVFilesDir, gKVRootDir, gRootDir);
//
//      means: search for a file 'toto.dat' in $KVROOT/KVFiles, in $KVROOT, and finally in $ROOTSYS
//}
UInt_t KVBase::fNbObj = 0;

TString KVBase::KVRootDir = "";
TString KVBase::KVBinDir = "";
TString KVBase::KVFilesDir = "";

Bool_t KVBase::fEnvIsInit = kFALSE;

const Char_t *KVBase::GetKVRootDir(void)
{
   // Initialises environment if neccesary.
   if (!fEnvIsInit) InitEnvironment();
   return KVRootDir.Data();
}
const Char_t *KVBase::GetKVBinDir(void)
{
   // Initialises environment if neccesary.
   if (!fEnvIsInit) InitEnvironment();
   return KVBinDir.Data();
}
const Char_t *KVBase::GetKVFilesDir(void)
{
   // Initialises environment if neccesary.
   if (!fEnvIsInit) InitEnvironment();
   return KVFilesDir.Data();
}

//_______________
void KVBase::init()
{
   //Default initialisation

   InitEnvironment();
   fNumber = 0;
   fNbObj++;
   fSLabel = "";
}

void KVBase::InitEnvironment()
{
   // STATIC method to Initialise KaliVeda environment
   // Stores paths to essential KaliVeda files using KVROOT environment variable
   // (which must be set by the user before using or indeed installing KaliVeda).
   // Reads config file $KVROOT/KVFiles/.kvrootrc and sets up environment
   // (data repositories, datasets, etc. etc.)
   // Resets the gRandom random number sequence using a clock-based seed
   // (i.e. random sequences do not repeat).
   // 
   // If automatic update checking is enabled (KaliVeda.AutoUpdateCheck:   yes),
   // we perform the check here. If updates are found, we download them and
   // then rebuild the sources. This will force the current application to terminate.
   //
   // Normally, the first object created which inherits from KVBase will
   // perform this initialisation; if you need to set up the environment before
   // creating a KVBase object, or if you just want to be absolutely sure that
   // the environment has been initialised, you can call this method.

   if (!fEnvIsInit) {//test if environment already initialised
   
      ::Info("KVBase::InitEnvironment", "Initialising KaliVeda environment...");
      KVRootDir = gSystem->Getenv("KVROOT");
      TString tmp;
      AssignAndDelete(tmp,
                      gSystem->ConcatFileName(KVRootDir.Data(), "KVFiles"));
      KVFilesDir = StrDup(tmp.Data());
      AssignAndDelete(tmp, gSystem->ConcatFileName(KVRootDir.Data(), "bin"));
      KVBinDir = StrDup(tmp.Data());
      //set up environment using kvrootrc file
      if (!gEnv->Defined("DataSet.DatabaseFile")) {
         AssignAndDelete(tmp,
                         gSystem->ConcatFileName(KVFilesDir.Data(),
                                                 ".kvrootrc"));
         gEnv->ReadFile(tmp.Data(), kEnvGlobal);

         AssignAndDelete(tmp,
                         gSystem->ConcatFileName(gSystem->Getenv("HOME"),
                                                 ".kvrootrc"));
         gEnv->ReadFile(tmp.Data(), kEnvUser);
         tmp = "./.kvrootrc";
         gEnv->ReadFile(tmp.Data(), kEnvLocal);

         gROOT->GetPluginManager()->LoadHandlersFromEnv(gEnv);
         
      }
      
      // update check - do not perform if ROOT is running in batch mode
      if(!gROOT->IsBatch() && gEnv->GetValue("KaliVeda.AutoUpdateCheck", kFALSE)){
         KVCVSUpdateChecker upchk;
         upchk.SetSourceDir( GetKVSourceDir() );
         if( upchk.NeedsUpdate() ){
            ::Info("KVBase::InitEnvironment",
                  "A more recent version of KaliVeda is available");
            cout << "Download updates (y/n) ? ";
            KVString answ; answ.ReadLine(cin);
				cout << endl;
            if(answ=="y") upchk.Update();
            //cout << "Build and install (y/n) ? ";
				//answ.ReadLine(cin);
         }
      }
      
      //generate new seed from system clock
      gRandom->SetSeed(0);
      
      // initialisation has been performed
      fEnvIsInit = kTRUE;
   }
}

//_______________________________________________________________________________
KVBase::KVBase()
{
   //Default constructor.
   init();
}

//_______________________________________________________________________________
KVBase::KVBase(const Char_t * name, const Char_t * type):TNamed(name, type)
{
   //Ctor for object with given name and type.
   init();
}

//______________________
KVBase::KVBase(const KVBase & obj)
{
   //copy ctor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVBase &) obj).Copy(*this);
#endif
}

//_______________________________________________________________________________
KVBase::~KVBase()
{
   fNbObj--;
}

//_______________________________________________________________________________

void KVBase::Clear(Option_t * opt)
{
   //Clear object properties : name, type/title, number, label
   TNamed::Clear(opt);
   fNumber = 0;
   fSLabel = "";
}

//___________________________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVBase::Copy(TObject & obj) const
#else
void KVBase::Copy(TObject & obj)
#endif
{
   //Copy this to obj
   //Redefinition of TObject::Copy

   TNamed::Copy(obj);
   ((KVBase &) obj).SetNumber(fNumber);
   ((KVBase &) obj).SetLabel(fSLabel);
}

//____________________________________________________________________________________
void KVBase::Print(Option_t * option) const
{
   cout << "KVBase object: fName=" << GetName() << " fType=" << GetType();
   if (fSLabel != "")
      cout << " fSLabel=" << GetLabel();
   cout << endl;
};

void KVBase::SetLabel(const Char_t * lab)
{
   //Affect a label to this object
   fSLabel = lab;
}

const Char_t *KVBase::GetLabel() const
{
   //Return object label

   return fSLabel.Data();
}

const Char_t *KVBase::GetKVRoot()
{
   // Returns string containing full path to $KVROOT directory.
   // Initialises environment if neccesary.
   if (!fEnvIsInit) InitEnvironment();
   return KVBase::KVRootDir.Data();
};

Bool_t KVBase::ArrContainsValue(Int_t n, Int_t * arr, Int_t val)
{
   //Utility function which returns kTRUE if value "val" is among the values
   //contained in the array "arr" of "n" members.
   //On the contrary to TMath::BinarySearch, it is not necessary to sort the
   //array beforehand.
   while (n--) {
      if (arr[n] == val)
         return kTRUE;
   }
   return kFALSE;
}

void KVBase::Streamer(TBuffer & R__b)
{
   //Backwards compatible streamer for KVBase objects
   //Needed to handle 'fLabel' char array in class version 1

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v > 1) {
         //AUTOMATIC STREAMER EVOLUTION FOR CLASS VERSION > 1
         KVBase::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
         return;
      }
      //OLD STREAMER FOR CLASS VERSION 1
      TNamed::Streamer(R__b);
      R__b >> fNumber;
      UInt_t LabelLength;
      R__b >> LabelLength;
      if (LabelLength) {
         Char_t *Label = new Char_t[LabelLength];
         R__b.ReadFastArray(Label, LabelLength);
         fSLabel = Label;
         delete[]Label;
      }
      R__b.CheckByteCount(R__s, R__c, KVBase::IsA());
   } else {
      KVBase::Class()->WriteBuffer(R__b, this);
   }
}

//________________________________________________________________________________//

Bool_t SearchFile(const Char_t * name, TString & fullpath, int ndirs, ...)
{
   //Search for file in an arbitrary number of locations, return kTRUE if file found and put full path to file in 'fullpath"
   //
   //'name' is a filename (not an absolute pathname) i.e. "toto.dat"
   //'fullpath" will contain the full path to the file if it is found (if file not found, fullpath="")
   //'ndirs" is the number of directories to search in
   //the remaining arguments are the names of 'ndirs' paths to search in, i.e.
   //
   //      SearchFile("toto.dat", fullpath, 2, gSystem->pwd(), gSystem->HomeDirectory());
   //
   //means: search for a file 'toto.dat' in current working directory, then user's home directory.
   //
   //      SearchFile("toto.dat", fullpath, 3, gKVFilesDir, gKVRootDir, gRootDir);
   //
   //means: search for a file 'toto.dat' in $KVROOT/KVFiles, in $KVROOT, and finally in $ROOTSYS

   if (ndirs <= 0)
      return kFALSE;

   va_list args;
   va_start(args, ndirs);

   for (; ndirs; ndirs--) {     //loop over directories

      AssignAndDelete(fullpath,
                      gSystem->ConcatFileName(va_arg(args, const char *),
                                              name));
      if (!gSystem->AccessPathName(fullpath.Data())) {
         va_end(args);
         return kTRUE;
      }

   }

   va_end(args);
   fullpath = "";               //clear fullpath string to avoid using it by mistake
   return kFALSE;
}

Bool_t KVBase::SearchKVFile(const Char_t * name, TString & fullpath,
                            const Char_t * kvsubdir)
{
   //search for files in the following order:
   //  if 'name' = absolute path the function returns kTRUE if the file exists
   //  if name != absolute path:
   //      1. a. if 'kvsubdir'="" (default) look for file in $KVROOT/KVFiles directory
   //      1. b. if 'kvsubdir'!="" look for file in $KVROOT/KVFiles/'kvsubdir'
   //      2. look for file with this name in user's home directory
   //      3. look for file with this name in working directory
   //in all cases the function returns kTRUE if the file was found.
   //'fullpath' then contains the absolute path to the file

   if (gSystem->IsAbsoluteFileName(name)) {
      //absolute path
      fullpath = name;
      return !gSystem->AccessPathName(name);
   }

   TString kvfile_dir(GetKVFilesDir());
   if (strcmp(kvsubdir, "")) {
      //KVFiles subdirectory name given
      AssignAndDelete(kvfile_dir,
                      gSystem->ConcatFileName(GetKVFilesDir(), kvsubdir));
   }
   return SearchFile(name, fullpath, 3, kvfile_dir.Data(),
                     gSystem->HomeDirectory(), gSystem->pwd());
}

//________________________________________________________________________________//

Bool_t KVBase::SearchAndOpenKVFile(const Char_t * name, ifstream & file, const Char_t * kvsubdir, KVLockfile* locks)
{
   //Search and open for READING a file:
   //
   //search for ascii file (and open it, if found) in the following order:
   //  if 'name' = absolute path the function returns kTRUE if the file exists
   //  if name != absolute path:
   //      1. a. if 'kvsubdir'="" (default) look for file in $KVROOT/KVFiles directory
   //      1. b. if 'kvsubdir'!="" look for file in $KVROOT/KVFiles/'kvsubdir'
   //      2. look for file with this name in user's home directory
   //      3. look for file with this name in working directory
   //if the file is not found, kFALSE is returned.
   //if file is found and can be opened, file' is then an ifstream connected to the open (ascii) file
   //
   //LOCKFILE:
   //If a KVLockfile pointer is given, we use it to get a lock on the file before opening it.
   //If this lock is not successful, the file is not opened and we return an error message.

   TString fullpath;
   if (SearchKVFile(name, fullpath, kvsubdir)) {
      //put lock on file if required
      if(locks && !locks->Lock( fullpath.Data() )) return kFALSE;
      file.open(fullpath.Data());
      if (file.good()) {
         //cout << "Opened file : " << fullpath.Data() << endl;
         return kTRUE;
      }
      //unlock file if not opened successfully
      if(locks) locks->Release();
   }
   return kFALSE;
}

//________________________________________________________________________________//

Bool_t KVBase::SearchAndOpenKVFile(const Char_t * name, ofstream & file, const Char_t * kvsubdir, KVLockfile* locks)
{
   //Search and CREATE i.e. open for WRITING a file:
   //
   //open for writing an ascii file in the location determined in the following way:
   //  if 'name' = absolute path we use the full path
   //  if name != absolute path:
   //      1. a. if 'kvsubdir'="" (default) file will be in $KVROOT/KVFiles directory
   //      1. b. if 'kvsubdir'!="":
   //               if 'kvsubdir' is an absolute pathname, file in 'kvsubdir'
   //               if 'kvsubdir' is not an absolute pathname (e.g. name of a dataset),
   //                    file will be in '$KVROOT/KVFiles/kvsubdir
   //if an existing file is found, a warning is printed and the existing file 'toto' is renamed
   //"toto.date". where 'date' is created with TDatime::AsSQLDate
   // file' is then an ofstream connected to the opened file
   //
   //LOCKFILE:
   //If a KVLockfile pointer is given, we use it to get a lock on the file before opening it.
   //If this lock is not successful, the file is not opened and we return an error message.

   KVString fullpath;
   if(gSystem->IsAbsoluteFileName(name)){
      fullpath = name;
   } else if(gSystem->IsAbsoluteFileName(kvsubdir)){
      AssignAndDelete(fullpath,
                      gSystem->ConcatFileName(kvsubdir, name));
   } else if(strcmp(kvsubdir,"")){
      KVString path;
      AssignAndDelete(path,
                      gSystem->ConcatFileName(GetKVFilesDir(), kvsubdir));
      AssignAndDelete(fullpath,
                      gSystem->ConcatFileName(path.Data(), name));
   } else {
      AssignAndDelete(fullpath,
                      gSystem->ConcatFileName(GetKVFilesDir(), name));
   }     
   //Backup file if necessary
   BackupFileWithDate( fullpath.Data() );
   //put lock on file if required
   if(locks && !locks->Lock( fullpath.Data() )) return kFALSE;
   file.open(fullpath.Data());
   return kTRUE;
}

//________________________________________________________________________________//

void KVBase::BackupFileWithDate(const Char_t * path)
{
   //'path' gives the full path (can include environment variables, special symbols)
   //to a file which will be renamed with an extension containing the current date and time
   //(in SQL format).
   //Example:
   //   KVBase::BackupFileWithDate("$(HOME)/toto.txt")
   //The file toto.txt will be renamed toto.txt.2007-05-02_16:22:37
   
   //does the file exist ?
   KVString fullpath=path;
   gSystem->ExpandPathName(fullpath);
   if( !gSystem->AccessPathName( fullpath.Data() ) ){
      //backup file
      TDatime now; KVString date(now.AsSQLString()); date.ReplaceAll(' ', '_');
      TString backup = fullpath + "." + date;
      //lock both files
      KVLockfile lf1(fullpath.Data()), lf2(backup.Data());
      if(lf1.Lock() && lf2.Lock()){
         gSystem->Rename(fullpath.Data(), backup.Data());
         printf("Info in <KVBase::BackupFileWithDate(const Char_t *)> : Existing file %s renamed %s\n",
            fullpath.Data(), backup.Data());
      }       
   }
}

//________________________________________________________________________________//

TPluginHandler *KVBase::LoadPlugin(const Char_t * base, const Char_t * uri)
{
   //Load plugin library in order to extend capabilities of base class "base", depending on
   //the given uri (these arguments are used to call TPluginManager::FindHandler).
   //Returns pointer to TPluginHandler.
   //Returns 0 in case of problems.

   //does plugin exist for given name ?
   TPluginHandler *ph =
       (TPluginHandler *) gROOT->GetPluginManager()->FindHandler(base,
                                                                 uri);
   if (!ph)
      return 0;

   //check plugin library/macro is available
   if (ph->CheckPlugin() != 0)
      return 0;

   //load plugin module
   if (ph->LoadPlugin() != 0)
      return 0;

   return ph;
}

//__________________________________________________________________________________________________________________

void KVBase::OpenTempFile(TString & base, ofstream & fp)
{
   //Opens a uniquely-named file in system temp directory (gSystem->TempDirectory)
   //Name of file is "basexxxxxxxxxx" where "xxxxxxxxx" is current time as returned
   //by gSystem->Now().
   //After opening file, 'base' contains full path to file.

   GetTempFileName(base);
   fp.open(base.Data());
}

//__________________________________________________________________________________________________________________

void KVBase::GetTempFileName(TString & base)
{
   //When called with base="toto.dat", the returned value of 'base' is
   //"/full/path/to/temp/dir/toto.dat15930693"
   //i.e. the full path to a file in the system temp directory (gSystem->TempDirectory)
   //appended with the current time as returned by gSystem->Now() in order to make
   //its name unique

   TString tmp1;
   AssignAndDelete(tmp1,
                   gSystem->ConcatFileName(gSystem->TempDirectory(),
                                           base.Data()));
   long lnow = (long) gSystem->Now();
   base = tmp1 + lnow;
   //make sure no existing file with same name
   while (!gSystem->AccessPathName(base)) {
      base = tmp1 + (++lnow);
   }
}

//__________________________________________________________________________________________________________________

const Char_t *KVBase::GetKVVersion()
{
   //Returns KaliVeda version string
   static TString tmp(KV_VERSION);
   return tmp.Data();
}

//__________________________________________________________________________________________________________________

const Char_t *KVBase::GetKVBuildUser()
{
   // Returns username of person who performed build
   static TString tmp(KV_BUILD_USER);
   return tmp.Data();
}

//__________________________________________________________________________________________________________________

const Char_t *KVBase::GetKVBuildDate()
{
   //Returns KaliVeda build date
   static TString tmp(KV_BUILD_DATE);
   return tmp.Data();
}

//__________________________________________________________________________________________________________________

const Char_t *KVBase::GetKVSourceDir()
{
   //Returns top-level directory of source tree used for build
   static TString tmp(KV_SOURCE_DIR);
   return tmp.Data();
}

//__________________________________________________________________________________________________________________

Bool_t KVBase::FindExecutable(TString & exec, const Char_t * path)
{
   //By default, FindExecutable(exec) will look for the executable named by 'exec'
   //in the directories contained in the environment variable PATH. You can override
   //this by giving your own search 'path' as second argument (remember to write
   //environment variables as $(PATH), for cross-platform compatibility).
   //
   //If 'exec' is not found, and if it does not end with '.exe', we look for 'exec.exe'
   //This is for compatibility with Windows/cygwin environments.
   //
   //If the executable is found, returns kTRUE and 'exec' then holds full path to executable.
   //Returns kFALSE if exec not found in path.
   //
   //If 'exec' is an absolute pathname, we return kTRUE if the file exists
   //(we do not use 'path').

   TString spath(path), backup(exec), backup2(exec),expandexec(exec);
   gSystem->ExpandPathName(expandexec);
   if (gSystem->IsAbsoluteFileName(expandexec.Data())) {
      //executable given as absolute path
      //we check if it exists
      if (!gSystem->AccessPathName(expandexec)){
         exec = expandexec;
         return kTRUE;
      } else {
         //try with ".exe" in case of Windows system
         if(!expandexec.EndsWith(".exe")){
            expandexec += ".exe";
            if (!gSystem->AccessPathName(expandexec)){
               exec = expandexec;
               return kTRUE;
            }
         }
      }
		exec=backup;
      return kFALSE;
   }
   gSystem->ExpandPathName(spath);
   if (KVBase::FindFile(spath.Data(), exec))
      return kTRUE;
   if (!backup.EndsWith(".exe")) {
      backup += ".exe";
      if (KVBase::FindFile(spath.Data(), backup)) {
         exec = backup;
         return kTRUE;
      }
   }
	exec=backup2;
   return kFALSE;
}

//__________________________________________________________________________________________________________________

const Char_t *KVBase::FindFile(const Char_t * search, TString & wfil)
{
   //Backwards compatible fix for TSystem::FindFile which only exists from 5.12/00 onwards
   //Use this method as a replacement for gSystem->FindFile (same arguments)
#ifdef __WITHOUT_TSYSTEM_FINDFILE
   Char_t *result = gSystem->Which(search, wfil.Data());
   if (result) {
      wfil = result;
      delete[]result;
   } else {
      wfil = "";
   }
   return wfil.Data();
#else
   return gSystem->FindFile(search,wfil);
#endif
}

//__________________________________________________________________________________________________________________

Bool_t KVBase::FindClassSourceFiles(const Char_t* class_name, KVString& imp_file, KVString& dec_file, const Char_t* dir_name)
{
   //Look for the source files corresponding to "class_name"
   //i.e. taking class_name as a base, we look for one of
   //  [class_name.C,class_name.cpp,class_name.cxx]
   //and one of
   //  [class_name.h,class_name.hh,class_name.H]
   //By default we look in the current working directory, unless argument 'dir_name' is given
   //If found, the names of the two files are written in 'imp_file' and 'dec_file'

   KVParameterList<int> impl_alt; int i=0;
   impl_alt.SetParameter("%s.C", i);
   impl_alt.SetParameter("%s.cpp", i);
   impl_alt.SetParameter("%s.cxx", i);
   KVParameterList<int> decl_alt;
   decl_alt.SetParameter("%s.h", i);
   decl_alt.SetParameter("%s.hh", i);
   decl_alt.SetParameter("%s.H", i);
   
   TString _dir_name = dir_name; gSystem->ExpandPathName(_dir_name);
   TSystemDirectory dir("LocDir", _dir_name.Data());
   TList *lf = dir.GetListOfFiles();
   Bool_t ok_imp, ok_dec; ok_imp = ok_dec = kFALSE;
   
   //look for implementation file
   for(i=0; i<impl_alt.GetNPar(); i++){
      if(lf->FindObject(Form( impl_alt.GetParameter(i)->GetName(), class_name ) )){
         imp_file = Form( impl_alt.GetParameter(i)->GetName(), class_name );
         ok_imp = kTRUE;
      }
   }
   //look for header file
   for(i=0; i<decl_alt.GetNPar(); i++){
      if(lf->FindObject(Form( decl_alt.GetParameter(i)->GetName(), class_name ) )){
         dec_file = Form( decl_alt.GetParameter(i)->GetName(), class_name );
         ok_dec = kTRUE;
      }
   }
   delete lf;
   return (ok_imp && ok_dec);
}
 
//__________________________________________________________________________________________________________________

const Char_t* KVBase::GetPluginURI(const Char_t* base, const Char_t* derived)
{
   //Inverse of gPluginMgr->FindHandler(const Char_t* base, const Char_t* uri).
   //Given a base class "base" and a derived class "derived", we search gEnv to find the
   //URI corresponding to this plugin.
   //
   //Example: given a plugin such as
   //
   //Plugin.KVIDTelescope:      ^PHOS$           KVIDPhoswich         KVIndra         "KVIDPhoswich()"
   //
   //then calling KVBase::GetPluginURI("KVIDTelescope", "KVIDPhoswich") will return "PHOS".
   //
   //Most of the code is copied from TPluginManager::LoadHandlersFromEnv
   
   TIter next(gEnv->GetTable());
   TEnvRec *er;
   static TString tmp;

   while ((er = (TEnvRec*) next())) {
      const char *s;
      if ((s = strstr(er->GetName(), "Plugin."))) {
         // use s, i.e. skip possible OS and application prefix to Plugin.
         // so that GetValue() takes properly care of returning the value
         // for the specified OS and/or application
         const char *val = gEnv->GetValue(s, (const char*)0);
         if (val) {
            Int_t cnt = 0;
            s += 7;
            //is it the right base class ?
            if( strcmp(s , base) ) continue;//skip to next env var if not right base
            
            char *v = StrDup(val);
            while (1) {
               TString regexp = strtok(!cnt ? v : 0, "; ");
               if (regexp.IsNull()) break;
               TString clss   = strtok(0, "; ");
               if (clss.IsNull()) break;
               TString plugin = strtok(0, "; ");
               if (plugin.IsNull()) break;
               TString ctor = strtok(0, ";\"");
               if (!ctor.Contains("("))
                  ctor = strtok(0, ";\"");
               if( clss == derived ) {
                  //found the required plugin
                  //we remove the 'regexp' operator '^' from the beginning
                  //and '$' from the end of the URI, if necessary
                  if(regexp.MaybeRegexp()){
                     regexp.Remove(TString::kBoth, '^'); regexp.Remove(TString::kBoth, '$');
                  }
                  tmp = regexp;
                  delete [] v;
                  return tmp.Data();
               } 
               cnt++;
            }
            delete [] v;
         }
      }
   }
   tmp = "";
   return tmp;
}
