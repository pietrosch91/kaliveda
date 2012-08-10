//Created by KVClassFactory on Thu Jun 21 17:01:26 2012
//Author: John Frankland,,,

#include "KVEventFiltering.h"
#include "KVMultiDetArray.h"
#include "KVDataBase.h"
#include "KV2Body.h"
#include "KVDBSystem.h"
#include "KVDBRun.h"
#include "KVDataSetManager.h"

ClassImp(KVEventFiltering)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVEventFiltering</h2>
<h4>event analysis class</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVEventFiltering::KVEventFiltering()
{
   // Default constructor
}

//________________________________________________________________

KVEventFiltering::KVEventFiltering (const KVEventFiltering& obj)  : KVEventSelector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVEventFiltering::~KVEventFiltering()
{
   // Destructor
}

//________________________________________________________________

void KVEventFiltering::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVEventFiltering::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVEventSelector::Copy(obj);
   //KVEventFiltering& CastedObj = (KVEventFiltering&)obj;
}

   
Bool_t KVEventFiltering::Analysis()
{
   GetEvent()->SetFrame("lab", fCMVelocity);
   gMultiDetArray->DetectEvent(GetEvent(), fReconEvent, "lab");
   fReconEvent->IdentifyEvent();
   fReconEvent->CalibrateEvent();
   fReconEvent->SecondaryIdentCalib();
   fTree->Fill();

   return kTRUE;
}
   
void KVEventFiltering::EndAnalysis()
{
   fFile->Write();
   delete fFile;
}
   
void KVEventFiltering::EndRun()
{
}
   
void KVEventFiltering::InitAnalysis()
{
   TString dataset = GetOpt("Dataset").Data();
   gDataSetManager->GetDataSet(dataset)->cd();
   if(gMultiDetArray) delete gMultiDetArray;
   gDataSet->BuildMultiDetector();
   gMultiDetArray->SetSimMode();
   
   TString system = GetOpt("System").Data();
   KVDBSystem* sys = (KVDBSystem*)gDataBase->GetTable("Systems")->GetRecord(system);
   fCMVelocity =  sys->GetKinematics()->GetCMVelocity();
   fCMVelocity*=-1.0;
   
   Int_t run=0;
   if(IsOptGiven("Run")) run = GetOpt("Run").Atoi();
   if(!run) run = ((KVDBRun*)sys->GetRuns()->First())->GetNumber();
   gMultiDetArray->SetParameters( run );
   
   TString geo = GetOpt("Geometry").Data();
   if(geo=="ROOT"){
      gMultiDetArray->SetROOTGeometry(kTRUE);
      gMultiDetArray->CreateGeoManager();
   }
   
   OpenOutputFile(sys,run);
   fTree = new TTree("ReconstructedEvents", Form("%s filtered with %s",fChain->GetTitle(),sys->GetName()));
   TString reconevclass = gDataSet->GetReconstructedEventClassName();
   fReconEvent = (KVReconstructedEvent*)TClass::GetClass(reconevclass)->New();
   fTree->Branch("ReconEvent", reconevclass,&fReconEvent,10000000,0)->SetAutoDelete(kFALSE);
}
   
void KVEventFiltering::InitRun()
{
}

void KVEventFiltering::OpenOutputFile(KVDBSystem*S,Int_t run)
{
   TString basefile = gSystem->BaseName(fChain->GetCurrentFile()->GetName());
   basefile.Remove(basefile.Index(".root"),5);
   TString outfile = basefile + "_geo=";
   outfile += GetOpt("Geometry");
   outfile+="_filt=";
   outfile += GetOpt("Filter");
   outfile+="_";
   outfile+=gDataSet->GetName();
   outfile+="_";
   outfile+=S->GetBatchName();
   outfile+="_run=";
   outfile+=Form("%d",run);
   outfile+=".root";
   
      TString fullpath;
      AssignAndDelete(fullpath, gSystem->ConcatFileName(GetOpt("OutputDir").Data(),outfile.Data()));
   
   fFile = new TFile(fullpath,"recreate");
   TNamed* system = new TNamed("System", S->GetName());
   system->Write();
   (new TNamed("Dataset",gDataSet->GetName()))->Write();
   (new TNamed("Run",Form("%d",run)))->Write();
   if(gMultiDetArray->IsROOTGeometry()){
      (new TNamed("Geometry", "ROOT"))->Write();
   }
   else
   {
      (new TNamed("Geometry", "KV"))->Write();
   }
   (new TNamed("Filter",GetOpt("Filter").Data()))->Write();
   (new TNamed("Origin", (basefile+".root").Data()))->Write();
}
