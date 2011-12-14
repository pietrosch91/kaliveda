#define KVIdentRoot_cxx
// The class definition in KVIdentRoot.h has been generated automatically
// by the KaliVeda utility KVSelector::Make().
//
// This class is derived from the KaliVeda class KVSelector.
// The following members functions are called by the TTree::Process() functions:
//    InitRun():       called everytime a run starts
//    EndRun():        called everytime a run ends
//    InitAnalysis():  called at the beginning of the analysis
//                     a convenient place to create your histograms.
//    Analysis():      called for each event. In this function you 
//                     fill your histograms.
//    EndAnalysis():   called at the end of a loop on the tree,
//                     a convenient place to draw/fit your histograms.
//
// Modify these methods as you wish in order to create your analysis class.
// Don't forget that for every class used in the analysis, you must put a
// line '#include' at the beginning of this file.
// E.g. if you want to access informations on the INDRA multidetector
// through the global pointer gIndra, you must AT LEAST add the line
//   #include "KVINDRA.h"
// (in this particular case, if you wish to access some information which
// is specific to a particular campaign, you should use the class declaration
// corresponding to that campaign, e.g.
//       #include "KVINDRA4.h"
// for the 4th campaign).

#include "KVIdentRoot.h"

ClassImp(KVIdentRoot)
//_____________________________________
void KVIdentRoot::InitAnalysis(void)
{
   // 
   // Declaration of histograms, global variables, etc.
   // Called at the beginning of the analysis
   //

   //
   // Enter your code here
   //
   // To access some information about INDRA, use the global pointer gIndra
   // e.g. to access the list of silicon detetors
   //             KVList *sil_list = gIndra->GetListOfSi();
}

//_____________________________________
void KVIdentRoot::InitRun(void)
{
   // 
   // Initialisations for each run
   // Called at the beginning of each run
   //

   //
   // Enter your code here
   //
   // To define which identification/calibration codes you want
   // to use in your analysis:
   //     GetEvent()->GetCodeMask()->SetIDMask(kIDCode2 | kIDCode6);//VEDA id-codes code(i)=2 and 6 accepted
   //     GetEvent()->GetCodeMask()->SetEMask( kECode2 );//VEDA ecode(i)=2 accepted
   //
   // If you want the angles of particles to be calculated using the
   // centre of each detector (instead of the randomised angles used by default):
   //     GetEvent()->UseMeanAngles();

}

//_____________________________________
Bool_t KVIdentRoot::Analysis(void)
{
   // 
   // Analysis method.
   // The event pointer can be retrieved by a call to GetEvent().
   // See KVINDRAReconEvent documentation for the available methods.
   // In order to loop over all correctly identified particles in the event
   // (assuming the corresponding code masks were set in InitRun):
   //             KVINDRAReconNuc* particle;
   //             while ( (particle = GetEvent()->GetNextParticle("ok")) ){
   //            int z = particle->GetZ();
   //            ...etc. etc.
   //     }

   //
   // Enter your code here
   //

   return kTRUE;
}

//_____________________________________
void KVIdentRoot::EndRun(void)
{
   // 
   // Called at the end of each run
   //

   //
   // Enter your code here
   //

}

//_____________________________________
void KVIdentRoot::EndAnalysis(void)
{
   // 
   // Global calculations, saving of histograms in files
   // Called at the end of the analysis
   //

   //
   // Enter your code here
   //

}
