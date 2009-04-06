/*
$Id: KVIDSiLiCsI_camp5.cpp,v 1.3 2009/04/06 09:28:58 franklan Exp $
$Revision: 1.3 $
$Date: 2009/04/06 09:28:58 $
*/

//Created by KVClassFactory on Mon Mar 16 09:50:33 2009
//Author: John Frankland,,,

#include "KVIDSiLiCsI_camp5.h"
#include "KVIDGridManager.h"
#include "KVINDRA.h"
#include "KVINDRAReconNuc.h"

ClassImp(KVIDSiLiCsI_camp5)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDSiLiCsI_camp5</h2>
<h4>SiLi-CsI identification for INDRA_camp5 dataset</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDSiLiCsI_camp5::KVIDSiLiCsI_camp5()
{
   // Default constructor
	fGGgrid=0;
	fPGgrid=0;
}

KVIDSiLiCsI_camp5::~KVIDSiLiCsI_camp5()
{
   // Destructor
}

//___________________________________________________________________________________________//

Bool_t KVIDSiLiCsI_camp5::SetIDGrid(KVIDGraph *grid)
{
   // Called by KVIDGridManager::FindGrid in order to set grids for telescope.
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
	if( !strcmp(grid->GetVarY(),"SILI_GG") ) fGGgrid = (KVIDZAGrid*)grid;
	else if( !strcmp(grid->GetVarY(),"SILI_PG") ) fPGgrid = (KVIDZAGrid*)grid;
      
   return kFALSE; // make gIDGridManager keep searching!
}


//___________________________________________________________________________________________

void KVIDSiLiCsI_camp5::Initialize()
{
   // Initialize telescope for current run.
   // Pointers to grids for run are set, and if there is at least 1 (GG) grid,
   // we set IsReadyForID = kTRUE
   
   if( fGGgrid ){
      SetBit(kReadyForID);
      fGGgrid->Initialize();
   	if( fPGgrid ) fPGgrid->Initialize();
   }
   else
		ResetBit(kReadyForID);
}

//___________________________________________________________________________________________

Double_t KVIDSiLiCsI_camp5::GetIDMapX(Option_t * opt)
{
   //X-coordinate for SiLi-CsI identification map is raw "R" coder value for CsI detector
   return (Double_t) GetDetector(2)->GetACQData("R");
}

//____________________________________________________________________________________

Double_t KVIDSiLiCsI_camp5::GetIDMapY(Option_t * opt)
{
   //Y-coordinate for SiLi-CsI identification map is raw "GG" or "PG" coder value,
   //depending on 'opt' string.
   return (Double_t) GetDetector(1)->GetACQData(opt);
}

//________________________________________________________________________________________//

Bool_t KVIDSiLiCsI_camp5::Identify(KVReconstructedNucleus * nuc)
{
   //Particle identification and code setting using identification grids.

      KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;
      
      //perform identification in SiLi(GG) - CsI(R) map
      
      Double_t sili = GetIDMapY("GG");
      Double_t csir = GetIDMapX();
      
      KVIDGrid* theIdentifyingGrid = 0;
      
      fGGgrid->Identify(csir, sili, irnuc);
      theIdentifyingGrid =(KVIDGrid*)fGGgrid;
		      
      if( fGGgrid->GetQualityCode() > KVIDZAGrid::kICODE6 && fPGgrid ){ //we have to try PG grid (if there is one)
         
         // try Z & A identification in SiLi(PG)-CsI(R) map
         sili = GetIDMapY("PG");
         fPGgrid->Identify(csir, sili, irnuc);
         theIdentifyingGrid = (KVIDGrid*)fPGgrid;
		}

      //set subcode in particle
      SetIDSubCode(irnuc->GetCodes().GetSubCodes(), theIdentifyingGrid->GetQualityCode());
		
		if(theIdentifyingGrid->GetQualityCode() == KVIDZAGrid::kICODE8){
			// only if the final quality code is kICODE8 do we consider that it is
			// worthwhile looking elsewhere. In all other cases, the particle has been
			// "identified", even if we still don't know its Z and/or A (in this case
			// we consider that we have established that they are unknowable).
			return kFALSE;
		}
		
		if(theIdentifyingGrid->GetQualityCode() == KVIDZAGrid::kICODE7){
			// if the final quality code is kICODE7 (above last line in grid) then the estimated 
			// Z is only a minimum value (Zmin)
			irnuc->SetIDCode( kIDCode5 );
			return kTRUE;
		}
			
		if(theIdentifyingGrid->GetQualityCode() > KVIDZAGrid::kICODE3 &&
				theIdentifyingGrid->GetQualityCode() < KVIDZAGrid::kICODE7){
			// if the final quality code is kICODE4, kICODE5 or kICODE6 then this "nucleus"
			// corresponds to a point which is inbetween the lines, i.e. noise
			irnuc->SetIDCode( kIDCode10 );
			return kTRUE;
		}
		
		// set general ID code SiLi-CsI
      irnuc->SetIDCode( kIDCode3 );
      return kTRUE;
}


