#include <KVDataRepositoryManager.h>
#include <KVDataSetManager.h>
#include <KVEventReconstructor.h>
#include <KVFAZIADetector.h>
#include <KVMFMDataFileReader.h>
#include <iostream>
#include "TApplication.h"
#include "TROOT.h"
#include "KVINDRA.h"
#include "KVFAZIA.h"
#include "TH2.h"
using namespace std;

int main(int argc, char* argv[])
{
   KVBase::InitEnvironment();

   //TApplication myapp("myapp", &argc, argv);

   KVDataRepositoryManager drm;
   drm.Init();
   gDataSetManager->GetDataSet("INDRAFAZIA")->cd();

   KVMultiDetArray::MakeMultiDetector(gDataSet->GetName());
   gMultiDetArray->SetIdentifications();
   gMultiDetArray->SetGridsInTelescopes(12);
   gMultiDetArray->InitializeIDTelescopes();
   gMultiDetArray->PrintStatusOfIDTelescopes();

   gFazia->SetParameters(12);

   unique_ptr<KVMFMDataFileReader> raw_file(KVMFMDataFileReader::Open(argv[1]));

   KVEventReconstructor erec(gMultiDetArray, new KVReconstructedEvent);
   //KVGroupReconstructor::SetDoIdentification(false);
   KVGroupReconstructor::SetDoCalibration(true);

   int first_frame = TString(argv[2]).Atoi();
   TFile f(Form("INDRAFAZIArecon_%d.root", first_frame), "recreate");

   int n_frames = -1;
   if (argc == 4) n_frames  = TString(argv[3]).Atoi();

   TTree* tree = new TTree("ReconEvents", "Reconstructed INDRA-FAZIA events");
   KVEvent::MakeEventBranch(tree, "ReconEvent", "KVReconstructedEvent", erec.GetEventReference());

   TTree* idtree = new TTree("Ident", "Status identifications");
   int z, a, code;
   double idx, idy, e, vz, v;
   TString detid, array;
   idtree->Branch("array", &array);
   idtree->Branch("detid", &detid);
   idtree->Branch("z", &z);
   idtree->Branch("a", &a);
   idtree->Branch("e", &e);
   idtree->Branch("vz", &vz);
   idtree->Branch("v", &v);
   idtree->Branch("code", &code);
   idtree->Branch("idx", &idx);
   idtree->Branch("idy", &idy);

   KVHashList my_csi_hists;
   TIter nxt_csi(gFazia->GetIDTelescopesWithType("Si-CsI"));
   KVIDTelescope* idt;
   while ((idt = (KVIDTelescope*)nxt_csi())) {
      if (((KVFAZIADetector*)idt->GetDetector(1))->GetBlockNumber() == 1)
         my_csi_hists.Add(new TH2F(idt->GetName(), idt->GetName(), 2000, 0, 6000, 1000, 0, 500));
   }

   int i = 0;
   int bad = 0;
   while (raw_file->GetNextEvent()) {
      ++i;
      if (n_frames > 0 && i > n_frames) break;
      if (i != first_frame) continue;
      first_frame += 1;
      //cout << "=================================================================" << endl;
      if (gMultiDetArray->HandleRawDataEvent(raw_file.get())) {
         erec.ReconstructEvent(gMultiDetArray->GetFiredDataParameters());
         erec.GetEvent()->SetNumber(i + 1);
         if (gFazia->HandledRawData()) {
//            cout << Form("FAZIA particle...") << endl;
            nxt_csi.Reset();
            while ((idt = (KVIDTelescope*)nxt_csi())) {
               if (idt->GetDetector(1)->Fired()) {
                  ((TH2*)my_csi_hists.FindObject(idt->GetName()))->Fill(idt->GetIDMapX(), idt->GetIDMapY());
               }
            }
         }
         tree->Fill();
         if (erec.GetEvent()->GetMult()) {
            for (KVEvent::Iterator it = erec.GetEvent()->begin(); it != erec.GetEvent()->end(); ++it) {
               KVReconstructedNucleus& n = it.reference<KVReconstructedNucleus>();
               array = n.GetParameters()->GetTStringValue("ARRAY");
//               if(array=="FAZIA")
//               {
//                  if(n.GetIdentificationResult("Si-CsI"))
//                  {
//                     n.GetIdentificationResult("Si-CsI")->Print();
//                     if(!n.IsIdentified()) cout << "mais en fait non..." << endl;
//                     if(n.IsIdentified()&&!n.IsZMeasured()) cout << "on comprend rien..." << endl;
//                  }

//               }



               if (n.IsIdentified()) {
//                  cout << Form("%s identified nuc (%d, *)", n.GetParameters()->GetTStringValue("ARRAY").Data(),n.GetZ()) << endl;
                  if (n.IsZMeasured()) {
                     code = n.GetIDCode();
                     idx = n.GetIdentifyingTelescope()->GetIDMapX();
                     idy = n.GetIdentifyingTelescope()->GetIDMapY();
                     z = n.GetZ();
                     if (n.IsAMeasured()) a = n.GetA();
                     else a = -1;
                     array = n.GetParameters()->GetTStringValue("ARRAY");
//                     if(array=="FAZIA") cout << Form("FAZIA identified nuc (%d, %d)",n.GetZ(),n.GetA()) << endl;
                     detid = n.GetIdentifyingTelescope()->GetName();
                     if (n.IsCalibrated()) e = n.GetKE();
                     else e = -1;
                     if (n.IsCalibrated()) vz = n.GetVelocity().Z();
                     else vz = -1;
                     if (n.IsCalibrated()) v = n.GetVelocity().Mag();
                     else v = -1;
                     idtree->Fill();
                  }
               }
            }
         }
      }
      else {
//         cout << "Frame " << i << " not handled by anybody:" << endl;
//         raw_file->PrintFrameRead();
         bad++;
      }
      if (!(i % 1000)) cout << "Treated " << i << " frames (" << bad << " bad)" << endl;
   }

   f.Write();
}
