//Created by KVClassFactory on Fri Nov 25 09:53:18 2011
//Author: John Frankland

#include "KVGELogReader.h"
#include "TObjString.h"
#include "Riostream.h"

ClassImp(KVGELogReader)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGELogReader</h2>
<h4>Read GE (Grid Engine) log files</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVGELogReader::KVGELogReader()
{
   // Default constructor
}

KVGELogReader::~KVGELogReader()
{
   // Destructor
}


void KVGELogReader::ReadLine(TString & line, Bool_t & ok)
{
   //analyse contents of line read from log file
   //if line contains "segmentation" then we put ok=kFALSE to stop reading file
   //status will equal the contents of the current line
	
	KVLogReader::ReadLine(line,ok);
	if(!ok) return;
	
   if (line.Contains("cpu time"))
      ReadCPULimit(line);
//   else if (line.Contains("SCRATCH:"))
//      ReadScratchUsed(line);
   else if (line.Contains(" vmem:"))
      ReadMemUsed(line);
   else if (line.Contains("Exit status:"))
      ReadStatus(line);
   else if (line.Contains("Jobname:"))
      ReadJobname(line);
   else if (line.Contains("DISK_REQ") || line.Contains("MEM_REQ"))
      ReadStorageReq(line);
}

void KVGELogReader::ReadCPULimit(TString & line)
{
   //read line of type "*   cpu time:              693 / 15000                        *"
   //which contains total normalized used CPU time and the time limit from the job request
	
   TObjArray *toks = line.Tokenize("*:/ ");
   //get used CPU time (in seconds)
   KV__TOBJSTRING_TO_INT(toks,2,sec)
   fCPUused = sec;
   //get requested CPU time (in seconds)
   KV__TOBJSTRING_TO_INT(toks,3,sec_)
   fCPUreq = sec_;
   delete toks;
}

void KVGELogReader::ReadScratchUsed(TString & line)
{
	// this just sets fScratcKB=0 because there is no information on
	// scratch disk usage in the GE logfiles
   fScratchKB=0;
}

void KVGELogReader::ReadMemUsed(TString & line)
{
   //read line of type "*   vmem:                  406.879M                           *"
   //corresponding to memory used by job
	
   TObjArray *toks = line.Tokenize("*: ");
   //value read is converted to KB, depending on units
	
	TString vmem = ((TObjString*)toks->At(1))->String();
   TString units = vmem[vmem.Length()-1];
	vmem.Remove(vmem.Length()-1);
   fMemKB = vmem.Atof()*GetByteMultiplier(units);
   delete toks;
}

Int_t KVGELogReader::GetByteMultiplier(TString & unit)
{
   //unit = "K", "M" or "G"
   //value returned is 1, 2**10 or 2**20, respectively
   static Int_t KB = 1;
   static Int_t MB = 2 << 9;
   static Int_t GB = 2 << 19;
   return (unit == "K" ? KB : (unit == "M" ? MB : GB));
}

void KVGELogReader::ReadStatus(TString & line)
{
   //read line of type "* Exit status:             0                                  *"
   //with final status of job.
   //if status = "0" then JobOK() will return kTRUE
   //otherwise, JobOK() will be kFALSE
	
	fGotStatus = kTRUE;
   TObjArray *toks = line.Tokenize("*: ");
   fStatus = ((TObjString *) toks->At(2))->GetString();
   fOK = (fStatus == "0");
   delete toks;
}

Int_t KVGELogReader::ReadStorage(KVString & stor)
{
   //'stor' is a string such as "200M", "3G" etc.
   //value returned is corresponding storage space in KB
   static const Char_t *units[] = { "K", "M", "G" };
   Int_t i = 0, index = -1;
   while ((index = stor.Index(units[i])) < 0 && i < 2)
      i++;
   if (index < 0)
      return 0;
   stor.Remove(index);
   TString u(units[i]);
   return (stor.Atoi() * GetByteMultiplier(u));
}

