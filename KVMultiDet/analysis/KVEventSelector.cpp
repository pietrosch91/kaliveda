#define KVEventSelector_cxx
#include "KVEventSelector.h"
#include <TStyle.h>
#include "TPluginManager.h"
#include "TSystem.h"

using namespace std;

ClassImp(KVEventSelector)

void KVEventSelector::Begin(TTree * /*tree*/)
{
   // Analyse comma-separated list of options given (if any) and look for:
   //
   //     BranchName=xxxx  :  change name of branch in TTree containing data
   //     EventsReadInterval=N: print "+++ 12345 events processed +++" every N events

	// parse options given to TTree::Process
   ParseOptions();
	fOptionList.Print();
	// check for branch name
	if(IsOptGiven("BranchName")) SetBranchName(GetOpt("BranchName"));
	// check for events read interval
	if(IsOptGiven("EventsReadInterval")) SetEventsReadInterval(GetOpt("EventsReadInterval").Atoi());
}

void KVEventSelector::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).
}

Bool_t KVEventSelector::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either KVEventSelector::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

   fTreeEntry = entry;

   if (!(fEventsRead % fEventsReadInterval) && fEventsRead) {
      Info("Process", " +++ %lld events processed +++ ", fEventsRead);
      ProcInfo_t pid;
      if(gSystem->GetProcInfo(&pid)==0){
         cout << "     ------------- Process infos -------------" << endl;
         printf(" CpuSys = %f  s.    CpuUser = %f s.    ResMem = %f MB   VirtMem = %f MB\n",
            pid.fCpuSys, pid.fCpuUser, pid.fMemResident/1024., pid.fMemVirtual/1024.);
      }
   }
   GetEntry(entry);
   fEventsRead++;

   KVNucleus* part = 0;
   //apply particle selection criteria
   if (fPartCond) {
      part = 0;
      while ((part = GetEvent()->GetNextParticle("ok"))) {
         part->SetIsOK(fPartCond->Test(part));
      }
   }

   // initialise global variables at first event
   if (fFirstEvent && gvlist) {
      gvlist->Init();
      fFirstEvent = kFALSE;
   }
   RecalculateGlobalVariables();

   Bool_t ok_anal = kTRUE;
   ok_anal = Analysis();     //user analysis

   // Testing whether EndRun() should be called
   if (AtEndOfRun()) {
      Info("Process", "End of file reached after %lld events", fEventsRead);

      EndRun();
      fNotifyCalled = kFALSE;//Notify will be called when next file is opened (in TChain)
   }

   return ok_anal;
}

void KVEventSelector::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void KVEventSelector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

   TDatime now;
   Info("Init", "Analysis ends at %s", now.AsString());
   EndAnalysis();               //user end of analysis routine
}

KVVarGlob *KVEventSelector::AddGV(const Char_t * class_name,
                                  const Char_t * name)
{
   //Add a global variable to the list of variables for the analysis.
   //
   //"class_name" must be the name of a valid class inheriting from KVVarGlob, e.g. any of the default global
   //variable classes defined as part of the standard KaliVeda package (in libKVvVarGlob.so). See
   //"Class Reference" page on website for the available classes (listed by category under "Global Variables: ...").
   //
   //USER-DEFINED GLOBAL VARIABLES
   //The user may use her own global variables in an analysis class, without having to add them to the main libraries.
   //If the given class name is not known, it is assumed to be a user-defined class and we attempt to compile and load
   //the class from the user's source code. For this to work, the user must:
   //
   //      (1) add to the ROOT macro path the directory where her class's source code is kept, e.g. in $HOME/.rootrc
   //              add the following line:
   //
   //              +Unix.*.Root.MacroPath:      $(HOME)/myVarGlobs
   //
   //      (2) for each user-defined class, add a line to $HOME/.kvrootrc to define a "plugin". E.g. for a class called MyNewVarGlob,
   //
   //              +Plugin.KVVarGlob:    MyNewVarGlob    MyNewVarGlob     MyNewVarGlob.cpp+   "MyNewVarGlob()"
   //
   //      It is assumed that MyNewVarGlob.h and MyNewVarGlob.cpp will be found in $HOME/myVarGlobs (in this example).
   //
   //"name" is a unique name for the new global variable object which will be created and added to the internal
   //list of global variables. This name can be used to retrieve the object (see GetGV) in the user's analysis.
   //
   //Returns pointer to new global variable object in case more than the usual default initialisation is necessary.

   KVVarGlob *vg = 0;
   TClass *clas = gROOT->GetClass(class_name);
   if (!clas) {
      //class not in dictionary - user-defined class ? Look for plugin.
      TPluginHandler *ph = KVBase::LoadPlugin("KVVarGlob", class_name);
      if (!ph) {
         //not found
         Error("AddGV(const Char_t*,const Char_t*)",
               "Called with class_name=%s.\nClass is unknown: not in standard libraries, and plugin (user-defined class) not found",
               class_name);
         return 0;
      } else {
         vg = (KVVarGlob *) ph->ExecPlugin(0);
      }
   } else if (!clas->InheritsFrom("KVVarGlob")) {
      Error("AddGV(const Char_t*,const Char_t*)",
            "%s is not a valid class deriving from KVVarGlob.",
            class_name);
      return 0;
   } else {
      vg = (KVVarGlob *) clas->New();
   }
   vg->SetName(name);
   AddGV(vg);
   return vg;
}

//____________________________________________________________________________

void KVEventSelector::RecalculateGlobalVariables()
{
   //Use this method if you change e.g. the particle selection criteria in your
   //Analysis() method and want to recalculate the values of all global variables
   //for your new selection.
   //
   //WARNING: the global variables are calculated automatically for you for each event
   //before method Analysis() is called. In order for the correct particles to be included in
   //this calculation, make sure that at the END of Analysis() you reset the selection
   //criteria.

   if (gvlist) {
      // 1st step: Reset global variables
      gvlist->Reset();

      //2nd step: loop over particles with correct codes
      //          and fill global variables
      KVNucleus *n1 = 0;
      // calculate 1-body variables
      if (gvlist->Has1BodyVariables()) {
         while ((n1 = GetEvent()->GetNextParticle("ok"))) {
            gvlist->Fill(n1);
         }
      }
      KVNucleus *n2 = 0;
      // calculate 2-body variables
      // we use every pair of particles (including identical pairs) in the event
      if (gvlist->Has2BodyVariables()) {
         Int_t N = GetEvent()->GetMult();
         for (int i1 = 1; i1 <= N ; i1++) {
            for (int i2 = 1 ; i2 <= N ; i2++) {
               n1 = GetEvent()->GetParticle(i1);
               n2 = GetEvent()->GetParticle(i2);
               if (n1->IsOK() && n2->IsOK())
                  gvlist->Fill2(n1, n2);
            }
         }
      }
      // calculate N-body variables
      if (gvlist->HasNBodyVariables()) gvlist->FillN(GetEvent());
   }
}

//____________________________________________________________________________

void KVEventSelector::SetParticleConditions(const KVParticleCondition& cond)
{
   //Use this method to set criteria for selecting particles to include in analysis.
   //The criteria defined in the KVParticleCondition object will be applied to every
   //particle and if they are not satisfied the particle's
   //"OK" flag will be set to false, i.e. the particle's IsOK() method will return kFALSE,
   //and the particle will not be included in iterations such as GetEvent()->GetNextParticle("OK").
   //Neither will the particle be included in the evaluation of any global variables.
   //
   //This method must be called in the user's InitAnalysis() or InitRun() method.

   if (!fPartCond) fPartCond = new KVParticleCondition(cond);
   else *fPartCond = cond;
}

//____________________________________________________________________________

KVHashList* KVEventSelector::GetHistoList()
{

   return lhisto;

}

//____________________________________________________________________________

TH1* KVEventSelector::GetHisto(const Char_t* histo_name)
{

   return (TH1*)lhisto->FindObject(histo_name);

}

//____________________________________________________________________________

void KVEventSelector::FillHisto(KVString sname, Double_t one, Double_t two, Double_t three, Double_t four)
{

   //Find in the list, if there is an histogram named "sname"
   //If not print an error message
   //If yes redirect to the right method according to its closest mother class
   //to fill it
   TH1* h1 = 0;
   if ((h1 = GetHisto(sname.Data()))) {
      if (h1->InheritsFrom("TH3"))
         FillTH3((TH3*)h1, one, two, three, four);
      else if (h1->InheritsFrom("TProfile2D"))
         FillTProfile2D((TProfile2D*)h1, one, two, three, four);
      else if (h1->InheritsFrom("TH2"))
         FillTH2((TH2*)h1, one, two, three);
      else if (h1->InheritsFrom("TProfile"))
         FillTProfile((TProfile*)h1, one, two, three);
      else if (h1->InheritsFrom("TH1"))
         FillTH1(h1, one, two);
      else
         Warning("FillHisto", "%s -> Classe non prevue ...", lhisto->FindObject(sname.Data())->ClassName());
   } else {
      Warning("FillHisto", "%s introuvable", sname.Data());
   }

}

//____________________________________________________________________________

void KVEventSelector::FillTH1(TH1* h1, Double_t one, Double_t two)
{

   h1->Fill(one, two);

}

//____________________________________________________________________________

void KVEventSelector::FillTProfile(TProfile* h1, Double_t one, Double_t two, Double_t three)
{

   h1->Fill(one, two, three);

}

//____________________________________________________________________________

void KVEventSelector::FillTH2(TH2* h2, Double_t one, Double_t two, Double_t three)
{

   h2->Fill(one, two, three);

}

//____________________________________________________________________________

void KVEventSelector::FillTProfile2D(TProfile2D* h2, Double_t one, Double_t two, Double_t three, Double_t four)
{

   h2->Fill(one, two, three, four);
}

//____________________________________________________________________________

void KVEventSelector::FillTH3(TH3* h3, Double_t one, Double_t two, Double_t three, Double_t four)
{

   h3->Fill(one, two, three, four);
}


//____________________________________________________________________________

void KVEventSelector::CreateHistos()
{

   Warning("CreateHistos", "To be redefined child class");

}

//____________________________________________________________________________

void KVEventSelector::WriteHistoToFile(KVString filename, Option_t* option)
{

   //If no filename is specified, assume that the current directory is writable
	
   if (filename == "") {
      GetHistoList()->Write();
   } else {
      TFile* file = 0;
      TDirectory* pwd = gDirectory;
      //if filename correspond to an already opened file, write in it
      //if not open/create it, depending on the option ("recreate" by default)
      //and write in it
      if (!(file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename.Data())))
         file = new TFile(filename.Data(), option);
      file->cd();
      GetHistoList()->Write();
      file->Close();
      pwd->cd();
   }

}

//____________________________________________________________________________

KVHashList* KVEventSelector::GetTreeList()
{

   return ltree;

}

//____________________________________________________________________________

TTree* KVEventSelector::GetTree(const Char_t* tree_name)
{

   return (TTree*)ltree->FindObject(tree_name);

}
//____________________________________________________________________________

void KVEventSelector::CreateTrees()
{

   Warning("CreateTrees", "To be redefined child class");

}

//____________________________________________________________________________

void KVEventSelector::FillTree(KVString sname)
{

   if (sname == "") {
      ltree->Execute("Fill", "");
   } else {
      TTree* tt = 0;
      if ((tt = GetTree(sname.Data()))) {
         tt->Fill();
      } else {
         Warning("FillTree", "%s introuvable", sname.Data());
      }
   }

}

//____________________________________________________________________________

void KVEventSelector::WriteTreeToFile(KVString filename, Option_t* option)
{

   //If no filename is specified, assume that the current directory is writable

   if (filename == "") {
      GetTreeList()->Write();
   } else {
      TFile* file = 0;
      TDirectory* pwd = gDirectory;
      //if filename correspond to an already opened file, write in it
      //if not open/create it, depending on the option ("recreate" by default)
      //and write in it
      if (!(file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename.Data())))
         file = new TFile(filename.Data(), option);
      file->cd();
      GetTreeList()->Write();
      file->Close();
      pwd->cd();
   }

}

void KVEventSelector::SetOpt(const Char_t* option, const Char_t* value)
{
   //Set a value for an option
   KVString tmp(value);
   fOptionList.SetParameter(option, tmp);
}

//_________________________________________________________________

Bool_t KVEventSelector::IsOptGiven(const Char_t* opt)
{
   //Returns kTRUE if the option 'opt' has been set
   
   return fOptionList.HasParameter(opt);
}

//_________________________________________________________________

KVString& KVEventSelector::GetOpt(const Char_t* opt) const
{
   //Returns the value of the option
   
   return (KVString&)fOptionList.GetParameter(opt);
}

//_________________________________________________________________

void KVEventSelector::UnsetOpt(const Char_t* opt)
{
   //Removes the option 'opt' from the internal lists, as if it had never been set
   
   fOptionList.RemoveParameter(opt);
}

void KVEventSelector::ParseOptions()
{
   // Analyse comma-separated list of options given to TTree::Process
   // and store all "option=value" pairs in fOptionList.
   // Options can then be accessed using IsOptGiven(), GetOptString(), etc.

	fOptionList.Clear(); // clear list
   KVString option = GetOption();
   option.Begin(",");
   while (!option.End()) {

      KVString opt = option.Next();
      opt.Begin("=");
      KVString param = opt.Next();
      KVString val = opt.Next();

      SetOpt(param.Data(), val.Data());
   }
}
