/***************************************************************************
$Id: KVINDRADB.h,v 1.26 2009/01/22 13:56:42 franklan Exp $
                          kvINDRADB.h  -  description
                             -------------------
    begin                : 9/12 2003
    copyright            : (C) 2003 by J.D. Frankland
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

#ifndef KV_INDRADB_H
#define KV_INDRADB_H

#include "KVDataBase.h"
#include "KVDataSet.h"
#include "KVINDRADBRun.h"
#include "KVDBSystem.h"
#include "KVDBTape.h"
#include "KVDetector.h"
#include "Riostream.h"
#include "TEnv.h"
#include "KVINDRARunListReader.h"
#include "KVINDRAPulserDataTree.h"

//dimension of run_range arrays
#define MAX_NUM_RUN_RANGES 100

class KVINDRADB:public KVDataBase, public KVINDRARunListReader {

 private:

   ifstream __ifpeaks;          //!ifstream for calibration peaks file

 protected:

   Bool_t OpenCalibFile(const Char_t * type, ifstream & fs) const;

   //General information

   Int_t kFirstRun;
   Int_t kLastRun;

   KVDBTable *fRuns;            //-> table of runs
   KVDBTable *fSystems;         //-> table of systems
   KVDBTable *fChIoPressures;   //-> table of chio pressures
   KVDBTable *fGains;           //(optional) table of detector gains, in case they change from run to run
   KVDBTable *fTapes;           //-> table of data tapes

	KVINDRAPulserDataTree *fPulserData;  //! mean values of pulsers for all detectors & runs
	
   virtual void LinkListToRunRanges(TList * list, UInt_t rr_number,
                                    UInt_t run_ranges[][2]);
   virtual void LinkRecordToRunRanges(KVDBRecord * rec, UInt_t rr_number,
                                      UInt_t run_ranges[][2]);
   virtual void LinkRecordToRunRange(KVDBRecord * rec, UInt_t first_run,
                                     UInt_t last_run);
   virtual void ReadSystemList();
   virtual void ReadGainList();
   virtual void ReadChIoPressures();

   //calibration peaks database
   Bool_t OpenCalibrationPeakFile();
   void CloseCalibrationPeakFile();
    ifstream & GetPeakFileStream() {
      return __ifpeaks;
   };

   void init();

 public:
   KVINDRADB();
   KVINDRADB(const Char_t * name);
   virtual ~ KVINDRADB();

   virtual void Build();
   virtual void GoodRunLine();

   void AddRun(KVDBRun * r) {
      fRuns->AddRecord(r);
   };
   void AddTape(KVDBTape * r) {
      fTapes->AddRecord(r);
   };
   void AddSystem(KVDBSystem * r) {
      fSystems->AddRecord(r);
   };

   virtual void Save(const Char_t*);

   KVINDRADBRun *GetRun(Int_t run) const;
   KVINDRADBRun *GetRun(const Char_t * run) const;

   virtual TList *GetRuns() const;
   virtual KVDBSystem *GetSystem(const Char_t * system) const;
   virtual TList *GetSystems() const;

   virtual KVDBTape *GetTape(Int_t tape) const;
   virtual TList *GetTapes() const;

   KVList *GetCalibrationPeaks(Int_t run, KVDetector * detector = 0,
                               Int_t peak_type = -1, Int_t signal_type = 0,
                               Double_t peak_energy = -1.0);

   Double_t GetEventCrossSection(Int_t run, Double_t Q_apres_cible,
                                 Double_t Coul_par_top = 1.e-10) const;
   Double_t GetTotalCrossSection(Int_t run, Double_t Q_apres_cible,
                                 Double_t Coul_par_top = 1.e-10) const;
   Double_t GetEventCrossSection(Int_t run1, Int_t run2,
                                 Double_t Q_apres_cible,
                                 Double_t Coul_par_top = 1.e-10) const;
   Double_t GetTotalCrossSection(Int_t run1, Int_t run2,
                                 Double_t Q_apres_cible,
                                 Double_t Coul_par_top = 1.e-10) const;
   Double_t GetEventCrossSection(const Char_t * system, Int_t Mult_trigger,
                                 Double_t Q_apres_cible,
                                 Double_t Coul_par_top = 1.e-10) const;
   Double_t GetTotalCrossSection(const Char_t * system, Int_t Mult_trigger,
                                 Double_t Q_apres_cible,
                                 Double_t Coul_par_top = 1.e-10) const;

   void WriteRunListFile() const;
   void ReadNewRunList();
   void WriteSystemsFile() const;

   const Char_t *GetDBEnv(const Char_t * type) const;
   const Char_t *GetCalibFileName(const Char_t * type) const {
      return GetDBEnv(type);
   };

   inline virtual void cd();
	virtual void WriteObjects(TFile*);
	virtual void ReadObjects(TFile*);
	
	KVINDRAPulserDataTree* GetPulserData() { return fPulserData; };

   ClassDef(KVINDRADB, 3)       //DataBase of parameters for an INDRA campaign
};

//........ global variable
R__EXTERN KVINDRADB *gIndraDB;

//.............inline functions

inline void KVINDRADB::cd()
{
   KVDataBase::cd();
   gIndraDB = this;
}

inline TList *KVINDRADB::GetRuns() const
{
   return fRuns->GetRecords();
}

inline KVDBSystem *KVINDRADB::GetSystem(const Char_t * system) const
{
   return (KVDBSystem *) fSystems->GetRecord(system);
}

inline TList *KVINDRADB::GetSystems() const
{
   return fSystems->GetRecords();
}

inline KVDBTape *KVINDRADB::GetTape(Int_t tape) const
{
   return (KVDBTape *) fTapes->GetRecord(tape);
}

inline TList *KVINDRADB::GetTapes() const
{
   return fTapes->GetRecords();
}


inline KVINDRADBRun *KVINDRADB::GetRun(Int_t run) const
{
   //Returns KVINDRADBRun describing run number 'run'
   return (KVINDRADBRun *) fRuns->GetRecord(run);
}

inline KVINDRADBRun *KVINDRADB::GetRun(const Char_t * run) const
{
   //Returns KVINDRADBRun describing run with name "run"
   return (KVINDRADBRun *) GetRuns()->FindObject(run);
}

#endif
