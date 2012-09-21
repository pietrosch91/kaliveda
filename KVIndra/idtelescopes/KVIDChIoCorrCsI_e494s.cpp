//Created by KVClassFactory on Thu Sep 20 17:24:51 2012
//Author: Ademard Guilain

#include "KVIDChIoCorrCsI_e494s.h"

ClassImp(KVIDChIoCorrCsI_e494s)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDChIoCorrCsI_e494s</h2>
<h4>ChIo-CsI identification with grids for E494S</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDChIoCorrCsI_e494s::KVIDChIoCorrCsI_e494s()
{
   // Default constructor
}
//________________________________________________________________

KVIDChIoCorrCsI_e494s::~KVIDChIoCorrCsI_e494s()
{
   // Destructor
}
//________________________________________________________________

Double_t KVIDChIoCorrCsI_e494s::GetIDMapY(Option_t *opt){
	// This method gives the Y-coordinate in a 2D identification map
	// associated whith the ChIo-CsI identification telescope.
	// The Y-coordinate is the ionisation chamber's current petit gain coder data minus the petit gain pedestal. If the grand gain coder 
	// data is less than 3900 then the petit gain value is calculated
	// from the current grand gain coder data (see KVINDRADetector::GetPGFromGG())
	
     opt = opt; // not used (keeps the compiler quiet)

    if(fChIo){

        if(fChIo->GetGG() < 3900.) return fChIo->GetPGfromGG() - fChIo->GetPedestal("PG");

		return fChIo->GetPG() - fChIo->GetPedestal("PG");

    }

	return 10000.;
}
