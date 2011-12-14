// This class is derived from the KaliVeda class KVIVSelector.
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

void KVIVSelectorTemplate::InitAnalysis(void)
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
 //		KVList *sil_list = gIndra->GetListOfSi();
 //
 //When running in batch mode, the global pointer gBatchSystem gives access
 //to the batch system running the analysis. You can use this to name files
 //according to the jobname, for example:
 //
 //   TFile *my_file;
 //   if( gBatchSystem )  // running in batch
 //      my_file = new TFile( Form("%s.root", gBatchSystem->GetJobName()), "create");
 //   else  // interactive
 //      my_file = new TFile( "KVSelectorTemplate.root", "create");
 }

//_____________________________________
void KVIVSelectorTemplate::InitRun(void)
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
 //     GetEvent()->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);//VEDA id-codes code(i)=2,3,4 and 6 accepted
 //     GetEvent()->AcceptECodes( kECode1|kECode2 );//VEDA ecode(i)=1&2 accepted
 //
 // If you want the angles of particles to be calculated using the
 // centre of each detector (instead of the randomised angles used by default):
 //     GetEvent()->UseMeanAngles();
 //
 //The run number of the currently analysed run is given by
 //   gIndra->GetCurrentRunNumber();

 }

//_____________________________________
Bool_t KVIVSelectorTemplate::Analysis(void)
 {
 // 
 // Analysis method.
 // The event pointer can be retrieved by a call to GetEvent().
 // See KVINDRAReconEvent documentation for the available methods.
 //
 // In order to loop over all correctly identified particles in the event
 // (assuming the corresponding code masks were set in InitRun):
 //		KVINDRAReconNuc* particle;
 //		while ( (particle = GetEvent()->GetNextParticle("ok")) ){
 //            int z = particle->GetZ();
 //            ...etc. etc.
 //     }
 //
 // The reconstructed VAMOS data can be accessed using the following variables:
 //
//    Int_t           EchiM;
//    UShort_t        Echi[];   //[EchiM]
//    UShort_t        EchiNr[];   //[EchiM]
//    Int_t           STRM1;
//    Int_t           STRM2;
//    Int_t           STRM3;
//    Int_t           STRM4;
//    UShort_t        STR1[];   //[STRM1]
//    UShort_t        STR2[];   //[STRM2]
//    UShort_t        STR3[];   //[STRM3]
//    UShort_t        STR4[];   //[STRM4]
//    UShort_t        STRNr1[];   //[STRM1]
//    UShort_t        STRNr2[];   //[STRM2]
//    UShort_t        STRNr3[];   //[STRM3]
//    UShort_t        STRNr4[];   //[STRM4]
//    Int_t           SIEM;
//    UShort_t        SIE[];   //[SIEM]
//    UShort_t        SIENr[];   //[SIEM]
//    Int_t           WCSIEM;
//    UShort_t        WCSIE[];   //[WCSIEM]
//    UShort_t        WCSIENr[];   //[WCSIEM]
//    Int_t           ICSILM;
//    UShort_t        ICSIL[];   //[ICSILM]
//    UShort_t        ICSILNr[];   //[ICSILM]
//    Int_t           ICSIRM;
//    UShort_t        ICSIR[];   //[ICSIRM]
//    UShort_t        ICSIRNr[];   //[ICSIRM]
//    Int_t           ICSITM;
//    UShort_t        ICSIT[];   //[ICSITM]
//    UShort_t        ICSITNr[];   //[ICSITM]
//    Int_t           ISILM;
//    UShort_t        ISIL[];   //[ISILM]
//    UShort_t        ISILNr[];   //[ISILM]
//    Int_t           ISIHM;
//    UShort_t        ISIH[];   //[ISIHM]
//    UShort_t        ISIHNr[];   //[ISIHM]
//    Int_t           ISITM;
//    UShort_t        ISIT[];   //[ISITM]
//    UShort_t        ISITNr[];   //[ISITM]
//    Int_t           ICILM;
//    UShort_t        ICIL[];   //[ICILM]
//    UShort_t        ICILNr[];   //[ICILM]
//    Int_t           ICIHM;
//    UShort_t        ICIH[];   //[ICIHM]
//    UShort_t        ICIHNr[];   //[ICIHM]
//    Int_t           ICITM;
//    UShort_t        ICIT[];   //[ICITM]
//    UShort_t        ICITNr[];   //[ICITM]
//    UShort_t        INCONF;
//    UShort_t        GATCONF;
//    UShort_t        INV_SIE_05;
//    UShort_t        INV_SIE_06;
//    UShort_t        INV_SIE_08;
//    UShort_t        ADC1_4;
//    UShort_t        ADC1_5;
//    UShort_t        ADC1_6;
//    UShort_t        SI_IND_61;
//    UShort_t        CSI_IND_1224;
//    UShort_t        EFIL_1;
//    UShort_t        EFIL_2;
//    UShort_t        TFIL_1;
//    UShort_t        TFIL_2;
//    UShort_t        TSI_HF;
//    UShort_t        TINDRA_SI;
//    UShort_t        TCSI_HF;
//    UShort_t        TINDRA_CSI;
//    UShort_t        TINDRA_HF;
//    UShort_t        TSI_MCP;
//    UShort_t        TMCP_HF;
//    UShort_t        TINDRA_MCP;
//
// For the arrays, the variable in between [] in the adjacent comment field
// gives the number of entries in the array for each event.

 //
 // Enter your code here
 //

 return kTRUE;
 }

//_____________________________________
void KVIVSelectorTemplate::EndRun(void)
 {
 // 
 // Called at the end of each run
 //

 //
 // Enter your code here
 //

 }

//_____________________________________
void KVIVSelectorTemplate::EndAnalysis(void)
 {
 // 
 // Global calculations, saving of histograms in files
 // Called at the end of the analysis
 //

 //
 // Enter your code here
 //

 }
