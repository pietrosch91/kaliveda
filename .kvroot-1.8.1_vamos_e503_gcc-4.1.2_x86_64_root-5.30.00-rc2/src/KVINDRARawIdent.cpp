/*
$Id: KVINDRARawIdent.cpp,v 1.2 2008/11/12 16:15:49 franklan Exp $
$Revision: 1.2 $
$Date: 2008/11/12 16:15:49 $
*/

//Created by KVClassFactory on Wed Feb 20 10:52:20 2008
//Author: franklan

#include "KVINDRARawIdent.h"
#include "KVINDRA.h"
#include "KVDataSet.h"
#include "KVINDRADB.h"
#include "KVDataRepositoryManager.h"
#include "KVDataRepository.h"

ClassImp(KVINDRARawIdent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRARawIdent</h2>
<h4>User raw data analysis class</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRARawIdent::KVINDRARawIdent()
{
   // Default constructor
   taskname = "RawIdent";
   datatype = "ident";
}

//________________________________________________________________

void KVINDRARawIdent::InitRun () 
{
   // Creates new ROOT file with TTree for reconstructed/calibrated events.
   // By default this file will be written in the same data repository as the raw data file we are reading.
   // This can be changed by setting the environment variable(s):
   //
   //     RawIdent.DataAnalysisTask.OutputRepository:     [name of repository]
   //     [name of dataset].RawIdent.DataAnalysisTask.OutputRepository:         [name of repository]
   //
   // If no value is set for the current dataset (second variable), the value of the
   // first variable will be used. If neither is defined, the new file will be written in the same repository as
   // the raw file (if possible, i.e. if repository is not remote).

   KVINDRARawDataReconstructor::InitRun();   
      
      // initialise identifications
      gIndra->InitializeIDTelescopes();
   
      // print status of identifications
      gIndra->PrintStatusOfIDTelescopes();
      // print status of calibrations
      gIndra->PrintCalibStatusOfDetectors();
}

//________________________________________________________________

void KVINDRARawIdent::ExtraProcessing () 
{
   // Identify and calibrate reconstructed events with 1 or more particles.
   
         if (recev->GetMult() > 0) {
            recev->IdentifyEvent();
            recev->CalibrateEvent();
         }
}

