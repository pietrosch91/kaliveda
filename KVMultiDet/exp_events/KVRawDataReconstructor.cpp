//Created by KVClassFactory on Mon Jul 16 15:20:21 2018
//Author: eindra

#include "KVRawDataReconstructor.h"

ClassImp(KVRawDataReconstructor)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVRawDataReconstructor</h2>
<h4>Reconstruct physical events from raw data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVRawDataReconstructor::KVRawDataReconstructor()
   : KVRawDataAnalyser()
{
   // Default constructor
   Info("KVRawDataReconstructor", "Constructed");
}

//____________________________________________________________________________//

KVRawDataReconstructor::~KVRawDataReconstructor()
{
   // Destructor
}

void KVRawDataReconstructor::InitAnalysis()
{

   Info("KVRawDataReconstructor", "InitAnalysis");
}

void KVRawDataReconstructor::InitRun()
{
   Info("KVRawDataReconstructor", "InitRun");

}

Bool_t KVRawDataReconstructor::Analysis()
{
   Info("KVRawDataReconstructor", "Analysis");
   return kTRUE;

}

void KVRawDataReconstructor::EndRun()
{
   Info("KVRawDataReconstructor", "EndRun");

}

void KVRawDataReconstructor::EndAnalysis()
{
   Info("KVRawDataReconstructor", "EndAnalysis");

}

//____________________________________________________________________________//

