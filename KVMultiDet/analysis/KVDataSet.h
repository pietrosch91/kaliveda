/*
$Id: KVDataSet.h,v 1.26 2009/01/27 08:06:56 franklan Exp $
$Revision: 1.26 $
$Date: 2009/01/27 08:06:56 $
$Author: franklan $
*/

#ifndef __KVDATASET_H
#define __KVDATASET_H

#include "KVString.h"
#include "KVBase.h"
#include "KVList.h"
#include "KVNumberList.h"
#include "TDatime.h"
#include "TFile.h"
#include "TEnv.h"
#include "TSystem.h"

class KVMultiDetArray;
class KVDataBase;
class KVDataSetManager;
class KVDataRepository;
class KVDBSystem;
class KVDataAnalysisTask;
class TObjArray;
class KVUpDater;
class KVDBRun;
class KVAvailableRunsFile;

class KVDataSet:public KVBase {

   friend class KVAvailableRunsFile;
   friend class KVDataSetManager;

 protected:
    KVDataRepository * fRepository;     //repository in which dataset is stored
   TString fSubdirs;            //list of subdirectories containing data for this dataset
   KVList fTasks;                //possible data analysis tasks for this dataset
   TString fCalibDir;           //directory containing database, calibration, identification parameters etc. for dataset
   Bool_t fDBBuild;             //has the database been built by us ?
   TFile *fDBase;               //file containing database
   TString fDBName;             //name of database
   TString fDBFileName;         //name of file in which database is stored on disk
   KVDataBase *fDataBase;       //pointer to dataset's database
   KVUpDater *fUpDater;         //!used to set run parameters of detectors
   KVList fAvailableRuns;       //!list of KVAvailableRunsFile objects used to read infos on available runs
   enum {
      kAvailable = BIT(14)      //flag set if this dataset is physically present on local machine
   };
   TString fUserGroups;         //list of user groups allowed to read data

   virtual const Char_t *GetBaseFileName(const Char_t * type, Int_t run);

   virtual void OpenDataBase(Option_t * opt = "");
   virtual void OpenDBFile(const Char_t * full_path_to_dbfile);
   virtual void WriteDBFile(const Char_t * full_path_to_dbfile);
   void SetDBFileName(const Char_t * name) {
      fDBFileName = name;
   };
   const Char_t *GetDBFileName() const;
   void SetDBName(const Char_t * name) {
      fDBName = name;
   };
   const Char_t *GetDBName() const;
   TObject* Open(const Char_t* type, Int_t run, Option_t* opt="");
   const Char_t* GetFullPathToDB();
   
 public:

    KVAvailableRunsFile * GetAvailableRunsFile(const Char_t *
                                                       type);

    KVDataSet();
    virtual ~ KVDataSet();

   virtual Bool_t CheckUserCanAccess();

   virtual void SetDatapathSubdir(const Char_t * s) {
      SetLabel(s);
   };
   // Returns name of top-level directory in data repository used to store data files for this dataset
   virtual const Char_t *GetDatapathSubdir() const {
      return GetLabel();
   };

   virtual void SetUserGroups(const Char_t * groups) {
      fUserGroups = groups;
   };
   // Returns kTRUE if this dataset is available for analysis, i.e. if any associated data files are stored in the data repository
   virtual Bool_t IsAvailable() const {
      return TestBit(kAvailable);
   };
   virtual void SetAvailable(Bool_t yes = kTRUE) {
      SetBit(kAvailable, yes);
   };
   virtual void CheckAvailable();
   // Returns kTRUE if "raw" data files are stored in the data repository
   virtual Bool_t HasRaw() const {
      return fSubdirs.Contains("raw");
   };
   virtual Bool_t HasDST() const {
      return fSubdirs.Contains("dst");
   };
   // Returns kTRUE if "recon" data files are stored in the data repository
   virtual Bool_t HasRecon() const {
      return fSubdirs.Contains("recon");
   };
   // Returns kTRUE if "ident" data files are stored in the data repository
   virtual Bool_t HasIdent() const {
      return fSubdirs.Contains("ident");
   };
   // Returns kTRUE if "root" data files are stored in the data repository
   virtual Bool_t HasPhys() const {
      return fSubdirs.Contains("root");
   };
   // Returns kTRUE if data files of the given type are stored in the data repository
   virtual Bool_t HasSubdir(const Char_t * sd) {
      return fSubdirs.Contains(sd);
   };

   virtual KVMultiDetArray *BuildMultiDetector() const;


   virtual KVUpDater *GetUpDater();

   virtual void ls(Option_t * opt = "") const;
   virtual void Print(Option_t * opt = "") const;

   virtual void SetAnalysisTasks(const KVList *);
   virtual KVDataAnalysisTask *GetAnalysisTask(Int_t) const;
   virtual KVDataAnalysisTask *GetAnalysisTask(const Char_t* keywords) const;
   virtual KVDataAnalysisTask *GetAnalysisTaskAny(const Char_t* keywords) const;
   virtual Int_t GetNtasks() const;

   virtual TList *GetListOfAvailableSystems(const Char_t * datatype,
                                            KVDBSystem * systol = 0);
   virtual TList *GetListOfAvailableSystems(KVDataAnalysisTask* datan,
                                            KVDBSystem * systol = 0);

   void cd();

   const Char_t *GetDataSetDir() const;
   void SetName(const char *name);


   KVDataBase *GetDataBase(Option_t * opt = "");
   virtual void SaveDataBase();

   const Char_t *GetDataSetEnv(const Char_t * type, const Char_t* defval="") const;
   Double_t GetDataSetEnv(const Char_t * type, Double_t defval) const;
   Bool_t GetDataSetEnv(const Char_t * type, Bool_t defval) const;

   const Char_t *GetFullPathToRunfile(const Char_t * type, Int_t run);
   const Char_t *GetRunfileName(const Char_t * type, Int_t run);
   TDatime GetRunfileDate(const Char_t * type, Int_t run);
   virtual TObject *OpenRunfile(const Char_t * type, Int_t run);
   Bool_t CheckRunfileAvailable(const Char_t * type, Int_t run);
   void UpdateAvailableRuns(const Char_t * type);
   TFile *NewRunfile(const Char_t * type, Int_t run);
   void CommitRunfile(const Char_t * type, Int_t run, TFile * file);

   void DeleteRunfile(const Char_t * type, Int_t run, Bool_t confirm=kTRUE);
   void DeleteRunfiles(const Char_t * type, KVNumberList lrun="", Bool_t confirm=kTRUE);
   KVNumberList GetRunList_DateSelection(const Char_t * type,TDatime* min=0,TDatime* max=0);
	KVNumberList GetRunList_StageSelection(const Char_t *type, const Char_t* in="raw");	
   void SetRepository(KVDataRepository *);
   KVDataRepository *GetRepository() const;

   const Char_t *GetSubdirs() const {
      return fSubdirs.Data();
   };
   void AddSubdir(const Char_t * subdir);

   void CheckMultiRunfiles(const Char_t * data_type);
   void CleanMultiRunfiles(const Char_t * data_type, Bool_t confirm=kTRUE);

   Bool_t CheckRunfileUpToDate(const Char_t * data_type, Int_t run,
                               KVDataRepository * other_repos);
   Bool_t CheckRunfileUpToDate(const Char_t * data_type, Int_t run,
                               const Char_t * other_repos);
   void CheckUpToDate(const Char_t * data_type,
                      const Char_t * other_repos);
   KVNumberList GetUpdatableRuns(const Char_t * data_type,
                                 const Char_t * other_repos);

   KVNumberList GetRunList(const Char_t * data_type,
                           const KVDBSystem * sys = 0);
   
   virtual void MakeAnalysisClass(const Char_t* task, const Char_t* classname);
   
   virtual Bool_t OpenDataSetFile(const Char_t* filename, ifstream& file);
	virtual Bool_t DataBaseNeedsUpdate();

   ClassDef(KVDataSet, 2)       //Describes a set of experimental data which may be analysed with KaliVeda
};

//........ global variable
R__EXTERN KVDataSet *gDataSet;

#endif
