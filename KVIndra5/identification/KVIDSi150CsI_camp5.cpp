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
   fGrid=0;
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

	if( !grid->HandlesIDTelescope(this) )
      return kFALSE;

   //get run number from INDRA, if it exists (should do!), otherwise accept
   if (gIndra) {
      Int_t run = gIndra->GetCurrentRunNumber();
      if (!grid->GetRuns().Contains(run)) return kFALSE;
   }

   //the grid is accepted
   fIDGrids->Add(grid);
   fGrid = (KVIDZAGrid*)grid;

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
   if( fGrid ){
      SetBit(kReadyForID);
      fGrid->Initialize();
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
   //Particle identification and code setting using identification grid

      KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;

      //perform identification in Si150(PG)-CsI map
      fGrid->Identify(GetIDMapX(), GetIDMapY(), irnuc);
      //set subcode in particle
      SetIDSubCode(irnuc->GetCodes().GetSubCodes(), fGrid->GetQualityCode());

		if(fGrid->GetQualityCode() == KVIDZAGrid::kICODE8){
			// only if the final quality code is kICODE8 do we consider that it is
			// worthwhile looking elsewhere. In all other cases, the particle has been
			// "identified", even if we still don't know its Z and/or A (in this case
			// we consider that we have established that they are unknowable).
			return kFALSE;
		}

		if(fGrid->GetQualityCode() == KVIDZAGrid::kICODE7){
			// if the final quality code is kICODE7 (above last line in grid) then the estimated
			// Z is only a minimum value (Zmin)
			irnuc->SetIDCode( kIDCode5 );
			return kTRUE;
		}

		if(fGrid->GetQualityCode() > KVIDZAGrid::kICODE3 &&
				fGrid->GetQualityCode() < KVIDZAGrid::kICODE7){
			// if the final quality code is kICODE4, kICODE5 or kICODE6 then this "nucleus"
			// corresponds to a point which is inbetween the lines, i.e. noise
			irnuc->SetIDCode( kIDCode10 );
			return kTRUE;
		}

		// set general ID code Si150-CsI
      irnuc->SetIDCode( kIDCode3 );
      return kTRUE;
}


