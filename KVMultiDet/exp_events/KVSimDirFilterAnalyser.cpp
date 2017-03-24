//Created by KVClassFactory on Thu Mar 23 14:02:40 2017
//Author: John Frankland,,,

#include "KVSimDirFilterAnalyser.h"
#include <KVDataSetManager.h>
#include <KVExpDB.h>
#include "KVMultiDetArray.h"

ClassImp(KVSimDirFilterAnalyser)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimDirFilterAnalyser</h2>
<h4>Analysis of filtered simulations</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimDirFilterAnalyser::KVSimDirFilterAnalyser()
   : KVSimDirAnalyser()
{
   // Default constructor
}

//____________________________________________________________________________//

KVSimDirFilterAnalyser::~KVSimDirFilterAnalyser()
{
   // Destructor

   if (fKinematics->TestBit(kCanDelete)) delete fKinematics;
}

void KVSimDirFilterAnalyser::preInitAnalysis()
{
   // Use options passed to KVEventSelector to initialise dataset used for filter

   if (!gDataSetManager) {
      gDataSetManager = new KVDataSetManager;
      gDataSetManager->Init();
   }
   if (!gDataSet) gDataSetManager->GetDataSet(fAnalysisClass->GetOpt("DataSet"))->cd();
}

void KVSimDirFilterAnalyser::preInitRun()
{
   // Use options passed to KVEventSelector to initialise kinematics for reaction
   // and build experimental set-up

   KVString system = fAnalysisClass->GetOpt("System");
   KVDBSystem* sys = ((KVExpDB*)gDataBase)->GetSystem(system);
   if (sys) {
      fKinematics = sys->GetKinematics();
   } else {
      fKinematics = new KV2Body(system);
      fKinematics->SetBit(kCanDelete); // clean up in destructor
      fKinematics->CalculateKinematics();
   }
   KVString run = fAnalysisClass->GetOpt("Run");
   KVMultiDetArray::MakeMultiDetector(gDataSet->GetName(), run.Atoi());
}

//____________________________________________________________________________//

