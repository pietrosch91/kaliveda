//Created by KVClassFactory on Thu Jul 19 15:38:10 2018
//Author: eindra

#include "KVReconEventSelector.h"

#include <KVClassFactory.h>
#include <KVReconDataAnalyser.h>

ClassImp(KVReconEventSelector)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVReconEventSelector</h2>
<h4>Base analysis class for reconstructed data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVReconEventSelector::Init(TTree* tree)
{
   // When using PROOF, need to set tree pointer in KVDataAnalyser
   KVEventSelector::Init(tree);
#ifdef WITH_CPP11
   if (tree && gDataAnalyser->GetProofMode() != KVDataAnalyser::EProofMode::None) {
#else
   if (tree && gDataAnalyser->GetProofMode() != KVDataAnalyser::None) {
#endif
      dynamic_cast<KVReconDataAnalyser*>(gDataAnalyser)->SetTree(tree);
   }
}

void KVReconEventSelector::SetAnalysisFrame()
{
   // Define the CM frame for the current event
   //calculate momenta of particles in reaction cm frame
   if (fCurrentRun->GetSystem() && fCurrentRun->GetSystem()->GetKinematics()) {
      GetEvent()->SetFrame("CM", fCurrentRun->GetSystem()->GetKinematics()->GetCMVelocity());
   }
}

void KVReconEventSelector::Make(const Char_t* kvsname)
{
   // Generate a new recon data analysis selector class

   KVClassFactory cf(kvsname, "Analysis of reconstructed events", "",
                     kTRUE, "ReconDataSelectorTemplate");
   cf.AddImplIncludeFile("KVReconstructedNucleus.h");
   cf.AddImplIncludeFile("KVBatchSystem.h");

   cf.GenerateCode();
}

