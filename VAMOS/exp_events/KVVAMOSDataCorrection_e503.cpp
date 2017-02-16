//Created by KVClassFactory on Mon Feb 13 18:45:07 2017
//Author: Quentin Fable,,,

#include "KVVAMOSDataCorrection_e503.h"
#include "KVIDTelescope.h"
#include "KVIDHarpeeICSi_e503.h"
#include "KVIDHarpeeSiCsI_e503.h"
#include "KVParticle.h"
#include "KVNucleus.h"
#include "TCutG.h"
#include "TKey.h"

ClassImp(KVVAMOSDataCorrection_e503)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSDataCorrection_e503</h2>
<h4>Base class to use for VAMOS data corrections for e503 experiment</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVVAMOSDataCorrection_e503::KVVAMOSDataCorrection_e503()
{
   // Default constructor
   fkverbose      = kTRUE;
   fkInitialised  = kFALSE;
   fRunNumber     = -1;

   flist_aoq_cut_sicsi = new KVHashList;
   flist_aoq_cut_sicsi->SetOwner(kTRUE);
   flist_aoq_cut_icsi  = new KVHashList;
   flist_aoq_cut_icsi->SetOwner(kTRUE);
}

//____________________________________________________________________________//
KVVAMOSDataCorrection_e503::~KVVAMOSDataCorrection_e503()
{
#if __cplusplus < 201103L
   if (flist_aoq_cut_sicsi) {
      delete flist_aoq_cut_sicsi;
      flist_aoq_cut_sicsi = NULL;
   }

   if (flist_aoq_cut_icsi) {
      delete flist_aoq_cut_icsi;
      flist_aoq_cut_icsi = NULL;
   }
#endif
}

//____________________________________________________________________________//
void KVVAMOSDataCorrection_e503::Copy(TObject& obj) const
{
   // Call parent class copy method (takes care of the inherited stuff)
   KVVAMOSDataCorrection::Copy(obj);

   // Then copy any desired member variables (not copied by the parent method)
   //KVVAMOSDataCorrection_e503 *casted_obj = static_cast<KVVAMOSDataCorrection_e503*>(&obj);
}

//____________________________________________________________________________//
void KVVAMOSDataCorrection_e503::Init()
{
   //Initialisation of the correction class:
   //-Creates the cuts from which the data should be modified
   //-Finds ToF corrections to apply to mass:charge ratios
   // in the above defined cuts

   //if already init
   if (fkInitialised) {
      Info("Init", "... correction class already initialised...");
      return;
   }

   //if not already init
   else {
      if (fRunNumber != -1) {
         Info("Init", "... starting initiliasing correction class  for run %d ...", fRunNumber);

         //Si-CsI duplication correction
         ftof_corr_sicsi = -666.;
         ReadDuplicationSiCsICutFileListInDataSet();

         //IC-Si duplication correction
         ftof_corr_icsi = -666.;
         ReadDuplicationICSiCutFileListInDataSet();


         Info("Init", "... initiliasing of correction class done ...");
         fkInitialised = kTRUE;

         //debug
         if (fkverbose) {
            Info("Init", "... printing ToF corrections then cuts lists to be applied ...");
            printf("[tof_sicsi=%e ; tof_icsi=%e]\n", ftof_corr_sicsi, ftof_corr_icsi);
            printf("list of Si-CsI cuts follows:\n");
            flist_aoq_cut_sicsi->ls();
            flist_aoq_cut_sicsi->Print();
            printf("list of IC-Si cuts follows:\n");
            flist_aoq_cut_icsi->ls();
            flist_aoq_cut_icsi->Print();
         }
      }

      else Error("Init", "... no run number set , will do nothing ...");
   }
}

//____________________________________________________________________________//
void KVVAMOSDataCorrection_e503::ReadDuplicationSiCsICutFileListInDataSet()
{
   // Read the file containing TCutG to use
   // for e503 duplication corrections from
   // a file set in the dataset directory:
   // $KVROOT/KVFiles/INDRA_e503
   //
   // This file, specific to a given dataset, is found and
   // opened from its name given by the following environment
   // variable defined in $KVROOT/KVFiles/.kvrootrc:
   //
   // INDRA_e503.KVVAMOSDataCorrection_e503.DuplicationCutListSiCsI:  [file_name]
   //
   // This file contains a list of *.root file names, each
   // of them associated to a system/range of runs.
   // Each of these *.root files contains more specificly
   // TCutG objects (in A/Q,Z) and a TEnv where is saved the run range
   // and the ToF correction (in ns) to apply to the VAMOS nucleus with
   // (A/Q, Z) inside the former cuts.

   //Find the list file to use
   TString filename = gDataSet->GetDataSetEnv("KVVAMOSDataCorrection_e503.DuplicationCutListSiCsI");
   if (filename == "") {
      Warning("ReadDuplicationSiCsICutFileListInDataSet", "No filename defined. It can be defined by %s.KVVAMOSDataCorrection_e503.DuplicationCutListSiCsI", gDataSet->GetName());
      return;
   }
   std::ifstream ifile;
   if (gDataSet->OpenDataSetFile(filename.Data(), ifile)) {
      Info("ReadDuplicationSiCsICutFileListInDataSet", "list of cut files in:  '%s' ", filename.Data());
      ReadDuplicationCutFileList(ifile, 0);
      ifile.close();
   }
}

//____________________________________________________________________________//
void KVVAMOSDataCorrection_e503::ReadDuplicationICSiCutFileListInDataSet()
{
   // Read the file containing TCutG to use
   // for e503 duplication corrections from
   // a file set in the dataset directory:
   // $KVROOT/KVFiles/INDRA_e503
   //
   // This file, specific to a given dataset, is found and
   // opened from its name given by the following environment
   // variable defined in $KVROOT/KVFiles/.kvrootrc:
   //
   // INDRA_e503.KVVAMOSDataCorrection_e503.DuplicationCutListICSi:  [file_name]
   //
   // This file contains a list of *.root file names, each
   // of them associated to a system/range of runs.
   // Each of these *.root files contains more specificly
   // TCutG objects (in A/Q,Z) and a TEnv where is saved the run range
   // and the ToF correction (in ns) to apply to the VAMOS nucleus with
   // (A/Q, Z) inside the former cuts.

   //Find the list file to use
   TString filename = gDataSet->GetDataSetEnv("KVVAMOSDataCorrection_e503.DuplicationCutListICSi");
   if (filename == "") {
      Warning("ReadDuplicationICSiCutFileListInDataSet", "No filename defined. It can be defined by %s.KVVAMOSDataCorrection_e503.DuplicationCutListICSi", gDataSet->GetName());
      return;
   }
   std::ifstream ifile;
   if (gDataSet->OpenDataSetFile(filename.Data(), ifile)) {
      Info("ReadDuplicationICSiCutFileListInDataSet", "list of cut files in:  '%s' ", filename.Data());
      ReadDuplicationCutFileList(ifile, 1);
      ifile.close();
   }
}

//____________________________________________________________________________//

void KVVAMOSDataCorrection_e503::ReadDuplicationCutFileList(std::ifstream& file, Int_t type)
{
   //Protected method to read the duplication cuts list file for the dataset e503
   //Set 'type' to 0 for SiCsI telescopes and set 'type' to 1 for ICSi telescopes
   //For each file listed in this list file we:
   //- read the file TEnv and to access to the run range of the file
   //- if 'fRunNumber' is in the run range we save all associated cuts in
   //  'flist_aoq_cut_sicsi' list and stop the research.
   //- if 'fRunNumber' is in the run range we also save the value of ToF correction

   Bool_t found = kFALSE; //flag set to kTRUE as soon as we have found the good file

   std::string newline;
   while ((std::getline(file, newline)) && (!found)) {
      //Ignore comments in the file
      if (newline.compare(0, 1, "#") != 0) {

         TString fullpath;
         TFile* file = NULL;
         if (gDataSet->SearchKVFile(newline.c_str(), fullpath, gDataSet->GetName())) {
            file = TFile::Open(fullpath.Data());
            if (file) {

               if (fkverbose) {
                  Info("ReadDuplicationCutFileList", "... file %s opened, infos follow ...", file->GetName());
                  file->ls();
               }

               //find the TEnv and access to run_range and tof_corr values
               TEnv* env = (TEnv*) file->Get("TEnv");
               assert(env);

               if (fkverbose) {
                  Info("ReadDuplicationCutFileList", "... TEnv infos follow ...");
                  env->Print();
               }

               const Char_t* run_range = env->GetValue("run_range", "");
               if (fkverbose) {
                  Info("ReadDuplicationCutFileList", "... run (=%d) | run range of the file (=%s) ...",
                       fRunNumber, run_range);
               }

               //check if 'fRunNumber' is in run range

               KVNumberList nl(run_range);
               if (nl.Contains(fRunNumber)) {

                  if (fkverbose) {
                     Info("ReadDuplicationCutFileList", "... run (=%d) is in the run range of the file (=%s) ...",
                          fRunNumber, run_range);

                  }

                  //extract and save ToF correction value
                  if (type == 0) {
                     ftof_corr_sicsi = env->GetValue("tof_corr_ns", (Float_t) - 666.);
                     if (fkverbose) Info("ReadDuplicationCutFileList", " ... Si-CsI tof_corr= %f ns ...", ftof_corr_sicsi);
                  }
                  if (type == 1) {
                     ftof_corr_icsi  = env->GetValue("tof_corr_ns", (Float_t) - 666.);
                     if (fkverbose) Info("ReadDuplicationCutFileList", " ... IC-Si tof_corr= %f ns ...", ftof_corr_icsi);
                  }

                  //exctract cuts list
                  TKey* key = NULL;
                  TList* list = file->GetListOfKeys();
                  assert(list);
                  TIter it(list);
                  while ((key = (TKey*) it.Next())) {
                     if ((key->ReadObj())->InheritsFrom("TCutG")) {
                        const TCutG* cutg = (TCutG*) key->ReadObj();
                        TCutG* copy_cut = new TCutG(*cutg);
                        if (type == 0) flist_aoq_cut_sicsi->Add(copy_cut);
                        else if (type == 1) flist_aoq_cut_icsi->Add(copy_cut);
                     }
                  }

                  found = kTRUE;
               }

               file->Close();
            }//end of file is ok
         }//end of SearchKVFile
      }//end of line ok
   }//all lines read
}

//____________________________________________________________________________//
void KVVAMOSDataCorrection_e503::ApplyCorrections(KVVAMOSReconNuc* nuc)
{
   assert(nuc);
   Int_t IDCode = nuc->GetIDCode();

   if (fkverbose) Info("ApplyCorrections", "... trying to apply e503 corrections to nucleus (IDCode=%d) ...", IDCode);

   //mass:charge duplication corrections
   if (IDCode == 3 && flist_aoq_cut_sicsi) {
      KVVAMOSDataCorrection_e503::ApplyAoverQDuplicationCorrections(nuc, flist_aoq_cut_sicsi, ftof_corr_sicsi);
   }
   if (IDCode == 4 && flist_aoq_cut_icsi) {
      KVVAMOSDataCorrection_e503::ApplyAoverQDuplicationCorrections(nuc, flist_aoq_cut_icsi, ftof_corr_icsi);
   }

   return;
}

//____________________________________________________________________________//
Bool_t KVVAMOSDataCorrection_e503::ApplyAoverQDuplicationCorrections(KVVAMOSReconNuc* nuc, KVHashList* aoq_cut, Float_t tof_corr)
{
   //Check if the provided nucleus, identified by either KVIDHarpeeSiCsI_e503 or
   //KVIDHarpeeICSi_e503 telescope, needs to be corrected for mass:charge
   //by comparing its (RealZ, RealAoQ) values to the cuts in 'flist_aoq_cut'.
   //Return kTRUE if correction applied.
   //Return kFALSE if no correction needed.

   assert(nuc);
   assert(aoq_cut);

   if (fkverbose) Info("ApplyAoverQDuplicationCorrections", "... starting AoQ corrections for the nucleus ...");

   Float_t Z_nuc   = nuc->GetRealZ();
   Float_t AoQ_nuc = nuc->GetRealAoverQ();

   TIter next_cut(aoq_cut);
   TCutG* cut = NULL;
   while ((cut = dynamic_cast<TCutG*>(next_cut.Next()))) {
      if (cut->IsInside(AoQ_nuc, Z_nuc)) {//if nuc needs to be corrected
         //Find new value of mass:charge ratio
         Double_t new_AoQ = -666.;
         Double_t brho    = nuc->GetBrho();
         Double_t path    = -666.;
         Double_t tof     = -666.;
         const Char_t* tof_name = "TSI_HF";
         nuc->GetCorrFlightDistanceAndTime(path, tof, tof_name);
         Double_t time  = tof + tof_corr;
         Double_t beta  = path / time / KVParticle::C();
         Double_t gamma = 1.0 / TMath::Sqrt(1. - beta * beta);
         Double_t tmp   = beta * gamma;
         new_AoQ = brho * KVParticle::C() * 10. / tmp / KVNucleus::u();

         //Set new value of mass:charge
         //nuc->SetRealAoverQ(new_AoQ);

         if (fkverbose) Info("ApplyAoverQDuplicationCorrections", "... finishing AoQ corrections [AoQ_old=%e, AoQ_new=%e] ...", AoQ_nuc, new_AoQ);
         return kTRUE;
      }
   }

   if (fkverbose) Info("ApplyAoverQDuplicationCorrections", "... finishing AoQ corrections, did nothing ...");
   return kFALSE; //no correction needed
}

//____________________________________________________________________________//
