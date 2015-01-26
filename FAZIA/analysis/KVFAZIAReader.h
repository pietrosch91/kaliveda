//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Nov 29 14:37:16 2013 by ROOT version 5.34/08
// from TTree tree_PSA_signals/Tree for PSA
// found on file: root://ccxroot.in2p3.fr:1999//hpss/in2p3.fr/group/fazia/LNS_2011/raw/run_4023.root
//////////////////////////////////////////////////////////

#ifndef KVFAZIAReader_h
#define KVFAZIAReader_h

#include <TChain.h>
#include <TSelector.h>
#include <TClonesArray.h>
#include <KVDetectorEvent.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class KVFAZIAReader : public TSelector {
public :
   TTree	*fChain;   //!pointer to the analyzed TTree or TChain
	Int_t fEventNumber;
   Int_t fCurrentRun;
   Int_t fReadEntries;
   TClonesArray* cl;
   KVDetectorEvent* fDetEv;
   
   KVFAZIAReader() : fChain(0) { fCurrentRun = 0; cl=0; fDetEv = new KVDetectorEvent(); }
   virtual ~KVFAZIAReader() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Bool_t  Analysis(){return kTRUE;};
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0);
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
   virtual void 	InitRun(){};
   virtual void 	EndRun(){};
	virtual Int_t GetEventNumber(){return fEventNumber;} 
   virtual Int_t GetCurrentRunNumber() {return fCurrentRun;} 
   virtual Int_t GetNumberOfReadEntries() {return fReadEntries;} 
   KVDetectorEvent* GetDetectorEvent() const { return fDetEv; }
   
   ClassDef(KVFAZIAReader,0);
};

#endif
