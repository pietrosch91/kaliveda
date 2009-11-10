/*
$Id: KVIDSiLiCsI_camp5.cpp,v 1.4 2009/04/06 15:23:58 franklan Exp $
$Revision: 1.4 $
$Date: 2009/04/06 15:23:58 $
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
	fSiLi = fCsI = 0;
}

KVIDSiLiCsI_camp5::~KVIDSiLiCsI_camp5()
{
   // Destructor
}

//___________________________________________________________________________________________

void KVIDSiLiCsI_camp5::Initialize()
{
   // Initialize telescope for current run.
   // Pointers to grids for run are set, and if there is at least 1 (GG) grid,
   // we set IsReadyForID = kTRUE

	fSiLi = GetDetector(1);
	fCsI = GetDetector(2);
	fGGgrid=fPGgrid=0;
	TIter next(fIDGrids); KVIDGrid*grid=0;
	while( (grid=(KVIDGrid*)next()) ){
        if( !strcmp(grid->GetVarY(),"SILI_GG") ) fGGgrid = (KVIDZAGrid*)grid;
        else if( !strcmp(grid->GetVarY(),"SILI_PG") ) fPGgrid = (KVIDZAGrid*)grid;
	}
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
   return (Double_t) fCsI->GetACQData("R");
}

//____________________________________________________________________________________

Double_t KVIDSiLiCsI_camp5::GetIDMapY(Option_t * opt)
{
   //Y-coordinate for SiLi-CsI identification map is raw "GG" or "PG" coder value,
   //depending on 'opt' string.
   return (Double_t) fSiLi->GetACQData(opt);
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


