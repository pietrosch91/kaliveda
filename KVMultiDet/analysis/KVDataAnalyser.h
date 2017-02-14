/*
$Id: KVDataAnalyser.h,v 1.18 2009/04/01 09:28:34 franklan Exp $
$Revision: 1.18 $
$Date: 2009/04/01 09:28:34 $
$Author: franklan $
*/

#ifndef __KVDATAANALYSER_H
#define __KVDATAANALYSER_H

#include "TObject.h"
#include "TTree.h"
#include "KVList.h"
#include "KVBatchSystem.h"
#include "KVString.h"
#include "KVNumberList.h"
#include "TEnv.h"

class KVDataSet;
class KVDataAnalysisTask;
class KVDBSystem;

class KVDataAnalyser : public TObject {

private:
   KVDataAnalysisTask* fTask;   //task chosen by user
   KVString fDataType;          //datatype chosen by user
   Bool_t fBatch;               //set to kTRUE when used in a non-interactive environment
   KVString fBatchName;         //name of batch job when used in a non-interactive environment
   KVDBSystem* fSystem;         //system chosen by user
   KVNumberList fRunList;       //list of runs to analyse
   KVNumberList fFullRunList;   //list of all runs for the analysis task
   KVDataSet* fDataSet;         //dataset chosen by user
   Long64_t nbEventToRead;

   Bool_t fMenus; //set to kTRUE in menu-driven mode
   Bool_t fQuit;                //set to kTRUE when user wants to quit
   Bool_t fChoozDataSet;        //set to kTRUE when user wants to choose a dataset
   Bool_t fChoozTask;           //set to kTRUE when user wants to choose analysis task
   Bool_t fChoozSystem;         //set to kTRUE when user wants to choose a system
   Bool_t fChoozRuns;           //set to kTRUE when user wants to choose runs
   Bool_t fSubmit;              //set to kTRUE when user wants to submit task

   KVString fIncludes;           //user's include directories
   KVString fLibraries;          //user's libraries
   KVString fUserClass;    //user's analysis class
   KVString fUserClassImp;    //user's analysis class implementation file
   KVString fUserClassDec;    //user's analysis class header file
   Bool_t fUserClassIsOK;  //set once user class has been validated
   KVString fUserClassOptions;   //options to be passed to user analysis class
   KVString fUserClassAlternativeBaseClass;    //alternative base class for user's analysis class

   KVDataAnalyser* fParent;   //holds address of parent analyser which created this one

   TEnv* fBatchEnv; // batch environment file
   KVBatchSystem* fBatchSystem;//batch system to use for submission of job
   Bool_t fChoseRunMode; //set to kTRUE once running mode has been set/chosen

   TList* fWorkDirInit;//list of files in working directory before task runs
   TList* fWorkDirEnd;//list of files in working directory after task runs
   static Bool_t fCleanAbort;//flag to force abort of processing

protected:
   void ScanWorkingDirectory(TList**);
   void CopyAnalysisResultsToLaunchDirectory();

   virtual KVNumberList PrintAvailableRuns(KVString& datatype);
   virtual Bool_t CheckIfUserClassIsValid(const KVString& alternative_base_class = "");
   TObject* GetInstanceOfUserClass(const KVString& alternative_base_class = "");

   const Char_t* GetACliCMode();

   virtual void set_dataset_pointer(KVDataSet* ds);
   virtual void set_dataset_name(const Char_t* name);
   void _set_dataset_pointer(KVDataSet* ds)
   {
      fDataSet = ds;
   }

   TEnv* GetBatchInfoFile() const
   {
      return fBatchEnv;
   }

public:
   enum EProofMode {
      None,
      Lite,
      Proof
   };
private:
   EProofMode fProofMode;
public:
   void SetProofMode(EProofMode e)
   {
      fProofMode = e;
   }
   EProofMode GetProofMode() const
   {
      return fProofMode;
   }

   static void SetAbortProcessingLoop(Bool_t now = kTRUE)
   {
      // Set flag to force a clean abort of the processing loop
      fCleanAbort = now;
   }
   static Bool_t AbortProcessingLoop()
   {
      return fCleanAbort;
   }

   KVDataAnalyser();
   virtual ~ KVDataAnalyser();

   void SetUserClass(const Char_t* kvs, Bool_t check = kTRUE);
   const Char_t* GetUserClass()
   {
      return fUserClass.Data();
   }
   Bool_t IsUserClassValid() const
   {
      return fUserClassIsOK;
   }

   virtual void Run();
   void RunMenus();
   void ChooseDataSet();
   void ChooseDataType();
   virtual void ChooseAnalysisTask();
   void ChooseSystem(const Char_t* data_type = "");
   void ChooseRuns(KVDBSystem* system = nullptr, const Char_t* data_type = "");
   void ChooseUserClass();
   void ChooseRunningMode();
   virtual void SubmitTask();
   virtual Bool_t CheckTaskVariables();
   Bool_t DoUserClassFilesExist();

   void SetBatchMode(Bool_t on = kTRUE)
   {
      fBatch = on;
   }
   Bool_t BatchMode() const
   {
      return fBatch;
   }
   void SetBatchName(const Char_t* batchname)
   {
      fBatchName = batchname;
   }
   const Char_t* GetBatchName()
   {
      return fBatchName.Data();
   }
   void SetBatchSystem(KVBatchSystem* bs)
   {
      fBatchSystem = bs;
      fChoseRunMode = kTRUE;
      if (bs) fBatchSystem->cd();
   }
   virtual const KVBatchSystem* GetBatchSystem()
   {
      return fBatchSystem;
   }
   Bool_t RunningInLaunchDirectory();
   const Char_t* SystemBatchName();

   const Char_t* GetLaunchDirectory() const;
   const Char_t* GetBatchStatusFileName() const;
   void UpdateBatchStatusFile(Int_t totev, Int_t evread, TString disk) const;
   void DeleteBatchStatusFile() const;
   virtual Long64_t GetTotalEntriesToRead() const
   {
      return 0;
   }
   Bool_t CheckStatusUpdateInterval(Int_t nevents) const;
   void DoStatusUpdate(Int_t nevents) const;

   KVDataAnalysisTask* GetAnalysisTask()
   {
      return fTask;
   }
   KVString& GetDataType()
   {
      return fDataType;
   }
   KVDBSystem* GetSystem()
   {
      return fSystem;
   }
   const KVNumberList& GetRunList()
   {
      return fRunList;
   }
   KVNumberList& GetFullRunList()
   {
      return fFullRunList;
   }
   KVDataSet* GetDataSet()
   {
      return fDataSet;
   }
   KVString& GetUserIncludes()
   {
      return fIncludes;
   }
   KVString& GetUserLibraries()
   {
      return fLibraries;
   }

   void SetNbEventToRead(Long64_t nb = 0)
   {
      nbEventToRead = nb;
   }

   Long64_t GetNbEventToRead(void)
   {
      return nbEventToRead;
   }

   void ChooseNbEventToRead();
   void SetDataSet(KVDataSet* ds)
   {
      set_dataset_pointer(ds);
   }
   void SetDataSet(const Char_t* name)
   {
      set_dataset_name(name);
   }
   void SetAnalysisTask(KVDataAnalysisTask* at);
   void SetDataType(const Char_t* name)
   {
      fDataType = name;
   }
   void SetSystem(KVDBSystem* syst);
   void SetRuns(const KVNumberList& nl, Bool_t check = kTRUE);
   void SetFullRunList(KVNumberList& nl);

   void SetUserIncludes(const Char_t* incDirs = 0);
   void SetUserLibraries(const Char_t* libs = 0);

   void ClearRunList()
   {
      fRunList.Clear();
   }
   virtual void Reset();

   void SetParent(KVDataAnalyser* da)
   {
      fParent = da;
   }
   KVDataAnalyser* GetParent() const
   {
      return fParent;
   }

   const KVString& GetUserClassImp() const
   {
      return fUserClassImp;
   }
   const KVString& GetUserClassDec() const
   {
      return fUserClassDec;
   }

   static KVDataAnalyser* GetAnalyser(const Char_t* plugin);
   virtual Bool_t ReadBatchEnvFile(const Char_t*);
   virtual void WriteBatchEnvFile(const Char_t*, Bool_t sav = kTRUE);

   virtual const Char_t* ExpandAutoBatchName(const Char_t* format);

   /* methods which can be used to customise analysis class behaviour.
      they can be called from the analysis class using the global
      pointer to the current data analyser, gDataAnalyser  */
   virtual void preInitAnalysis() {}
   virtual void postInitAnalysis() {}
   virtual void preInitRun() {}
   virtual void postInitRun() {}
   virtual void preAnalysis() {}
   virtual void postAnalysis() {}
   virtual void preEndRun() {}
   virtual void postEndRun() {}
   virtual void preEndAnalysis() {}
   virtual void postEndAnalysis() {}

   void WriteBatchInfo(TTree*);
   Int_t GetRunNumberFromFileName(const Char_t*);

   virtual void RegisterUserClass(TObject*) {}

   void SetUserClassOptions(const Char_t* o = "")
   {
      fUserClassOptions = o;
   }
   const KVString& GetUserClassOptions() const
   {
      return fUserClassOptions;
   }

   static void RunAnalyser(const Char_t* plugin = "");

   static Bool_t IsRunningBatchAnalysis();

   void AddJobDescriptionList(TList*);

   void SetMenus(Bool_t on = kTRUE)
   {
      fMenus = on;
   }
   Bool_t IsMenus() const
   {
      return fMenus;
   }
   void SetQuit(Bool_t yes = kTRUE)
   {
      fQuit = yes;
   }
   Bool_t IsQuit() const
   {
      return fQuit;
   }
   void SetChooseDataSet(Bool_t yes = kTRUE)
   {
      fChoozDataSet = yes;
   }
   Bool_t IsChooseDataSet() const
   {
      return fChoozDataSet;
   }
   void SetChooseTask(Bool_t yes = kTRUE)
   {
      fChoozTask = yes;
   }
   Bool_t IsChooseTask() const
   {
      return fChoozTask;
   }
   void SetChooseSystem(Bool_t yes = kTRUE)
   {
      fChoozSystem = yes;
   }
   Bool_t IsChooseSystem() const
   {
      return fChoozSystem;
   }
   void SetChooseRuns(Bool_t yes = kTRUE)
   {
      fChoozRuns = yes;
   }
   Bool_t IsChooseRuns() const
   {
      return fChoozRuns;
   }
   void SetSubmit(Bool_t yes = kTRUE)
   {
      fSubmit = yes;
   }
   Bool_t IsSubmit() const
   {
      return fSubmit;
   }

   ClassDef(KVDataAnalyser, 0)  //For submitting & performing data analysis tasks
};

//................  global variable
R__EXTERN KVDataAnalyser* gDataAnalyser;

#endif
