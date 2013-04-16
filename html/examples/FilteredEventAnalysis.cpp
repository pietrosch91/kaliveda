//Created by KVClassFactory on Thu Jun 21 17:01:26 2012
//Author: John Frankland,,,

#include "FilteredEventAnalysis.h"

#include "KVDataRepositoryManager.h"
#include "KVINDRADB.h"
#include "KVINDRA.h"
#include "KV2Body.h"
#include "KVINDRAReconEvent.h"
#include "KVINDRAReconNuc.h"
#include "KVZmax.h"

ClassImp(FilteredEventAnalysis)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Example of PROOF-compatible KVEventSelector for analysis of simulated or
filtered data with KVSimDirGUI</h2>
<p> This analysis class can be used with KVSimDirGUI in order to analyse
simulated or filtered simulated data. It works with or without PROOF.
It relies on the fact that when KVSimDirGUI is used to analyse filtered data,
we pass the name of the simulated DataSet, System, and Run as options to
the analysis class in order to detect whether simulated or filtered data
is being analysed.</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

FilteredEventAnalysis::FilteredEventAnalysis()
{
   // Default constructor
}

FilteredEventAnalysis::~FilteredEventAnalysis()
{
   // Destructor
}


void FilteredEventAnalysis::EndRun()
{
   // Not used
}

void FilteredEventAnalysis::EndAnalysis()
{
   // Not used
}


void FilteredEventAnalysis::InitRun()
{
   // Only used for filtered data
   if (IsOptGiven("System")) {
      // When analysing filtered data, we need to calculate the centre of mass velocity
      // of the simulated experiment, to be used in SetAnalysisFrame()
      KVString system = GetOpt("System");
      KVDBSystem* sys = gIndraDB->GetSystem(system);
      sys->GetKinematics()->Print();
      fCMVelocity =  sys->GetKinematics()->GetCMVelocity();
      
      // We also set the state of the multidetector to the currently analysed run
      KVString run = GetOpt("Run");
      gIndra->SetParameters(run.Atoi());

      // We set the filtered particles to be accepted in our analysis
      ((KVINDRAReconEvent*)GetEvent())->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4  | kIDCode6);
      ((KVINDRAReconEvent*)GetEvent())->AcceptECodes(kECode1 | kECode2);
   }
}

void FilteredEventAnalysis::InitAnalysis()
{
   // Declares all global variables, trees and histograms
   
   AddGV("KVZtot", "ZTOT");
   AddGV("KVZtotAv", "ZTOTAV");
   AddGV("KVZVtot", "ZVTOT");
   AddGV("KVPtot", "PTOTCM")->SetFrame("CM");
   AddGV("KVZmax", "ZMAX");
   AddGV("KVEtransLCP", "ET12");
   KVVarGlob* v = AddGV("KVTensE", "TENS1");
   v->SetFrame("CM");
   v->SetParameter("Zmin", 1);
   v = AddGV("KVTensE", "TENS5");
   v->SetFrame("CM");
   v->SetParameter("Zmin", 5);
   AddGV("KVMultIMF", "MULT5")->SetParameter("Zmin", 5);
   AddGV("KVMultLeg", "MLCP");
   AddGV("KVZbound", "ZBOUND5")->SetParameter("Zmin", 5);
   v = AddGV("KVRiso", "RISO");
   v->SetFrame("CM");
   v = AddGV("KVRiso", "RISO5");
   v->SetFrame("CM");
   v->SetSelection(KVParticleCondition("_NUC_->GetZ()>4"));
   AddGV("KVQuadMoment", "QUAD")->SetFrame("CM");

   // creation of tree file compatible with PROOF
   CreateTreeFile();

   fTree = new TTree("data", "fChain->GetTitle()");
   
   // automatically generate branches for all global variables
   GetGVList()->MakeBranches(fTree);

   fTree->Branch("MCHA", &MCHA);
   fTree->Branch("Z", Z, "Z[MCHA]/I");
   fTree->Branch("A", A, "A[MCHA]/I");
   fTree->Branch("Vper", Vper, "Vper[MCHA]");
   fTree->Branch("Vpar", Vpar, "Vpar[MCHA]");
   fTree->Branch("Ek", Ek, "Ek[MCHA]");
   fTree->Branch("Theta", Theta, "Theta[MCHA]");
   fTree->Branch("Phi", Phi, "Phi[MCHA]");
   fTree->Branch("VperL", Vperl, "VperL[MCHA]");
   fTree->Branch("VparL", Vparl, "VparL[MCHA]");
   fTree->Branch("EkL", Ekl, "EkL[MCHA]");
   fTree->Branch("ThetaL", Thetal, "ThetaL[MCHA]");
   fTree->Branch("PhiL", Phil, "PhiL[MCHA]");

   // essential for PROOF
   AddTree(fTree);

   if (!IsOptGiven("System")) {
      // simulated data: all particles are accepted, except neutrons
      KVParticleCondition pc("_NUC_->GetZ()>0");
      SetParticleConditions(pc);
   } else {
      // filtered data: add some histos with detection-specific informations
      AddHisto(new TH2F("z-e", "Z vs E", 500, 0, 0, 100, .5, 100.5));
      AddHisto(new TH2F("icode-ecode", "E code vs. ID code", 16, -.5, 15.5, 20, -.5, 19.5));
      AddHisto(new TH2F("ring-mod", "Ring vs. module all", 17, .5, 17.5, 24, .5, 24.5));
      AddHisto(new TH2F("ring-mod-i0", "Ring vs. module c0", 17, .5, 17.5, 24, .5, 24.5));
      AddHisto(new TH2F("ring-mod-i2", "Ring vs. module c2", 17, .5, 17.5, 24, .5, 24.5));
      AddHisto(new TH2F("ring-mod-i3", "Ring vs. module c3", 17, .5, 17.5, 24, .5, 24.5));
      AddHisto(new TH2F("ring-mod-i4", "Ring vs. module c4", 17, .5, 17.5, 24, .5, 24.5));
      AddHisto(new TH2F("ring-mod-i5", "Ring vs. module c5", 17, .5, 17.5, 24, .5, 24.5));
      AddHisto(new TH2F("ring-mod-i6", "Ring vs. module c6", 17, .5, 17.5, 24, .5, 24.5));
      AddHisto(new TH2F("ring-mod-i8", "Ring vs. module c8", 17, .5, 17.5, 24, .5, 24.5));
      AddHisto(new TH2F("ring-mod-i14", "Ring vs. module c14", 17, .5, 17.5, 24, .5, 24.5));
      
      // when running PROOF, we cannot assume that gDataSetManager & al. exist
      if (!gDataRepositoryManager) {
         new KVDataRepositoryManager;
         gDataRepositoryManager->Init();
      }
      if (!gDataSet) gDataSetManager->GetDataSet(GetOpt("DataSet"))->cd();
      if (!gIndra) gDataSet->BuildMultiDetector();
   }
}

Bool_t FilteredEventAnalysis::Analysis()
{
   // Analysis of an event
   
   if (!GetEvent()->IsOK()) return kTRUE;
   
   if (IsOptGiven("System")) {
      // fill detection-specific histos for filtered data
      KVINDRAReconNuc* nuc;
      while ((nuc = (KVINDRAReconNuc*)GetEvent()->GetNextParticle())) {
         int ring = nuc->GetRingNumber();
         int mod = nuc->GetModuleNumber();
         int code = nuc->GetCodes().GetVedaIDCode();
         int eode = nuc->GetCodes().GetVedaECode();
         FillHisto("ring-mod", ring, mod);
         FillHisto(Form("ring-mod-i%d", code), ring, mod);
         FillHisto("icode-ecode", code, eode);
         if (nuc->IsOK()) FillHisto("z-e", nuc->GetEnergy(), nuc->GetZ());
      }
   }
   
   // fill arrays with particle properties
   MCHA = GetEvent()->GetMult("ok");
   KVNucleus* part;
   int i = 0;
   KVZmax* ZMAX = (KVZmax*)GetGV("ZMAX");
   for (; i < MCHA; i++) {
      part = ZMAX->GetZmax(i + 1);
      Z[i] = part->GetZ();
      A[i] = part->GetA();
      Vper[i] = part->GetFrame("CM")->GetVperp();
      Vpar[i] = part->GetFrame("CM")->GetVpar();
      Ek[i] = part->GetFrame("CM")->GetE();
      Theta[i] = part->GetFrame("CM")->GetTheta();
      Phi[i] = part->GetFrame("CM")->GetPhi();
      Vperl[i] = part->GetVperp();
      Vparl[i] = part->GetVpar();
      Ekl[i] = part->GetE();
      Thetal[i] = part->GetTheta();
      Phil[i] = part->GetPhi();
   }
   // fill global variable branches
   GetGVList()->FillBranches();
   // fill TTree
   fTree->Fill();

   return kTRUE;
}
