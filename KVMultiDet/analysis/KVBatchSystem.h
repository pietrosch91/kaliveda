/*
$Id: KVBatchSystem.h,v 1.11 2008/04/03 07:35:45 franklan Exp $
$Revision: 1.11 $
$Date: 2008/04/03 07:35:45 $
*/

//Created by KVClassFactory on Thu Apr 13 13:07:59 2006
//Author: John Frankland

#ifndef __KVBATCHSYSTEM_H
#define __KVBATCHSYSTEM_H

#include "KVParameterList.h"
#include "KVString.h"
#include "KVBase.h"

class KVDataAnalyser;
class TEnv;

class KVBatchSystem : public KVBase {

 protected:

   KVDataAnalyser* fAnalyser;//the analyser object which requests job submission, it has all details on the job
   KVParameterList < KVString > fParList;        //list of parameters/switches to be passed on job submission command line
   KVString fJobName;            //base job name
   KVString fJobSubCmd;          //shell command for submitting job
   KVString fJobScript;          //full path of shell script to be executed by batch system
   KVString fDefOpt;             //default options for job submission command
   Bool_t fMultiJobs;  //set to kTRUE if a job is to be created for each run in the runlist set in fAnalyser
   Int_t fRunNumber; //run number of current job
   KVString fCurrJobName; //name of current job being submitted

	virtual void ChangeDefJobOpt(KVDataAnalyser* da);
	
 public:

    KVBatchSystem(const Char_t* name);
    virtual ~ KVBatchSystem();

    KVParameterList < KVString > &GetParameters() {
      return fParList;
   };

   virtual const Char_t *GetJobSubCmd() const {
      return fJobSubCmd.Data();
   };
   virtual void SubmitTask(KVDataAnalyser* da);
   virtual void SubmitJob();  
   virtual void PrintJobs(Option_t* /* opt */ = ""){;};   
   virtual Bool_t CheckJobParameters();      

   virtual void Run();
   
   virtual void SetJobScript(const Char_t * path) {
      fJobScript = path;
   };

   virtual void SetDefaultJobOptions(const Char_t * opt) {
      fDefOpt = opt;
   };
   virtual const Char_t *GetDefaultJobOptions() const {
      return fDefOpt.Data();
   };

   virtual void SetJobName(const Char_t *name); 
   virtual const Char_t *GetJobName();

   virtual const Char_t *GetJobSubCmdLine();

   virtual void Clear(Option_t* opt="");

   static KVBatchSystem *GetBatchSystem(const Char_t* plugin);
   
   void cd();

   virtual void SetMultiJobsMode(Bool_t on=kTRUE) {fMultiJobs=on;};
   virtual Bool_t MultiJobsMode() const { return fMultiJobs; };
   
   virtual void WriteBatchEnvFile(TEnv*);
   virtual void ReadBatchEnvFile(TEnv*);
   virtual void Print(Option_t* /*option*/ = "") const;
   
   virtual void SetAnalyser(KVDataAnalyser* da){ fAnalyser = da; };
   
   ClassDef(KVBatchSystem, 1)   //Base class for interface to batch job management system
};

//................  global variable
R__EXTERN KVBatchSystem *gBatchSystem;

#endif
