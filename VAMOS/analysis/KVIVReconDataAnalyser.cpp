//Created by KVClassFactory on Mon Feb 25 09:01:43 2013
//Author: Guilain ADEMARD

#include "KVIVReconDataAnalyser.h"
#include "KVVAMOS.h"

ClassImp(KVIVReconDataAnalyser)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIVReconDataAnalyser</h2>
<h4>For analysing reconstructed INDRA+VAMOS data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIVReconDataAnalyser::KVIVReconDataAnalyser()
{
   // Default constructor
}
//________________________________________________________________

KVIVReconDataAnalyser::~KVIVReconDataAnalyser()
{
   // Destructor
}
//________________________________________________________________

void KVIVReconDataAnalyser::preAnalysis()
{
	// Read and set raw data for the current reconstructed event.
	// Initialize VAMOS spectrometer before starting the analysis
	// of the new event.
	KVINDRAReconDataAnalyser::preAnalysis();
	if(gVamos) gVamos->Initialize();
}
