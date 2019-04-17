#include "KVRunListCreator.h"
#include "TEnv.h"

#include <KVSystemDirectory.h>
#include <KVAvailableRunsFile.h>
#include <KVSystemFile.h>

ClassImp(KVRunListCreator)

KVRunListCreator::KVRunListCreator(const TString& dir, const TString& datatype, const TString& fmt)
{
   // Give directory to scan, optional format for runfile names
   // "datatype" should correspond to a known plugin for reading raw data files,
   // see available plugins for KVRawDataReader (output of method KVBase::GetListOfPluginURIs("KVRawDataReader"))

   SetRunDir(dir);
   if (fmt != "") SetFileFormat(fmt);
   else SetFileFormat(gEnv->GetValue("DataSet.RunFileName.raw", "run_%04d.dat%*"));
   fRunInfos.SetOwner();
   fDataType = datatype;
   ScanDirectory();
   TFile f("runinfos.root", "recreate");
   GetRunInfos().Write();
   f.Close();
}

Int_t KVRunListCreator::ScanDirectory()
{
   // Return number of runs found

   KVSystemDirectory sysdir("rundir", GetRunDir());
   TIter it(sysdir.GetListOfFiles());
   KVSystemFile* sysfile;
   int howmany = 0;
   while ((sysfile = (KVSystemFile*)it())) {
      int run_num;
      if ((run_num = KVAvailableRunsFile::IsRunFileName(GetFileFormat(), sysfile->GetName()))) {
         KVDatime when;
         if (KVAvailableRunsFile::ExtractDateFromFileName(GetFileFormat(), sysfile->GetName(), when)) {
            ++howmany;
            KVNameValueList* infos = new KVNameValueList;
            infos->SetName(Form("run%010d", run_num));
            infos->SetValue("Run", run_num);
            infos->SetValue("Start", when.AsSQLString());
            infos->SetValue("End", sysfile->GetDate());
            Int_t x = sysfile->GetSize();
            if (x == sysfile->GetSize())
               infos->SetValue("Size", x);
            else
               infos->SetValue64bit("Size", sysfile->GetSize());
            if (fDataType != "") {
               fReader.reset(KVRawDataReader::OpenFile(fDataType, Form("%s/%s", sysdir.GetTitle(), sysfile->GetName())));
               ULong64_t events = 0;
               // correct start date/time from infos in file (GANIL/MFM data) ?
               if (fReader->GetRunInfos().HasStringParameter("FileCreationTime")) {
                  when.SetGanacqNarvalDate(fReader->GetRunInfos().GetStringValue("FileCreationTime"));
                  infos->SetValue("Start", when.AsSQLString());
               }
               while (fReader->GetNextEvent()) ++events;

               infos->SetValue64bit("Events", events);
            }
            infos->ls();
            fRunInfos.Add(infos);
         }
      }
   }
   return howmany;
}
