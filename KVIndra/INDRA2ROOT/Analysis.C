#define Analysis_cxx
//$Id: Analysis.C,v 1.2 2003/10/08 16:25:24 franklan Exp $
// The class definition in Analysis.h has been generated automatically
// by the ROOT utility TTree::MakeSelector().
//
// This class is derived from the ROOT class TSelector.
// The following members functions are called by the TTree::Process() functions:
//    Begin():       called everytime a loop on the tree starts,
//                   a convenient place to create your histograms.
//    Notify():      this function is called at the first entry of a new Tree
//                   in a chain.
//    Process():     called for each event. In this function you decide what 
//                   to read and you fill your histograms.
//    Terminate():   called at the end of a loop on the tree,
//                   a convenient place to draw/fit your histograms.
//
//   To use this file, try the following session on your Tree T
//
// Root > T->Process("Analysis.C")
// Root > T->Process("Analysis.C","some options")
// Root > T->Process("Analysis.C+")
//
#include "Analysis.h"
#include "TH2.h"
#include "TH1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRA.h"
#include "TStopwatch.h"

TStopwatch *timer;

void Analysis::Begin(TTree *tree)
{
   // Function called before starting the event loop.
   // Initialize the tree branches.

	timer = new TStopwatch();
	
   Init(tree);

   TString option = GetOption();
   
   data->GetCodeMask()->SetIDMask(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);
   data->GetCodeMask()->SetEMask(kECode1 | kIDCode2);
   
   gIndra->SetTrigger(4);
   
   new TH2F("hmult","Mult all vs OK",61,-.5,60.5,61,-.5,60.5);
   new TH1F("hz","Z all",61,-.5,60.5);
   new TH1F("hzok","Z OK",61,-.5,60.5);
   new TH1F("hmtok","MT OK",4096,-.5,4095.5);
   new TH2F("hzmax","Zmax all vs OK",61,-.5,60.5,61,-.5,60.5);
   new TH2F("hcomp","Ztot-Ptot all",100,0.,50000.,110,-.5,109.5);
   new TH2F("hcompok","Ztot-Ptot OK",100,0.,50000.,110,-.5,109.5);
}

Bool_t Analysis::Process(Int_t entry)
{
   // Processing function. This function is called
   // to process an event. It is the user's responsability to read
   // the corresponding entry in memory (may be just a partial read).
   // Once the entry is in memory one can apply a selection and if the
   // event is selected histograms can be filled. Processing stops
   // when this function returns kFALSE. This function combines the
   // next two functions in one, avoiding to have to maintain state
   // in the class to communicate between these two funtions.
   // You should not implement ProcessCut and ProcessFill if you write
   // this function. This method is used by PROOF.

   return kTRUE;
}

Bool_t Analysis::ProcessCut(Int_t entry)
{
   // Selection function.
   // Entry is the entry number in the current tree.
   // Read only the necessary branches to select entries.
   // Return kFALSE as soon as a bad entry is detected.
   // To read complete event, call fChain->GetTree()->GetEntry(entry).

   fChain->GetTree()->GetEntry(entry);
   
   KVINDRAReconNuc *frag = 0;
   
   while( (frag = data->GetNextParticle("ok")) ){
	   if(frag->GetTimeMarker()<50 || frag->GetTimeMarker()>150) frag->SetIsOK(kFALSE);
   }
  
   return data->IsOK();
}

void Analysis::ProcessFill(Int_t entry)
{
   // Function called for selected entries only.
   // Entry is the entry number in the current tree.
   // Read branches not processed in ProcessCut() and fill histograms.
   // To read complete event, call fChain->GetTree()->GetEntry(entry).

   
   KVINDRAReconNuc *frag = 0;
   
   TH2F* h=0;
   
   h=(TH2F*)gROOT->FindObject("hmult");
   if(h) h->Fill(data->GetMult("ok"), data->GetMult());
   
   h=(TH2F*)gROOT->FindObject("hzmax");
   if(h) h->Fill(data->GetZmax("ok")->GetZ(), data->GetZmax()->GetZ());
   
   h=(TH2F*)gROOT->FindObject("hcomp");
   if(h) h->Fill(data->GetPtot().Mag(), data->GetZtot());
   
   h=(TH2F*)gROOT->FindObject("hcompok");
   if(h) h->Fill(data->GetPtot("ok").Mag(), data->GetZtot("ok"));
   
   TH1F *h1=0;
   h1=(TH1F*)gROOT->FindObject("hz");
   
   while( (frag = data->GetNextParticle()) ){
	   if(h1) h1->Fill(frag->GetZ());
   }
   
   h1=(TH1F*)gROOT->FindObject("hzok");
   TH1F *h2=(TH1F*)gROOT->FindObject("hmtok");
   
   while( (frag = data->GetNextParticle("ok")) ){
	   if(h1) h1->Fill(frag->GetZ());
	   if(h2) h2->Fill(frag->GetTimeMarker());
   }
   
}

void Analysis::Terminate()
{
   // Function called at the end of the event loop.

	timer->Print("m");
	delete timer;
}
