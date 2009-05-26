/***************************************************************************
$Id: KVINDRADB5.cpp,v 1.45 2009/04/07 07:45:04 franklan Exp $
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "Riostream.h"
#include "KVINDRA5.h"
#include "KVINDRADB5.h"
#include "KVINDRADBRun.h"
#include "KVDBSystem.h"
#include "KVDBPeak.h"
#include "KVDBAlphaPeak.h"
#include "KVDBElasticPeak.h"
#include "KVDBParameterSet.h"
#include "KVChIo.h"
#include "KVCsI.h"
#include "KVString.h"

//dimension of run_range arrays
#define MAX_NUM_RUN_RANGES 100

ClassImp(KVINDRADB5)
//___________________________________________________________________________
//   Base de donnee pour la 5e campagne INDRA.
//   Cette base contiendra la liste:
//         - des runs (unique)
//         - des DLT  (unique) 
//         - des systemes (unique)
//         - des pressions des chios
//         - des parametres de calibration Canal-Volts
//         - des parametres de calibration Volt-Energies des ChIo & Si
//         - des parametres de calibration du defaut d'ionisation des Si
//         - des pics de calibration alpha pour le premier pic thoron 
//         - des pics de calibration alpha pour le deuxieme pic thoron 
//         - des pics de calibration elastique 7 MeV/A 
//         - des pics de calibration elastique 20 MeV/A
//         
//   
//    la mention (unique) signifie que 2 enregistrements ne peuvent avoir
//    le meme nom
//    Ce ne peut etre le cas des parametres de calibration puisque plusieurs 
//    peuvent exister pour un meme detecteur et que les parametres portent le nom
//    du detecteur correspondant
//
//
void KVINDRADB5::init()
{
   //Define duplicate run, create the tables of the database.
   kDuplicatedRun = 8449;

   fChanVolt =
       AddTable("Channel-Volt",
                "Calibration parameters for Channel-Volts conversion");
   fVoltMeVChIoSi =
       AddTable("Volt-Energy ChIo-Si",
                "Calibration parameters for ChIo-Si Volts-Energy conversion");
   fPedestals = AddTable("Pedestals", "List of pedestal files");
   fLitEnerCsIZ1 =
       AddTable("Light-Energy CsI Z=1",
                "Calibration parameters for CsI detectors");
   fLitEnerCsI =
       AddTable("Light-Energy CsI Z>1",
                "Calibration parameters for CsI detectors");
}

KVINDRADB5::KVINDRADB5()
{
   //ctor.
   init();
}

KVINDRADB5::KVINDRADB5(const Char_t * name):KVINDRADB(name)
{
   //ctor.
   init();
}

//___________________________________________________________________________

KVINDRADB5::~KVINDRADB5()
{

}

//___________________________________________________________________________

void KVINDRADB5::Build()
{
   //Fill the tables of the database.

   //Use KVINDRARunListReader utility subclass to read complete runlist
   TString runlist_fullpath;
   KVBase::SearchKVFile(GetCalibFileName("Runlist"), runlist_fullpath,
                        fDataSet);
   SetRLCommentChar('#');
   SetRLSeparatorChar('|');
   GetLineReader()->SetFieldKeys(3, "Tape", "Run", "Events");
   GetLineReader()->SetRunKeys(2, "Tape", "Run");
   kFirstRun = 999999;
   kLastRun = 0;
   ReadRunList(runlist_fullpath.Data());

   ReadSystemList();
   ReadGainList();
   ReadChIoPressures();
   ReadChannelVolt();
   ReadVoltEnergyChIoSi();
   ReadCalibCsI();
   ReadPedestalList();
	// read all available mean pulser data and store in tree
	if( !fPulserData ) fPulserData = new KVINDRAPulserDataTree;
	fPulserData->SetRunList( GetRuns() );
	fPulserData->Build();
}

//____________________________________________________________________________
void KVINDRADB5::GoodRunLine()
{
   //For each "good run line" in the run list file, we:
   //      add a KVDBRun to the database if it doesn't already exist
   //      add a KVDBTape to the database if it doesn't already exist
   //      set properties of run and tape objects
   //kFirstRun & kLastRun are set

   KVRunListLine *csv_line = GetLineReader();

   //run number
   Int_t run_n = csv_line->GetIntField("Run");

   if (!run_n) {
      cout << "run_n = 0 ?????????  line number =" << GetRLLineNumber() <<
          endl;
      GetLineReader()->Print();
      return;
   }
   kLastRun = TMath::Max(kLastRun, run_n);
   kFirstRun = TMath::Min(kFirstRun, run_n);

        /*********************************************
		IF LINE HAS A TAPE NUMBER WE
		LOOK FOR THE TAPE IN THE DATA
		BASE. IF IT DOESN'T EXIST WE
		CREATE IT.
	*********************************************/
   KVDBTape *tape = 0;
   //tape number 
   Int_t tape_n = csv_line->GetIntField("Tape");
   //already exists ?
   tape = GetTape(tape_n);
   if (!tape) {
      tape = new KVDBTape(tape_n);
      AddTape(tape);
   }

        /*********************************************
		WE CREATE A NEW RUN AND ADD
		IT TO THE DATABASE. WE SET ALL
		AVAILABLE INFORMATIONS ON
		RUN FROM THE FILE. IF
		DBASE RUN ALREADY EXISTS =>
		DUPLICATE RUN IS CALLED "RUN xxxxb"
	*********************************************/
   KVINDRADBRun *run = (KVINDRADBRun *) GetRun(run_n);
   if (!run) {
      run = new KVINDRADBRun(run_n);
   } else {
      //run already exists : we will call this one (duplicate) "Run xxxxb"
      run = new KVINDRADBRun(run_n);    //same number
      run->SetName(Form("Run %db", run_n));
   }
   AddRun(run);
   //add run to tape ?
   if (tape)
      tape->AddRun(run);

   if (csv_line->HasFieldValue("Events"))
      run->SetEvents(csv_line->GetIntField("Events"));
   else
      Error("GoodRunLine", "No Events field ? run=%d", run_n);
   if (csv_line->HasFieldValue("Faraday1"))
      run->SetScaler("Faraday 1", csv_line->GetIntField("Faraday1"));
   else
      Error("GoodRunLine", "No Faraday field ? run=%d", run_n);
   if (csv_line->HasFieldValue("Buffers"))
      run->SetScaler("Buffers", csv_line->GetIntField("Buffers"));
   else
      Error("GoodRunLine", "No Buffers field ? run=%d", run_n);
   if (csv_line->HasFieldValue("Gene direct"))
      run->SetScaler("Gene DIRECT",csv_line->GetIntField("Gene direct"));
   else
      Error("GoodRunLine", "No Gene direct field ? run=%d", run_n);
   if (csv_line->HasFieldValue("Gene mrq"))
      run->SetScaler("Gene MARQ",csv_line->GetIntField("Gene mrq"));
   else
      Error("GoodRunLine", "No Gene mrq field ? run=%d", run_n);
   if (csv_line->HasFieldValue("G TM"))
      run->SetScaler("Gene TM", csv_line->GetIntField("G TM"));
   else
      Error("GoodRunLine", "No Gene TM field ? run=%d", run_n);
   if (csv_line->HasFieldValue("TM %"))
      run->SetTMpercent(csv_line->GetFloatField("TM %"));
   else
      Error("GoodRunLine", "No TM %% field ? run=%d", run_n);
   if (csv_line->HasField("Trigger"))
      run->SetTrigger(GetRunListTrigger("Trigger", "M>=%d"));
   else
      Error("GoodRunLine", "No Trigger field ? run=%d", run_n);
   if (csv_line->HasFieldValue("Remarque etc."))
      run->SetComments(csv_line->GetField("Remarque etc."));
   if (csv_line->HasFieldValue("T(min)"))
      run->SetTime(csv_line->GetFloatField("T(min)"));
   if (csv_line->HasFieldValue("Date"))
      run->SetDate(csv_line->GetField("Date"));
   if (csv_line->HasFieldValue("cahier-page"))
      run->SetCahier(csv_line->GetField("cahier-page"));
}

//________________________________________________________________________________
void KVINDRADB5::ReadChannelVolt()
{

   ifstream fin;
   if (!OpenCalibFile("ElectronicCalibration", fin)) {
      Error("ReadChannelVolt()", "Could not open file %s",
            GetCalibFileName("ElectronicCalibration"));
      return;
   }
   Info("ReadChannelVolt()",
        "Reading electronic calibration for ChIo and Si...");

   TString sline;

   UInt_t frun = 0, lrun = 0;
   UInt_t run_ranges[MAX_NUM_RUN_RANGES][2];
   UInt_t rr_number = 0;
   Bool_t prev_rr = kFALSE;     // was the last line a run range indication ?

   UInt_t cour, modu, sign;
   Float_t a0, a1, a2, dum1, dum2;

   Char_t det_name[80];
   Char_t cal_type[80];
   KVDBParameterSet *parset;
   TList *par_list = new TList();


   while (fin.good()) {         //reading the file
      sline.ReadLine(fin);
      if (fin.eof()) {          //fin du fichier
         LinkListToRunRanges(par_list, rr_number, run_ranges);
         par_list->Clear();
         break;
      }
      if (sline.BeginsWith("Run Range :")) {    // Run Range found
         if (!prev_rr) {        // new run ranges set
            if (par_list->GetSize() > 0)
               LinkListToRunRanges(par_list, rr_number, run_ranges);
            par_list->Clear();
            rr_number = 0;
         }
         if (sscanf(sline.Data(), "Run Range : %u %u", &frun, &lrun) != 2) {
            Warning("ReadChannelVolt()",
                    "Bad format in line :\n%s\nUnable to read run range values",
                    sline.Data());
         } else {
            prev_rr = kTRUE;
            run_ranges[rr_number][0] = frun;
            run_ranges[rr_number][1] = lrun;
            rr_number++;
            if (rr_number == MAX_NUM_RUN_RANGES) {
               Error("ReadChannelVolt", "Too many run ranges (>%d)",
                     rr_number);
               rr_number--;
            }
         }
      }                         //Run Range found
      else if (sline.Sizeof() > 1 && !sline.BeginsWith("#")) {  //non void nor comment line
         if (sscanf(sline.Data(), "%u %u %u %f %f %f %f %f",
                    &cour, &modu, &sign, &a0, &a1, &a2, &dum1,
                    &dum2) != 8) {
            Warning("ReadChannelVolt()",
                    "Bad format in line :\n%s\nUnable to read",
                    sline.Data());
         } else {               //parameters correctly read
            // naming detector
            switch (sign) {
            case ChIo_GG:
               sprintf(det_name, "CI_%02u%02u_GG", cour, modu);
               strcpy(cal_type, "Channel-Volt GG");
               break;
            case ChIo_PG:
               sprintf(det_name, "CI_%02u%02u_PG", cour, modu);
               strcpy(cal_type, "Channel-Volt PG");
               break;
            case Si_GG:
               sprintf(det_name, "SI_%02u%02u_GG", cour, modu);
               strcpy(cal_type, "Channel-Volt GG");
               break;
            case Si_PG:
               sprintf(det_name, "SI_%02u%02u_PG", cour, modu);
               strcpy(cal_type, "Channel-Volt PG");
               break;
            }
            parset = new KVDBParameterSet(det_name, cal_type, 5);
            parset->SetParameters(a0, a1, a2, dum1, dum2);
            prev_rr = kFALSE;
            fChanVolt->AddRecord(parset);
            par_list->Add(parset);
         }                      //parameters correctly read
      }                         //non void nor comment line
   }                            //reading the file
   delete par_list;
   fin.close();
	
	/********** ETALONS ***************/
	
   ifstream fin2;
   if (!OpenCalibFile("ElectronicCalibration.Etalons", fin2)) {
      Error("ReadChannelVolt()", "Could not open file %s",
            GetCalibFileName("ElectronicCalibration.Etalons"));
      return;
   }
   Info("ReadChannelVolt()",
        "Reading electronic calibration for Si75 and SiLi...");
   frun = lrun = 0;
   rr_number = 0;
   prev_rr = kFALSE;     // was the last line a run range indication ?
	par_list = new TList;
	
   while (fin2.good()) {         //reading the file
      sline.ReadLine(fin2);
      if (fin2.eof()) {          //fin du fichier
         LinkListToRunRanges(par_list, rr_number, run_ranges);
         par_list->Clear();
         delete par_list;
         fin2.close();
         return;
      }
      if (sline.BeginsWith("Run Range :")) {    // Run Range found
         if (!prev_rr) {        // new run ranges set
            if (par_list->GetSize() > 0)
               LinkListToRunRanges(par_list, rr_number, run_ranges);
            par_list->Clear();
            rr_number = 0;
         }
         if (sscanf(sline.Data(), "Run Range : %u %u", &frun, &lrun) != 2) {
            Warning("ReadChannelVolt()",
                    "Bad format in line :\n%s\nUnable to read run range values",
                    sline.Data());
         } else {
            prev_rr = kTRUE;
            run_ranges[rr_number][0] = frun;
            run_ranges[rr_number][1] = lrun;
            rr_number++;
            if (rr_number == MAX_NUM_RUN_RANGES) {
               Error("ReadChannelVolt", "Too many run ranges (>%d)",
                     rr_number);
               rr_number--;
            }
         }
      }                         //Run Range found
      else if (sline.Sizeof() > 1 && !sline.BeginsWith("#")) {  //non void nor comment line
         if (sscanf(sline.Data(), "%s %f %f %f",
                    det_name, &a0, &a1, &a2) != 4) {
            Warning("ReadChannelVolt()",
                    "Bad format in line :\n%s\nUnable to read",
                    sline.Data());
         } else {               //parameters correctly read
            strcpy(cal_type, "Channel-Volt PG");
            parset = new KVDBParameterSet(det_name, cal_type, 3);
            parset->SetParameters(a0, a1, a2);
            prev_rr = kFALSE;
            fChanVolt->AddRecord(parset);
            par_list->Add(parset);
         }                      //parameters correctly read
      }                         //non void nor comment line
   }                            //reading the file
   delete par_list;
}

//__________________________________________________________________________________

void KVINDRADB5::ReadVoltEnergyChIoSi()
{

   ifstream fin;
   if (!OpenCalibFile("ChIoSiVoltMeVCalib", fin)) {
      Error("ReadVoltEnergyChIoSi()", "Could not open file %s",
            GetCalibFileName("ChIoSiVoltMevCalib"));
      return;
   }
   Info("ReadVoltEnergyChIoSi()",
        "Reading ChIo/Si calibration parameters...");

   TString sline;

   UInt_t frun = 0, lrun = 0;
   UInt_t run_ranges[MAX_NUM_RUN_RANGES][2];
   UInt_t rr_number = 0;
   Bool_t prev_rr = kFALSE;     // was the last line a run range indication ?

   Char_t det_name[80];
   KVDBParameterSet *parset;
   TList *par_list = new TList();

   Float_t a0, a1, chi;         // calibration parameters

   while (fin.good()) {         //reading the file
      sline.ReadLine(fin);
      if (fin.eof()) {          //fin du fichier
         LinkListToRunRanges(par_list, rr_number, run_ranges);
         par_list->Clear();
         delete par_list;
         fin.close();
         return;
      }
      if (sline.BeginsWith("Run Range :")) {    // Run Range found
         if (!prev_rr) {        // new run ranges set
            if (par_list->GetSize() > 0)
               LinkListToRunRanges(par_list, rr_number, run_ranges);
            par_list->Clear();
            rr_number = 0;
         }
         if (sscanf(sline.Data(), "Run Range : %u %u", &frun, &lrun) != 2) {
            Warning("ReadVoltEnergyAlpha()",
                    "Bad format in line :\n%s\nUnable to read run range values",
                    sline.Data());
         } else {
            prev_rr = kTRUE;
            run_ranges[rr_number][0] = frun;
            run_ranges[rr_number][1] = lrun;
            rr_number++;
            if (rr_number == MAX_NUM_RUN_RANGES) {
               Error("ReadVoltEnergyAlpha", "Too many run ranges (>%d)",
                     rr_number);
               rr_number--;
            }
         }
      }                         //Run Range found
      if (sline.BeginsWith("SI") || sline.BeginsWith("CI")) {   //data line
         if (sscanf(sline.Data(), "%7s %f %f %f", det_name, &a0, &a1, &chi)
             != 4) {
            Warning("ReadVoltEnergyAlpha()",
                    "Bad format in line :\n%s\nUnable to read parameters",
                    sline.Data());
         } else {               //parameters correctly read
            parset = new KVDBParameterSet(det_name, "Volt-Energy", 3);
            parset->SetParameters(a0, a1, chi);
            prev_rr = kFALSE;
            fVoltMeVChIoSi->AddRecord(parset);
            par_list->Add(parset);
         }                      //parameters correctly read
      }                         //data line
   }                            //reading the file
   delete par_list;
   fin.close();
}

//__________________________________________________________________________

void KVINDRADB5::ReadPedestalList()
{
   //Read from file the pedestal files to use for each run range


   ifstream fin;
   if (!OpenCalibFile("Pedestals", fin)) {
      Error("ReadPedestalList()", "Could not open file %s",
            GetCalibFileName("Pedestals"));
      return;
   }
   Info("ReadPedestalList()", "Reading pedestal file list...");

   KVString line;
   Char_t filename_chio[80], filename_csi[80];
   UInt_t runlist[1][2];

   while (fin.good()) {         //lecture du fichier

      // lecture des informations
      line.ReadLine(fin);

      //recherche une balise '+'
      if (line.BeginsWith('+')) {       //balise trouvee

         line.Remove(0, 1);

         if (sscanf
             (line.Data(), "Run Range : %u %u", &runlist[0][0],
              &runlist[0][1]) != 2) {
            Warning("ReadPedestalList()", "Format problem in line \n%s",
                    line.Data());
         }

         line.ReadLine(fin);

         sscanf(line.Data(), "%s", filename_chio);

         line.ReadLine(fin);

         sscanf(line.Data(), "%s", filename_csi);

         TList RRList;
         KVDBRecord *dummy = 0;
         dummy =
             new KVDBRecord(filename_chio, "ChIo/Si/Etalons pedestals");
         dummy->AddKey("Runs", "Runs for which to use this pedestal file");
         fPedestals->AddRecord(dummy);
         RRList.Add(dummy);
         dummy = new KVDBRecord(filename_csi, "CsI pedestals");
         dummy->AddKey("Runs", "Runs for which to use this pedestal file");
         fPedestals->AddRecord(dummy);
         RRList.Add(dummy);
         LinkListToRunRanges(&RRList, 1, runlist);
      }                         // balise trouvee
   }                            // lecture du fichier
   fin.close();
   cout << "Pedestals Read" << endl;
}

//_____________________________________________________________________

void KVINDRADB5::LinkRecordToRunRange(KVDBRecord * rec, UInt_t first_run,
                                      UInt_t last_run)
{
   //If the KVDBRecord 'rec' (i.e. set of calibration parameters, reaction system, etc.) is
   //associated to, or valid for, a range of runs, we use this method in order to link the record
   //and the runs. The list of associated runs will be kept with the record, and each of the runs
   //will have a link to the record.
   //Redefined in order to handle the duplicated run 8449.
   // For the case of the duplicated run, if it's the first run of the run range
   // then only run "b" will be considered.
   // if it's the last run, "a" will be considered
   // if it's in the run range both will be considered

   KVDBRun *run;
   for (UInt_t i = first_run; i <= last_run; i++) {
      if (i == kDuplicatedRun) {
         if (i == first_run)
            run = GetRun(Form("Run %db", kDuplicatedRun));
         else if (i == last_run)
            run = GetRun(Form("Run %da", kDuplicatedRun));
         else {
            run = GetRun(Form("Run %da", kDuplicatedRun));
            if (run)
               rec->AddLink("Runs", run);
            run = GetRun(Form("Run %db", kDuplicatedRun));
         }
      } else
         run = GetRun(i);

      if (run)
         rec->AddLink("Runs", run);
   }
}

void KVINDRADB5::LinkListToRunRanges(TList * list, UInt_t rr_number,
                                     UInt_t run_ranges[][2])
{
   //Link the records contained in the list to the set of runs (see LinkRecordToRunRanges).
   //Redefined in order to handle the duplicated run 8449.
   // For the case of the duplicated run, if it's the first run of the run range
   // then only run "b" will be considered.
   // if it's the last run, "a" will be considered
   // if it's in the run range both will be considered

   if (!list) {
      Error("LinkListToRunRanges",
            "NULL pointer passed for parameter TList");
      return;
   }
   if (list->GetSize() == 0) {
      Error("LinkListToRunRanges(TList*,UInt_t,UInt_t*)",
            "The list is empty");
      return;
   }
   TIter next(list);
   KVDBRecord *rec;
   for (UInt_t ru_ra = 0; ru_ra < rr_number; ru_ra++) {
      UInt_t first_run = run_ranges[ru_ra][0];
      UInt_t last_run = run_ranges[ru_ra][1];
      for (UInt_t i = first_run; i <= last_run; i++) {
         KVDBRun *run;
         if (i == kDuplicatedRun) {
            if (i == first_run)
               run = GetRun(Form("Run %db", kDuplicatedRun));
            else if (i == last_run)
               run = GetRun(Form("Run %da", kDuplicatedRun));
            else {
               run = GetRun(Form("Run %da", kDuplicatedRun));
               while ((rec = (KVDBRecord *) next())) {
                  if (run)
                     rec->AddLink("Runs", run);
               }
               next.Reset();
               run = GetRun(Form("Run %db", kDuplicatedRun));
            }
         } else
            run = GetRun(i);
         while ((rec = (KVDBRecord *) next())) {
            if (run)
               rec->AddLink("Runs", run);
         }
         next.Reset();
      }
   }
}

//_____________________________________________________________________________

void KVINDRADB5::ReadCalibCsI()
{
   //Read CsI Light-Energy calibrations for Z=1 and Z>1
   //The parameter filenames are taken from the environment variables
   //        INDRA_camp5.INDRADB.CalibCsI.Z=1
   //        INDRA_camp5.INDRADB.CalibCsI.Z>1  
   //These calibrations are valid for all runs
   ReadLightEnergyCsI("Z=1", fLitEnerCsIZ1);
   ReadLightEnergyCsI("Z>1", fLitEnerCsI);
}
//_____________________________________________________________________________

void KVINDRADB5::ReadLightEnergyCsI(const Char_t* zrange, KVDBTable* table)
{
   //Read CsI Light-Energy calibrations for Z=1 (zrange="Z=1") or Z>1 (zrange="Z>1")
   //and add them to the KVDBTable whose pointer is given as 2nd argument.
   //These calibrations are valid for all runs

   //need description of INDRA geometry
   if (!gIndra) {
      KVMultiDetArray::MakeMultiDetector(fDataSet.Data());
   }
   //gIndra exists, but has it been built ?
   if (!gIndra->IsBuilt())
      gIndra->Build();

   ifstream fin;
   TString filename; filename.Form("CalibCsI.%s", zrange);
   if (!OpenCalibFile( filename.Data() , fin)) {
      Error("ReadLightEnergyCsI()", "Could not open file %s",
            GetCalibFileName( filename.Data() ));
      return;
   }
   Info("ReadLightEnergyCsI()",
        "Reading Light-Energy CsI calibration parameters (%s)...", zrange);

   TString sline;

   KVDBParameterSet *parset;
   TList *par_list = new TList();

   Float_t a2, a1, a3, chi2;    // calibration parameters
   Int_t ring, mod, npoints;

   while (fin.good()) {         //reading the file
      sline.ReadLine(fin);
      if (fin.good()) {
         if (!sline.BeginsWith("#")) {  //skip comments
            if (sscanf
                (sline.Data(), "%d %d %f %f %f %f %d", &ring, &mod, &a1,
                 &a2, &a3, &chi2, &npoints) != 7) {
               Warning("ReadLightEnergyCsI()",
                       "Bad format in line :\n%s\nUnable to read parameters",
                       sline.Data());
               return;
            } else {            //parameters correctly read
               KVCsI *csi =
                   (KVCsI *) gIndra->GetDetectorByType(ring, mod, CsI_R);
               if (!csi) {
                  Warning("ReadLightEnergyCsI()", "Cant find CsI %d.%d",
                          ring, mod);
                  return;
               }
               parset =
                   new KVDBParameterSet(csi->GetName(), Form("Light-Energy CsI %s", zrange),
                                        3);
               parset->SetParameters(a1, a2, a3);
               table->AddRecord(parset);
               par_list->Add(parset);
            }                   //parameters correctly read
         }                      //data line
      }                         //if(fin.good
   }                            //reading the file
   fin.close();

   //these calibrators are valid for all runs
   UInt_t nranges, r_ranges[MAX_NUM_RUN_RANGES][2];
   nranges = 1;
   r_ranges[0][0] = kFirstRun;
   r_ranges[0][1] = kLastRun;
   LinkListToRunRanges(par_list, nranges, r_ranges);
   par_list->Clear();
   delete par_list;
}

//_____________________________________________________________________________
