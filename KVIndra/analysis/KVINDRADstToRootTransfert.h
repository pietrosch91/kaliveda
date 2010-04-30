/*
$Id: KVINDRADstToRootTransfert.h,v 1.3 2007/05/31 09:59:22 franklan Exp $
$Revision: 1.3 $
$Date: 2007/05/31 09:59:22 $
*/

//Created by KVClassFactory on Fri May  4 15:26:02 2007
//Author: franklan

#ifndef __KVINDRADSTTOROOTTRANSFERT_H
#define __KVINDRADSTTOROOTTRANSFERT_H

#include "KVINDRADataAnalyser.h"
#include "KVINDRATriggerInfo.h"
#include "KVINDRARawDataReader.h"
#include "KVDetectorEvent.h"
#include "KVINDRAReconEvent.h"
#include "KVDataSet.h"
#include "KVDetector.h"
#include "KVIDTelescope.h"
#include "Riostream.h"
#include "TTree.h"

class KVINDRADstToRootTransfert : public KVINDRADataAnalyser
{

   protected:
   
	Int_t mt;
	Double_t de1,de2,de3,de4,de5;
	Double_t canal[16];
	Int_t code_idf[4];
	KVIDTelescope* identifying_telescope;

	Bool_t camp1;
	//set to kTRUE when reading 2nd campaign: => phoswich code is 4, not 2
	Bool_t camp2;
	//set to kTRUE when reading 4th campaign: => ring1 is Si-CsI
	Bool_t camp4;
	//total number of events read from each file - for tests
	Int_t events_in_file;
	//totals of events read and trees filled
	Int_t events_good, events_read;
	//tree for writing events
	TTree* data_tree;

	Int_t fRunNumber;//run number of current file
   Int_t fCampNumber;
	
	void SetCampagneNumber(){
		fCampNumber=-1;
		KVString snom; snom.Form("%s",gDataSet->GetName());
		KVString snom2;
		if (snom.Sscanf("INDRA_camp%d")){
			
			KVNumberList nl = "1 2 4";
			nl.Begin();
			while (!nl.End()){
				Int_t cc = nl.Next();
				snom2.Form("INDRA_camp%d",cc);
				if (snom==snom2) {fCampNumber=cc; break;}
			}
		}
		Info("SetCampagneNumber","%s -> Campagne numero %d",gDataSet->GetName(),fCampNumber);
	}
   
	virtual void ProcessRun();
   
   public:

   KVINDRADstToRootTransfert();
   virtual ~KVINDRADstToRootTransfert();
   
   virtual void InitRun();
   virtual void EndRun();
   
   virtual void SubmitTask();
   
	void DefineSHELLVariables();
   void ReadDST();
	
	KVDetector* Code2and9and10(int ring, int mod);
	KVDetector* Code3(int ring, int mod);
	KVDetector* Code0(int ring, int mod);
	KVDetector* Code1(int ring, int mod);
	KVDetector* Code7(int ring, int mod);
	KVDetector* Code4and5and6and8(int ring, int mod);
	
	void lire_evt(ifstream &f_in,KVINDRAReconEvent *evt);
   
	ClassDef(KVINDRADstToRootTransfert,1)//Analysis of raw INDRA data
};

#endif
