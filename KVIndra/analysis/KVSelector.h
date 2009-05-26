//////////////////////////////////////////////////////////
//   This class has been automatically generated 
//     (Mon Feb 23 13:13:30 2004 by ROOT version3.10/02)
//   from TChain tree/
//////////////////////////////////////////////////////////


#ifndef KVSelector_h
#define KVSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include "KVINDRAReconEvent.h"
#include "KVINDRA.h"
#include "KV2Body.h"
#include "KVGVList.h"
#include "TStopwatch.h"
#include "TEventList.h"
#include "KVDataSelector.h"
#include "KVDataSet.h"
#include "KVDataBase.h"
#include "KVParticleCondition.h"
#include "KVString.h"
#include "KVLockfile.h"
#include "KVConfig.h"

class KVSelector:public TSelector {

friend class KVDataSet;
friend class KVINDRAReconDataAnalyser;

 protected:

   enum {
      kDeleteGVList = BIT(14),
      kChangeMasses = BIT(15)
   };

   Int_t callnotif;
   Int_t totentry;
   Int_t NbTreeEntry;
//Declaration of leaves types
   KVINDRAReconEvent *data;

//List of branches
   TBranch *b_data;             //!

//List of global variables
   KVGVList *gvlist;            //!

   TStopwatch *fTimer;          //!used to time analysis

#ifdef __WITHOUT_TSELECTOR_LONG64_T
   Int_t fTreeEntry;            //!this is the current TTree entry number, i.e. the argument passed to TSelector::Process(Long64_t entry)
#else
   Long64_t fTreeEntry;         //!this is the current TTree entry number, i.e. the argument passed to TSelector::Process(Long64_t entry)
#endif

   TEventList *fEvtList;        //!this is the current event list.
#ifdef __WITHOUT_TSELECTOR_LONG64_T
   Int_t *fTreeOffset;          //!this is the current TTree offset table
#else
   Long64_t *fTreeOffset;       //!this is the current TTree offset table
#endif
   Int_t fCurrentTreeNumber;    //!This is the current tree number 

   TString fDataSelector;       //! Name of the KVDataSelector
   KVDataSelector *fKVDataSelector;     //! KVDataSelector
   Bool_t needToSelect;         //! tells if one needs to build the TEventList
   Bool_t *fTEVLexist;          //! tells if the TEventList exist for each run
   Bool_t needToCallEndRun;     //! tells if one needs to call EndRun
   KVINDRADBRun *fCurrentRun;   //! current run

   UInt_t fNewMassFormula;//new mass formula to apply to fragments, if required

   KVParticleCondition* fPartCond;//(optional) conditions for selecting particles
   
   static KVString fBranchName; //name of branch which contains KVINDRAReconEvent objects in the TTree
   
    TTree * fChain;             //!pointer to the analyzed TTree or TChain
 
   TTree* fGeneData; //!pointer to tree containing pulser and laser data for run
   TTree* fRawData; //!pointer to tree containing raw data for run
   
   KVINDRAReconEvent **GetEventReference() {
      return &data;
   };

   virtual KVINDRADBRun *GetCurrentRun() const {
      return fCurrentRun;
 };
 
   void SetINDRAReconEventBranchName(const Char_t* br_name){ fBranchName = br_name; };
   void CheckIfINDRAUpdateRequired();
   
   KVLockfile dataselector_lock;//for locking user's data selector file
  
 public:

    KVSelector(TTree * tree = 0);
   virtual ~ KVSelector();

   KVINDRAReconEvent *GetEvent() {
      return data;
   };

   Int_t Version() const {
      return 1;
   };
   
   void Begin(TTree * tree);
   void SlaveBegin(TTree * tree);
   void Init(TTree * tree);
   Bool_t Notify();
#ifdef __WITHOUT_TSELECTOR_LONG64_T
   Bool_t Process(Int_t entry); 
#else
   Bool_t Process(Long64_t entry);
#endif
   void SetOption(const char *option) {
      fOption = option;
   }
   void SetObject(TObject * obj) {
      fObject = obj;
   }
   void SetInputList(TList * input) {
      fInput = input;
   }
   TList *GetOutputList() const {
      return fOutput;
   }
   void SlaveTerminate();
   void Terminate();

	/* user entry points */
   virtual void InitAnalysis() {
   };
   virtual void InitRun() {
   };
   virtual Bool_t Analysis() {
      return kTRUE;
   };
   virtual void EndRun() {
   };
   virtual void EndAnalysis() {
   };

   //handling global variables for analysis
   virtual void SetGVList(KVGVList * list);
   virtual KVGVList *GetGVList(void);
   virtual void AddGV(KVVarGlob *);
   virtual KVVarGlob *AddGV(const Char_t * class_name,
                            const Char_t * name);
   virtual KVVarGlob *GetGV(const Char_t *) const;
   virtual void RecalculateGlobalVariables();

#ifdef __WITHOUT_TSELECTOR_LONG64_T
   virtual Int_t GetTreeEntry() const;
#else
   virtual Long64_t GetTreeEntry() const;
#endif

   virtual void BuildEventList(void);
   virtual Bool_t AtEndOfRun(void);

   virtual void SetDataSelector(const Char_t * dataSel = "") {
      fDataSelector = dataSel;
   }
   virtual const Char_t *GetDataSelector(void) {
      return fDataSelector.Data();
   }

   virtual void LoadDataSelector(void);
   virtual void SaveCurrentDataSelection(void);

   virtual Int_t GetRunNumberFromFileName(const Char_t * fileName);
   virtual const Char_t *GetDataSelectorFileName(void);

   virtual void ChangeFragmentMasses(UInt_t mass_formula);
   
   virtual void SetParticleConditions(const KVParticleCondition&);

   //Return pointer to tree containing pulser and laser events for the current run
   TTree* GetGeneData() { return fGeneData; };
   
   //Return pointer to tree containing raw data for the current run
   TTree* GetRawData() { return fRawData; };
   
   static void Make(const Char_t * kvsname = "MyOwnKVSelector");

   ClassDef(KVSelector, 0);     //Analysis class for TChains of KVINDRAReconEvents
};

#endif

#ifdef KVSelector_cxx
KVSelector::KVSelector(TTree * tree)
{
   //ctor
   fChain=0;
   callnotif = 0;
   gvlist = 0;                  // Global variable list set to nil.
   //create stopwatch
   fTimer = new TStopwatch;
   // event list
   fEvtList = 0;
   fTEVLexist = 0;
   fKVDataSelector = 0;
   fDataSelector = "";
   needToSelect = kFALSE;
   needToCallEndRun = kFALSE;
   fCurrentRun = 0;
   fPartCond = 0;
   fGeneData = 0;
   fRawData = 0;
   data = 0;
   dataselector_lock.SetTimeout( 60 ); // 60-second timeout in case of problems
   dataselector_lock.SetSuspend( 5 ); // suspension after timeout
   dataselector_lock.SetSleeptime( 1 ); // try lock every second
}

KVSelector::~KVSelector()
{
   //dtor
   //delete global variable list if it belongs to us, i.e. if created by a
   //call to GetGVList
   if (TestBit(kDeleteGVList)) {
      delete gvlist;
      gvlist = 0;
      ResetBit(kDeleteGVList);
   }
   delete fTimer;
   SafeDelete(fPartCond);
}

void KVSelector::Init(TTree * tree)
{
            
   //if data!=0, Init has already been called: nothing more needs to be done
   if(data){
      return;
   }
            
   if (tree == 0){
      return;
   }
//   Set branch addresses
   fChain = tree;
   fChain->SetMakeClass(1);

   if (fChain->InheritsFrom("TChain"))
      fTreeOffset = ((TChain *) fChain)->GetTreeOffset();
   else
      fTreeOffset = 0;

   gIndra = (KVINDRA *) fChain->GetCurrentFile()->Get("INDRA");
   fChain->SetBranchAddress( fBranchName.Data() , &data);

// Setting the current run
   fCurrentRun = gIndra->GetCurrentRun();
   
   if (!fCurrentRun) {
      Int_t nrun =
          GetRunNumberFromFileName(fChain->GetCurrentFile()->GetName());
      fCurrentRun = ((KVINDRADB *) gDataBase)->GetRun(nrun);
   }
}

Bool_t KVSelector::Notify()
{
   // Called when loading a new file.
   // Get branch pointers.

   b_data = fChain->GetBranch(fBranchName.Data());
   b_data->SetAutoDelete(kFALSE);

   cout << "Analyse du fichier " << fChain->GetCurrentFile()->GetName()
       << " : " << fChain->GetTree()->GetEntries() << endl;
   NbTreeEntry = (Int_t) fChain->GetTree()->GetEntries();
   fCurrentTreeNumber = fChain->GetTreeNumber();

   needToCallEndRun = kTRUE;

   Int_t nrun =
       GetRunNumberFromFileName(fChain->GetCurrentFile()->GetName());

   if (fEvtList)
      needToSelect = !(fTEVLexist[fCurrentTreeNumber]);
   else
      needToSelect = kFALSE;

   if (needToSelect) {
      if (!fKVDataSelector) {
         LoadDataSelector();
      }
      fKVDataSelector->Reset(nrun);
   }
   //only call if data=0 i.e. if we have just changed runs, not at the
   //beginning of the analysis, where we have just performed SetBranchAddress
   //in Init called by Begin
   if (!data) {
      fChain->SetBranchAddress(fBranchName.Data(), &data);
   }

   cout << endl << " ===================  New Run  =================== " <<
       endl << endl;

   fCurrentRun = gIndra->GetCurrentRun();
   if (!fCurrentRun) {
      fCurrentRun = ((KVINDRADB *) gDataBase)->GetRun(nrun);
   }
   if (fCurrentRun) {
      fCurrentRun->Print();
      if (fCurrentRun->GetSystem()) {
         fCurrentRun->GetSystem()->GetKinematics()->Print();
      }
   } else {
      cout << "     Run = " << nrun << endl;
      cout << " Trigger = " << gIndra->GetTrigger() << endl;
   }

   cout << endl << " ================================================= " <<
       endl << endl;
   
      //for the second & subsequent files to be analysed, this is where the INDRA object
      //is read in from the current run file. we check if the user has requested an update of the
      //identification/calibration parameters.
      CheckIfINDRAUpdateRequired();

	// Retrieving the pointer to the raw data tree
   fRawData = (TTree*) fChain->GetCurrentFile()->Get("RawData");
	// Retrieving the pointer to the gene tree
   fGeneData = (TTree*) fChain->GetCurrentFile()->Get("GeneData");
   //old files
   if(!fGeneData) fGeneData = (TTree*) fChain->GetCurrentFile()->Get("gene");
   if(!fGeneData) {
      cout << "  --> No pulser & laser data for this run !!!" << endl << endl;
   } else {
      cout << "  --> Pulser & laser data tree contains " << fGeneData->GetEntries()
            << " events" << endl << endl;
   }
   
   callnotif++;

   if (callnotif > ((TChain *) fChain)->GetNtrees()) {
      cout << "Appels a Notify trop nombreux ." << endl;
      return kFALSE;
   }

   if (needToSelect) {
      cout << " Building new TEventList : " << fKVDataSelector->
          GetTEventList()->GetName()
          << endl;
   }

   InitRun();                   //user initialisations for run

   return kTRUE;
}


#endif                          // #ifdef KVSelector_cxx
