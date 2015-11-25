//Created by KVClassFactory on Mon Sep 17 17:49:41 2012
//Author: Ademard Guilain
// Updated by A. Chbihi on 20/08/2009
// In addition to set the VAMOS scalers in the data base using ReadVamosScalers ()  method


#include "KVIVDB.h"
#include "KVDBParameterSet.h"

ClassImp(KVIVDB)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
   <h2>KVIVDB</h2>
   <h4>Database for e494s experiment coupling INDRA with VAMOS</h4>
   <!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIVDB::KVIVDB()
{
   // Default constructor
   init();
}
//________________________________________________________________

KVIVDB::KVIVDB(const Char_t* name) : KVINDRADB(name)
{
   init();
}
//________________________________________________________________

KVIVDB::~KVIVDB()
{
   // Destructor
}
//________________________________________________________________

void KVIVDB::init()
{
   // Default initialisations
   fDeltaPed = AddTable("DeltaPedestal", "Pedestal correction value of detectors");
   fVAMOSCalConf = AddTable("VAMOS calib. & conf.", "Calibration and configuration parameters for VAMOS and its detectors");

}
//________________________________________________________________

void KVIVDB::Build()
{
   // Read runlist and systems list, then read VAMOS scalers
   // Read calibration and configuration (Brho, angle, ...) parameters
   // of VAMOS for each run.

   KVINDRADB::Build();
   ReadPedestalCorrection();
   ReadVamosScalers();
   ReadVamosCalibrations();
}
//________________________________________________________________

void KVIVDB::ReadDeltaPedestal(ifstream& ifile)
{
   // Reading the pedestal correction (DeltaPed). Method called by
   // KVINDRADB_e503::ReadPedestalCorrection(). DeltaPed is equal to
   // DeltaNoise for high gain coder data. For anyother coder data,
   // DetaPed is given by DeltaGene if DeltaGene is less than 20 channels,
   // otherwise it is given by DeltaNoise. DeltaGene (DeltaNoise) is the
   // difference between pedestal positions from the generator (noise)
   // of the current run and of the reference run.


   KVString sline, signal, parname;
   KVDBParameterSet* parset = NULL;
   Int_t numQDC = -1;
   KVNumberList runs;
   Double_t Dped = -1., Dgene = -1., Dbruit = -1.;

   while (ifile.good()) {         //reading the file
      sline.ReadLine(ifile);
      // Skip comment line
      if (sline.BeginsWith("#")) continue;
      // End character
      if (sline.BeginsWith("!")) return;

      // QDC number
      if (sline.BeginsWith("QDC=")) {
         sline.Remove(0, 4);
         numQDC = sline.Atof();
         continue;
      }
      // DeltaPed for each signal
      Ssiz_t idx = sline.Index(":");
      runs.SetList(TString(sline).Remove(idx));
      sline.Remove(0, idx + 1);
      sline.Begin(" ");
      while (!sline.End()) {
         signal = sline.Next(kTRUE);
         Dgene  = sline.Next(kTRUE).Atof();
         Dbruit = sline.Next(kTRUE).Atof();

         // Marie-France's selection
         Dped = ((Dgene > 20) || !strcmp(signal.Data(), "GG") ? Dbruit : Dgene);
         if (Dped > 20) Warning("ReadDeltaPedestal", "DeltaPed>20 for runs %s, signal %s", runs.GetList(), signal.Data());

         parname.Form("QDC%.2d", numQDC);
         parset = new KVDBParameterSet(parname.Data(), signal.Data(), 1);
         parset->SetParameter(Dped);
         fDeltaPed->AddRecord(parset);
         LinkRecordToRunRange(parset, runs);
      }
   }
}
//________________________________________________________________

void KVIVDB::ReadPedestalCorrection()
{
   // Reading the pedestal correction (DeltaPed). DeltaPed depends on
   // the run number but it is the same for all detectors connected to the
   // same QDC. The corrected pedestal is given by the reference
   // pedestal + DeltaPed.
   ifstream fin;

   if (!OpenCalibFile("PedestalCorrections", fin)) {
      Warning("ReadPedestalCorrection", "Pedestal correction file not found : %s",
              GetCalibFileName("PedestalCorrections"));
      return;
   }
   Info("ReadPedestalCorrection", "Reading in pedestal correction file : %s",
        GetCalibFileName("PedestalCorrections"));


   TString sline;
   while (fin.good()) {         //reading the file
      sline.ReadLine(fin);
      // Skip comment line
      if (sline.BeginsWith("#")) continue;

      if (sline.BeginsWith("+DeltaPed")) ReadDeltaPedestal(fin);
   }
   fin.close();
}
//________________________________________________________________

void KVIVDB::ReadPedestalList()
{
   //Read the names of pedestal files to use for each run range, found
   //in file with name defined by the environment variable:
   //   [dataset name].INDRADB.Pedestals:    ...

   ifstream fin;
   if (!OpenCalibFile("Pedestals", fin)) {
      Error("ReadPedestalList()", "Could not open file %s",
            GetCalibFileName("Pedestals"));
      return;
   }
   Info("ReadPedestalList()", "Reading pedestal file list...");

   KVString line;
   Char_t filename_chio[80], filename_csi[80], filename_etalons[80];
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

         line.ReadLine(fin);
         sscanf(line.Data(), "%s", filename_etalons);

         TList RRList;
         KVDBRecord* dummy = 0;
         dummy = new KVDBRecord(filename_chio, "ChIo/Si pedestals");
         dummy->AddKey("Runs", "Runs for which to use this pedestal file");
         fPedestals->AddRecord(dummy);
         RRList.Add(dummy);
         dummy = new KVDBRecord(filename_csi, "CsI pedestals");
         dummy->AddKey("Runs", "Runs for which to use this pedestal file");
         fPedestals->AddRecord(dummy);
         RRList.Add(dummy);
         dummy = new KVDBRecord(filename_etalons, "Si75/SiLi pedestals");
         dummy->AddKey("Runs", "Runs for which to use this pedestal file");
         fPedestals->AddRecord(dummy);
         RRList.Add(dummy);
         LinkListToRunRanges(&RRList, 1, runlist);
      }                         // balise trouvee
   }                            // lecture du fichier
   fin.close();
   cout << "Pedestals Read" << endl;
}
//________________________________________________________________

Bool_t KVIVDB::ReadVamosCalibFile(ifstream& ifile)
{
   // Reads the calibration file loaded in 'infile'.
   // Retruns kTRUE if the mimimum of information is
   // given inside the file (Run list, type of calibration,
   // function of calibration, acquisition parameter
   // associated to the calibration parameters... ).
   // The format is the following:
   //
   // RUNS: 1-30 34 50-200
   // TYPE: channel->Volt  (or channel->ns, Volt->MeV)
   // FUNCTION: pol1       (or formula format of TF1)
   //
   // these tree infromations are the minimum to give in the file.
   //
   // If the FUNCTION is 'no'( 'NO' or empty ) then it is assume that
   // the calibrator is defined by a class inheriting from KVCalibrator
   // different from KVFunctionCal.
   //
   // The following depends of the calibration type:
   // - If the type contains 'channel' then the calibration
   //   parameters have to be associated to an acquisition
   //   parameter. In this case the name of the acquisition
   //   parameter has to be given first followed by ':' and the parameters.
   // - else the  calibration parameters have to be
   //   associated to the corresponding detector.
   //
   // The parameters can be add on a new line if the previous line end
   // with the character '\'
   //
   // The comment line begins with '#'.
   //
   // You can change when you want the run list, or the
   // function. The example below illustrates these
   // possibilities:
   //
   // RUNS: 1-30 34 50-200
   // TYPE: channel->Volt
   // FUNCTION: pol1
   // SED1_X_001: par0 par1
   // SED1_X_002: par0 par1
   // #
   // FUNCTION: [0]+[1]*x+[2]/x
   // SED1_X_003: par0 par1 par2
   // ...
   // ...
   // RUNS: 201 203 207-266
   // SED1_X_001: par0 par1 par2
   // SED1_X_002: par0 par1 par3
   // ...
   // ...
   // RUNS: 1-266
   // FUNCTION: pol6
   // TYPE: channel->ns
   // SED1_HF: par0 par1 par2 par3 par4 par5 par6
   //
   // TYPE: position->cm
   // FUNCTION: no
   // SED1: par1 par2 par3 par4 par5 \.
   //       par6 par7 par8 par8 par10
   //
   //
   // It is possible to set a parameter for a detector (or for VAMOS) if
   // a Setter method exists for this parameter in the classe describing
   // the detector (or VAMOS). In this case the name of the parameter has
   // to be given in TYPE and FUNCTION has to be set to 'no' or null. For
   // example to set the reference Brho 0.2 T.m to VAMOS for run 1 to 100
   // write:
   //
   // RUNS: 1-100
   // TYPE: BrhoRef
   // FUNCTION: no
   // VAMOS: 0.2


   KVString sline, name;
   Int_t idx;
   Int_t OKbit = 0;
   KVDBParameterSet* parset = NULL;
   KVNumberList runs;
   TObjArray* tok = NULL;
   KVString infoNames[3] = {"RUNS", "TYPE", "FUNCTION"};
   KVString infos[3];

   while (ifile.good()) {         //reading the file
      sline.ReadLine(ifile);

      if (sline.IsNull()) continue;
      // Skip comment line
      if (sline.BeginsWith("#")) continue;

      // Mandatory character ':'
      if ((idx = sline.Index(":")) < 0) continue;

      name = sline;
      name.Remove(idx);
      name.Remove(KVString::kBoth, ' ');
      sline.Remove(0, idx + 1);
      sline.Remove(KVString::kBoth, ' ');

      Int_t i;
      for (i = 0; i < 3; i++)
         if (name == infoNames[i]) break;

      // if line contains an info  ( RUNS, TYPE or FUNCTION )
      if (i < 3) {
         infos[i] = sline;
         OKbit = OKbit | (1 << i);
         if (i == 0) runs.SetList(sline);
      } else if (OKbit != 7) {
         // OKbit different from 7 means RUNS, TYPE or FUNCTION are missing
         for (Int_t j = 0; j < 3; j++) {
            if (!(OKbit & (1 << j)))
               Error("ReadVamosCalibFile", "Missing %s for the calibration of %s", infoNames[j].Data(), name.Data());

         }
         return kFALSE;
      } else {

         if (sline.EndsWith("\\")) {
            sline.Remove(KVString::kTrailing, '\\');
            sline.Remove(KVString::kTrailing, ' ');
            KVString extra;
            Bool_t next = kTRUE;
            while (next) {
               extra.ReadLine(ifile);
               if (extra.BeginsWith("#")) continue;
               if (extra.EndsWith("\\")) extra.Remove(KVString::kTrailing, '\\');
               else  next = kFALSE;
               extra.Remove(KVString::kBoth, ' ');
               sline.Append(" ");
               sline.Append(extra);
            }
         }

         tok = sline.Tokenize(" ");

         if (infos[2].CompareTo("no") && infos[2].CompareTo("NO") && !(infos[2].IsNull())) {
            // for calibrator with formula expression
            sline.Form("f%s", name.Data());
            TF1 f(sline.Data(), infos[2].Data());
            if (tok->GetEntries() != f.GetNpar()) {
               cout << "info 2: " << infos[2].Data() << endl;
               Error("ReadVamosCalibFile", "Different numbers of parameters:");
               cout << "- the function " << f.GetExpFormula().Data() << " ( " << f.GetNpar() << " parameters )" << endl;
               cout << "- the calibrator of " << name.Data() << " ( " << tok->GetEntries() << " parameters )" << endl;
               delete tok;
               return kFALSE;
            }
            TString pname(infos[1]);
            pname.Append(" f(x)=");
            pname.Append(f.GetExpFormula());
            parset = new KVDBParameterSet(name.Data(), pname.Data(), f.GetNpar() + 1);
            parset->SetParamName(0, "Npar");
            parset->SetParameter(0, f.GetNpar());
            for (Int_t j = 0; j < f.GetNpar(); j++) {
//             parset->SetParamName( j+1, f.GetParName(j));
//             parset->SetParameter( j+1, ((TObjString *)tok->At(j))->GetString().Atof() );
               parset->SetParamName(j + 1, ((TObjString*)tok->At(j))->GetString().Data());
            }
         } else {
            // for calibrator without formula expression
            parset = new KVDBParameterSet(name.Data(), infos[1].Data(), tok->GetEntries() + 1);
            parset->SetParamName(0, "Npar");
            parset->SetParameter(0, tok->GetEntries());
            for (Int_t j = 0; j < tok->GetEntries() ; j++) {
//             parset->SetParamName( j+1, Form("par%d",j) );
//             parset->SetParameter( j+1, ((TObjString *)tok->At(j))->GetString().Atof() );
               parset->SetParamName(j + 1, ((TObjString*)tok->At(j))->GetString().Data());
            }
         }
         delete tok;
         fVAMOSCalConf->AddRecord(parset);
         LinkRecordToRunRange(parset, runs);

//       cout<<parset->GetName()<<" "<<parset->GetTitle()<<" "<<runs.GetList()<<" "<<parset->GetParamName(0);
//       for( Int_t j=0; j< parset->GetParamNumber(); j++ )
//          cout<<" "<<parset->GetParameter(j);
//          cout<<endl;
      }
   }

   return kTRUE;
}
//________________________________________________________________

void KVIVDB::ReadVamosCalibrations()
{
   // All the calibration files of VAMOS have the same structure.
   // This method look for the global file where the list of calibration
   // files are written.
   // This global file is given in the  environment variable
   // 'VamosCalibFile' (see OpenCalibFile(...)).
   //

   ifstream inf;
   if (!OpenCalibFile("VamosCalibFile", inf)) {
      Error("ReadVamosCalibrations", "Could not open file %s", GetCalibFileName("VamosCalibFile"));
      return;
   }

   Info("ReadVamosCalibrations", "Reading calibration parameters of VAMOS...");

   TString sline;
   while (!inf.eof()) {
      sline.ReadLine(inf);
      if (sline.IsNull()) continue;

      // Skip comment line
      if (sline.BeginsWith("#")) continue;

      ifstream inf2;
      if (!gDataSet->OpenDataSetFile(sline.Data(), inf2)) {
         Error("ReadVamosCalibrations", "Could not open file %s", sline.Data());
         inf2.close();
         continue;
      }

      Info("ReadVamosCalibrations", "in file %s", sline.Data());
      if (!ReadVamosCalibFile(inf2)) Error("ReadVamosCalibrations", "Bad structure inside the file %s", sline.Data());

      inf2.close();
   }
   inf.close();
}
//________________________________________________________________

void KVIVDB::ReadVamosScalers()
{
   ifstream fin;

   if (!OpenCalibFile("VamosScaler", fin)) {
      Warning("ReadVamosScalers", "VAMOS scalers file not found : %s",
              GetCalibFileName("VamosScaler"));
      return;
   }
   Info("ReadVamosScalers", "Reading in Vamos Scaler file : %s",
        GetCalibFileName("VamosScaler"));

   Float_t  NormScaler = 0;
   Float_t DT = 0;
   Float_t NormVamos = 0;
   TString sline;
   Int_t irun = 0;
   Int_t ScalerValue;
   Char_t Crunnumber[40];
   Char_t str[30];
   while (fin.good()) {         //reading the file
      sline.ReadLine(fin);
      if (!fin.eof()) {          //fin du fichier
         if (sline.Sizeof() > 1 && !sline.BeginsWith("#")) {
            if (sline.BeginsWith("ScalersOfRunNumber")) {
               sscanf(sline.Data(), "%30s %d ", Crunnumber, &irun);
               KVINDRADBRun* idb = GetRun(irun);
               if (idb) {
                  for (Int_t i = 0; i < 5; i++) {  //retrouve dans les 4 prochaines lignes NormVamos,normSca etDT
                     sline.ReadLine(fin);

                     if (sline.BeginsWith("NormVamos")) {
                        sscanf(sline.Data(), "%20s %f ", str, &NormVamos);
                        idb->Set(str, NormVamos);
                     }
                     if (sline.BeginsWith("NormScalers")) {
                        sscanf(sline.Data(), "%20s %f ", str, &NormScaler);
                        idb->Set(str, NormScaler);
                     }
                     if (sline.BeginsWith("DT")) {
                        sscanf(sline.Data(), "%20s %f ", str, &DT);
                        idb->Set(str, DT);
                     }

                  }
                  for (Int_t i = 0; i < 40; i++) {
                     sline.ReadLine(fin);
                     sscanf(sline.Data(), "%s %d ", str, &ScalerValue);
                     idb->SetScaler(str, ScalerValue);
                  }

               }
            }
         }
      }
   }
   fin.close();
}
