/*
$Id: KVINDRAUpDater_e475s.cpp,v 1.7 2009/04/15 09:52:45 ebonnet Exp $
$Revision: 1.7 $
$Date: 2009/04/15 09:52:45 $
*/

//Created by KVClassFactory on Tue Sep 18 12:14:51 2007
//Author: Eric Bonnet

#include "KVINDRAUpDater_e475s.h"
#include "KVDBRun.h"
#include "KVDetector.h"
#include "KVCalibrator.h"
#include "KVDBParameterSet.h"
#include "KVINDRA.h"
#include "KVSilicon_e475s.h"
#include "KVChIo_e475s.h"
#include "KVCsI_e475s.h"

#include "TList.h"
#include "TString.h"

ClassImp(KVINDRAUpDater_e475s)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRAUpDater_e475s</h2>
<h4>Sets run parameters for INDRA_e475s dataset</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRAUpDater_e475s::KVINDRAUpDater_e475s()
{
   //Default constructor
}

KVINDRAUpDater_e475s::~KVINDRAUpDater_e475s()
{
   //Destructor
}

void KVINDRAUpDater_e475s::SetCalibParameters(KVDBRun* kvrun)
{
   
	KVRList *param_list = kvrun->GetLinks("Calibrations");

   if (!param_list)
      return;
   if (!param_list->GetSize())
      return;

   //KVDetector *kvd;
   KVDBParameterSet *kvps;
   KVCalibrator *kvc;
   TIter next_ps(param_list);
   TString str;

   // Setting all calibration parameters available
   while ((kvps = (KVDBParameterSet *) next_ps())) {    // boucle sur les parametres
      str = kvps->GetName();
		if (gIndra->GetDetector(str.Data())){
		   
			if ( (kvc = gIndra->GetDetector(str.Data())->GetCalibrator(kvps->GetTitle())) ) {
				gIndra->GetDetector(str.Data())->RemoveCalibrators();
				//delete kvc;
         }
			/*
			if ( gIndra->GetDetector(str.Data())->GetListOfCalibrators() ){
				printf("le detecteur %s a encore une liste de calibrateurs ...\n",str.Data());
				gIndra->GetDetector(str.Data())->GetListOfCalibrators()->Print();
			}
			*/
			/*
			if ( gIndra->GetDetector(str.Data())->GetListOfCalibrators() ) {
				gIndra->GetDetector(str.Data())->RemoveCalibrators();
			}
			*/
			if (gIndra->GetDetector(str.Data())->InheritsFrom("KVSilicon_e475s")) {
				((KVSilicon_e475s *)gIndra->GetDetector(str.Data()))->SetCalibrator(kvps);
           	kvc = ((KVSilicon_e475s *)gIndra->GetDetector(str.Data()))->GetCalibrator(kvps->GetTitle());
				if (kvps->GetParameter(1)>0) kvc->SetStatus(kTRUE); 
         }
			else if (gIndra->GetDetector(str.Data())->InheritsFrom("KVChIo_e475s")){
				((KVChIo_e475s *)gIndra->GetDetector(str.Data()))->SetCalibrator(kvps);
				kvc = ((KVChIo_e475s *)gIndra->GetDetector(str.Data()))->GetCalibrator(kvps->GetTitle());
				if (kvps->GetParameter(1)>0) kvc->SetStatus(kTRUE); 
			}
			else if (gIndra->GetDetector(str.Data())->InheritsFrom("KVCsI_e475s")){
				((KVCsI_e475s *)gIndra->GetDetector(str.Data()))->SetCalibrator(kvps);
				kvc = ((KVCsI_e475s *)gIndra->GetDetector(str.Data()))->GetCalibrator(kvps->GetTitle());
				if (kvps->GetParameter(1)!=0) kvc->SetStatus(kTRUE); 
			}
			else {
				Warning("SetCalibParameters(KVDBRun*)",
         		"Calibrator %s %s not found ! ",
            	kvps->GetName(), kvps->GetTitle());
			}
		}	                      //detector found
   }     	                     //boucle sur les parameters

}
