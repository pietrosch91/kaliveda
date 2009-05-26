/*******************************************************************************
$Id: KVLogReader.cpp,v 1.13 2008/12/08 14:07:37 franklan Exp $
*******************************************************************************/
#include "KVLogReader.h"
#include "Riostream.h"
#include "TObjArray.h"
#include "TObjString.h"

ClassImp(KVLogReader)
//////////////////////////////
//For reading BQS log files
//
    KVLogReader::KVLogReader()
{
   fFMT = "%d";
   Reset();
}

void KVLogReader::Reset()
{
   //reset informations read from file
   fCPUused = fCPUreq = fSCRreq = fMEMreq = fScratchKB = fMemKB = 0;
   fStatus = "";
   fOK = kFALSE;
   fGotRequests = kFALSE;
}

void KVLogReader::ReadFile(const Char_t * fname)
{
   //Open file 'fname' and read contents

   Reset();                     //clear last read infos

   ifstream filestream(fname);
   if (!filestream.good()) {
      cout << "Error in KVLogReader::ReadFile - cannot open file " << fname
          << endl;
      return;
   }
   TString line;
   line.ReadLine(filestream);
   Bool_t ok = kTRUE;
   while (filestream.good() && ok) {
      ReadLine(line, ok);       //perform action based on content of line
      line.ReadLine(filestream);
   }
   filestream.close();
}

void KVLogReader::ReadLine(TString & line, Bool_t & ok)
{
   //analyse contents of line read from log file
   //if line contains "segmentation" then we put ok=kFALSE to stop reading file
   //status will equal the contents of the current line
   ok = kTRUE;
   if (line.Contains("egmentation")) {
      ok = kFALSE;
      fStatus = line;
      fOK = kFALSE;
      return;
   } else if (line.Contains("Temps limite atteint")) {
      //VEDA FORTRAN programme 'out of time' error seen during DST conversion
      ok = kFALSE;
      fStatus = "VEDA Fortran out of time";
      fOK = kFALSE;
      return;
   } else if (line.Contains("rfcp : Input/output error")) {
      //failure to copy new run to hpss system at ccali
      ok = kFALSE;
      fStatus = "rfcp error";
      fOK = kFALSE;
      return;
   }
   if (line.Contains("time limit"))
      ReadCPULimit(line);
   else if (line.Contains("SCRATCH:"))
      ReadScratchUsed(line);
   else if (line.Contains("VIRTUAL STORAGE:"))
      ReadMemUsed(line);
   else if (line.Contains("with status:"))
      ReadStatus(line);
   else if (line.Contains("Jobname:"))
      ReadJobname(line);
   else if (line.Contains("DISK_REQ") || line.Contains("MEM_REQ"))
      ReadStorageReq(line);
}

void KVLogReader::ReadCPULimit(TString & line)
{
   //read line of type "*     total normalized:    68:57:52 (time limit: 100:00:00)   *"
   //which contains total normalized used CPU time and the time limit from the job request
   TObjArray *toks = line.Tokenize("(:) ");
   
   //get used CPU time (in seconds)
   KV__TOBJSTRING_TO_INT(toks,3,hrs)
   KV__TOBJSTRING_TO_INT(toks,4,mins)
   KV__TOBJSTRING_TO_INT(toks,5,sec)
   fCPUused = 3600 * hrs + 60 * mins + sec;
   //get requested CPU time (in seconds)
   KV__TOBJSTRING_TO_INT(toks,8,hrs_)
   KV__TOBJSTRING_TO_INT(toks,9,mins_)
   KV__TOBJSTRING_TO_INT(toks,10,sec_)
   fCPUreq = 3600 * hrs_ + 60 * mins_ + sec_;
   delete toks;
}

Double_t KVLogReader::GetCPUratio() const
{
   //calculate ratio of used CPU to requested CPU
   return (fCPUreq ? fCPUused / fCPUreq : 0.);
}

void KVLogReader::ReadScratchUsed(TString & line)
{
   //read line of type "* SCRATCH:                 1042 MB                            *"
   //corresponding to disk space used by job
   TObjArray *toks = line.Tokenize("*: ");
   //value read is converted to KB, depending on units
   KV__TOBJSTRING_TO_INT(toks,1,ScratchKB_)
   fScratchKB = ScratchKB_;
   TString units = ((TObjString *) toks->At(2))->GetString();
   fScratchKB *= GetByteMultiplier(units);
   delete toks;
}

void KVLogReader::ReadMemUsed(TString & line)
{
   //read line of type "* VIRTUAL STORAGE:         103 MB                             *"
   //corresponding to memory used by job
   TObjArray *toks = line.Tokenize("*: ");
   //value read is converted to KB, depending on units
   KV__TOBJSTRING_TO_INT(toks,2,MemKB_)
   fMemKB = MemKB_;
   TString units = ((TObjString *) toks->At(3))->GetString();
   fMemKB *= GetByteMultiplier(units);
   delete toks;
}

Int_t KVLogReader::GetByteMultiplier(TString & unit)
{
   //unit = "KB", "MB" or "GB"
   //value returned is 1, 2**10 or 2**20, respectively
   static Int_t KB = 1;
   static Int_t MB = 2 << 9;
   static Int_t GB = 2 << 19;
   return (unit == "KB" ? KB : (unit == "MB" ? MB : GB));
}

void KVLogReader::ReadStatus(TString & line)
{
   //read line of type "* with status:             ENDED                              *"
   //with final status of job.
   //if status = "ENDED" then JobOK() will return kTRUE
   //otherwise, JobOK() will be kFALSE
   TObjArray *toks = line.Tokenize("*: ");
   fStatus = ((TObjString *) toks->At(2))->GetString();
   fOK = (fStatus == "ENDED");
   delete toks;
}

void KVLogReader::ReadStorageReq(TString & line)
{
   //Read lines "DISK_REQ:                 3GB" and "MEM_REQ:         150MB"
   //and fill corresponding variable with appropriate value
   TObjArray *toks = line.Tokenize("*: ");
   TString type = ((TObjString *) toks->At(0))->GetString();
   KVString stor = ((TObjString *) toks->At(1))->GetString();
   Int_t size = ReadStorage(stor);
   if (type == "DISK_REQ")
      fSCRreq = size;
   else if (type == "MEM_REQ")
      fMEMreq = size;
   // both requests must be present in the logfile otherwise
   // something very wrong has happened
   if( fSCRreq && fMEMreq ) fGotRequests = kTRUE;
   delete toks;
}

void KVLogReader::ReadJobname(TString & line)
{
   //read line of type "* Jobname:                 run4049                            *"
   //with name of job.
   TObjArray *toks = line.Tokenize("*: ");
   fJobname = ((TObjString *) toks->At(1))->GetString();
   delete toks;
}

Int_t KVLogReader::GetRunNumber() const
{
   //try to get run number from jobname using format string fFMT
   Int_t run;
   sscanf(fJobname.Data(), fFMT.Data(), &run);
   return run;
}

Int_t KVLogReader::ReadStorage(KVString & stor)
{
   //'stor' is a string such as "200MB", "3GB" etc.
   //value returned is corresponding storage space in KB
   static const Char_t *units[] = { "KB", "MB", "GB" };
   Int_t i = 0, index = -1;
   while ((index = stor.Index(units[i])) < 0 && i < 2)
      i++;
   if (index < 0)
      return 0;
   stor.Remove(index);
   TString u(units[i]);
   return (stor.Atoi() * GetByteMultiplier(u));
}

Bool_t KVLogReader::Incomplete() const {
      return (
	(!Killed() && !SegFault())
	&&
	(
		(!fGotRequests)
		|| (fStatus == "VEDA Fortran out of time")
		|| (fStatus.BeginsWith("rfcp"))
	)
      );
};
