// This class is derived from the KaliVeda class KVFAZIAReader.
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
// E.g. if you want to access informations on the FAZIA multidetector
// through the global pointer gFazia, you must AT LEAST add the line
//   #include "KVFAZIA.h"

ClassImp(KVFAZIAReaderTemplate)

void KVFAZIAReaderTemplate::InitAnalysis(void)
{
   // Declaration of histograms, global variables, etc.
   // Called at the beginning of the analysis
   //

   //
   // Enter your code here
   //
   // To access some information about FAZIA, use the global pointer gFazia
   // e.g. to access the list of silicon detetors
   //    KVList *list = gFazia->GetListOfDetectors();
   //
   // When running in batch mode, the global pointer gBatchSystem gives access
   // to the batch system running the analysis. You can use this to name files
   // according to the jobname, for example:
   //
   //   TFile *my_file;
   //   if( gBatchSystem )  // running in batch
   //      my_file = new TFile( Form("%s.root", gBatchSystem->GetJobName()), "create");
   //   else  // interactive
   //      my_file = new TFile( "KVFAZIAReaderTemplate.root", "create");
}

//_____________________________________
void KVFAZIAReaderTemplate::InitRun(void)
{
   // Initialisations for each run
   // Called at the beginning of each run
   //

   //The run number of the currently analysed run is given by
   //   GetCurrentRunNumber();

}

//_____________________________________
Bool_t KVFAZIAReaderTemplate::Analysis(void)
{
   // // Analysis method.
   // // The event pointer can be retrieved by a call to GetEvent().
   // // See KVFAZIARawEvent documentation for the available methods.
   // // You can uncomment the following to see
   // // Put a small number of events in the KaliVedaGUI
   // // Because printing is heavy
   //
   // TClonesArray* cl = GetEvent()->GetSignals();
   // TIter next_signal(cl);
   // KVSignal* sig = 0;
   // KVSignal* sigc = 0;
   // while ( sig = (KVSignal* )next_signal() )
   // {
   //    //To link a signal to its detector
   //    sig->DeduceFromName();
   //    printf("%s\n",sig->GetDetectorName());
   //
   //    //to draw histogram (don't use in batch mode
   //    sig->Draw("APL");
   //    //duplicate signal to the corresponding daughter class
   //    //This is mandatory to perform the right PSA with the correct parameters
   //    //associated to the current dataset
   //    sigc = sig->ConvertTo(sig->GetType());
   //    //print parameter values for PSA
   //    sigc->Print();
   //    //perform PSA
   //    sigc->TreateSignal();
   //    //print PSA results
   //    sigc->GetPSAResult()->Print();
   //    //access to a given output of the PSA
   //    Double_t val1 = sigc->GetBaseLine();
   //    Double_t val2 = sigc->GetSigmaBaseLine();
   //    Double_t val3 = sigc->GetAmplitude();
   //    printf("BaseLine=%lf, SigmaBaseLine=%lf - Amplitude=%lf\n",val1,val2,val3);
   //    if (sigc)
   //       delete sigc;
   // }
   //
   // // Additionnal values stored reading the acquisition file
   // //    DETTAG
   // //    GTTAG
   // //    results of the FPGA PSA
   //
   // KVNameValueList* lval = GetEvent()->GetValues();
   // lval->Print();
   //

   return kTRUE;
}

//_____________________________________
void KVFAZIAReaderTemplate::EndRun(void)
{
   // Called at the end of each run
   //

   //
   // Enter your code here
   //

}

//_____________________________________
void KVFAZIAReaderTemplate::EndAnalysis(void)
{
   // Global calculations, saving of histograms in files
   // Called at the end of the analysis
   //

   //
   // Enter your code here
   //

}
