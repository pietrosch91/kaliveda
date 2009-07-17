//Created by KVClassFactory on Fri Jul 17 15:10:29 2009
//Author: John Frankland

#include "SRB.h"
#include "TSystem.h"

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
	Int_t r=gSystem->ClosePipe(pipe);
	return fout;
}

Int_t SRB::Sinit()
{
	buildCommand("Sinit");
	return execCommand();
}

TString SRB::Sls(const Char_t* directory)
{
	// returns listing of current directory (default) or given directory
	// returns empty string if directory is unknown.
	
	buildCommand("Sls",directory);
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

Int_t SRB::Sexit()
{
	buildCommand("Sexit");
	return execCommand();
}
