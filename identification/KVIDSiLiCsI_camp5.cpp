/*
$Id: KVIDSiLiCsI_camp5.cpp,v 1.2 2009/04/03 14:40:45 franklan Exp $
$Revision: 1.2 $
$Date: 2009/04/03 14:40:45 $
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
      
   return kFALSE; // make gIDGridManager keep searching!
}


//___________________________________________________________________________________________

void KVIDSiLiCsI_camp5::Initialize()
{
   // Initialize telescope for current run.
   // Pointers to grids for run are set, and if there is at least 1 (GG) grid,
   // we set fCanIdentify = kTRUE
   // "Natural" line widths are calculated for KVIDZAGrids.
   
   fGGgrid = (KVIDZAGrid*)GetIDGrid("GG");
   if( fGGgrid ){
      SetBit(kReadyForID);
      fGGgrid->Initialize();
   }
   fPGgrid = (KVIDZAGrid*)GetIDGrid("PG");
   if( fPGgrid ) fPGgrid->Initialize();
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
   //Particle identification and code setting using identification grids

      KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;
      
      //perform identification in SiLi(GG) - CsI(R) map
      
      Double_t sili = GetIDMapY("GG");
      Double_t csir = GetIDMapX();
      
      KVIDGrid* theIdentifyingGrid = 0;
      
      fGGgrid->Identify(csir, sili, irnuc);
      
      if( fGGgrid->GetQualityCode() > 5 && fPGgrid ){ // any code > 5 means we have to try PG grid (if there is one)
         
         // try Z & A identification in SiLi(PG)-CsI(R) map
         sili = GetIDMapY("PG");
         fPGgrid->Identify(sili, csir, irnuc);
         
         if( fPGgrid->GetQualityCode() > 5 ){
            
               // complete and utter failure
               //set subcode 8 in particle
               SetIDSubCode(irnuc->GetCodes().GetSubCodes(), KVIDZAGrid::kICODE8);
					
         }
         else
         {
            theIdentifyingGrid = (KVIDGrid*)fPGgrid;
         }
      }
      else
      {
         theIdentifyingGrid =(KVIDGrid*)fGGgrid;
      }

      //set subcode in particle
      SetIDSubCode(irnuc->GetCodes().GetSubCodes(), theIdentifyingGrid->GetQualityCode());

      //ID totally unsuccessful if ICode=8
      if (theIdentifyingGrid->GetQualityCode() == KVIDZAGrid::kICODE8)
         return kFALSE;

      //ID should be attempted in preceding telescope if ICode=6 or 7
      if (theIdentifyingGrid->GetQualityCode() == KVIDZAGrid::kICODE6
          || theIdentifyingGrid->GetQualityCode() == KVIDZAGrid::kICODE7)
         return kFALSE;

      return kTRUE;
}


