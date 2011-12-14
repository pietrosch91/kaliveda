/*
$Id: GetGeneMeanPin.cpp,v 1.1 2009/01/21 11:15:08 franklan Exp $
$Revision: 1.1 $
$Date: 2009/01/21 11:15:08 $
*/

//Created by KVClassFactory on Thu Jun 28 12:08:25 2007
//Author: franklan

#include "GetGeneMeanPin.h"
#include "KVINDRA.h"
#include "KVDetector.h"
#include "KVACQParam.h"
#include "KVBatchSystem.h"
#include "KVINDRADBRun.h"
#include "Riostream.h"

ClassImp(GetGeneMeanPin)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>GetGeneMean</h2>
<h4>Example gene data analysis class</h4>
This class reads the pulser & laser data for each run and does the following:
1. fills a TTree with the data for each detector
2. writes a text file "runxxxx.gene" which contains the mean value for each parameter
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

GetGeneMeanPin::GetGeneMeanPin()
{
   //Default constructor
}

GetGeneMeanPin::~GetGeneMeanPin()
{
   //Destructor
}


//________________________________________________________________
void GetGeneMeanPin::InitAnalysis () 
{
   //Initialisation of e.g. histograms, performed once at beginning of analysis
   //Create the TTree
   if( gBatchSystem ) geneFile = new TFile( Form("%s.root", gBatchSystem->GetJobName()), "recreate" );
   else geneFile = new TFile( "GetPinMean.root", "recreate" );
   
   geneTree = new TTree("geneTree", "Data for pulser & laser events");
   geneTree->Branch("run", &run, "run/I");
   geneTree->Branch("ring", &ring, "ring/I");
   geneTree->Branch("module", &module, "module/I");
// ajout Pin diodes
  geneTree->Branch("pingg_laser", &pingg_laser, "pingg_laser/I");
  geneTree->Branch("pinpg_laser", &pinpg_laser, "pinpg_laser/I");
  geneTree->Branch("pingg_gene", &pingg_gene, "pingg_gene/I");
  geneTree->Branch("pinpg_gene", &pinpg_gene, "pinpg_gene/I");
    
   geneHist = new TH1F("geneHist", "Projection for mean value", 4096, -.5, 4095.5);
}


//________________________________________________________________
void GetGeneMeanPin::InitRun () 
{
   //Initialisation performed at beginning of each run
   //Int RunNumber holds number of current run
   run = gIndra->GetCurrentRunNumber();   
   cout << "Debut lecture run : " << RunNumber << endl;
   
   //set run number for TTree
//   run = RunNumber;
   
   //reset number of events read for run
   runEvents=0;
   nlaser = 0;
   ngene = 0;
}


//________________________________________________________________
Bool_t GetGeneMeanPin::Analysis () 
{
   //Analysis method called for each event
   //Int RunNumber holds number of current run
   //Int EventNumber holds number of current event
   //KVINDRATriggerInfo* TriggerInfo holds info on INDRA trigger for current event
  
//   gIndra->GetACQParams()->Print();
  
      TIter next_par( gIndra->GetACQParams() ); KVACQParam* par=0;
      while( (par = (KVACQParam*)next_par()) ){
               
         TString par_name = par->GetName(); 
         TString par_type; // = "PG", "GG", "R", "L", or "T"
  	 if(par_name.Contains("GG")) par_type = "GG";  // par_type est vide pour les pins  
	 if(par_name.Contains("PG")) par_type = "PG";
         Bool_t pinlas = ( (par_name.BeginsWith("PILA") || par_name.BeginsWith("SI_PIN")) && TriggerInfo->IsLaser());
         Bool_t pingen = ( (par_name.BeginsWith("PILA") || par_name.BeginsWith("SI_PIN")) && TriggerInfo->IsPulser());
 
         if(pinlas || pingen) {
	   ring =18;        
           Int_t ajout=0;
  	   if(pinlas) ajout = 10; if(pingen) ajout = 0;
	
   	   TObjArray* toks = par_name.Tokenize('_'); // split up par_name into parts separated by _
//           Int_t module; // an integer we want to read in the file
           for(int i=0; i<toks->GetEntries(); i++) { //loop over fields
           TString field = ((TObjString*)(*toks)[i])->GetString();
           if( field.IsDigit() ) { // is this a number ?
           module = 2 + ajout + field.Atoi();}} // read integer value written in string
  	   if(par_name.Contains("PIN1")) module = 1 + ajout;  
	   if(par_name.Contains("PIN2")) module = 2 + ajout;

	   if( par->Fired() ){
                           
	     if( pinlas) {
                     nlaser++;
	             pingg_gene = -1;
	             pinpg_gene = -1;
                     if ( par_type=="GG" ) {
		       pingg_laser = par->GetCoderData();   }
                     else if ( par_type=="PG" ) {
		       pinpg_laser = par->GetCoderData();   }                    
	     } 
	     else if( pingen) {
	             ngene++;
	             pingg_laser = -1;
	             pinpg_laser = -1;
                     if ( par_type=="GG" ) {
		       pingg_gene = par->GetCoderData();    }
                     else if ( par_type=="PG" ) {
		       pinpg_gene = par->GetCoderData();    }
                  }  //fin type trigger
                  
//          cout << " Ring module" << ring << module << "pingg_laser" << pingg_laser << endl; 	                  
//          cout <<  " Ring module" <<  ring << module << "pingg_gene" << pingg_gene << endl; 	                  
            
            //fill tree with acquisition parameters for the detector
            geneTree->Fill();
            runEvents++;
              } // fin detector fired
         
	 }  // fin pinlas
	
   }  // fin next_par
   return kTRUE;
}


//________________________________________________________________
void GetGeneMeanPin::EndRun () 
{
   //Method called at end of each run
   //Calculate mean value of each parameter and write in file
   
   cout << "Fin lecture run : " << RunNumber << endl;
   
   cout << "Nombre total de detecteurs touches : " << runEvents << endl;
   cout << "Nombre total de pin laser : " << nlaser << endl;
   cout << "Nombre total de pin gene : " << ngene << endl;
   
   //do nothing if nothing was read i.e. no gene events in run
   if( !runEvents ) return;
   
   //open file for mean values
   cout << "Ouverture du fichier : ";
   // choix du nom suivant le contenu : laser ou gene ou les deux
    TString output;
    ofstream geneRun;
  
  if(ngene > 0 && nlaser == 0) 
  {
      output.Form("run%d.genepin", RunNumber);
      cout << output.Data() << endl;
      //ofstream geneRun;
      geneRun.open( output.Data() );
  }
  else if(ngene == 0 && nlaser > 0) 
  {
       output.Form("run%d.laserpin", RunNumber);
       cout << output.Data() << endl;
       //ofstream geneRun;
       geneRun.open( output.Data() );
  }
  else if(ngene > 0 && nlaser > 0) 
  {
       output.Form("run%d.genelaserpin", RunNumber);
       cout << output.Data() << endl;
       //ofstream geneRun;
       geneRun.open( output.Data() );
  }
     
   
      TIter next_par( gIndra->GetACQParams() ); KVACQParam* par=0;
      while( (par = (KVACQParam*)next_par()) ){
      
           TString par_name = par->GetName(); 
               
           Bool_t pin = ( (par_name.BeginsWith("PILA") || par_name.BeginsWith("SI_PIN")) );
 
           if(pin) {
	     ring =18;        
             Int_t mod=0;
	
	     TObjArray* toks = par_name.Tokenize('_'); // split up par_name into parts separated by _
             for(int i=0; i<toks->GetEntries(); i++) { //loop over fields
             TString field = ((TObjString*)(*toks)[i])->GetString();
             if( field.IsDigit() ) { // is this a number ?
             mod = 2  + field.Atoi();}} // read integer value written in string
             if(par_name.Contains("PIN1")) mod = 1 ;  
  	     if(par_name.Contains("PIN2")) mod = 2 ;
                  
             TString selection; // condition sur C et M
             TString varname;  // nom variable dans l'arbre
	     TString nom;      // nom du paramètre dans le fichier

	     cout << "avant gene : module=  " << mod << endl;
	     
	     if(ngene > 0) 	     {
             selection.Form("ring==%d&&module==%d&&", ring, mod);
             //cout << "gene : module=  " << mod << endl;
                  
             if ( par_name.Contains("GG") ){
		  varname="pingg_gene";	  }
             else if ( par_name.Contains("PG") ) {
		  varname="pinpg_gene";	  }		     
                  
            //cout << "varname " << varname << " varname.Data " << varname.Data() << endl;
            //add selection to exclude events where detector not hit (= -1)
             selection.Append( Form("%s>-1", varname.Data() ) );
             selection.Append( Form("&&%s<4000", varname.Data()) );
            //cout << "selection 2  " << selection << endl;
                  
	     nom.Form("%s_gene",par->GetName());
             //project into histo geneHist  to get mean value
             TString draw = Form("%s >> geneHist", varname.Data());
	     //cout << "draw  " << draw << endl;
             geneTree->Draw( draw.Data(), selection.Data(), "goff" );
             geneRun << nom << "     " << geneHist->GetMean() << endl;
             cout << nom << "     " << geneHist->GetMean() << endl;
     	     //cout << "fin if gene  : module=  " << mod << endl;
	     }  // ngene > 0
	     
	   //cout << "entre gene et laser : module=  " << mod << endl;
  
          if(nlaser > 0) 	     {   
	     mod = mod + 10;
             //cout << "laser : module=  " << mod << endl;
             if ( par_name.Contains("GG") ){
		  varname="pingg_laser";	   }
              else if ( par_name.Contains("PG") ){
		  varname="pinpg_laser";      } 
            //cout << "varname " << varname << " varname.Data " << varname.Data() << endl;
              selection.Form("ring==%d&&module==%d&&", ring, mod);
              selection.Append( Form("%s>-1", varname.Data()) );
             selection.Append( Form("&&%s<4000", varname.Data()) );
            //cout << "selection 2  " << selection << endl;
	      nom.Form("%s_laser",par->GetName());
              TString draw = Form("%s >> geneHist", varname.Data());
	     //cout << "draw  " << draw << endl;
              geneTree->Draw( draw.Data(), selection.Data(), "goff" );
              geneRun << nom << "     " << geneHist->GetMean() << endl;
              cout << nom << "     " << geneHist->GetMean() << endl;
	     } //nlaser > 0
	     
	   } //if pin
	
      }  // while par
           
   //close file
   cout << "Fermeture du fichier : " << output.Data() << endl;
   geneRun.close();   
}


//________________________________________________________________
void GetGeneMeanPin::EndAnalysis () 
{
   //Method called at end of analysis: save/display histograms etc.
   
   //save the tree
   geneFile->Write();
   delete geneFile;
}

