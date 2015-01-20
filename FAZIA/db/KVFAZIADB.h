//Created by KVClassFactory on Tue Jan 20 11:52:49 2015
//Author: E. Bonnet,,,

#ifndef __KVFAZIADB_H
#define __KVFAZIADB_H

#include "KVDataBase.h"
#include "KVDBTable.h"
#include "KVDBRun.h"
#include "KVDBRecord.h"

class KVNumberList;
class KVDBSystem;

class KVFAZIADB : public KVDataBase
{

   protected:
   Int_t kFirstRun;
   Int_t kLastRun;
	KVDBTable *fRuns;            //-> table of runs
   KVDBTable *fSystems;         //-> table of systems
   
   virtual void ReadSystemList();
   virtual void LinkListToRunRanges(TList * list, UInt_t rr_number,
                                    UInt_t run_ranges[][2]);
   virtual void LinkRecordToRunRanges(KVDBRecord * rec, UInt_t rr_number,
                                      UInt_t run_ranges[][2]);
   virtual void LinkRecordToRunRange(KVDBRecord * rec, UInt_t first_run,
                                     UInt_t last_run);
   virtual void LinkListToRunRange(TList * list, KVNumberList nl);
   virtual void LinkRecordToRunRange(KVDBRecord * rec,  KVNumberList nl);
   virtual void LinkRecordToRun(KVDBRecord * rec,  Int_t run);
   
	void init();
   Bool_t OpenCalibFile(const Char_t * type, std::ifstream & fs) const;
	const Char_t *GetDBEnv(const Char_t * type) const;
   
   public:
   
   KVFAZIADB();
   KVFAZIADB(const Char_t* name);
   virtual ~KVFAZIADB();
   
   virtual void Build();
//   virtual void GoodRunLine();
	void AddSystem(KVDBSystem * r) { fSystems->AddRecord(r); };

   void AddRun(KVDBRun * r) { fRuns->AddRecord(r); }
   virtual void Save(const Char_t*);

   KVDBRun *GetRun(Int_t run) const
   {
   	//Returns KVDBRun describing run number 'run'
   	return (KVDBRun *) fRuns->GetRecord(run);
   }
   KVDBRun *GetRun(const Char_t * run) const
	{
   	//Returns KVDBRun describing run with name "run"
   	return (KVDBRun *) GetRuns()->FindObject(run);
	}

   virtual TList *GetRuns() const { return fRuns->GetRecords(); }
	
   virtual KVDBSystem *GetSystem(const Char_t * system) const
   {
   return (KVDBSystem *) fSystems->GetRecord(system);
	}
	
   virtual TList *GetSystems() const
	{
	   return fSystems->GetRecords();
	}
   
   void WriteRunListFile() const;
   void ReadNewRunList();
   void WriteSystemsFile() const;

   virtual void PrintRuns(KVNumberList&) const;
	virtual void cd();
    const Char_t *GetCalibFileName(const Char_t * type) const {
      return GetDBEnv(type);
   };
   ClassDef(KVFAZIADB,1)//database for FAZIA detector
};
//........ global variable
R__EXTERN KVFAZIADB *gFaziaDB;

#endif
