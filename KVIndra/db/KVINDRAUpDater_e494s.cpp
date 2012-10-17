//Created by KVClassFactory on Thu Oct 11 18:23:43 2012
//Author: Dijon Aurore

#include "KVINDRAUpDater_e494s.h"
#include "KVMultiDetArray.h"
#include "KVIDGridManager.h"
#include "KVRTGIDManager.h"

ClassImp(KVINDRAUpDater_e494s)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRAUpDater_e494s</h2>
<h4>Class for setting INDRA-VAMOS parameter for each run (e494s/e503 experiment)</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRAUpDater_e494s::KVINDRAUpDater_e494s()
{
   // Default constructor
}
//________________________________________________________________

KVINDRAUpDater_e494s::~KVINDRAUpDater_e494s()
{
   // Destructor
}
//________________________________________________________________

void KVINDRAUpDater_e494s::SetIDGrids(UInt_t run){
	// Set identification grid or identification function (KVTGID) 
	// for all ID telescopes for this run. 
    // The global ID grid manager gIDGridManager is used to set the
    // grids. The static function KVRTGIDManager::SetIDFuncInTelescopes
    //  is used to set the ID functions. First, any previously set grids/functions are removed.
    // Then all grids/functions for current run are set in the associated ID telescopes.

	Info("KVINDRAUpDater_e494s::SetIDGrids","Setting Identification Grids/Functions");
    TIter next_idt(gMultiDetArray->GetListOfIDTelescopes());

    KVIDTelescope  *idt    = NULL;
	KVRTGIDManager *rtgidm = NULL;

    while ((idt = (KVIDTelescope *) next_idt()))
    {
        idt->RemoveGrids();
		if(idt->InheritsFrom("KVRTGIDManager")){
 			rtgidm = (KVRTGIDManager *)idt->IsA()->DynamicCast(KVRTGIDManager::Class(),idt);
 			rtgidm->RemoveAllTGID();
		}

    }
    gIDGridManager->SetGridsInTelescopes(run);
	KVRTGIDManager::SetIDFuncInTelescopes(run);
}
