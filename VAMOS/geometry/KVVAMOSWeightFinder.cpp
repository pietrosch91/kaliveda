//Created by KVClassFactory on Thu Feb  9 00:40:28 2017
//Author: Quentin Fable,,,

#include "KVVAMOSWeightFinder.h"
#include "KVDataSet.h"

ClassImp(KVVAMOSWeightFinder)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSWeightFinder</h2>
<h4>Class used to estimate normalisation weights with VAMOS events</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVVAMOSWeightFinder::KVVAMOSWeightFinder(KVNumberList& nl) : KVBase("VAMOSWeightFinder", "Normalisation weight estimator of VAMOS")
{
   // Default constructor
   fRunList = nl;
   ReadRunListInDataSet();
   SortInfoVector();
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

   //The runlist must be set before doing anything
   if (!fRunList) {
      Warning("ReadRunListInDataSet", "runlist is empty, will do nothing");
      return;
   }

   //Find the file to use
   TString filename = gDataSet->GetDataSetEnv("KVVAMOSWeightFinder.RunListFile");
   if (filename == "") {
      Warning("ReadRunListInDataSet", "No filename defined. Should be given by %s.KVVAMOSWeightFinder.RunListFile", gDataSet->GetName());
      return;
   }
   std::ifstream ifile;
   if (gDataSet->OpenDataSetFile(filename.Data(), ifile)) {
      Info("ReadRunListInDataSet", "run list informations in %s", filename.Data());
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


Bool_t KVVAMOSWeightFinder::SortVectors(const std::vector<Float_t>& vector1, const std::vector<Float_t>& vector2)
{
   //Loop over vectors delta, theta and phi values, sort them
   //in ascending order first by delta, then theta, then phi.

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

Float_t KVVAMOSWeightFinder::GetInfoValue(Int_t run_number, Int_t num_value)
{
   //Returns the value of the given information for the given run.
   Float_t val = -666.;
   Int_t pos = GetRunPosition(run_number);

   if (pos > 0) val = GetInfoLine(pos).at(num_value);

   return val;
}
