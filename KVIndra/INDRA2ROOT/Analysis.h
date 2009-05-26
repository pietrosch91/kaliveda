//////////////////////////////////////////////////////////
//$Id: Analysis.h,v 1.1 2003/10/08 06:42:46 franklan Exp $
//   This class has been automatically generated 
//     (Mon Oct  6 23:44:17 2003 by ROOT version3.10/00)
//   from TTree Arbre/Arbre d'Event
//   found on file: test_evt.root
//////////////////////////////////////////////////////////


#ifndef Analysis_h
#define Analysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

#include "KVINDRAReconEvent.h"

class Analysis : public TSelector {
   public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
//Declaration of leaves types
   KVINDRAReconEvent *data;

//List of branches
   TBranch        *b_data;   //!

   Analysis(TTree *tree=0) { }
   ~Analysis() { }
   void    Begin(TTree *tree);
   void    Init(TTree *tree);
   Bool_t  Notify();
   Bool_t  Process(Int_t entry);
   Bool_t  ProcessCut(Int_t entry);
   void    ProcessFill(Int_t entry);
   void    SetOption(const char *option) { fOption = option; }
   void    SetObject(TObject *obj) { fObject = obj; }
   void    SetInputList(TList *input) {fInput = input;}
   TList  *GetOutputList() const { return fOutput; }
   void    Terminate();
   ClassDef(Analysis,0);
};

#endif

#ifdef Analysis_cxx
void Analysis::Init(TTree *tree)
{
//   Set object pointer
   data = 0;
//   Set branch addresses
   if (tree == 0) return;
   fChain    = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("data",&data);
}

Bool_t Analysis::Notify()
{
   // Called when loading a new file.
   // Get branch pointers.
   b_data = fChain->GetBranch("data");
   return kTRUE;
}

#endif // #ifdef Analysis_cxx

