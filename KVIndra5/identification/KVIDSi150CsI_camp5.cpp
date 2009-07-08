//Created by KVClassFactory on Wed Jun 10 16:45:12 2009
//Author: John Frankland,,,

#include "KVIDSi150CsI_camp5.h"
#include "KVINDRAReconNuc.h"

ClassImp(KVIDSi150CsI_camp5)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDSi150CsI_camp5</h2>
<h4>Si(150)-CsI identifications for 5th campaign INDRA data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDSi150CsI_camp5::KVIDSi150CsI_camp5()
{
   // Default constructor
   fZGrid=fZAGrid=0;
   fSi=0;
   fCsI=0;
}

KVIDSi150CsI_camp5::~KVIDSi150CsI_camp5()
{
   // Destructor
}

Bool_t KVIDSi150CsI_camp5::SetIDGrid(KVIDGraph *grid)
{
   // Called by KVIDGridManager::FindGrid in order to set grids for telescope.
   // There should be 2: one with full isotopic identification, one with just Z identification.

	if( !grid->HandlesIDTelescope(this) )
      return kFALSE;

   //get run number from INDRA, if it exists (should do!), otherwise accept
   if (gIndra) {
      Int_t run = gIndra->GetCurrentRunNumber();
      if (!grid->GetRuns().Contains(run)) return kFALSE;
   }

   //the grid is accepted
   fIDGrids->Add(grid);
   if(grid->OnlyZId()) fZGrid = (KVIDZAGrid*)grid;
   else fZAGrid = (KVIDZAGrid*)grid;

   return kFALSE; // make gIDGridManager keep searching!
}

//___________________________________________________________________________________________

void KVIDSi150CsI_camp5::Initialize()
{
   // Initialize telescope for current run.
   // If there is at least 1 grid, we set IsReadyForID = kTRUE
   // "Natural" line widths are calculated for grids.

	fSi=(KVSilicon*)GetDetector(1);
	fSIPG = fSi->GetACQParam("PG");
	fCsI=(KVCsI*)GetDetector(2);
   if( fZAGrid ){
      SetBit(kReadyForID);
      fZAGrid->Initialize();
      if(fZGrid) fZGrid->Initialize();
   }
	else
		ResetBit(kReadyForID);
}

//___________________________________________________________________________________________

Double_t KVIDSi150CsI_camp5::GetIDMapX(Option_t * opt)
{
   //X-coordinate for Si150-CsI identification map is corrected light output of CsI
   return fCsI->GetCorrectedLumiereTotale();
}

//____________________________________________________________________________________

Double_t KVIDSi150CsI_camp5::GetIDMapY(Option_t * opt)
{
   //Y-coordinate for Si150-CsI identification map is raw "PG" coder value of Silicon
   return (Double_t)fSIPG->GetData();
}

//________________________________________________________________________________________//

Bool_t KVIDSi150CsI_camp5::Identify(KVReconstructedNucleus * nuc)
{
    //Particle identification and code setting using identification grids

      KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;
      KVIDGrid* theIdentifyingGrid = 0;

      // try full isotopic identification
      fZAGrid->Identify(GetIDMapX(), GetIDMapY(), irnuc);
      theIdentifyingGrid = fZAGrid;

      if(fZAGrid->GetQualityCode() > KVIDZAGrid::kICODE6 && fZGrid){

          // particle is above Z&A grid: try Z only ID
          fZGrid->Identify(GetIDMapX(), GetIDMapY(), irnuc);
          theIdentifyingGrid = fZGrid;
      }

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

		// set general ID code Si150-CsI
      irnuc->SetIDCode( kIDCode3 );
      return kTRUE;
}


