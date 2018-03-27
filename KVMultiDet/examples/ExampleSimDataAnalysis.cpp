//Created by KVClassFactory on Tue Mar 27 21:24:44 2018
//Author: John Frankland,,,

#include "ExampleSimDataAnalysis.h"
#include "KVSimNucleus.h"
#include "KVBatchSystem.h"

ClassImp(ExampleSimDataAnalysis)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>ExampleSimDataAnalysis</h2>
<h4>Analysis of simulated events</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void ExampleSimDataAnalysis::InitAnalysis()
{
   // INITIALISATION PERFORMED AT BEGINNING OF ANALYSIS
   // Here you define:
   //   - global variables
   //   - histograms
   //   - trees

   // DEFINITION OF GLOBAL VARIABLES FOR ANALYSIS

   // charged particle multiplicity
   KVVarGlob* v = AddGV("KVVGSum", "Mcha");
   v->SetOption("mode", "mult");
   v->SetSelection(KVParticleCondition("_NUC_->GetZ()>0"));

   ZMAX = (KVZmax*)AddGV("KVZmax", "ZMAX");//fragments sorted by Z

   // DEFINITION OF TREE USED TO STORE RESULTS
   CreateTreeFile();

   TTree* t = new TTree("data", GetOpt("SimulationInfos"));

   // add a branch to tree for each defined global variable
   GetGVList()->MakeBranches(t);

   // add branches to be filled by user
   t->Branch("mult", &mult);
   t->Branch("Z", Z, "Z[mult]/I");
   t->Branch("A", A, "A[mult]/I");
   t->Branch("Vper", Vper, "Vper[mult]/D");
   t->Branch("Vpar", Vpar, "Vpar[mult]/D");
   t->Branch("E", E, "E[mult]/D");
   t->Branch("Theta", Theta, "Theta[mult]/D");
   t->Branch("Phi", Phi, "Phi[mult]/D");

   AddTree(t);

}

//____________________________________________________________________________________

Bool_t ExampleSimDataAnalysis::Analysis()
{
   // EVENT BY EVENT ANALYSIS

   mult = GetEvent()->GetMult();

   for (int i = 0; i < mult; i++) {
      KVSimNucleus* part = (KVSimNucleus*)ZMAX->GetZmax(i);
      Z[i] = part->GetZ();
      A[i] = part->GetA();
      Vper[i] = part->GetVperp();
      Vpar[i] = part->GetVpar();
      E[i] = part->GetEnergy();
      Theta[i] = part->GetTheta();
      Phi[i] = part->GetPhi();
   }

   GetGVList()->FillBranches();
   FillTree();

   return kTRUE;
}

