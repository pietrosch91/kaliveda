//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Nov 29 14:37:16 2013 by ROOT version 5.34/08
// from TTree tree_PSA_signals/Tree for PSA
// found on file: root://ccxroot.in2p3.fr:1999//hpss/in2p3.fr/group/fazia/LNS_2011/raw/run_4023.root
//////////////////////////////////////////////////////////

#ifndef KVFAZIARawDataReader_LNS_2011_h
#define KVFAZIARawDataReader_LNS_2011_h

#include <KVFAZIARawDataReader.h>
#include "TClonesArray.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class KVFAZIARawDataReader_LNS_2011 : public KVFAZIARawDataReader {
public :
   
   // Declaration of leaf types
   /*
   Float_t         tele111_si1_energy;
   Float_t         tele111_si1_A;
   Float_t         tele111_si1_Z;
   UInt_t          tele111_si1_nsamples_i;
   UInt_t          tele111_si1_nsamples_q;
   UInt_t          tele111_si1_nsamples_fpga;
   Short_t         tele111_si1_data_i_s[1024];   //[tele111_si1.nsamples_i]
   Short_t         tele111_si1_data_q_s[2048];   //[tele111_si1.nsamples_q]
   Short_t         tele111_si1_data_fpga[4];   //[tele111_si1.nsamples_fpga]
   Int_t           tele111_si1_data_MAR[3];
   Float_t         tele111_si2_energy;
   Float_t         tele111_si2_A;
   Float_t         tele111_si2_Z;
   UInt_t          tele111_si2_nsamples_i;
   UInt_t          tele111_si2_nsamples_q;
   UInt_t          tele111_si2_nsamples_fpga;
   Short_t         tele111_si2_data_i_s[1024];   //[tele111_si2.nsamples_i]
   Short_t         tele111_si2_data_q_s[2048];   //[tele111_si2.nsamples_q]
   Short_t         tele111_si2_data_fpga[4];   //[tele111_si2.nsamples_fpga]
   Int_t           tele111_si2_data_MAR[3];
   Float_t         tele111_csi_energy;
   Float_t         tele111_csi_A;
   Float_t         tele111_csi_Z;
   UInt_t          tele111_csi_nsamples_i;
   UInt_t          tele111_csi_nsamples_q;
   UInt_t          tele111_csi_nsamples_fpga;
   Short_t         tele111_csi_data_i_s[1];   //[tele111_csi.nsamples_i]
   Short_t         tele111_csi_data_q_s[3074];   //[tele111_csi.nsamples_q]
   Short_t         tele111_csi_data_fpga[1];   //[tele111_csi.nsamples_fpga]
   Int_t           tele111_csi_data_MAR[3];
   Float_t         tele112_si1_energy;
   Float_t         tele112_si1_A;
   Float_t         tele112_si1_Z;
   UInt_t          tele112_si1_nsamples_i;
   UInt_t          tele112_si1_nsamples_q;
   UInt_t          tele112_si1_nsamples_fpga;
   Short_t         tele112_si1_data_i_s[1024];   //[tele112_si1.nsamples_i]
   Short_t         tele112_si1_data_q_s[2048];   //[tele112_si1.nsamples_q]
   Short_t         tele112_si1_data_fpga[4];   //[tele112_si1.nsamples_fpga]
   Int_t           tele112_si1_data_MAR[3];
   Float_t         tele112_SCT_energy;
   Float_t         tele112_SCT_A;
   Float_t         tele112_SCT_Z;
   UInt_t          tele112_SCT_nsamples_i;
   UInt_t          tele112_SCT_nsamples_q;
   UInt_t          tele112_SCT_nsamples_fpga;
   Short_t         tele112_SCT_data_i_s[1];   //[tele112_SCT.nsamples_i]
   Short_t         tele112_SCT_data_q_s[2049];   //[tele112_SCT.nsamples_q]
   Short_t         tele112_SCT_data_fpga[4];   //[tele112_SCT.nsamples_fpga]
   Int_t           tele112_SCT_data_MAR[3];
   Float_t         tele112_iSCT_energy;
   Float_t         tele112_iSCT_A;
   Float_t         tele112_iSCT_Z;
   UInt_t          tele112_iSCT_nsamples_i;
   UInt_t          tele112_iSCT_nsamples_q;
   UInt_t          tele112_iSCT_nsamples_fpga;
   Short_t         tele112_iSCT_data_i_s[1026];   //[tele112_iSCT.nsamples_i]
   Short_t         tele112_iSCT_data_q_s[1];   //[tele112_iSCT.nsamples_q]
   Short_t         tele112_iSCT_data_fpga[1];   //[tele112_iSCT.nsamples_fpga]
   Int_t           tele112_iSCT_data_MAR[3];
   Float_t         tele113_si1_energy;
   Float_t         tele113_si1_A;
   Float_t         tele113_si1_Z;
   UInt_t          tele113_si1_nsamples_i;
   UInt_t          tele113_si1_nsamples_q;
   UInt_t          tele113_si1_nsamples_fpga;
   Short_t         tele113_si1_data_i_s[1024];   //[tele113_si1.nsamples_i]
   Short_t         tele113_si1_data_q_s[2048];   //[tele113_si1.nsamples_q]
   Short_t         tele113_si1_data_fpga[4];   //[tele113_si1.nsamples_fpga]
   Int_t           tele113_si1_data_MAR[3];
   Float_t         tele113_si2_energy;
   Float_t         tele113_si2_A;
   Float_t         tele113_si2_Z;
   UInt_t          tele113_si2_nsamples_i;
   UInt_t          tele113_si2_nsamples_q;
   UInt_t          tele113_si2_nsamples_fpga;
   Short_t         tele113_si2_data_i_s[1024];   //[tele113_si2.nsamples_i]
   Short_t         tele113_si2_data_q_s[2048];   //[tele113_si2.nsamples_q]
   Short_t         tele113_si2_data_fpga[4];   //[tele113_si2.nsamples_fpga]
   Int_t           tele113_si2_data_MAR[3];
   Float_t         tele113_csi_energy;
   Float_t         tele113_csi_A;
   Float_t         tele113_csi_Z;
   UInt_t          tele113_csi_nsamples_i;
   UInt_t          tele113_csi_nsamples_q;
   UInt_t          tele113_csi_nsamples_fpga;
   Short_t         tele113_csi_data_i_s[1];   //[tele113_csi.nsamples_i]
   Short_t         tele113_csi_data_q_s[3074];   //[tele113_csi.nsamples_q]
   Short_t         tele113_csi_data_fpga[1];   //[tele113_csi.nsamples_fpga]
   Int_t           tele113_csi_data_MAR[3];
   Float_t         tele114_si1_energy;
   Float_t         tele114_si1_A;
   Float_t         tele114_si1_Z;
   UInt_t          tele114_si1_nsamples_i;
   UInt_t          tele114_si1_nsamples_q;
   UInt_t          tele114_si1_nsamples_fpga;
   Short_t         tele114_si1_data_i_s[1024];   //[tele114_si1.nsamples_i]
   Short_t         tele114_si1_data_q_s[2048];   //[tele114_si1.nsamples_q]
   Short_t         tele114_si1_data_fpga[4];   //[tele114_si1.nsamples_fpga]
   Int_t           tele114_si1_data_MAR[3];
   Float_t         tele114_si2_energy;
   Float_t         tele114_si2_A;
   Float_t         tele114_si2_Z;
   UInt_t          tele114_si2_nsamples_i;
   UInt_t          tele114_si2_nsamples_q;
   UInt_t          tele114_si2_nsamples_fpga;
   Short_t         tele114_si2_data_i_s[1024];   //[tele114_si2.nsamples_i]
   Short_t         tele114_si2_data_q_s[2048];   //[tele114_si2.nsamples_q]
   Short_t         tele114_si2_data_fpga[4];   //[tele114_si2.nsamples_fpga]
   Int_t           tele114_si2_data_MAR[3];
   Float_t         tele114_csi_energy;
   Float_t         tele114_csi_A;
   Float_t         tele114_csi_Z;
   UInt_t          tele114_csi_nsamples_i;
   UInt_t          tele114_csi_nsamples_q;
   UInt_t          tele114_csi_nsamples_fpga;
   Short_t         tele114_csi_data_i_s[1];   //[tele114_csi.nsamples_i]
   Short_t         tele114_csi_data_q_s[3075];   //[tele114_csi.nsamples_q]
   Short_t         tele114_csi_data_fpga[1];   //[tele114_csi.nsamples_fpga]
   Int_t           tele114_csi_data_MAR[3];
   Float_t         tele115_si1_energy;
   Float_t         tele115_si1_A;
   Float_t         tele115_si1_Z;
   UInt_t          tele115_si1_nsamples_i;
   UInt_t          tele115_si1_nsamples_q;
   UInt_t          tele115_si1_nsamples_fpga;
   Short_t         tele115_si1_data_i_s[1024];   //[tele115_si1.nsamples_i]
   Short_t         tele115_si1_data_q_s[2048];   //[tele115_si1.nsamples_q]
   Short_t         tele115_si1_data_fpga[4];   //[tele115_si1.nsamples_fpga]
   Int_t           tele115_si1_data_MAR[3];
   Float_t         tele115_si2_energy;
   Float_t         tele115_si2_A;
   Float_t         tele115_si2_Z;
   UInt_t          tele115_si2_nsamples_i;
   UInt_t          tele115_si2_nsamples_q;
   UInt_t          tele115_si2_nsamples_fpga;
   Short_t         tele115_si2_data_i_s[1024];   //[tele115_si2.nsamples_i]
   Short_t         tele115_si2_data_q_s[2048];   //[tele115_si2.nsamples_q]
   Short_t         tele115_si2_data_fpga[4];   //[tele115_si2.nsamples_fpga]
   Int_t           tele115_si2_data_MAR[3];
   Float_t         tele115_csi_energy;
   Float_t         tele115_csi_A;
   Float_t         tele115_csi_Z;
   UInt_t          tele115_csi_nsamples_i;
   UInt_t          tele115_csi_nsamples_q;
   UInt_t          tele115_csi_nsamples_fpga;
   Short_t         tele115_csi_data_i_s[1];   //[tele115_csi.nsamples_i]
   Short_t         tele115_csi_data_q_s[3074];   //[tele115_csi.nsamples_q]
   Short_t         tele115_csi_data_fpga[1];   //[tele115_csi.nsamples_fpga]
   Int_t           tele115_csi_data_MAR[3];
   Float_t         tele116_si1q_energy;
   Float_t         tele116_si1q_A;
   Float_t         tele116_si1q_Z;
   UInt_t          tele116_si1q_nsamples_i;
   UInt_t          tele116_si1q_nsamples_q;
   UInt_t          tele116_si1q_nsamples_fpga;
   Short_t         tele116_si1q_data_i_s[1];   //[tele116_si1q.nsamples_i]
   Short_t         tele116_si1q_data_q_s[2050];   //[tele116_si1q.nsamples_q]
   Short_t         tele116_si1q_data_fpga[1];   //[tele116_si1q.nsamples_fpga]
   Int_t           tele116_si1q_data_MAR[3];
   Float_t         tele116_si1i_energy;
   Float_t         tele116_si1i_A;
   Float_t         tele116_si1i_Z;
   UInt_t          tele116_si1i_nsamples_i;
   UInt_t          tele116_si1i_nsamples_q;
   UInt_t          tele116_si1i_nsamples_fpga;
   Short_t         tele116_si1i_data_i_s[1026];   //[tele116_si1i.nsamples_i]
   Short_t         tele116_si1i_data_q_s[1];   //[tele116_si1i.nsamples_q]
   Short_t         tele116_si1i_data_fpga[1];   //[tele116_si1i.nsamples_fpga]
   Int_t           tele116_si1i_data_MAR[3];
   Float_t         tele116_si2q_energy;
   Float_t         tele116_si2q_A;
   Float_t         tele116_si2q_Z;
   UInt_t          tele116_si2q_nsamples_i;
   UInt_t          tele116_si2q_nsamples_q;
   UInt_t          tele116_si2q_nsamples_fpga;
   Short_t         tele116_si2q_data_i_s[1];   //[tele116_si2q.nsamples_i]
   Short_t         tele116_si2q_data_q_s[2050];   //[tele116_si2q.nsamples_q]
   Short_t         tele116_si2q_data_fpga[1];   //[tele116_si2q.nsamples_fpga]
   Int_t           tele116_si2q_data_MAR[3];
   Float_t         tele116_si2i_energy;
   Float_t         tele116_si2i_A;
   Float_t         tele116_si2i_Z;
   UInt_t          tele116_si2i_nsamples_i;
   UInt_t          tele116_si2i_nsamples_q;
   UInt_t          tele116_si2i_nsamples_fpga;
   Short_t         tele116_si2i_data_i_s[1026];   //[tele116_si2i.nsamples_i]
   Short_t         tele116_si2i_data_q_s[1];   //[tele116_si2i.nsamples_q]
   Short_t         tele116_si2i_data_fpga[1];   //[tele116_si2i.nsamples_fpga]
   Int_t           tele116_si2i_data_MAR[3];
   Float_t         tele116_csi_energy;
   Float_t         tele116_csi_A;
   Float_t         tele116_csi_Z;
   UInt_t          tele116_csi_nsamples_i;
   UInt_t          tele116_csi_nsamples_q;
   UInt_t          tele116_csi_nsamples_fpga;
   Short_t         tele116_csi_data_i_s[1];   //[tele116_csi.nsamples_i]
   Short_t         tele116_csi_data_q_s[3074];   //[tele116_csi.nsamples_q]
   Short_t         tele116_csi_data_fpga[1];   //[tele116_csi.nsamples_fpga]
   Int_t           tele116_csi_data_MAR[3];
   Float_t         beam_energy;
   Float_t         beam_A;
   Float_t         beam_Z;
   UInt_t          beam_nsamples_i;
   UInt_t          beam_nsamples_q;
   UInt_t          beam_nsamples_fpga;
   Short_t         beam_data_i_s[1];   //[beam.nsamples_i]
   Short_t         beam_data_q_s[1539];   //[beam.nsamples_q]
   Short_t         beam_data_fpga[1];   //[beam.nsamples_fpga]
   Int_t           beam_data_MAR[3];
   Double_t        gonio_thetah;
   Double_t        gonio_thetav;
   Double_t        xy_position;
   Double_t        scale_count;
   Double_t        scale_elapsed;
   Double_t        scale_approx_rate;
   UInt_t          bitpattern;
	
   // List of branches
   TBranch        *b_tele111_si1_energy;   //!
   TBranch        *b_tele111_si1_A;   //!
   TBranch        *b_tele111_si1_Z;   //!
   TBranch        *b_tele111_si1_nsamples_i;   //!
   TBranch        *b_tele111_si1_nsamples_q;   //!
   TBranch        *b_tele111_si1_nsamples_fpga;   //!
   TBranch        *b_tele111_si1_data_i_s;   //!
   TBranch        *b_tele111_si1_data_q_s;   //!
   TBranch        *b_tele111_si1_data_fpga;   //!
   TBranch        *b_tele111_si1_data_MAR;   //!
   TBranch        *b_tele111_si2_energy;   //!
   TBranch        *b_tele111_si2_A;   //!
   TBranch        *b_tele111_si2_Z;   //!
   TBranch        *b_tele111_si2_nsamples_i;   //!
   TBranch        *b_tele111_si2_nsamples_q;   //!
   TBranch        *b_tele111_si2_nsamples_fpga;   //!
   TBranch        *b_tele111_si2_data_i_s;   //!
   TBranch        *b_tele111_si2_data_q_s;   //!
   TBranch        *b_tele111_si2_data_fpga;   //!
   TBranch        *b_tele111_si2_data_MAR;   //!
   TBranch        *b_tele111_csi_energy;   //!
   TBranch        *b_tele111_csi_A;   //!
   TBranch        *b_tele111_csi_Z;   //!
   TBranch        *b_tele111_csi_nsamples_i;   //!
   TBranch        *b_tele111_csi_nsamples_q;   //!
   TBranch        *b_tele111_csi_nsamples_fpga;   //!
   TBranch        *b_tele111_csi_data_i_s;   //!
   TBranch        *b_tele111_csi_data_q_s;   //!
   TBranch        *b_tele111_csi_data_fpga;   //!
   TBranch        *b_tele111_csi_data_MAR;   //!
   TBranch        *b_tele112_si1_energy;   //!
   TBranch        *b_tele112_si1_A;   //!
   TBranch        *b_tele112_si1_Z;   //!
   TBranch        *b_tele112_si1_nsamples_i;   //!
   TBranch        *b_tele112_si1_nsamples_q;   //!
   TBranch        *b_tele112_si1_nsamples_fpga;   //!
   TBranch        *b_tele112_si1_data_i_s;   //!
   TBranch        *b_tele112_si1_data_q_s;   //!
   TBranch        *b_tele112_si1_data_fpga;   //!
   TBranch        *b_tele112_si1_data_MAR;   //!
   TBranch        *b_tele112_SCT_energy;   //!
   TBranch        *b_tele112_SCT_A;   //!
   TBranch        *b_tele112_SCT_Z;   //!
   TBranch        *b_tele112_SCT_nsamples_i;   //!
   TBranch        *b_tele112_SCT_nsamples_q;   //!
   TBranch        *b_tele112_SCT_nsamples_fpga;   //!
   TBranch        *b_tele112_SCT_data_i_s;   //!
   TBranch        *b_tele112_SCT_data_q_s;   //!
   TBranch        *b_tele112_SCT_data_fpga;   //!
   TBranch        *b_tele112_SCT_data_MAR;   //!
   TBranch        *b_tele112_iSCT_energy;   //!
   TBranch        *b_tele112_iSCT_A;   //!
   TBranch        *b_tele112_iSCT_Z;   //!
   TBranch        *b_tele112_iSCT_nsamples_i;   //!
   TBranch        *b_tele112_iSCT_nsamples_q;   //!
   TBranch        *b_tele112_iSCT_nsamples_fpga;   //!
   TBranch        *b_tele112_iSCT_data_i_s;   //!
   TBranch        *b_tele112_iSCT_data_q_s;   //!
   TBranch        *b_tele112_iSCT_data_fpga;   //!
   TBranch        *b_tele112_iSCT_data_MAR;   //!
   TBranch        *b_tele113_si1_energy;   //!
   TBranch        *b_tele113_si1_A;   //!
   TBranch        *b_tele113_si1_Z;   //!
   TBranch        *b_tele113_si1_nsamples_i;   //!
   TBranch        *b_tele113_si1_nsamples_q;   //!
   TBranch        *b_tele113_si1_nsamples_fpga;   //!
   TBranch        *b_tele113_si1_data_i_s;   //!
   TBranch        *b_tele113_si1_data_q_s;   //!
   TBranch        *b_tele113_si1_data_fpga;   //!
   TBranch        *b_tele113_si1_data_MAR;   //!
   TBranch        *b_tele113_si2_energy;   //!
   TBranch        *b_tele113_si2_A;   //!
   TBranch        *b_tele113_si2_Z;   //!
   TBranch        *b_tele113_si2_nsamples_i;   //!
   TBranch        *b_tele113_si2_nsamples_q;   //!
   TBranch        *b_tele113_si2_nsamples_fpga;   //!
   TBranch        *b_tele113_si2_data_i_s;   //!
   TBranch        *b_tele113_si2_data_q_s;   //!
   TBranch        *b_tele113_si2_data_fpga;   //!
   TBranch        *b_tele113_si2_data_MAR;   //!
   TBranch        *b_tele113_csi_energy;   //!
   TBranch        *b_tele113_csi_A;   //!
   TBranch        *b_tele113_csi_Z;   //!
   TBranch        *b_tele113_csi_nsamples_i;   //!
   TBranch        *b_tele113_csi_nsamples_q;   //!
   TBranch        *b_tele113_csi_nsamples_fpga;   //!
   TBranch        *b_tele113_csi_data_i_s;   //!
   TBranch        *b_tele113_csi_data_q_s;   //!
   TBranch        *b_tele113_csi_data_fpga;   //!
   TBranch        *b_tele113_csi_data_MAR;   //!
   TBranch        *b_tele114_si1_energy;   //!
   TBranch        *b_tele114_si1_A;   //!
   TBranch        *b_tele114_si1_Z;   //!
   TBranch        *b_tele114_si1_nsamples_i;   //!
   TBranch        *b_tele114_si1_nsamples_q;   //!
   TBranch        *b_tele114_si1_nsamples_fpga;   //!
   TBranch        *b_tele114_si1_data_i_s;   //!
   TBranch        *b_tele114_si1_data_q_s;   //!
   TBranch        *b_tele114_si1_data_fpga;   //!
   TBranch        *b_tele114_si1_data_MAR;   //!
   TBranch        *b_tele114_si2_energy;   //!
   TBranch        *b_tele114_si2_A;   //!
   TBranch        *b_tele114_si2_Z;   //!
   TBranch        *b_tele114_si2_nsamples_i;   //!
   TBranch        *b_tele114_si2_nsamples_q;   //!
   TBranch        *b_tele114_si2_nsamples_fpga;   //!
   TBranch        *b_tele114_si2_data_i_s;   //!
   TBranch        *b_tele114_si2_data_q_s;   //!
   TBranch        *b_tele114_si2_data_fpga;   //!
   TBranch        *b_tele114_si2_data_MAR;   //!
   TBranch        *b_tele114_csi_energy;   //!
   TBranch        *b_tele114_csi_A;   //!
   TBranch        *b_tele114_csi_Z;   //!
   TBranch        *b_tele114_csi_nsamples_i;   //!
   TBranch        *b_tele114_csi_nsamples_q;   //!
   TBranch        *b_tele114_csi_nsamples_fpga;   //!
   TBranch        *b_tele114_csi_data_i_s;   //!
   TBranch        *b_tele114_csi_data_q_s;   //!
   TBranch        *b_tele114_csi_data_fpga;   //!
   TBranch        *b_tele114_csi_data_MAR;   //!
   TBranch        *b_tele115_si1_energy;   //!
   TBranch        *b_tele115_si1_A;   //!
   TBranch        *b_tele115_si1_Z;   //!
   TBranch        *b_tele115_si1_nsamples_i;   //!
   TBranch        *b_tele115_si1_nsamples_q;   //!
   TBranch        *b_tele115_si1_nsamples_fpga;   //!
   TBranch        *b_tele115_si1_data_i_s;   //!
   TBranch        *b_tele115_si1_data_q_s;   //!
   TBranch        *b_tele115_si1_data_fpga;   //!
   TBranch        *b_tele115_si1_data_MAR;   //!
   TBranch        *b_tele115_si2_energy;   //!
   TBranch        *b_tele115_si2_A;   //!
   TBranch        *b_tele115_si2_Z;   //!
   TBranch        *b_tele115_si2_nsamples_i;   //!
   TBranch        *b_tele115_si2_nsamples_q;   //!
   TBranch        *b_tele115_si2_nsamples_fpga;   //!
   TBranch        *b_tele115_si2_data_i_s;   //!
   TBranch        *b_tele115_si2_data_q_s;   //!
   TBranch        *b_tele115_si2_data_fpga;   //!
   TBranch        *b_tele115_si2_data_MAR;   //!
   TBranch        *b_tele115_csi_energy;   //!
   TBranch        *b_tele115_csi_A;   //!
   TBranch        *b_tele115_csi_Z;   //!
   TBranch        *b_tele115_csi_nsamples_i;   //!
   TBranch        *b_tele115_csi_nsamples_q;   //!
   TBranch        *b_tele115_csi_nsamples_fpga;   //!
   TBranch        *b_tele115_csi_data_i_s;   //!
   TBranch        *b_tele115_csi_data_q_s;   //!
   TBranch        *b_tele115_csi_data_fpga;   //!
   TBranch        *b_tele115_csi_data_MAR;   //!
   TBranch        *b_tele116_si1q_energy;   //!
   TBranch        *b_tele116_si1q_A;   //!
   TBranch        *b_tele116_si1q_Z;   //!
   TBranch        *b_tele116_si1q_nsamples_i;   //!
   TBranch        *b_tele116_si1q_nsamples_q;   //!
   TBranch        *b_tele116_si1q_nsamples_fpga;   //!
   TBranch        *b_tele116_si1q_data_i_s;   //!
   TBranch        *b_tele116_si1q_data_q_s;   //!
   TBranch        *b_tele116_si1q_data_fpga;   //!
   TBranch        *b_tele116_si1q_data_MAR;   //!
   TBranch        *b_tele116_si1i_energy;   //!
   TBranch        *b_tele116_si1i_A;   //!
   TBranch        *b_tele116_si1i_Z;   //!
   TBranch        *b_tele116_si1i_nsamples_i;   //!
   TBranch        *b_tele116_si1i_nsamples_q;   //!
   TBranch        *b_tele116_si1i_nsamples_fpga;   //!
   TBranch        *b_tele116_si1i_data_i_s;   //!
   TBranch        *b_tele116_si1i_data_q_s;   //!
   TBranch        *b_tele116_si1i_data_fpga;   //!
   TBranch        *b_tele116_si1i_data_MAR;   //!
   TBranch        *b_tele116_si2q_energy;   //!
   TBranch        *b_tele116_si2q_A;   //!
   TBranch        *b_tele116_si2q_Z;   //!
   TBranch        *b_tele116_si2q_nsamples_i;   //!
   TBranch        *b_tele116_si2q_nsamples_q;   //!
   TBranch        *b_tele116_si2q_nsamples_fpga;   //!
   TBranch        *b_tele116_si2q_data_i_s;   //!
   TBranch        *b_tele116_si2q_data_q_s;   //!
   TBranch        *b_tele116_si2q_data_fpga;   //!
   TBranch        *b_tele116_si2q_data_MAR;   //!
   TBranch        *b_tele116_si2i_energy;   //!
   TBranch        *b_tele116_si2i_A;   //!
   TBranch        *b_tele116_si2i_Z;   //!
   TBranch        *b_tele116_si2i_nsamples_i;   //!
   TBranch        *b_tele116_si2i_nsamples_q;   //!
   TBranch        *b_tele116_si2i_nsamples_fpga;   //!
   TBranch        *b_tele116_si2i_data_i_s;   //!
   TBranch        *b_tele116_si2i_data_q_s;   //!
   TBranch        *b_tele116_si2i_data_fpga;   //!
   TBranch        *b_tele116_si2i_data_MAR;   //!
   TBranch        *b_tele116_csi_energy;   //!
   TBranch        *b_tele116_csi_A;   //!
   TBranch        *b_tele116_csi_Z;   //!
   TBranch        *b_tele116_csi_nsamples_i;   //!
   TBranch        *b_tele116_csi_nsamples_q;   //!
   TBranch        *b_tele116_csi_nsamples_fpga;   //!
   TBranch        *b_tele116_csi_data_i_s;   //!
   TBranch        *b_tele116_csi_data_q_s;   //!
   TBranch        *b_tele116_csi_data_fpga;   //!
   TBranch        *b_tele116_csi_data_MAR;   //!
   TBranch        *b_beam_energy;   //!
   TBranch        *b_beam_A;   //!
   TBranch        *b_beam_Z;   //!
   TBranch        *b_beam_nsamples_i;   //!
   TBranch        *b_beam_nsamples_q;   //!
   TBranch        *b_beam_nsamples_fpga;   //!
   TBranch        *b_beam_data_i_s;   //!
   TBranch        *b_beam_data_q_s;   //!
   TBranch        *b_beam_data_fpga;   //!
   TBranch        *b_beam_data_MAR;   //!
   TBranch        *b_gonio_thetah;   //!
   TBranch        *b_gonio_thetav;   //!
   TBranch        *b_xy_position;   //!
   TBranch        *b_scale_count;   //!
   TBranch        *b_scale_elapsed;   //!
   TBranch        *b_scale_approx_rate;   //!
   TBranch        *b_bitpattern;   //!
	*/
   KVFAZIARawDataReader_LNS_2011() : KVFAZIARawDataReader() { }
   virtual ~KVFAZIARawDataReader_LNS_2011() { }
   
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   
   virtual void    InitRun();
   
   virtual void    SlaveTerminate();
   virtual void    Terminate();
	
   ClassDef(KVFAZIARawDataReader_LNS_2011,0);
};

#endif
