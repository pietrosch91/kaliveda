//Created by KVClassFactory on Fri Jul 20 15:49:04 2018
//Author: eindra

#include "KVExpSetUpDB.h"
#include "KVDataSet.h"
#include "KVMultiDetArray.h"

#include <TKey.h>

ClassImp(KVExpSetUpDB)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVExpSetUpDB</h2>
<h4>Database for dataset using KVExpSetUp</h4>
<!-- */
// --> END_HTML
/*
KVExpSetUp combines two or more existing KVMultiDetArray objects to describe the
geometry, detectors, etc. of an experiment. Each KVMultiDetArray can have its own
dedicated KVExpDB and KVUpdater class used to store and set calibration parameters.
In this case building the database for the experiment means building each individual
database...
*/
////////////////////////////////////////////////////////////////////////////////

KVExpSetUpDB::KVExpSetUpDB()
   : KVExpDB()
{
   // Default constructor
}

//____________________________________________________________________________//

KVExpSetUpDB::KVExpSetUpDB(const Char_t* name)
   : KVExpDB(name)
{
   // Constructor inherited from KVExpDB
}

//____________________________________________________________________________//

KVExpSetUpDB::KVExpSetUpDB(const Char_t* name, const Char_t* title)
   : KVExpDB(name, title)
{
   // Constructor inherited from KVExpDB
}

//____________________________________________________________________________//

KVExpSetUpDB::~KVExpSetUpDB()
{
   // Destructor
}

void KVExpSetUpDB::Build()
{
   // Build the database.
   // Runs & Systems tables are handled by us, calibrations are handled by
   // individual databases for each multidetector
   // The classes for these are deduced from the variable
   //   [dataset].ExpSetUp.DataBases: [dataset1] [dataset2] ...
   // containing the names of datasets for which we should use the defined
   // database plugin

   FillRunsTable();
   ReadComments();
   ReadSystemList();

   KVMultiDetArray::MakeMultiDetector(fDataSet);
   gMultiDetArray->MakeCalibrationTables(this);
}

void KVExpSetUpDB::FillRunsTable()
{
   // Fill the Runs table using the informations in file runinfos.root
   // (which can be generated using KVRunListCreator).

   TString runinfos = KVDataSet::GetFullPathToDataSetFile(fDataSet, "runinfos.root");
   Info("FillRunsTable", "Reading run infos from %s", runinfos.Data());
   TFile runinfos_file(runinfos);
   TIter it(runinfos_file.GetListOfKeys());
   TKey* run_key;
   KVList garbage;
   while ((run_key = (TKey*)it())) {
      if (TString(run_key->GetClassName()) == "KVNameValueList") {
         // make sure we only use the highest cycle number of each key
         if (run_key->GetCycle() == runinfos_file.GetKey(run_key->GetName())->GetCycle()) {
            KVNameValueList* run = (KVNameValueList*)run_key->ReadObj();
            garbage.Add(run);
            KVDBRun* dbrun = new KVDBRun;
            dbrun->SetNumber(run->GetIntValue("Run"));
            dbrun->SetStartDate(run->GetStringValue("Start"));
            dbrun->SetEndDate(run->GetStringValue("End"));
            if (run->HasValue64bit("Size"))
               dbrun->SetSize(run->GetValue64bit("Size") / 1024. / 1024.);
            else
               dbrun->SetSize(run->GetIntValue("Size") / 1024. / 1024.);

            if (run->HasValue64bit("Events")) {
               dbrun->SetEvents(run->GetValue64bit("Events"));
            }
            else
               dbrun->SetEvents(run->GetIntValue("Events"));
            AddRun(dbrun);
         }
      }
   }
}

//____________________________________________________________________________//

