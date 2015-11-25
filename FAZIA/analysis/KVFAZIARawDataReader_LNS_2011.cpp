#define KVFAZIARawDataReader_LNS_2011_cxx
// The class definition in KVFAZIARawDataReader_LNS_2011.h has been generated automatically
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
// Root > T->Process("KVFAZIARawDataReader_LNS_2011.C")
// Root > T->Process("KVFAZIARawDataReader_LNS_2011.C","some options")
// Root > T->Process("KVFAZIARawDataReader_LNS_2011.C+")
//

#include "KVFAZIARawDataReader_LNS_2011.h"
#include "TFile.h"


void KVFAZIARawDataReader_LNS_2011::Begin(TTree* /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void KVFAZIARawDataReader_LNS_2011::SlaveBegin(TTree* /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void KVFAZIARawDataReader_LNS_2011::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void KVFAZIARawDataReader_LNS_2011::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

}

void KVFAZIARawDataReader_LNS_2011::InitRun()
{
   // Set branch addresses and branch pointers
   //Info("InitRun","Changement de run name=%s number=%d",fChain->GetCurrentFile()->GetName(),fChain->GetFileNumber());
   /*
   fChain->SetBranchAddress("tele111_si1.energy", &tele111_si1_energy, &b_tele111_si1_energy);
   fChain->SetBranchAddress("tele111_si1.A", &tele111_si1_A, &b_tele111_si1_A);
   fChain->SetBranchAddress("tele111_si1.Z", &tele111_si1_Z, &b_tele111_si1_Z);
   fChain->SetBranchAddress("tele111_si1.nsamples_i", &tele111_si1_nsamples_i, &b_tele111_si1_nsamples_i);
   fChain->SetBranchAddress("tele111_si1.nsamples_q", &tele111_si1_nsamples_q, &b_tele111_si1_nsamples_q);
   fChain->SetBranchAddress("tele111_si1.nsamples_fpga", &tele111_si1_nsamples_fpga, &b_tele111_si1_nsamples_fpga);
   fChain->SetBranchAddress("tele111_si1.data_i_s", &tele111_si1_data_i_s, &b_tele111_si1_data_i_s);
   fChain->SetBranchAddress("tele111_si1.data_q_s", &tele111_si1_data_q_s, &b_tele111_si1_data_q_s);
   fChain->SetBranchAddress("tele111_si1.data_fpga", &tele111_si1_data_fpga, &b_tele111_si1_data_fpga);
   fChain->SetBranchAddress("tele111_si1.data_MAR", tele111_si1_data_MAR, &b_tele111_si1_data_MAR);
   fChain->SetBranchAddress("tele111_si2.energy", &tele111_si2_energy, &b_tele111_si2_energy);
   fChain->SetBranchAddress("tele111_si2.A", &tele111_si2_A, &b_tele111_si2_A);
   fChain->SetBranchAddress("tele111_si2.Z", &tele111_si2_Z, &b_tele111_si2_Z);
   fChain->SetBranchAddress("tele111_si2.nsamples_i", &tele111_si2_nsamples_i, &b_tele111_si2_nsamples_i);
   fChain->SetBranchAddress("tele111_si2.nsamples_q", &tele111_si2_nsamples_q, &b_tele111_si2_nsamples_q);
   fChain->SetBranchAddress("tele111_si2.nsamples_fpga", &tele111_si2_nsamples_fpga, &b_tele111_si2_nsamples_fpga);
   fChain->SetBranchAddress("tele111_si2.data_i_s", &tele111_si2_data_i_s, &b_tele111_si2_data_i_s);
   fChain->SetBranchAddress("tele111_si2.data_q_s", &tele111_si2_data_q_s, &b_tele111_si2_data_q_s);
   fChain->SetBranchAddress("tele111_si2.data_fpga", &tele111_si2_data_fpga, &b_tele111_si2_data_fpga);
   fChain->SetBranchAddress("tele111_si2.data_MAR", tele111_si2_data_MAR, &b_tele111_si2_data_MAR);
   fChain->SetBranchAddress("tele111_csi.energy", &tele111_csi_energy, &b_tele111_csi_energy);
   fChain->SetBranchAddress("tele111_csi.A", &tele111_csi_A, &b_tele111_csi_A);
   fChain->SetBranchAddress("tele111_csi.Z", &tele111_csi_Z, &b_tele111_csi_Z);
   fChain->SetBranchAddress("tele111_csi.nsamples_i", &tele111_csi_nsamples_i, &b_tele111_csi_nsamples_i);
   fChain->SetBranchAddress("tele111_csi.nsamples_q", &tele111_csi_nsamples_q, &b_tele111_csi_nsamples_q);
   fChain->SetBranchAddress("tele111_csi.nsamples_fpga", &tele111_csi_nsamples_fpga, &b_tele111_csi_nsamples_fpga);
   fChain->SetBranchAddress("tele111_csi.data_i_s", &tele111_csi_data_i_s, &b_tele111_csi_data_i_s);
   fChain->SetBranchAddress("tele111_csi.data_q_s", &tele111_csi_data_q_s, &b_tele111_csi_data_q_s);
   fChain->SetBranchAddress("tele111_csi.data_fpga", &tele111_csi_data_fpga, &b_tele111_csi_data_fpga);
   fChain->SetBranchAddress("tele111_csi.data_MAR", tele111_csi_data_MAR, &b_tele111_csi_data_MAR);
   fChain->SetBranchAddress("tele112_si1.energy", &tele112_si1_energy, &b_tele112_si1_energy);
   fChain->SetBranchAddress("tele112_si1.A", &tele112_si1_A, &b_tele112_si1_A);
   fChain->SetBranchAddress("tele112_si1.Z", &tele112_si1_Z, &b_tele112_si1_Z);
   fChain->SetBranchAddress("tele112_si1.nsamples_i", &tele112_si1_nsamples_i, &b_tele112_si1_nsamples_i);
   fChain->SetBranchAddress("tele112_si1.nsamples_q", &tele112_si1_nsamples_q, &b_tele112_si1_nsamples_q);
   fChain->SetBranchAddress("tele112_si1.nsamples_fpga", &tele112_si1_nsamples_fpga, &b_tele112_si1_nsamples_fpga);
   fChain->SetBranchAddress("tele112_si1.data_i_s", &tele112_si1_data_i_s, &b_tele112_si1_data_i_s);
   fChain->SetBranchAddress("tele112_si1.data_q_s", &tele112_si1_data_q_s, &b_tele112_si1_data_q_s);
   fChain->SetBranchAddress("tele112_si1.data_fpga", &tele112_si1_data_fpga, &b_tele112_si1_data_fpga);
   fChain->SetBranchAddress("tele112_si1.data_MAR", tele112_si1_data_MAR, &b_tele112_si1_data_MAR);
   fChain->SetBranchAddress("tele112_SCT.energy", &tele112_SCT_energy, &b_tele112_SCT_energy);
   fChain->SetBranchAddress("tele112_SCT.A", &tele112_SCT_A, &b_tele112_SCT_A);
   fChain->SetBranchAddress("tele112_SCT.Z", &tele112_SCT_Z, &b_tele112_SCT_Z);
   fChain->SetBranchAddress("tele112_SCT.nsamples_i", &tele112_SCT_nsamples_i, &b_tele112_SCT_nsamples_i);
   fChain->SetBranchAddress("tele112_SCT.nsamples_q", &tele112_SCT_nsamples_q, &b_tele112_SCT_nsamples_q);
   fChain->SetBranchAddress("tele112_SCT.nsamples_fpga", &tele112_SCT_nsamples_fpga, &b_tele112_SCT_nsamples_fpga);
   fChain->SetBranchAddress("tele112_SCT.data_i_s", &tele112_SCT_data_i_s, &b_tele112_SCT_data_i_s);
   fChain->SetBranchAddress("tele112_SCT.data_q_s", &tele112_SCT_data_q_s, &b_tele112_SCT_data_q_s);
   fChain->SetBranchAddress("tele112_SCT.data_fpga", &tele112_SCT_data_fpga, &b_tele112_SCT_data_fpga);
   fChain->SetBranchAddress("tele112_SCT.data_MAR", tele112_SCT_data_MAR, &b_tele112_SCT_data_MAR);
   fChain->SetBranchAddress("tele112_iSCT.energy", &tele112_iSCT_energy, &b_tele112_iSCT_energy);
   fChain->SetBranchAddress("tele112_iSCT.A", &tele112_iSCT_A, &b_tele112_iSCT_A);
   fChain->SetBranchAddress("tele112_iSCT.Z", &tele112_iSCT_Z, &b_tele112_iSCT_Z);
   fChain->SetBranchAddress("tele112_iSCT.nsamples_i", &tele112_iSCT_nsamples_i, &b_tele112_iSCT_nsamples_i);
   fChain->SetBranchAddress("tele112_iSCT.nsamples_q", &tele112_iSCT_nsamples_q, &b_tele112_iSCT_nsamples_q);
   fChain->SetBranchAddress("tele112_iSCT.nsamples_fpga", &tele112_iSCT_nsamples_fpga, &b_tele112_iSCT_nsamples_fpga);
   fChain->SetBranchAddress("tele112_iSCT.data_i_s", &tele112_iSCT_data_i_s, &b_tele112_iSCT_data_i_s);
   fChain->SetBranchAddress("tele112_iSCT.data_q_s", &tele112_iSCT_data_q_s, &b_tele112_iSCT_data_q_s);
   fChain->SetBranchAddress("tele112_iSCT.data_fpga", &tele112_iSCT_data_fpga, &b_tele112_iSCT_data_fpga);
   fChain->SetBranchAddress("tele112_iSCT.data_MAR", tele112_iSCT_data_MAR, &b_tele112_iSCT_data_MAR);
   fChain->SetBranchAddress("tele113_si1.energy", &tele113_si1_energy, &b_tele113_si1_energy);
   fChain->SetBranchAddress("tele113_si1.A", &tele113_si1_A, &b_tele113_si1_A);
   fChain->SetBranchAddress("tele113_si1.Z", &tele113_si1_Z, &b_tele113_si1_Z);
   fChain->SetBranchAddress("tele113_si1.nsamples_i", &tele113_si1_nsamples_i, &b_tele113_si1_nsamples_i);
   fChain->SetBranchAddress("tele113_si1.nsamples_q", &tele113_si1_nsamples_q, &b_tele113_si1_nsamples_q);
   fChain->SetBranchAddress("tele113_si1.nsamples_fpga", &tele113_si1_nsamples_fpga, &b_tele113_si1_nsamples_fpga);
   fChain->SetBranchAddress("tele113_si1.data_i_s", &tele113_si1_data_i_s, &b_tele113_si1_data_i_s);
   fChain->SetBranchAddress("tele113_si1.data_q_s", &tele113_si1_data_q_s, &b_tele113_si1_data_q_s);
   fChain->SetBranchAddress("tele113_si1.data_fpga", &tele113_si1_data_fpga, &b_tele113_si1_data_fpga);
   fChain->SetBranchAddress("tele113_si1.data_MAR", tele113_si1_data_MAR, &b_tele113_si1_data_MAR);
   fChain->SetBranchAddress("tele113_si2.energy", &tele113_si2_energy, &b_tele113_si2_energy);
   fChain->SetBranchAddress("tele113_si2.A", &tele113_si2_A, &b_tele113_si2_A);
   fChain->SetBranchAddress("tele113_si2.Z", &tele113_si2_Z, &b_tele113_si2_Z);
   fChain->SetBranchAddress("tele113_si2.nsamples_i", &tele113_si2_nsamples_i, &b_tele113_si2_nsamples_i);
   fChain->SetBranchAddress("tele113_si2.nsamples_q", &tele113_si2_nsamples_q, &b_tele113_si2_nsamples_q);
   fChain->SetBranchAddress("tele113_si2.nsamples_fpga", &tele113_si2_nsamples_fpga, &b_tele113_si2_nsamples_fpga);
   fChain->SetBranchAddress("tele113_si2.data_i_s", &tele113_si2_data_i_s, &b_tele113_si2_data_i_s);
   fChain->SetBranchAddress("tele113_si2.data_q_s", &tele113_si2_data_q_s, &b_tele113_si2_data_q_s);
   fChain->SetBranchAddress("tele113_si2.data_fpga", &tele113_si2_data_fpga, &b_tele113_si2_data_fpga);
   fChain->SetBranchAddress("tele113_si2.data_MAR", tele113_si2_data_MAR, &b_tele113_si2_data_MAR);
   fChain->SetBranchAddress("tele113_csi.energy", &tele113_csi_energy, &b_tele113_csi_energy);
   fChain->SetBranchAddress("tele113_csi.A", &tele113_csi_A, &b_tele113_csi_A);
   fChain->SetBranchAddress("tele113_csi.Z", &tele113_csi_Z, &b_tele113_csi_Z);
   fChain->SetBranchAddress("tele113_csi.nsamples_i", &tele113_csi_nsamples_i, &b_tele113_csi_nsamples_i);
   fChain->SetBranchAddress("tele113_csi.nsamples_q", &tele113_csi_nsamples_q, &b_tele113_csi_nsamples_q);
   fChain->SetBranchAddress("tele113_csi.nsamples_fpga", &tele113_csi_nsamples_fpga, &b_tele113_csi_nsamples_fpga);
   fChain->SetBranchAddress("tele113_csi.data_i_s", &tele113_csi_data_i_s, &b_tele113_csi_data_i_s);
   fChain->SetBranchAddress("tele113_csi.data_q_s", &tele113_csi_data_q_s, &b_tele113_csi_data_q_s);
   fChain->SetBranchAddress("tele113_csi.data_fpga", &tele113_csi_data_fpga, &b_tele113_csi_data_fpga);
   fChain->SetBranchAddress("tele113_csi.data_MAR", tele113_csi_data_MAR, &b_tele113_csi_data_MAR);
   fChain->SetBranchAddress("tele114_si1.energy", &tele114_si1_energy, &b_tele114_si1_energy);
   fChain->SetBranchAddress("tele114_si1.A", &tele114_si1_A, &b_tele114_si1_A);
   fChain->SetBranchAddress("tele114_si1.Z", &tele114_si1_Z, &b_tele114_si1_Z);
   fChain->SetBranchAddress("tele114_si1.nsamples_i", &tele114_si1_nsamples_i, &b_tele114_si1_nsamples_i);
   fChain->SetBranchAddress("tele114_si1.nsamples_q", &tele114_si1_nsamples_q, &b_tele114_si1_nsamples_q);
   fChain->SetBranchAddress("tele114_si1.nsamples_fpga", &tele114_si1_nsamples_fpga, &b_tele114_si1_nsamples_fpga);
   fChain->SetBranchAddress("tele114_si1.data_i_s", &tele114_si1_data_i_s, &b_tele114_si1_data_i_s);
   fChain->SetBranchAddress("tele114_si1.data_q_s", &tele114_si1_data_q_s, &b_tele114_si1_data_q_s);
   fChain->SetBranchAddress("tele114_si1.data_fpga", &tele114_si1_data_fpga, &b_tele114_si1_data_fpga);
   fChain->SetBranchAddress("tele114_si1.data_MAR", tele114_si1_data_MAR, &b_tele114_si1_data_MAR);
   fChain->SetBranchAddress("tele114_si2.energy", &tele114_si2_energy, &b_tele114_si2_energy);
   fChain->SetBranchAddress("tele114_si2.A", &tele114_si2_A, &b_tele114_si2_A);
   fChain->SetBranchAddress("tele114_si2.Z", &tele114_si2_Z, &b_tele114_si2_Z);
   fChain->SetBranchAddress("tele114_si2.nsamples_i", &tele114_si2_nsamples_i, &b_tele114_si2_nsamples_i);
   fChain->SetBranchAddress("tele114_si2.nsamples_q", &tele114_si2_nsamples_q, &b_tele114_si2_nsamples_q);
   fChain->SetBranchAddress("tele114_si2.nsamples_fpga", &tele114_si2_nsamples_fpga, &b_tele114_si2_nsamples_fpga);
   fChain->SetBranchAddress("tele114_si2.data_i_s", &tele114_si2_data_i_s, &b_tele114_si2_data_i_s);
   fChain->SetBranchAddress("tele114_si2.data_q_s", &tele114_si2_data_q_s, &b_tele114_si2_data_q_s);
   fChain->SetBranchAddress("tele114_si2.data_fpga", &tele114_si2_data_fpga, &b_tele114_si2_data_fpga);
   fChain->SetBranchAddress("tele114_si2.data_MAR", tele114_si2_data_MAR, &b_tele114_si2_data_MAR);
   fChain->SetBranchAddress("tele114_csi.energy", &tele114_csi_energy, &b_tele114_csi_energy);
   fChain->SetBranchAddress("tele114_csi.A", &tele114_csi_A, &b_tele114_csi_A);
   fChain->SetBranchAddress("tele114_csi.Z", &tele114_csi_Z, &b_tele114_csi_Z);
   fChain->SetBranchAddress("tele114_csi.nsamples_i", &tele114_csi_nsamples_i, &b_tele114_csi_nsamples_i);
   fChain->SetBranchAddress("tele114_csi.nsamples_q", &tele114_csi_nsamples_q, &b_tele114_csi_nsamples_q);
   fChain->SetBranchAddress("tele114_csi.nsamples_fpga", &tele114_csi_nsamples_fpga, &b_tele114_csi_nsamples_fpga);
   fChain->SetBranchAddress("tele114_csi.data_i_s", &tele114_csi_data_i_s, &b_tele114_csi_data_i_s);
   fChain->SetBranchAddress("tele114_csi.data_q_s", &tele114_csi_data_q_s, &b_tele114_csi_data_q_s);
   fChain->SetBranchAddress("tele114_csi.data_fpga", &tele114_csi_data_fpga, &b_tele114_csi_data_fpga);
   fChain->SetBranchAddress("tele114_csi.data_MAR", tele114_csi_data_MAR, &b_tele114_csi_data_MAR);
   fChain->SetBranchAddress("tele115_si1.energy", &tele115_si1_energy, &b_tele115_si1_energy);
   fChain->SetBranchAddress("tele115_si1.A", &tele115_si1_A, &b_tele115_si1_A);
   fChain->SetBranchAddress("tele115_si1.Z", &tele115_si1_Z, &b_tele115_si1_Z);
   fChain->SetBranchAddress("tele115_si1.nsamples_i", &tele115_si1_nsamples_i, &b_tele115_si1_nsamples_i);
   fChain->SetBranchAddress("tele115_si1.nsamples_q", &tele115_si1_nsamples_q, &b_tele115_si1_nsamples_q);
   fChain->SetBranchAddress("tele115_si1.nsamples_fpga", &tele115_si1_nsamples_fpga, &b_tele115_si1_nsamples_fpga);
   fChain->SetBranchAddress("tele115_si1.data_i_s", &tele115_si1_data_i_s, &b_tele115_si1_data_i_s);
   fChain->SetBranchAddress("tele115_si1.data_q_s", &tele115_si1_data_q_s, &b_tele115_si1_data_q_s);
   fChain->SetBranchAddress("tele115_si1.data_fpga", &tele115_si1_data_fpga, &b_tele115_si1_data_fpga);
   fChain->SetBranchAddress("tele115_si1.data_MAR", tele115_si1_data_MAR, &b_tele115_si1_data_MAR);
   fChain->SetBranchAddress("tele115_si2.energy", &tele115_si2_energy, &b_tele115_si2_energy);
   fChain->SetBranchAddress("tele115_si2.A", &tele115_si2_A, &b_tele115_si2_A);
   fChain->SetBranchAddress("tele115_si2.Z", &tele115_si2_Z, &b_tele115_si2_Z);
   fChain->SetBranchAddress("tele115_si2.nsamples_i", &tele115_si2_nsamples_i, &b_tele115_si2_nsamples_i);
   fChain->SetBranchAddress("tele115_si2.nsamples_q", &tele115_si2_nsamples_q, &b_tele115_si2_nsamples_q);
   fChain->SetBranchAddress("tele115_si2.nsamples_fpga", &tele115_si2_nsamples_fpga, &b_tele115_si2_nsamples_fpga);
   fChain->SetBranchAddress("tele115_si2.data_i_s", &tele115_si2_data_i_s, &b_tele115_si2_data_i_s);
   fChain->SetBranchAddress("tele115_si2.data_q_s", &tele115_si2_data_q_s, &b_tele115_si2_data_q_s);
   fChain->SetBranchAddress("tele115_si2.data_fpga", &tele115_si2_data_fpga, &b_tele115_si2_data_fpga);
   fChain->SetBranchAddress("tele115_si2.data_MAR", tele115_si2_data_MAR, &b_tele115_si2_data_MAR);
   fChain->SetBranchAddress("tele115_csi.energy", &tele115_csi_energy, &b_tele115_csi_energy);
   fChain->SetBranchAddress("tele115_csi.A", &tele115_csi_A, &b_tele115_csi_A);
   fChain->SetBranchAddress("tele115_csi.Z", &tele115_csi_Z, &b_tele115_csi_Z);
   fChain->SetBranchAddress("tele115_csi.nsamples_i", &tele115_csi_nsamples_i, &b_tele115_csi_nsamples_i);
   fChain->SetBranchAddress("tele115_csi.nsamples_q", &tele115_csi_nsamples_q, &b_tele115_csi_nsamples_q);
   fChain->SetBranchAddress("tele115_csi.nsamples_fpga", &tele115_csi_nsamples_fpga, &b_tele115_csi_nsamples_fpga);
   fChain->SetBranchAddress("tele115_csi.data_i_s", &tele115_csi_data_i_s, &b_tele115_csi_data_i_s);
   fChain->SetBranchAddress("tele115_csi.data_q_s", &tele115_csi_data_q_s, &b_tele115_csi_data_q_s);
   fChain->SetBranchAddress("tele115_csi.data_fpga", &tele115_csi_data_fpga, &b_tele115_csi_data_fpga);
   fChain->SetBranchAddress("tele115_csi.data_MAR", tele115_csi_data_MAR, &b_tele115_csi_data_MAR);
   fChain->SetBranchAddress("tele116_si1q.energy", &tele116_si1q_energy, &b_tele116_si1q_energy);
   fChain->SetBranchAddress("tele116_si1q.A", &tele116_si1q_A, &b_tele116_si1q_A);
   fChain->SetBranchAddress("tele116_si1q.Z", &tele116_si1q_Z, &b_tele116_si1q_Z);
   fChain->SetBranchAddress("tele116_si1q.nsamples_i", &tele116_si1q_nsamples_i, &b_tele116_si1q_nsamples_i);
   fChain->SetBranchAddress("tele116_si1q.nsamples_q", &tele116_si1q_nsamples_q, &b_tele116_si1q_nsamples_q);
   fChain->SetBranchAddress("tele116_si1q.nsamples_fpga", &tele116_si1q_nsamples_fpga, &b_tele116_si1q_nsamples_fpga);
   fChain->SetBranchAddress("tele116_si1q.data_i_s", &tele116_si1q_data_i_s, &b_tele116_si1q_data_i_s);
   fChain->SetBranchAddress("tele116_si1q.data_q_s", &tele116_si1q_data_q_s, &b_tele116_si1q_data_q_s);
   fChain->SetBranchAddress("tele116_si1q.data_fpga", &tele116_si1q_data_fpga, &b_tele116_si1q_data_fpga);
   fChain->SetBranchAddress("tele116_si1q.data_MAR", tele116_si1q_data_MAR, &b_tele116_si1q_data_MAR);
   fChain->SetBranchAddress("tele116_si1i.energy", &tele116_si1i_energy, &b_tele116_si1i_energy);
   fChain->SetBranchAddress("tele116_si1i.A", &tele116_si1i_A, &b_tele116_si1i_A);
   fChain->SetBranchAddress("tele116_si1i.Z", &tele116_si1i_Z, &b_tele116_si1i_Z);
   fChain->SetBranchAddress("tele116_si1i.nsamples_i", &tele116_si1i_nsamples_i, &b_tele116_si1i_nsamples_i);
   fChain->SetBranchAddress("tele116_si1i.nsamples_q", &tele116_si1i_nsamples_q, &b_tele116_si1i_nsamples_q);
   fChain->SetBranchAddress("tele116_si1i.nsamples_fpga", &tele116_si1i_nsamples_fpga, &b_tele116_si1i_nsamples_fpga);
   fChain->SetBranchAddress("tele116_si1i.data_i_s", &tele116_si1i_data_i_s, &b_tele116_si1i_data_i_s);
   fChain->SetBranchAddress("tele116_si1i.data_q_s", &tele116_si1i_data_q_s, &b_tele116_si1i_data_q_s);
   fChain->SetBranchAddress("tele116_si1i.data_fpga", &tele116_si1i_data_fpga, &b_tele116_si1i_data_fpga);
   fChain->SetBranchAddress("tele116_si1i.data_MAR", tele116_si1i_data_MAR, &b_tele116_si1i_data_MAR);
   fChain->SetBranchAddress("tele116_si2q.energy", &tele116_si2q_energy, &b_tele116_si2q_energy);
   fChain->SetBranchAddress("tele116_si2q.A", &tele116_si2q_A, &b_tele116_si2q_A);
   fChain->SetBranchAddress("tele116_si2q.Z", &tele116_si2q_Z, &b_tele116_si2q_Z);
   fChain->SetBranchAddress("tele116_si2q.nsamples_i", &tele116_si2q_nsamples_i, &b_tele116_si2q_nsamples_i);
   fChain->SetBranchAddress("tele116_si2q.nsamples_q", &tele116_si2q_nsamples_q, &b_tele116_si2q_nsamples_q);
   fChain->SetBranchAddress("tele116_si2q.nsamples_fpga", &tele116_si2q_nsamples_fpga, &b_tele116_si2q_nsamples_fpga);
   fChain->SetBranchAddress("tele116_si2q.data_i_s", &tele116_si2q_data_i_s, &b_tele116_si2q_data_i_s);
   fChain->SetBranchAddress("tele116_si2q.data_q_s", &tele116_si2q_data_q_s, &b_tele116_si2q_data_q_s);
   fChain->SetBranchAddress("tele116_si2q.data_fpga", &tele116_si2q_data_fpga, &b_tele116_si2q_data_fpga);
   fChain->SetBranchAddress("tele116_si2q.data_MAR", tele116_si2q_data_MAR, &b_tele116_si2q_data_MAR);
   fChain->SetBranchAddress("tele116_si2i.energy", &tele116_si2i_energy, &b_tele116_si2i_energy);
   fChain->SetBranchAddress("tele116_si2i.A", &tele116_si2i_A, &b_tele116_si2i_A);
   fChain->SetBranchAddress("tele116_si2i.Z", &tele116_si2i_Z, &b_tele116_si2i_Z);
   fChain->SetBranchAddress("tele116_si2i.nsamples_i", &tele116_si2i_nsamples_i, &b_tele116_si2i_nsamples_i);
   fChain->SetBranchAddress("tele116_si2i.nsamples_q", &tele116_si2i_nsamples_q, &b_tele116_si2i_nsamples_q);
   fChain->SetBranchAddress("tele116_si2i.nsamples_fpga", &tele116_si2i_nsamples_fpga, &b_tele116_si2i_nsamples_fpga);
   fChain->SetBranchAddress("tele116_si2i.data_i_s", &tele116_si2i_data_i_s, &b_tele116_si2i_data_i_s);
   fChain->SetBranchAddress("tele116_si2i.data_q_s", &tele116_si2i_data_q_s, &b_tele116_si2i_data_q_s);
   fChain->SetBranchAddress("tele116_si2i.data_fpga", &tele116_si2i_data_fpga, &b_tele116_si2i_data_fpga);
   fChain->SetBranchAddress("tele116_si2i.data_MAR", tele116_si2i_data_MAR, &b_tele116_si2i_data_MAR);
   fChain->SetBranchAddress("tele116_csi.energy", &tele116_csi_energy, &b_tele116_csi_energy);
   fChain->SetBranchAddress("tele116_csi.A", &tele116_csi_A, &b_tele116_csi_A);
   fChain->SetBranchAddress("tele116_csi.Z", &tele116_csi_Z, &b_tele116_csi_Z);
   fChain->SetBranchAddress("tele116_csi.nsamples_i", &tele116_csi_nsamples_i, &b_tele116_csi_nsamples_i);
   fChain->SetBranchAddress("tele116_csi.nsamples_q", &tele116_csi_nsamples_q, &b_tele116_csi_nsamples_q);
   fChain->SetBranchAddress("tele116_csi.nsamples_fpga", &tele116_csi_nsamples_fpga, &b_tele116_csi_nsamples_fpga);
   fChain->SetBranchAddress("tele116_csi.data_i_s", &tele116_csi_data_i_s, &b_tele116_csi_data_i_s);
   fChain->SetBranchAddress("tele116_csi.data_q_s", &tele116_csi_data_q_s, &b_tele116_csi_data_q_s);
   fChain->SetBranchAddress("tele116_csi.data_fpga", &tele116_csi_data_fpga, &b_tele116_csi_data_fpga);
   fChain->SetBranchAddress("tele116_csi.data_MAR", tele116_csi_data_MAR, &b_tele116_csi_data_MAR);
   fChain->SetBranchAddress("beam.energy", &beam_energy, &b_beam_energy);
   fChain->SetBranchAddress("beam.A", &beam_A, &b_beam_A);
   fChain->SetBranchAddress("beam.Z", &beam_Z, &b_beam_Z);
   fChain->SetBranchAddress("beam.nsamples_i", &beam_nsamples_i, &b_beam_nsamples_i);
   fChain->SetBranchAddress("beam.nsamples_q", &beam_nsamples_q, &b_beam_nsamples_q);
   fChain->SetBranchAddress("beam.nsamples_fpga", &beam_nsamples_fpga, &b_beam_nsamples_fpga);
   fChain->SetBranchAddress("beam.data_i_s", &beam_data_i_s, &b_beam_data_i_s);
   fChain->SetBranchAddress("beam.data_q_s", beam_data_q_s, &b_beam_data_q_s);
   fChain->SetBranchAddress("beam.data_fpga", &beam_data_fpga, &b_beam_data_fpga);
   fChain->SetBranchAddress("beam.data_MAR", beam_data_MAR, &b_beam_data_MAR);
   fChain->SetBranchAddress("gonio_thetah", &gonio_thetah, &b_gonio_thetah);
   fChain->SetBranchAddress("gonio_thetav", &gonio_thetav, &b_gonio_thetav);
   fChain->SetBranchAddress("xy_position", &xy_position, &b_xy_position);
   fChain->SetBranchAddress("scale_count", &scale_count, &b_scale_count);
   fChain->SetBranchAddress("scale_elapsed", &scale_elapsed, &b_scale_elapsed);
   fChain->SetBranchAddress("scale_approx_rate", &scale_approx_rate, &b_scale_approx_rate);
   fChain->SetBranchAddress("bitpattern", &bitpattern, &b_bitpattern);

   */
}
