/*
$Id: GetPiedPin.cpp,v 1.1 2009/01/21 11:15:08 franklan Exp $

*/


#include "GetPiedPin.h"
#include "KVINDRA.h"
#include "KVDetector.h"
#include "KVACQParam.h"
#include "KVBatchSystem.h"
#include "KVINDRADBRun.h"
#include "Riostream.h"

ClassImp(GetPiedPin)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>GetPiedPin</h2>
<h4>Example gene data analysis class</h4>
This class reads the pulser & laser data for each run and does the following:
1. fills a TTree with the data for each detector
2. writes a text file "runxxxx.gene" which contains the mean value for each parameter
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

GetPiedPin::GetPiedPin()
{
   //Default constructor
}

GetPiedPin::~GetPiedPin()
{
   //Destructor
}


//________________________________________________________________
void GetPiedPin::InitAnalysis () 
{
   //Initialisation of e.g. histograms, performed once at beginning of analysis
   //Create the TTree
   if( gBatchSystem ) geneFile = new TFile( Form("%s.root", gBatchSystem->GetJobName()), "recreate" );
   else geneFile = new TFile( "GetPiedPin.root", "recreate" );
   
   geneTree = new TTree("geneTree", "Data for pin pedestal  events");
   geneTree->Branch("run", &run, "run/I");
   geneTree->Branch("ring", &ring, "ring/I");
   geneTree->Branch("module", &module, "module/I");
// ajout Pin diodes
  geneTree->Branch("pingg", &pingg, "pingg/I");
  geneTree->Branch("pinpg", &pinpg, "pinpg/I");
    
   geneHist = new TH1F("geneHist", "Projection for mean value", 4096, -.5, 4095.5);
}


//________________________________________________________________
void GetPiedPin::InitRun () 
{
   //Initialisation performed at beginning of each run
   //Int RunNumber holds number of current run
  
   
   //set run number for TTree
   run = gIndra->GetCurrentRunNumber();
   cout << "Debut lecture run : " << run << endl;
   
   //reset number of events read for run
   runEvents=0;
}


//________________________________________________________________
Bool_t GetPiedPin::Analysis () 
{
   //Analysis method called for each event
   //Int RunNumber holds number of current run
   //Int EventNumber holds number of current event
   //KVINDRATriggerInfo* TriggerInfo holds info on INDRA trigger for current event
  
//   gIndra->GetACQParams()->Print();
  
      TIter next_par( gIndra->GetACQParams() ); KVACQParam* par=0;
      while( (par = (KVACQParam*)next_par()) ){

         pingg = -1; pinpg = -1;
	
         TString par_name = par->GetName(); 
         Bool_t pin = ( (par_name.BeginsWith("PILA") || par_name.BeginsWith("SI_PIN")) );

	 if(pin) {

	   TString par_type; // = "PG", "GG", "R", "L", or "T"
  	   if(par_name.Contains("GG")) par_type = "GG";  // par_type est vide pour les pins  
	   if(par_name.Contains("PG")) par_type = "PG";
 
	   if(runEvents < 96) {
	     cout << par->GetName() << "  " << par->GetCoderData() << endl;
	   }
	   
	   
	   ring =18;        
	
   	   TObjArray* toks = par_name.Tokenize('_'); // split up par_name into parts separated by _
//           Int_t module; // an integer we want to read in the file
           for(int i=0; i<toks->GetEntries(); i++) { //loop over fields
           TString field = ((TObjString*)(*toks)[i])->GetString();
           if( field.IsDigit() ) { // is this a number ?
           module = 2 + field.Atoi();}} // read integer value written in string
  	   if(par_name.Contains("PIN1")) module = 1 ;
	   if(par_name.Contains("PIN2")) module = 2 ;

//   	   if( par->Fired() ){
                           
               if ( par_type=="GG" ) {
                 pinpg = -1;
		 pingg  = par->GetCoderData();
	       } else if ( par_type=="PG" ) {
		 pingg = -1;
		 pinpg  = par->GetCoderData();
	       }
                  
//          cout << " Ring module" << ring << module << "pingg_laser" << pingg_laser << endl; 	                  
//          cout <<  " Ring module" <<  ring << module << "pingg" << pingg << endl; 	                  
            
            //fill tree with acquisition parameters for the detector
               if (pingg > -1 || pinpg > -1){
		 geneTree->Fill();
                 runEvents++;
	       }
	   
//              } // fin detector fired
         
	 }  // fin pin
	
   }  // fin next_par
   return kTRUE;
}


//________________________________________________________________
void GetPiedPin::EndRun () 
{
   //Method called at end of each run
   //Calculate mean value of each parameter and write in file
   
   cout << "Fin lecture run : " << run << endl;
   
   cout << "Nombre total de detecteurs touches : " << runEvents << endl;
   
   //do nothing if nothing was read i.e. no gene events in run
   if( !runEvents ) return;
   
   //open file for mean values
   cout << "Ouverture du fichier : ";
   TString output= Form("run%d.piedpin", run);
   cout << output.Data() << endl;
   
   ofstream geneRun;
   geneRun.open( output.Data() );
   //loop over pin modules
  
      TIter next_par( gIndra->GetACQParams() ); KVACQParam* par=0;
      while( (par = (KVACQParam*)next_par()) ){
      
           TString par_name = par->GetName(); 
               
           Bool_t pin = ( (par_name.BeginsWith("PILA") || par_name.BeginsWith("SI_PIN")));
 
           if(pin) {
	     ring =18;        
	
	     TObjArray* toks = par_name.Tokenize('_'); // split up par_name into parts separated by _
             for(int i=0; i<toks->GetEntries(); i++) { //loop over fields
               TString field = ((TObjString*)(*toks)[i])->GetString();
               if( field.IsDigit() ) { // is this a number ?
                  module = 2  + field.Atoi();}} // read integer value written in string
             if(par_name.Contains("PIN1")) module = 1 ;  
     	     if(par_name.Contains("PIN2")) module = 2 ;
                  
             TString selection; // condition sur C et M
             selection.Form("ring==%d&&module==%d&&", ring, module);
//             cout << "selection 1  " << selection << endl;

             TString varname;
             if ( par_name.Contains("GG") ) {
		 varname="pingg";
	      } else if ( par_name.Contains("PG") ){
		 varname="pinpg";
	      }
                  
         	  //cout << "varname " << varname << " varname.Data " << varname.Data() << endl;
                  //add selection to exclude events where detector not hit (= -1)
                  selection.Append( Form("%s>-1", varname.Data()) );
                  //cout << "selection 2  " << selection << endl;
                  
	          TString nom;
	          if(pin) nom.Form("%s",par->GetName());
                  //project into histo geneHist  to get mean value
                  TString draw = Form("%s >> geneHist", varname.Data());
	          //cout << "draw  " << draw << endl;
                  geneTree->Draw(draw.Data(), selection.Data(), "goff" );
                  geneRun << nom << "     " << geneHist->GetMean() << endl;
                  cout << nom << "     " << geneHist->GetMean() << endl;
	   } //if pin
	
      }  // while par
           
   //close file
   cout << "Fermeture du fichier : " << output.Data() << endl;
   geneRun.close();   
}


//________________________________________________________________
void GetPiedPin::EndAnalysis () 
{
   //Method called at end of analysis: save/display histograms etc.
   
   //save the tree
   geneFile->Write();
   delete geneFile;
}

