//Created by KVClassFactory on Fri Jul 17 15:10:29 2009
//Author: John Frankland

#include "SRB.h"
#include "TSystem.h"
#include "TList.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "Riostream.h"

ClassImp(SRB)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>SRB</h2>
<h4>Interface to SRB commands</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

SRB::SRB()
{
   // Initializes SRB session - calls Sinit()
	Sinit();
}

SRB::~SRB()
{
   // Ends SRB session - calls Sexit()
	Sexit();
}

Bool_t SRB::buildCommand(const Char_t* scmd, const Char_t* args, Option_t* opts)
{
	// scmd = one of the SRB Scommands (Sinit, Sls, SgetD, etc.)
	// args = (optional) list of arguments for command (filename, etc.)
	// opts = (optional) list of options for command
	// If the command give in 'scmd' is found in the path specified by the
	// user's PATH environment variables, this method returns kTRUE and
	// private member variables fexec and fcmd will hold the full path to
	// the executable and the full command line to pass to gSystem->Exec.
	// If executable is not found, returns kFALSE.
	
	fexec=scmd;
	if(!FindExecutable(fexec)){
		fexec=""; fcmd="";
		return kFALSE;
	}
	fcmd = fexec + " ";
	fcmd+=opts;
	fcmd+=" ";
	fcmd+=args;
	return kTRUE;
}

Int_t SRB::execCommand()
{
	// Execute last command initialised with buildCommand() and return
	// the value given by the operating system.
	// Returns -1 if no command has been defined.
	
	if(fcmd=="") return -1;
	return gSystem->Exec(fcmd.Data());
}

TString SRB::pipeCommand()
{
	// Execute last command initialised with buildCommand() and return
	// the value given by the operating system in a string.
	
	if(fcmd=="") return TString("");
	fout="";
	FILE *pipe=gSystem->OpenPipe(fcmd.Data(),"r");
	TString line;
	while(line.Gets(pipe)){
		if(fout!="")
			fout+="\n";
		fout+=line;
	}
	/*Int_t r=*/gSystem->ClosePipe(pipe);
	return fout;
}

Int_t SRB::Sinit()
{
	buildCommand("Sinit");
	return execCommand();
}

TString SRB::Sls(const Char_t* directory, Option_t* opt)
{
	// returns listing of current directory (default) or given directory.
	// options are same as for Sls command.
	// returns empty string if directory is unknown.
	
	buildCommand("Sls",directory,opt);
	return pipeCommand();
}

Int_t SRB::Scd(const Char_t* directory)
{
	// change directory
	
	buildCommand("Scd",directory);
	return execCommand();
}

TString SRB::SgetD(const Char_t* file, Option_t* opt)
{
	// returns meta-infos on given file.
	
	buildCommand("SgetD",file,opt);
	return pipeCommand();
}

Int_t SRB::Sput(const Char_t* source, const Char_t* target, Option_t* opt)
{
	// put a new file into SRB space. options as for Sput command.
	
	TString args; args.Form("%s %s",source,target);
	buildCommand("Sput",args.Data(),opt);
	return execCommand();
}

Int_t SRB::Sget(const Char_t* source, const Char_t* target, Option_t* opt)
{
	// copy a file from SRB space. options as for Sget command.
	
	TString args; args.Form("%s %s",source,target);
	buildCommand("Sget",args.Data(),opt);
	return execCommand();
}

Int_t SRB::Srm(const Char_t* path, Option_t* opt)
{
	// Delete a file. options as for Srm command.
	// WARNING: THIS WILL DELETE THE PHYSICAL FILE,
	// not just the catalogue entry!!!
	
	buildCommand("Srm",path,opt);
	return execCommand();
}

Int_t SRB::Smkdir(const Char_t* path, Option_t* opt)
{
	// Create new directory in SRB space
	
	buildCommand("Smkdir",path,opt);
	return execCommand();
}

Int_t SRB::Sexit()
{
	buildCommand("Sexit");
	return execCommand();
}

TList *SRB::GetFullListing(const Char_t* directory)
{
	// Create and fill TList with info (name, size, modification date)
	// on all files & containers in current directory
	// (default) or in given directory.
	// TList is filled with SRBFile_t objects which belong to the list, list must be deleted after use.
	
	Sls(directory,"-l");
	if(fout==""){
		Error("GetListing", "Unknown directory %s", directory);
		return 0;
	}
	
	TObjArray *toks = fout.Tokenize("\n");
	TList* list=new TList;
	list->SetOwner(kTRUE);
	list->SetName(((TObjString*)(*toks)[0])->String().Remove(TString::kBoth,' ').Data());
	for(int i=1; i<toks->GetEntries(); i++){
		TString tmp = ((TObjString*)(*toks)[i])->String().Remove(TString::kBoth,' ');
		SRBFile_t *f = new SRBFile_t;
		if(tmp.BeginsWith("C-/")){ // container
			f->SetName(gSystem->BaseName(tmp.Data()));
			f->SetIsContainer();
		}
		else {
			// files have lines such as
   		//   nief        0 Lyon                    230663725 2008-12-19-15.21   run788.root.2006-10-30_08:03:15
			TObjArray *fstats = tmp.Tokenize(" ");
			f->SetName(((TObjString*)(*fstats)[5])->String().Remove(TString::kBoth,' ').Data());
			f->SetSize((UInt_t)((TObjString*)(*fstats)[3])->String().Remove(TString::kBoth,' ').Atoi());
			KVDatime mt(((TObjString*)(*fstats)[4])->String().Remove(TString::kBoth,' ').Data(), KVDatime::kSRB);
			f->SetModTime(mt);
			delete fstats;
		}
		list->Add(f);
	}
	delete toks;
	return list;
}

Bool_t SRB::GetPathInfo(const Char_t* path, SRBFile_t& fs)
{
	// Returns kTRUE if 'path' exists (file or directory, absolute or relative pathname).
	// If so, SRBFile_t object will be filled with information on file/container
	
	TString filename = gSystem->BaseName(path);
	TString dirname = gSystem->DirName(path);
	if(DirectoryContains(filename.Data(),dirname.Data())){
		Sls(path,"-l");
		fout.Remove(TString::kBoth,' ');
		fs.SetName(filename.Data());
		if(fout.BeginsWith("C-/")){ // container
			fs.SetIsContainer();
		}
		else {
			// files have lines such as
   		//   nief        0 Lyon                    230663725 2008-12-19-15.21   run788.root.2006-10-30_08:03:15
			TObjArray *fstats = fout.Tokenize(" ");
			fs.SetSize((UInt_t)((TObjString*)(*fstats)[3])->String().Remove(TString::kBoth,' ').Atoi());
			KVDatime mt(((TObjString*)(*fstats)[4])->String().Remove(TString::kBoth,' ').Data(), KVDatime::kSRB);
			fs.SetModTime(mt);
			delete fstats;
		}
		return kTRUE;
	}
	return kFALSE;
}
	
TList *SRB::GetListing(const Char_t* directory)
{
	// Create and fill TList with just the names of files & containers in current directory
	// (default) or in given directory.
	// TList is filled with SRBFile_t objects which belong to the list, list must be deleted after use.
	
	Sls(directory);
	if(fout==""){
		Error("GetListing", "Unknown directory %s", directory);
		return 0;
	}
	
	TObjArray *toks = fout.Tokenize("\n");
	TList* list=new TList;
	list->SetOwner(kTRUE);
	list->SetName(((TObjString*)(*toks)[0])->String().Remove(TString::kBoth,' ').Data());
	for(int i=1; i<toks->GetEntries(); i++){
		TString tmp = ((TObjString*)(*toks)[i])->String().Remove(TString::kBoth,' ');
		SRBFile_t *f = new SRBFile_t;
		if(tmp.BeginsWith("C-/")){ // container
			f->SetName(gSystem->BaseName(tmp.Data()));
			f->SetIsContainer();
		}
		else {
			f->SetName(tmp.Data());
		}
		list->Add(f);
	}
	delete toks;
	return list;
}
	
Bool_t SRB::DirectoryContains(const Char_t* name, const Char_t* directory)
{
	// Returns true if the current directory (default) or the given directory
	// contains a file or a container with given name.
	
	TList *ls = GetListing(directory);
	Bool_t ok = ls->FindObject(name);
	delete ls;
	return ok;
}

ClassImp(SRBFile_t)
		
/////////////////////////////////////////////
// SRBFile_t
//
// Describes SRB file/container attributes
/////////////////////////////////////////////
		
void SRBFile_t::ls(Option_t */*opt*/) const
{
   // List SRB file/container attributes

   TROOT::IndentLevel();
	if(IsContainer())
		cout << GetName() << "/" << endl;
   else
		cout << GetName() << "\t" << GetSize() << "\t" << GetModTime().AsString() << endl;
}
