/*
$Id: GetGeneMean.cpp,v 1.3 2007/06/29 10:46:46 franklan Exp $
$Revision: 1.3 $
$Date: 2007/06/29 10:46:46 $
*/

//Created by KVClassFactory on Thu Jun 28 12:08:25 2007
//Author: franklan

#include "GetGeneMean.h"
#include "KVINDRADetector.h"
#include "KVACQParam.h"
#include "KVBatchSystem.h"
#include "Riostream.h"

ClassImp(GetGeneMean)

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

GetGeneMean::GetGeneMean()
{
   //Default constructor
}

GetGeneMean::~GetGeneMean()
{
   //Destructor
}


//________________________________________________________________
void GetGeneMean::InitAnalysis () 
{
   //Initialisation of e.g. histograms, performed once at beginning of analysis
   //Create the TTree
   if( gBatchSystem ) geneFile = new TFile( Form("%s.root", gBatchSystem->GetJobName()), "recreate" );
   else geneFile = new TFile( "GetGeneMean.root", "recreate" );
   
   geneTree = new TTree("geneTree", "Data for pulser & laser events");
   geneTree->Branch("run", &run, "run/I");
   geneTree->Branch("ring", &ring, "ring/I");
   geneTree->Branch("module", &module, "module/I");
   geneTree->Branch("cigg", &cigg, "cigg/I");
   geneTree->Branch("cipg", &cipg, "cipg/I");
   geneTree->Branch("sipg", &sipg, "sipg/I");
   geneTree->Branch("sigg", &sigg, "sigg/I");
   geneTree->Branch("si75gg", &si75gg, "si75gg/I");
   geneTree->Branch("si75pg", &si75pg, "si75pg/I");
   geneTree->Branch("siligg", &siligg, "siligg/I");
   geneTree->Branch("silipg", &silipg, "silipg/I");
   geneTree->Branch("csir", &csir, "csir/I");
   geneTree->Branch("csil", &csil, "csil/I");
   
   geneHist = new TH1F("geneHist", "Projection for mean value", 4096, -.5, 4095.5);
}


//________________________________________________________________
void GetGeneMean::InitRun () 
{
   //Initialisation performed at beginning of each run
   //Int RunNumber holds number of current run
   
   cout << "Debut lecture run : " << RunNumber << endl;
   
   //set run number for TTree
   run = RunNumber;
   
   //reset number of events read for run
   runEvents=0;
}


//________________________________________________________________
Bool_t GetGeneMean::Analysis () 
{
   //Analysis method called for each event
   //Int RunNumber holds number of current run
   //Int EventNumber holds number of current event
   //KVINDRATriggerInfo* TriggerInfo holds info on INDRA trigger for current event
   
   //loop over fired detectors
   TIter next_det( gIndra->GetListOfDetectors() ); KVINDRADetector* det=0;
   while( (det = (KVINDRADetector*)next_det()) ){
      
      TString det_type = det->GetType(); // = "CI", "SI", "CSI", "SI75" or "SILI"
      
      //only use CsI params if GeneLaser; only use ChIo/Si params if GeneElec
      if( (det_type=="CSI" && TriggerInfo->IsLaser()) || (det_type=="CI" && TriggerInfo->IsPulser())
            || (det_type.BeginsWith("SI") && TriggerInfo->IsPulser()) ){   //include types "SI", "SI75" & "SILI"
         
         if( det->Fired() ){
         
            ring = det->GetRingNumber();
            module = det->GetModuleNumber();
            cigg=cipg=sipg=sigg=si75gg=si75pg=siligg=silipg=csir=csil=-1;
            Bool_t csi = (det_type=="CSI");
            Bool_t si = (det_type=="SI");
            Bool_t si75 = (det_type=="SI75");
            Bool_t sili = (det_type=="SILI");
            Bool_t ci = (det_type=="CI");
            
            //loop over acquisition parameters associated to detector
            TIter next_par( det->GetACQParamList() ); KVACQParam* par=0;
            while( (par = (KVACQParam*)next_par()) ){
               
               TString par_type = par->GetType(); // = "PG", "GG", "R", "L", or "T"
               
               if( par_type != "T" ){ //skip time marker
                  
                  if( csi ) {
                     if ( par_type=="R" ) csir = par->GetCoderData();
                     else if ( par_type=="L" ) csil = par->GetCoderData();
                  } else if( ci ) {
                     if ( par_type=="GG" ) cigg = par->GetCoderData();
                     else if ( par_type=="PG" ) cipg = par->GetCoderData();
                  } else if( si ) {
                     if ( par_type=="GG" ) sigg = par->GetCoderData();
                     else if ( par_type=="PG" ) sipg = par->GetCoderData();
                  } else if( si75 ) {
                     if ( par_type=="GG" ) si75gg = par->GetCoderData();
                     else if ( par_type=="PG" ) si75pg = par->GetCoderData();
                  } else if( sili ) {
                     if ( par_type=="GG" ) siligg = par->GetCoderData();
                     else if ( par_type=="PG" ) silipg = par->GetCoderData();
                  }
                  
               }
               
            }
            
            //fill tree with acquisition parameters for the detector
            geneTree->Fill();
            runEvents++;
         }
         
      }
      
   }
   return kTRUE;
}


//________________________________________________________________
void GetGeneMean::EndRun () 
{
   //Method called at end of each run
   //Calculate mean value of each parameter and write in file
   
   cout << "Fin lecture run : " << RunNumber << endl;
   
   cout << "Nombre total de detecteurs touches : " << runEvents << endl;
   
   //do nothing if nothing was read i.e. no gene events in run
   if( !runEvents ) return;
   
   //open file for mean values
   cout << "Ouverture du fichier : ";
   TString output= Form("run%d.gene", RunNumber);
   cout << output.Data() << endl;
   
   ofstream geneRun;
   geneRun.open( output.Data() );
   //loop over detectors
   TIter next_det( gIndra->GetListOfDetectors() ); KVINDRADetector* det=0;
   while( (det = (KVINDRADetector*)next_det()) ){
      
      TString det_type = det->GetType(); // = "CI", "SI", "CSI", "SI75" or "SILI"
               
            Bool_t csi = (det_type=="CSI");
            Bool_t si = (det_type=="SI");
            Bool_t si75 = (det_type=="SI75");
            Bool_t sili = (det_type=="SILI");
            Bool_t ci = (det_type=="CI");
            
            //loop over acquisition parameters associated to detector
            TIter next_par( det->GetACQParamList() ); KVACQParam* par=0;
            while( (par = (KVACQParam*)next_par()) ){
               
               TString par_type = par->GetType(); // = "PG", "GG", "R", "L", or "T"
               
               if( par_type != "T" ){ //skip time marker
                  
                  TString selection;
                  selection.Form("ring==%d&&module==%d&&", det->GetRingNumber(), det->GetModuleNumber());
                  TString varname;
                  
                  if( csi ) {
                     if ( par_type=="R" ) varname="csir";
                     else if ( par_type=="L" ) varname="csil";
                  } else if( ci ) {
                     if ( par_type=="GG" ) varname="cigg";
                     else if ( par_type=="PG" ) varname="cipg";
                  } else if( si ) {
                     if ( par_type=="GG" ) varname="sigg";
                     else if ( par_type=="PG" ) varname="sipg";
                  } else if( si75 ) {
                     if ( par_type=="GG" ) varname="si75gg";
                     else if ( par_type=="PG" ) varname="si75pg";
                  } else if( sili ) {
                     if ( par_type=="GG" ) varname="siligg";
                     else if ( par_type=="PG" ) varname="silipg";
                  }
                  
                  //add selection to exclude events where detector not hit (= -1)
                  selection.Append( Form("%s>-1", varname.Data()) );
                  
                  //project into histo geneHist  to get mean value
                  TString draw = Form("%s >> geneHist", varname.Data());
                  geneTree->Draw( draw.Data(), selection.Data(), "goff" );
                  geneRun << par->GetName() << "     " << geneHist->GetMean() << endl;
                  cout << par->GetName() << "     " << geneHist->GetMean() << endl;                  
               }
            }
         }   
   //close file
   cout << "Fermeture du fichier : " << output.Data() << endl;
   geneRun.close();   
}


//________________________________________________________________
void GetGeneMean::EndAnalysis () 
{
   //Method called at end of analysis: save/display histograms etc.
   
   //save the tree
   geneFile->Write();
   delete geneFile;
}

