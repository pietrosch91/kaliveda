//Created by KVClassFactory on Wed Apr 27 15:43:08 CEST 2011
//Author: John Frankland

#include "KV_CCIN2P3_GE.h"
#include "TSystem.h"
#include "TEnv.h"
#include "KVDataAnalyser.h"
#include "KVDataAnalysisTask.h"
#include "KVGEBatchJob.h"

using namespace std;

ClassImp(KV_CCIN2P3_GE)
////////////////////////////////////////////////////////////////////////////////
/*
begin_html
<H1>Interface to CCIN2P3 Grid Engine batch job management system</H1>
<p>
<a href="http://cc.in2p3.fr/docenligne/969">Submit a job</a><br>
<a href="http://cc.in2p3.fr/docenligne/970">List of commands</a><br>
<a href="http://cc.in2p3.fr/docenligne/972">Translation of BSQ to GE commands</a>
</p>
end_html
*/
////////////////////////////////////////////////////////////////////////////////
KV_CCIN2P3_GE::KV_CCIN2P3_GE(const Char_t* name)
   :KVBatchSystem(name)
{
   //Default constructor
   //Sets default job time, memory and disk space as defined in $KVROOT/KVFiles/.kvrootrc
   
   fDefJobTime = gEnv->GetValue("GE.BatchSystem.DefaultJobTime", 4000);
   fDefJobMem = gEnv->GetValue("GE.BatchSystem.DefaultJobMemory", "256M");
   fDefJobDisk = gEnv->GetValue("GE.BatchSystem.DefaultJobDisk", "100M");
   fTimeSet=fDiskSet=fMemSet=kFALSE;
}

//_______________________________________________________________________________//

void KV_CCIN2P3_GE::Clear(Option_t*opt)
{
   //Clear previously set parameters in order to create a new job submission command
   KVBatchSystem::Clear(opt);
   fTimeSet=fDiskSet=fMemSet=kFALSE;
}

//_______________________________________________________________________________//

KV_CCIN2P3_GE::~KV_CCIN2P3_GE()
{
   //Destructor
}

void KV_CCIN2P3_GE::SetJobTime(Int_t ss, Int_t mm, Int_t hh)
{
   //Set CPU time for batch job.
   //      SetJobTime(0) => use default time
   //      SetJobTime(100) => 100 seconds
   //      SetJobTime(30,2)        => 2 minutes 30 seconds
   //      SetJobTime(0,30,5)      => 5 hours 30 minutes (0 seconds)
   
   KVString tmp="";
   if (hh)
      tmp.Form("%d:%02d:%02d", hh, mm, ss);
   else if (mm)
      tmp.Form("%d:%02d", mm, ss);
   else if (ss)
      tmp.Form("%d", ss);
   SetJobTimeString(tmp.Data());
}

void KV_CCIN2P3_GE::SetJobTimeString(const Char_t* time)
{
   //Set CPU time for batch job.
   //      SetJobTime() => use default time
   KVString tmp(time);
   if(tmp=="") tmp = Form("%d", fDefJobTime);
   fParList.SetParameter("-l ct=", tmp);
   fTimeSet = kTRUE;
}

void KV_CCIN2P3_GE::SetJobMemory(const Char_t * mem)
{
   //Set maximum memory used by job.
   //Include units in string, i.e. "100M", "1G" etc.
   //If mem="", use default value
   KVString tmp(mem);
   if(tmp=="") tmp = fDefJobMem;
   fParList.SetParameter("-l vmem=", tmp);
   fMemSet = kTRUE;
}

void KV_CCIN2P3_GE::SetJobDisk(const Char_t * diks)
{
   //Set maximum disk space used by job.
   //Include units in string, i.e. "100M", "1G" etc.
   //If diks="", use default value
   KVString tmp(diks);
   if(tmp=="") tmp = fDefJobDisk;
   fParList.SetParameter("-l fsize=", tmp);
   fDiskSet = kTRUE;
}

void KV_CCIN2P3_GE::PrintJobs(Option_t * opt)
{
   //Print list of owner's jobs.
   KVList* j = GetListOfJobs();
   j->ls();
   delete j;
}

Bool_t KV_CCIN2P3_GE::CheckJobParameters()
{
   // Checks the job and asks for any missing parameters
   
   if(!KVBatchSystem::CheckJobParameters()) return kFALSE;

   if(!fTimeSet) ChooseJobTime();
     
   if(!fDiskSet) ChooseJobDisk();
         
   if(!fMemSet) ChooseJobMemory();
         
   return kTRUE;
}    

void KV_CCIN2P3_GE::ChooseJobTime()
{
    KVString tmp="";
    cout << "Enter max CPU time per job (ss/mn:ss/hh:mn:ss) [" 
     << fDefJobTime << "] : ";cout.flush();
    tmp.ReadToDelim(cin);
    if(!tmp.Length()){
       SetJobTimeString();
       return;
    }
    Int_t sec=tmp.Atoi();
    Int_t i=0;
    while((i=tmp.Index(":")) > 0)
     {
     sec*=60;
     tmp.Remove(0,i+1);
     sec+=tmp.Atoi();
     }
    SetJobTime(sec);
}

void KV_CCIN2P3_GE::ChooseJobMemory()
{
    KVString tmp="";
    cout << "Enter max memory per job (xKB/xMB/xGB) ["
     << fDefJobMem.Data() << "] : ";cout.flush();
    tmp.ReadToDelim(cin);
    SetJobMemory(tmp.Data());
}

void KV_CCIN2P3_GE::ChooseJobDisk()
{
    KVString tmp="";
    cout << "Enter max scratch disk per job (xKB/xMB/xGB) ["
     << fDefJobDisk.Data() << "] : ";cout.flush();
    tmp.ReadToDelim(cin);
    SetJobDisk(tmp.Data());
}

const Char_t* KV_CCIN2P3_GE::GetJobTime(void)
{
// returns the parameter string corresponding to the job CPU time
 return fParList.GetParameter("-l ct=").Data();
}  

const Char_t* KV_CCIN2P3_GE::GetJobMemory(void)
{
// returns the parameter string corresponding to the job Memory
 return fParList.GetParameter("-l vmem=").Data();
}

const Char_t* KV_CCIN2P3_GE::GetJobDisk(void) 
{
// returns the parameter string corresponding to the job Disk
 return fParList.GetParameter("-l fsize=").Data();
}

//_______________________________________________________________________________//

void KV_CCIN2P3_GE::WriteBatchEnvFile(TEnv* env)
{
   //Store any useful information on batch system in the TEnv
   //(this method is used by KVDataAnalyser::WriteBatchEnvFile)
   KVBatchSystem::WriteBatchEnvFile(env);
   env->SetValue("BatchSystem.Time", GetJobTime());
   env->SetValue("BatchSystem.Memory", GetJobMemory());
   env->SetValue("BatchSystem.Disk", GetJobDisk());
}

//_______________________________________________________________________________//

void KV_CCIN2P3_GE::ReadBatchEnvFile(TEnv* env)
{
   //Read any useful information on batch system from the TEnv
   //(this method is used by KVDataAnalyser::ReadBatchEnvFile)
   KVBatchSystem::ReadBatchEnvFile(env);
   SetJobTimeString(env->GetValue("BatchSystem.Time", ""));
   SetJobMemory(env->GetValue("BatchSystem.Memory", ""));
   SetJobDisk(env->GetValue("BatchSystem.Disk", ""));
}

//_______________________________________________________________________________//

void KV_CCIN2P3_GE::Print(Option_t* option) const
{
   //if option="log", print infos for batch log file
   //if option="all", print detailed info on batch system
   if(!strcmp(option, "log")){
      KVBatchSystem::Print(option);
      cout << "* DISK_REQ:                 " << const_cast<KV_CCIN2P3_GE*>(this)->GetJobDisk() << "                            *" << endl;
      cout << "* MEM_REQ:         " << const_cast<KV_CCIN2P3_GE*>(this)->GetJobMemory() << "                             *" << endl;
   }
   else
      KVBatchSystem::Print(option);
}

//_______________________________________________________________________________//

void KV_CCIN2P3_GE::ChangeDefJobOpt(KVDataAnalyser* da)
{
	// PRIVATE method called by SubmitTask() at moment of job submission.
	// Depending on the current environment, the default job submission options
	// may be changed by this method.
   // 
	// This method overrides and augments KVBatchSystem::ChangeDefJobOpt (which
	// changes the options as a function of the type of analysis task).
	// Here we add the CCIN2P3-specific case where the job is launched from a directory
	// on the /sps/ semi-permanent storage facility. In this case we need to add
	// the option '-l u_sps_indra' to the 'qsub' command (if not already in the
	// default job options)
	//
	// Due to many people being caught out by this mechanism when submitting
	// raw->recon, raw->ident, etc. jobs from an SPS directory (and thus being penalised
	// unfairly by the limited number of SPS-ressource-declaring jobs), we only declare
	// u_sps_indra if the analysis task is not "Reconstruction", "ReconIdent",
	// "IdentRoot". We also add some warning messages.

	KVBatchSystem::ChangeDefJobOpt(da);
	KVString taskname = da->GetAnalysisTask()->GetName();
	Bool_t recId = (taskname=="Reconstruction"||taskname=="ReconIdent"||taskname=="IdentRoot");
	KVString wrkdir( gSystem->WorkingDirectory() );
	KVString oldoptions( GetDefaultJobOptions() );
	if( wrkdir.Contains("/sps/") && !oldoptions.Contains("sps") ){
		if( recId ){
			// submitting recon/ident job from /sps directory. do not add sps ressource
			Warning("ChangeDefJobOpt",
					"Your job is being submitted from %s.\nHowever, for reconstruction/identification tasks, we do not declare the 'u_sps_indra' ressource, so that the number of jobs which can be treated concurrently will not be limited.",
					wrkdir.Data());
		}
		else
		{
			oldoptions += " -l sps=1";
			SetDefaultJobOptions( oldoptions.Data() );
			Warning("ChangeDefJobOpt",
				"Your job is being submitted from %s.\nTherefore the ressource 'sps' has been declared and the number of jobs which can be treated concurrently will be limited.",
				wrkdir.Data());
		}
	}	
}

         
void KV_CCIN2P3_GE::SanitizeJobName()
{
   // Batch-system dependent sanitization of jobnames
   // Grid Engine does not allow:
   //   :
   // Any such character appearing in the current jobname will be replaced
   // with '_'
   
    fCurrJobName.ReplaceAll(":","_");
}

KVList *KV_CCIN2P3_GE::GetListOfJobs()
{
    // Create and fill list with KVBatchJob objects describing current jobs
    // Delete list after use

    KVList* list_of_jobs = new KVList;

    // use qstat -r to get list of job ids and jobnames
    TString reply = gSystem->GetFromPipe("qstat -r");

    TObjArray* lines = reply.Tokenize("\n");
    Int_t nlines = lines->GetEntries();
    for(Int_t line_number=0; line_number<nlines; line_number++){
        TString thisLine = ((TObjString*)(*lines)[line_number])->String();
        if(thisLine.Contains("Full jobname:")){
            // previous line contains job-id and status
            TString lastLine = ((TObjString*)(*lines)[line_number-1])->String();
            TObjArray* bits = lastLine.Tokenize(" ");
            Int_t jobid = ((TObjString*)(*bits)[0])->String().Atoi();
            TString status = ((TObjString*)(*bits)[4])->String();
            // date & time jobs started (running job) or submitted (queued job)
            TString sdate = ((TObjString*)(*bits)[5])->String();// mm/dd/yyyy
            TString stime = ((TObjString*)(*bits)[6])->String();// hh:mm:ss
            Int_t dd,MM,yyyy,hh,mm,ss;
            sscanf(sdate.Data(), "%d/%d/%d", &MM, &dd, &yyyy);
            sscanf(stime.Data(), "%d:%d:%d", &hh, &mm, &ss);
            KVDatime submitted(yyyy,MM,dd,hh,mm,ss);
            delete bits;
            bits = thisLine.Tokenize(": ");
            TString jobname =  ((TObjString*)(*bits)[2])->String();
            delete bits;

            KVGEBatchJob* job = new KVGEBatchJob();
            job->SetName(jobname);
            job->SetJobID(jobid);
            job->SetStatus(status);
            job->SetSubmitted(submitted);
            list_of_jobs->Add(job);
        }
    }
    delete lines;

    if(!list_of_jobs->GetEntries()) return list_of_jobs;

    // for each running job, use qstat -j [jobid] to get cpu and memory used
    // and also the resource requests
    TIter next_job(list_of_jobs);
    KVGEBatchJob* job;
    while( (job = (KVGEBatchJob*)next_job()) ){
        if(!strcmp(job->GetStatus(),"r")){
            reply = gSystem->GetFromPipe(Form("qstat -j %d", job->GetJobID()));
            lines = reply.Tokenize("\n");
            nlines = lines->GetEntries();
            for(Int_t line_number=0; line_number<nlines; line_number++){
                TString thisLine = ((TObjString*)(*lines)[line_number])->String();
                if(thisLine.BeginsWith("usage")){
                    TObjArray* bits = thisLine.Tokenize("=,");
                    TString stime = ((TObjString*)(*bits)[1])->String();// hh:mm:ss
                    Int_t hh,mm,ss;
                    sscanf(stime.Data(), "%2d:%2d:%2d", &hh,&mm,&ss);
                    job->SetCPUusage(hh*3600+mm*60+ss);
                    TString smem = ((TObjString*)(*bits)[7])->String();// xxx.xxxxM
                    job->SetMemUsed(smem);
                    delete bits;
                }
                else if(thisLine.BeginsWith("hard resource_list:")){
                    TObjArray* bits = thisLine.Tokenize(": ");
                    TString res = ((TObjString*)(*bits)[2])->String();//os=sl5,xrootd=1,irods=1,s_vmem=1024M,s_fsize=50M,s_cpu=36000
                    res.ReplaceAll("s_vmem","vmem");
                    res.ReplaceAll("s_fsize","fsize");
                    res.ReplaceAll("s_cpu","ct");
                    job->SetResources(res);
                    TObjArray* bbits = res.Tokenize(",");
                    TIter next_res(bbits);
                    TObjString* ss;
                    while( (ss = (TObjString*)next_res()) ){
                       TString g = ss->String();
                       if(g.BeginsWith("ct=")){
                          g.Remove(0,3);
                           job->SetCPUmax(g.Atoi());
                        }
                        else if(g.BeginsWith("vmem=")){
                           g.Remove(0,5);
                           job->SetMemMax(g);
                        }
                        else if(g.BeginsWith("fsize=")){
                           g.Remove(0,6);
                           job->SetDiskMax(g);
                        }
                     }
                     delete bits;
                     delete bbits;
                }
            }
            delete lines;
        }
    }

    return list_of_jobs;
}
