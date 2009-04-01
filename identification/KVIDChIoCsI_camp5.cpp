/*
$Id: KVIDChIoCsI_camp5.cpp,v 1.1 2009/04/01 09:30:41 franklan Exp $
$Revision: 1.1 $
$Date: 2009/04/01 09:30:41 $
*/

//Created by KVClassFactory on Mon Mar 30 16:44:34 2009
//Author: John Frankland,,,

#include "KVIDChIoCsI_camp5.h"
#include "KVINDRA.h"

ClassImp(KVIDChIoCsI_camp5)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDChIoCsI_camp5</h2>
<h4>ChIo-CsI id with grids for INDRA_camp5</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDChIoCsI_camp5::KVIDChIoCsI_camp5()
{
   // Default constructor
	
	fGGgrid = fPGgrid = 0;
}

KVIDChIoCsI_camp5::~KVIDChIoCsI_camp5()
{
   // Destructor
}

//___________________________________________________________________________________________//

Bool_t KVIDChIoCsI_camp5::SetIDGrid(KVIDGraph *grid)
{
   // Called by KVIDGridManager::FindGrid in order to set grids for telescope.
   // We check the grid handles this ID telescope and is OK for current run number.
   // NB. if this method returns kTRUE, KVIDGridManager::FindGrid stops searching
   // for grids for this telescope. Therefore, we never return kTRUE as there can be
   // several grids for each telescope.
   
	if( !grid->HandlesIDTelescope(this) )   
      return kFALSE;
   
   //get run number from INDRA, if it exists (should do!), otherwise accept
   if (gIndra) {
      Int_t run = gIndra->GetCurrentRunNumber();
      if (!grid->GetRuns().Contains(run)) return kFALSE;
   }
   
   //the grid is accepted
   fIDGrids->Add(grid);
	
	if( !strcmp(grid->GetVarY(),"CHIO-GG") ) fGGgrid = (KVIDZAGrid*)grid;
	else if( !strcmp(grid->GetVarY(),"CHIO-PG") ) fPGgrid = (KVIDZAGrid*)grid;
      
   return kFALSE; // make gIDGridManager keep searching!
}

//___________________________________________________________________________________________

void KVIDChIoCsI_camp5::Initialize()
{
   // Initialize telescope for current run.
   // If there is at least 1 grid, we set fCanIdentify = kTRUE
   // "Natural" line widths are calculated for KVIDZAGrids.
   
   if( fGGgrid ){
      SetBit(kReadyForID);
      fGGgrid->Initialize();
   }
   if( fPGgrid ) fPGgrid->Initialize();
}


