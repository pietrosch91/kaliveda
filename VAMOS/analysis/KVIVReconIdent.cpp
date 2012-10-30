/*
$Id: KVIVReconIdent.cpp,v 1.3 2007/11/21 11:22:59 franklan Exp $
$Revision: 1.3 $
$Date: 2007/11/21 11:22:59 $
*/

//Created by KVClassFactory on Thu Jun  7 13:52:25 2007
//Author: John Frankland

#include "KVIVReconIdent.h"
#include "Analysisv.h"
#include "LogFile.h"
#include "KVBatchSystem.h"
#include "KVDataSet.h"

using namespace std;

ClassImp(KVIVReconIdent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIVReconIdent</h2>
<h4>INDRA identification and calibration, VAMOS trajectory reconstruction,
calibration and identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIVReconIdent::KVIVReconIdent()
{
   //Default constructor
}

KVIVReconIdent::~KVIVReconIdent()
{
   //Destructor
}

void KVIVReconIdent::InitAnalysis(void)
{
   //Create VAMOS Analysisv object
   fLogV = new LogFile;
   if( gBatchSystem ) fLogV->Open( Form( "%s_vamos.log", gBatchSystem->GetJobName() ) );
   else fLogV->Open("Calibration_vamos.log");
   //get VAMOS calibrator for current dataset
   fAnalyseV = Analysisv::NewAnalyser( gDataSet->GetName(), fLogV );
}

//_____________________________________
void KVIVReconIdent::InitRun(void)
{
   //Connect VAMOS branches in input and output trees

   KVINDRAReconIdent::InitRun();
   fAnalyseV->OpenOutputTree(fIdentTree);
   fAnalyseV->outAttach();
   fAnalyseV->CreateHistograms();
   fAnalyseV->OpenInputTree(fChain->GetTree());
   fAnalyseV->inAttach();
   

   ifstream in;   
   Int_t  run1, run2;
    
   //Float_t  thetavam,brho;
   //Float_t  brhorun = 0.;
   //Float_t  thetavamrun = 0.;
   TString sline;
   double brho,thetavam,brhorun,thetavamrun;
     
   // NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW
   // NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW
   // NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW
   // NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW
   //reading brho and thetavamos values
   if(!gDataSet->OpenDataSetFile("brho.dat",in))
  {
     fLogV->Log << "Could not open brho.dat file !!!" << endl;
     return;
  }
  else 
  {
   fLogV->Log << "Reading brho.dat!!!" <<endl;
   while(!in.eof()){
       sline.ReadLine(in);
       if(!in.eof()){
	   if (!sline.BeginsWith("#")){
	     sscanf(sline.Data(),"%d %d %lf %lf", &run1, &run2, &brho, &thetavam);
	     if((run1<gIndra->GetCurrentRunNumber() && gIndra->GetCurrentRunNumber()<run2) || gIndra->GetCurrentRunNumber()==run2 || gIndra->GetCurrentRunNumber()==run1)
	     	{
		brhorun = double(brho);
		thetavamrun = double(thetavam);
		fAnalyseV->SetBrhoRef(brhorun);
   		fAnalyseV->SetAngleVamos(thetavamrun);
		}
		//else file->Log<<"Mauvais numéro de run!!!"<<endl;		
	   }
         }
       }
     }
   in.close();
      
   fLogV->Log<<"-----------"<<endl;
   fLogV->Log<<"RUN "<<   gIndra->GetCurrentRunNumber()<<endl;
   fLogV->Log<<"Brho	"<<fAnalyseV->GetBrhoRef()<<endl;
   fLogV->Log<<"ThetaV	"<<fAnalyseV->GetAngleVamos()<<endl;
   fLogV->Log<<"-----------"<<endl;   
   // NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW
   // NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW
   // NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW
   // NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW NEW
   
}

//_____________________________________
Bool_t KVIVReconIdent::Analysis(void)
{
   //Identification of INDRA events and ident/reconstruction of VAMOS data

   fEventNumber = GetEvent()->GetNumber();
   if (GetEvent()->GetMult() > 0) {
      GetEvent()->IdentifyEvent();
      GetEvent()->CalibrateEvent();
   }
   fAnalyseV->Treat();
   fIdentTree->Fill();
   fAnalyseV->FillHistograms();
   return kTRUE;
}

//_____________________________________
void KVIVReconIdent::EndAnalysis(void)
{
   delete fLogV;
}
