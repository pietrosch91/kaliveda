/*
$Id: KVIDSi75SiLi_camp5.cpp,v 1.6 2009/04/06 09:28:37 franklan Exp $
$Revision: 1.6 $
$Date: 2009/04/06 09:28:37 $
*/

//Created by KVClassFactory on Mon Oct 29 16:45:49 2007
//Author: franklan

#include "KVIDSi75SiLi_camp5.h"
#include "KVDataSet.h"
#include "KVIDGridManager.h"
#include "KVINDRA.h"
#include "KVINDRAReconNuc.h"

ClassImp(KVIDSi75SiLi_camp5)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDSi75SiLi_camp5</h2>
<h4>Identification in Si75-SiLi telescopes for INDRA_camp5 dataset</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDSi75SiLi_camp5::KVIDSi75SiLi_camp5()
{
   //Default constructor
   fGGgrid = 0;
   fPGgrid = 0;
   fPGZgrid = 0;
}

KVIDSi75SiLi_camp5::~KVIDSi75SiLi_camp5()
{
   //Destructor
}

//___________________________________________________________________________________________//

Bool_t KVIDSi75SiLi_camp5::SetIDGrid(KVIDGraph *grid)
{
   // Called by KVIDGridManager::FindGrid in order to set grids for telescope.
   // 
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
	if( !strcmp(grid->GetVarY(),"SI75_GG") ) fGGgrid = (KVIDZAGrid*)grid;
	else if( !strcmp(grid->GetVarY(),"SI75_PG") ){
		if( grid->OnlyZId() ) fPGZgrid = (KVIDZAGrid*)grid;
		else fPGgrid = (KVIDZAGrid*)grid;
	}
      
   return kFALSE; // make gIDGridManager keep searching!
}

//___________________________________________________________________________________________

void KVIDSi75SiLi_camp5::Initialize()
{
   // Initialize telescope for current run.
   // If there is at least 1 (GG) grid, we set IsReadyForID = kTRUE
   // "Natural" line widths are calculated for grids.
   
   if( fGGgrid ){
      SetBit(kReadyForID);
      fGGgrid->Initialize();
   	if( fPGgrid ) fPGgrid->Initialize();
   	if( fPGZgrid ) fPGZgrid->Initialize();
   }
	else
		ResetBit(kReadyForID);
}

//___________________________________________________________________________________________

Double_t KVIDSi75SiLi_camp5::GetIDMapX(Option_t * opt)
{
   //X-coordinate for Si75-SiLi identification map is raw "PG" coder value for SiLi detector
   return (Double_t) GetDetector(2)->GetACQData("PG");
}

//____________________________________________________________________________________

Double_t KVIDSi75SiLi_camp5::GetIDMapY(Option_t * opt)
{
   //Y-coordinate for Si75-SiLi identification map is raw "GG" or "PG" coder value,
   //depending on 'opt' string.
   return (Double_t) GetDetector(1)->GetACQData(opt);
}

//________________________________________________________________________________________//

Bool_t KVIDSi75SiLi_camp5::Identify(KVReconstructedNucleus * nuc)
{
   //Particle identification and code setting using identification grids.

      KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;
      
      //perform identification in Si75(GG) - SiLi(PG) map
      
      Double_t si75 = GetIDMapY("GG");
      Double_t sili = GetIDMapX();
      
      KVIDGrid* theIdentifyingGrid = 0;
      
      fGGgrid->Identify(sili, si75, irnuc);
      theIdentifyingGrid =(KVIDGrid*)fGGgrid;
		      
      if( fGGgrid->GetQualityCode() > KVIDZAGrid::kICODE6 && fPGgrid ){ //we have to try PG grid (if there is one)
         
         // try Z & A identification in Si75(PG)-SiLi(PG) map
         si75 = GetIDMapY("PG");
         fPGgrid->Identify(sili, si75, irnuc);
         theIdentifyingGrid = (KVIDGrid*)fPGgrid;
         
         if( fPGgrid->GetQualityCode() > KVIDZAGrid::kICODE6 && fPGZgrid ){ //we have to try PGZ grid (if there is one)
            
            fPGZgrid->Identify(sili,si75,irnuc);
         	theIdentifyingGrid = (KVIDGrid*)fPGZgrid;
         }
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
		
		// set general ID code Si75-SiLi
      irnuc->SetIDCode( kIDCode3 );
      return kTRUE;
}


