//Created by KVClassFactory on Thu Feb  9 00:40:28 2017
//Author: Quentin Fable,,,

#include "KVVAMOSWeightFinder.h"
#include "KVDataSet.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "Riostream.h"
#include "TLeaf.h"

ClassImp(KVVAMOSWeightFinder)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSWeightFinder</h2>
<h4>Class used to estimate normalisation weights with VAMOS events</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVVAMOSWeightFinder::KVVAMOSWeightFinder() : KVBase("VAMOSWeightFinder", "Normalisation weight estimator of VAMOS")
{
   // Default constructor
   fkIsInit = kFALSE;
   fkverbose = kFALSE;

   fchain = new TChain("tree");
}
//____________________________________________________________________________//

KVVAMOSWeightFinder::KVVAMOSWeightFinder(const KVVAMOSWeightFinder& obj) : KVBase("VAMOSWeightFinder", "Normalisation weight estimator of VAMOS")
{
   // Copy constructor
   // Use this constructor to copy an existing object
   obj.Copy(*this);
}

//____________________________________________________________________________//

KVVAMOSWeightFinder::~KVVAMOSWeightFinder()
{
   // Destructor
   fRunList.Clear();
   fvec_infos.clear();
   fvec_TCfiles.clear();

   delete fchain;
   fchain = NULL;
}
//____________________________________________________________________________//

void KVVAMOSWeightFinder::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' KVVAMOSWeightFinder
   // object into 'obj'.

   KVBase::Copy(obj);
   KVVAMOSWeightFinder& CastedObj = (KVVAMOSWeightFinder&)obj;
   CastedObj.fRunList = fRunList;
   CastedObj.fvec_infos = fvec_infos;
   CastedObj.fvec_TCfiles = fvec_TCfiles;
   CastedObj.fchain = fchain;
}
//____________________________________________________________________________//
void KVVAMOSWeightFinder::SetRunList(KVNumberList& nl)
{
   fRunList.Clear();
   fRunList = nl;
}

//____________________________________________________________________________//

Bool_t KVVAMOSWeightFinder::Init()
{
   //Initialise the weight finder.
   //Need to have set the run list correctly first...

   if (!fRunList.IsEmpty()) {
      ReadRunListInDataSet();
      SortInfoVector();
      ReadTransCoefFileListInDataSet();
      if (CheckTransCoefSteps()) fkIsInit = kTRUE;
   } else fkIsInit = kFALSE;

   if (!fkIsInit) Warning("Init", "... runlist is empty, initialisation went wrong ...");

   return fkIsInit;
}
//____________________________________________________________________________//

void KVVAMOSWeightFinder::ReadRunListInDataSet()
{
   // Read the experimental informations listed for the given run list
   // in file set in the dataset directory:
   // $KVROOT/KVFiles/<name_of_dataset>
   //
   // This file, specific to a given dataset, is found and
   // opened from its name given by the following environment
   // variable defined in $KVROOT/KVFiles/.kvrootrc:
   //
   // [dataset_name].KVVAMOSWeightFinder.RunListFile:  [file_name]
   //
   // This file contains a list of: Run, Brho, ThetaVamos, DT, Scalers

   //Find the file to use
   TString filename = gDataSet->GetDataSetEnv("KVVAMOSWeightFinder.RunListFile");
   if (filename == "") {
      Warning("ReadRunListInDataSet", "No filename defined. Should be given by %s.KVVAMOSWeightFinder.RunListFile", gDataSet->GetName());
      return;
   }
   std::ifstream ifile;
   if (gDataSet->OpenDataSetFile(filename.Data(), ifile)) {
      Info("ReadRunListInDataSet", "run list informations in: '%s'", filename.Data());
      ReadInformations(ifile);
      ifile.close();
   }
}
//____________________________________________________________________________//

void KVVAMOSWeightFinder::ReadInformations(std::ifstream& file)
{
   // Read informations in run list file and save them only for
   // the chosen runs in 'nl'.

   Float_t run;
   Float_t brho;
   Float_t vamos_angle;
   Float_t dt;
   Float_t scalers;

   std::string newline;
   while (std::getline(file, newline)) {
      //Ignore comments in the file
      if (newline.compare(0, 1, "#") != 0) {
         sscanf(newline.c_str(), "%f %f %f %f %f", &run, &brho, &vamos_angle, &dt, &scalers);

         if (fRunList.Contains((Int_t) run)) {
            std::vector<Float_t> line;
            line.push_back(run);
            line.push_back(brho);
            line.push_back(vamos_angle);
            line.push_back(dt);
            line.push_back(scalers);
            fvec_infos.push_back(line);
         }
      }
   }
}
//____________________________________________________________________________//

void KVVAMOSWeightFinder::SortInfoVector()
{
   //Sort all info vectors contained in fvec_infos
   //according to their run number in ascending order.
   //It ensures that the position of the run in fRunList
   //is the same than in fvec_infos.

   std::sort(fvec_infos.begin(), fvec_infos.end(), KVVAMOSWeightFinder::SortVectors);

   if (fkverbose) Info("SortLineVector", "... informations saved in ascending order of run ...");
}
//____________________________________________________________________________//

Bool_t KVVAMOSWeightFinder::SortVectors(const std::vector<Float_t>& vector1, const std::vector<Float_t>& vector2)
{
   //Loop over and sort vectors
   //in ascending order of their first value (so run number).

   //different run
   if (vector1.at(0) != vector2.at(0)) {
      if (vector1.at(0) < vector2.at(0)) return kTRUE;
      else return kFALSE;
   }

   return kTRUE;
}
//____________________________________________________________________________//

Int_t KVVAMOSWeightFinder::GetRunPosition(Int_t run_number)
{
   //Returns the position of the run in fvec_infos/fRunList
   Int_t pos = -1;

   Bool_t found = kFALSE;
   Int_t next_pos = 0;
   fRunList.Begin();
   while (!fRunList.End() && !found) {
      Int_t next_val = fRunList.Next();
      if (next_val == run_number) {
         pos = next_pos;
         found = kTRUE;
      }

      next_pos++;
   }

   return pos;
}

//____________________________________________________________________________//
Float_t KVVAMOSWeightFinder::GetInfoValue(Int_t run_number, Int_t num_value)
{
   //Returns the value of the given information for the given run.
   Float_t val = -666.;
   Int_t pos = GetRunPosition(run_number);

   if (pos > 0) val = GetInfoLine(pos).at(num_value);

   return val;
}

//____________________________________________________________________________//
void KVVAMOSWeightFinder::ReadTransCoefFileListInDataSet()
{
   // Read the trans. coef. files from
   // a file set in the dataset directory:
   // $KVROOT/KVFiles/<name_of_dataset>
   //
   // This file, specific to a given dataset, is found and
   // opened from its name given by the following environment
   // variable defined in $KVROOT/KVFiles/.kvrootrc:
   //
   // [dataset_name].KVVAMOSWeightFinder.TransCoefFileList:  [file_name]
   //
   // This file contains a list of trans. coef. *.root files, each
   // of them associated to a ThetaVamos. Each of these *.root files
   // contains more specificly a TTree with trans.coef values and a TEnv
   // of (delta, thetaI) ranges and steps, mandatory to use the TTree.

   //Find the list file to use
   TString filename = gDataSet->GetDataSetEnv("KVVAMOSWeightFinder.TransCoefFileList");
   if (filename == "") {
      Warning("ReadTransCoefFilesInDataSet", "No filename defined. Should be given by %s.KVVAMOSWeightFinder.TransCoefFileList", gDataSet->GetName());
      return;
   }
   std::ifstream ifile;
   if (gDataSet->OpenDataSetFile(filename.Data(), ifile)) {
      Info("ReadTransCoefFilesInDataSet", "trans. coef. list of files in:  '%s' ", filename.Data());
      ReadTransCoefListFile(ifile);
      ifile.close();
   }
}

//____________________________________________________________________________//
void KVVAMOSWeightFinder::ReadTransCoefListFile(std::ifstream& file)
{
   //Read the trans. coef. list file. for the data set
   //For each file listed in this list file we:
   //- save in 'fvec_TCfiles' a pair (VamosAngle, file_name) containing the VamosAngle and its associated file
   //- read the file TEnv and save steps informations in 'fvec_TCsteps'
   //- Add the tree in fchain

   std::string newline;
   while (std::getline(file, newline)) {
      //Ignore comments in the file
      if (newline.compare(0, 1, "#") != 0) {
         TString ss(newline);
         TObjArray* obj_arr = ss.Tokenize(" ");
         obj_arr->ls();
         TString str_va = ((TObjString*) obj_arr->At(0))->GetString();
         TString str_na = ((TObjString*) obj_arr->At(1))->GetString();

         fvec_TCfiles.push_back(std::make_pair(str_va.Atof(), str_na));

         TString fullpath;
         TFile* file = NULL;
         if (gDataSet->SearchKVFile(str_na.Data(), fullpath, gDataSet->GetName())) {
            file = TFile::Open(fullpath.Data());
            if (file) {

               if (fkverbose) {
                  Info("ReadTransCoefListFile", "... file %s opened, infos follow ...", file->GetName());
                  file->ls();
               }

               //find the TEnv and fill the vector with step values
               TEnv* env = (TEnv*) file->Get("TEnv");
               assert(env);
               std::vector<Float_t> line;
               line.push_back(env->GetValue("ThetaVamos_deg", (Float_t) - 1));
               line.push_back(env->GetValue("delta_min", (Float_t) - 1));
               line.push_back(env->GetValue("delta_max", (Float_t) - 1));
               line.push_back(env->GetValue("delta_step", (Float_t) - 1));
               line.push_back(env->GetValue("delta_Nstep", (Float_t) - 1));
               line.push_back(env->GetValue("thetaI_min_rad", (Float_t) - 1));
               line.push_back(env->GetValue("thetaI_max_rad", (Float_t) - 1));
               line.push_back(env->GetValue("thetaI_step_rad", (Float_t) - 1));
               line.push_back(env->GetValue("thetaI_Nstep_rad", (Float_t) - 1));
               line.push_back(env->GetValue("MonteCarlo_stat", (Float_t) - 1));
               fvec_TCsteps.push_back(line);

               //add the TTree in fchain
               fchain->Add(file->GetName());

               //close file
               //file->Close();
            }
         }
      }
   }//all lines read
}

//____________________________________________________________________________//
Bool_t KVVAMOSWeightFinder::CheckTransCoefSteps()
{
   //Internal check of all trans. coef. files steps and infos.
   //Use the steps informations extracted from all trans. coef. files
   //and saved in fvec_TCsteps vector.
   //Check if the steps in delta, the number of values and even the Monte-Carlo statistic
   //used to generate the trans. coef. *.root files are the same for all used
   //files saved in the dataset directory.
   Bool_t ok = kFALSE;

   //just one line
   if (fvec_TCsteps.size() == 1) ok = kTRUE;

   //more than one line
   else {
      for (unsigned i = 0; i < fvec_TCsteps.size() - 1; i++) {
         std::vector<Float_t> line0 = fvec_TCsteps.at(i);
         std::vector<Float_t> line1 = fvec_TCsteps.at(i + 1);

         Int_t nmc0 = (Int_t) line0.at(9);
         Int_t nmc1 = (Int_t) line1.at(9);
         Int_t nd0  = (Int_t) line0.at(4);
         Int_t nd1  = (Int_t) line1.at(4);
         Int_t nt0  = (Int_t) line0.at(8);
         Int_t nt1  = (Int_t) line1.at(8);

         //Ndelta, Ntheta and NMonteCarlo is OK
         if ((nmc0 == nmc1) && (nd0 == nd1) && (nt0 == nt1)) {
            Float_t dmin0 = line0.at(1);
            Float_t dmax0 = line0.at(2);
            Float_t dst0  = line0.at(3);
            Float_t dmin1 = line1.at(1);
            Float_t dmax1 = line1.at(2);
            Float_t dst1  = line1.at(3);
            Float_t epsd  = TMath::Min(dst0, dst1);

            //Delta steps and ranges should be the same
            if ((TMath::Abs(dmin0 - dmin1) <= epsd / 2.) && (TMath::Abs(dmax0 - dmax1) <= epsd / 2.) && (TMath::Abs(dst0 - dst1) <= epsd / 2.)) {
               ok = kTRUE;
            }

            else if (fkverbose) {
               Error("ChecTransCoefSteps", "... error in delta ranges ...\n (dmin0=%f, dmin1=%f) | (dmax0=%f, dmax1=%f) | (dstep0=%f, dstep1=%f)",
                     dmin0, dmin1, dmax0, dmax1, dst0, dst1);
            }
         }

         //Ndelta, Ntheta and NMonteCarlo is not OK
         else if (fkverbose) {
            Error("ChecTransCoefSteps", "... error in Nsteps ...\n (nMC0=%d, nMC1=%d) | (nDelta0=%d, nDelta1=%d) | (nTheta0=%d, nTheta1=%d)",
                  nmc0, nmc1, nd0, nd1, nt0, nt1);
         }
      }
   }

   return ok;
}

//____________________________________________________________________________//
Float_t KVVAMOSWeightFinder::GetTransCoef(Float_t VamosAngle_deg, Float_t delta, Float_t thetaI_rad)
{
   //Find the trans. coef. for a given (ThetaVamos, delta, thetaI) triplet.
   //First find the path to the file coresponding to the given ThetaVamos
   //(precision of 0.1 deg) and containing a TTree of its associated trans. coefs.
   //Then use the associated steps to extract the correct entry number of the
   //couple (delta, thetaI) in the TTree.

   //Find VamosAngle position in fvec_TCfiles
   Int_t num_angle = 0;
   Int_t nn = 0;
   for (std::vector< std::pair<Float_t, TString> >::iterator it = fvec_TCfiles.begin(); it != fvec_TCfiles.end(); ++it) {
      std::pair<Float_t, TString> par = *it;
      Float_t angle = par.first;
      TString path  = par.second;

      if (TMath::Abs(VamosAngle_deg - angle) <= 0.1) {
         num_angle = nn;
         //if(fkverbose){
         Info("GetTransCoef", "... (VamosAngle=%f (deg), delta=%f, thetaI=%f (rad)) -> num_angle=%d ...\n",
              VamosAngle_deg, delta, thetaI_rad, num_angle);
         //}
      }

      nn++;
   }

   //Find, using the associated VamosAngle TEnv saved in fvec_TCsteps:
   //- delta position in its range
   //- thetaI position in its range
   //- total number of delta
   //- total number of thetaI
   //Also check if the VamosAngle is OK.
   std::vector<Float_t> line = fvec_TCsteps.at(num_angle);
   if (TMath::Abs(line.at(0) - VamosAngle_deg) > 0.1) {
      Error("GetTransCoef", "... Found ThetaVamos in steps vector (=%f) is different from given angle (=%f) ...",
            line.at(0), VamosAngle_deg);
      return -666.;
   }

   Int_t ndelta = GetNValue(delta, line.at(1), line.at(2) , line.at(3));
   Int_t ntheta = GetNValue(thetaI_rad, line.at(5), line.at(6), line.at(7));
   //Int_t ntot_delta = line.at(4)+1;
   Int_t ntot_delta = line.at(4);
   Int_t ntot_theta = line.at(8) + 1;

   if (fkverbose) {
      Info("GetTransCoef", "... ndelta=%d, ntheta=%d, ntot_delta=%d, ntot_theta=%d ...",
           ndelta, ntheta, ntot_delta, ntot_theta);
   }

   //Use the found values to exctract the entry in fchain corresponding
   //to the wanted trans. coef.
   if (ndelta > 0 && ntheta > 0 && ntot_delta > 0 && ntot_theta > 0) {
      Long64_t pos_tree = (ndelta * ntot_theta) + ntheta; //entry in the tree
      Long64_t pos_chain = num_angle * (ntot_delta * ntot_theta) + pos_tree; //entry in the chain

      if (fkverbose) Info("GetTransCoef", "... pos_tree=%lli, pos_chain=%lli, nentry_tot_chain=%lli", pos_tree, pos_chain, fchain->GetEntries());

      Float_t tc, dd, tt;
      fchain->SetBranchAddress("TransCoef", &tc);
      fchain->SetBranchAddress("Zdelta", &dd);
      fchain->SetBranchAddress("ZthetaI", &tt);
      fchain->GetEntry(pos_chain, 1);

      //debug
      if (fkverbose) {
         Info("GetTransCoef", "(VamosAngle, delta, thetaI) | input=(%f,%f,%f), entry=(%f,%f), closest=(%f,%f) | pos=%lli",
              VamosAngle_deg, delta, thetaI_rad, dd, tt , GetClosestValue(delta, line.at(3)), GetClosestValue(thetaI_rad, line.at(7)), pos_chain);
      }

      return tc;
   }

   else return -666.;
}

Int_t KVVAMOSWeightFinder::GetNValue(Float_t val_exp, Float_t val_min, Float_t val_max, Float_t val_step)
{
   //Return position of the experimental observable 'val_exp'
   //in its associated range '[val_min, val_max]'
   //and with the step 'val_step'.

   //Get the closest value considering the step
   Float_t cl_val = GetClosestValue(val_exp, val_step);

   //If the closest delta is out of the given range
   if ((cl_val > val_max + val_step / 2.) || (cl_val < val_min - val_step / 2.)) {
      if (fkverbose) {
         if (cl_val > val_max + val_step / 2.) Error("GetNValue", "... closest value (=%f) above val_max+step/2. (=%f) ...", cl_val, val_max + val_step / 2.);
         if (cl_val < val_min - val_step / 2.) Error("GetNValue", "... closest value (=%f) below val_min-step/2. (=%f) ...", cl_val, val_min - val_step / 2.);
      }
      return -1;
   }

   //Closest value is in given range
   else {
      Int_t val_num = TMath::Nint((cl_val - val_min) / val_step);
      return val_num;
   }
}


//____________________________________________________________________________//
void KVVAMOSWeightFinder::PrintRunInfoVector()
{
   //Print the current state of fvec_infos, vector containing
   //'Run   BrhoRef   ThetaVamos   DeadTime   Scalers_INDRA' for each run,
   //and used for weight computation.

   Info("PrintRunInfoVector", "... printing list of available run infos ...\n(Run, BrhoRef ThetaVamos DeadTime Scalers_INDRA)\n\n");

   Int_t num = 0;
   for (std::vector< std::vector<Float_t> >::iterator it = fvec_infos.begin(); it != fvec_infos.end(); ++it) {
      std::vector<Float_t> line = *it;
      printf("[%d] %d %f %f %f %d\n", num, (int) line.at(0), line.at(1), line.at(2), line.at(3), (int) line.at(4));
      num++;
   }
}

//____________________________________________________________________________//
void KVVAMOSWeightFinder::PrintTransCoefInfoVector()
{
   //Print the current state of fvec_TCfiles, vector containing
   //'VamosAngle   TransCoef_file' for each VamosAngle,
   //and used for weight computation.

   Info("PrintTransCoefInfoVector", "... printing list of available transmission coefficients files infos ...\n\n");

   Int_t num = 0;
   for (std::vector< std::pair<Float_t, TString> >::iterator it = fvec_TCfiles.begin(); it != fvec_TCfiles.end(); ++it) {
      std::pair<Float_t, TString> line = *it;
      printf("[%d] %f %s\n", num, line.first, (line.second).Data());
      num++;
   }
}

//____________________________________________________________________________//
void KVVAMOSWeightFinder::PrintTransCoefStepVector()
{
   //Print the current state of fvec_TCsteps, vector containing
   //'VamosAngle_deg   delta_min   delta_max   delta_step   Ndelta   theta_min   theta_max   theta_step  Ntheta',
   //for each VamosAngle and used for weight computation.

   Info("PrintTransCoefStepVector", "... printing list of available trans. coef. infos  ...\n(VamosAngle_deg delta_min delta_max delta_step Ndelta theta_min theta_max theta_step Ntheta)\n\n");

   Int_t num = 0;
   for (std::vector< std::vector<Float_t> >::iterator it = fvec_TCsteps.begin(); it != fvec_TCsteps.end(); ++it) {
      std::vector<Float_t> line = *it;
      printf("[%d] %f %f %f %f %d %f %f %f %d\n",
             num, line.at(0), line.at(1), line.at(2), line.at(3), (int) line.at(4), line.at(5), line.at(6), line.at(7), (int) line.at(8));
      num++;
   }
}
