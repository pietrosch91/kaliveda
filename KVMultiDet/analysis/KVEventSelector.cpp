#define KVEventSelector_cxx
// The class definition in KVEventSelector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("KVEventSelector.C")
// Root > T->Process("KVEventSelector.C","some options")
// Root > T->Process("KVEventSelector.C+")
//

#include "KVEventSelector.h"
#include <TStyle.h>
#include "TPluginManager.h"

void KVEventSelector::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void KVEventSelector::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

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

   if (!(fEventsRead % fEventsReadInterval) && fEventsRead) Info("Process", " +++ %ld events processed +++ ", fEventsRead);

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
      Info("Process", "End of file reached after %ld events", fEventsRead);

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
	
	//dissociate all histos from directories/files etc. to avoid double deletions with ROOT cleanup
	//mechanism when exiting ROOT session (all histos are deleted in our dtor)
	lhisto->R__FOR_EACH(TH1,SetDirectory)(0);
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

	//dissociate all trees from directories/files etc. to avoid double deletions with ROOT cleanup
	//mechanism when exiting ROOT session (all trees are deleted in our dtor)
	ltree->R__FOR_EACH(TTree,SetDirectory)(0);
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
