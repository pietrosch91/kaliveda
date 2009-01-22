/*
$Id: KVIVRawDataReconstructor.cpp,v 1.5 2009/01/14 15:59:11 franklan Exp $
$Revision: 1.5 $
$Date: 2009/01/14 15:59:11 $
*/

//Created by KVClassFactory on Fri Jun  1 14:46:26 2007
//Author: John Frankland

#include "KVIVRawDataReconstructor.h"

ClassImp(KVIVRawDataReconstructor)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIVRawDataReconstructor</h2>
<h4>Reconstructs raw data from INDRA-VAMOS experiments</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIVRawDataReconstructor::KVIVRawDataReconstructor()
{
   //Default constructor
}

KVIVRawDataReconstructor::~KVIVRawDataReconstructor()
{
   //Destructor
}

//______________________________________________________________________________________//

Bool_t KVIVRawDataReconstructor::Analysis()
{
   //Non-INDRA (i.e. VAMOS seul) events are treated here, i.e. we call tree->Fill()
   //INDRA events are treated by calling KVINDRARawDataReconstructor::Analysis()
   
   if( !IsINDRAEvent() ){
      tree->Fill();
      return kTRUE;
   }
   return KVINDRARawDataReconstructor::Analysis();
}

