/*
$Id: KVIDChIoCorrCsI.cpp,v 1.2 2009/04/06 15:21:31 franklan Exp $
$Revision: 1.2 $
$Date: 2009/04/06 15:21:31 $
*/

//Created by KVClassFactory on Mon Mar 30 16:44:34 2009
//Author: John Frankland,,,

#include "KVIDChIoCorrCsI.h"
#include "KVINDRA.h"
#include "KVINDRAReconNuc.h"
#include "KVIdentificationResult.h"

ClassImp(KVIDChIoCorrCsI)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDChIoCorrCsI</h2>
<h4>ChIo-CsI id with grids for INDRA_E503 and INDRA_E494s</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDChIoCorrCsI::KVIDChIoCorrCsI()
{
    // Default constructor

    fGrid = 0;
    fChIo = 0;
    fCsI = 0;

}

KVIDChIoCorrCsI::~KVIDChIoCorrCsI()
{
    // Destructor
}

//___________________________________________________________________________________________

void KVIDChIoCorrCsI::Initialize()
{
    // Initialize telescope for current run.

	fChIo = (KVChIo *) GetDetector(1);
    fChIoGG = -5.;
    fChIoPG = -5.;
    fChIoGGPedestal = -5.;
    fChIoPGPedestal = -5.;
    fChIoCorr = -5.;

	fCsI = (KVCsI *) GetDetector(2);
	fCsIRPedestal = fCsI->GetPedestal("R");
	fCsILPedestal = fCsI->GetPedestal("L");

    fGrid = 0;
    fGrid = (KVIDZAGrid*) GetIDGrid();

    if (fGrid != 0) {

        fGrid->Initialize();
        fGrid->SetOnlyZId(kTRUE);
        SetBit(kReadyForID);

    }else{
        ResetBit(kReadyForID);
    }

}


Double_t KVIDChIoCorrCsI::GetIDMapX(Option_t * opt)
{
    //Calculates current X coordinate for identification.
    //'opt' has no effect.

    Double_t h = (Double_t)fCsI->GetLumiereTotale();

    return h;
}

//__________________________________________________________________________//


Double_t KVIDChIoCorrCsI::GetIDMapY(Option_t *)
{
    Double_t chIo = -5.;

    if(fChIo != 0){

        fChIoGG = fChIo->GetGG();
        fChIoPG = fChIo->GetPG();
        fChIoPGPedestal = fChIo->GetPedestal("PG");
        fChIoGGPedestal = fChIo->GetPedestal("GG");

        /***********************
            if(fChIoGG < 3900.){                 //No pedestal correction for chIoGG

            // For ChIo correlation we don't subtract the pedestal for GG for E503/E494s
            // (Manually set to zero)

            fChIo->SetPedestal("GG", 0.); 
            fChIoGGPedestal = fChIo->GetPedestal("GG"); //resets the value;

            chIo = fChIo->GetPGfromGG(fChIoGG) - fChIoPGPedestal;

        }else{
        ************************/

            chIo = fChIoPG - fChIoPGPedestal;
        //}

    }

    fChIoCorr = chIo;

    return chIo;

}

//________________________________________________________________________________________//

Bool_t KVIDChIoCorrCsI::Identify(KVIdentificationResult* idr, Double_t x, Double_t y)
{
    idr->SetIDType(GetType());
    idr->IDattempted = kTRUE;
	
    Double_t chIoCorr = GetIDMapY("");
    Double_t csiLight = GetIDMapX("");

    fGrid->Identify(csiLight, chIoCorr, idr);

    if(fGrid->GetQualityCode() > KVIDZAGrid::kICODE6 ){
        return kFALSE;
    }

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
        idr->IDcode = kIDCode5;
        return kTRUE;
    }

    if(fGrid->GetQualityCode() > KVIDZAGrid::kICODE3 && fGrid->GetQualityCode() < KVIDZAGrid::kICODE7){
        // if the final quality code is kICODE4, kICODE5 or kICODE6 then this "nucleus"
        // corresponds to a point which is inbetween the lines, i.e. noise
        idr->IDcode = kIDCode10;
        return kTRUE;
    }

    // set general ID code ChIo-CsI
    idr->IDcode = kIDCode4;
    
    return kTRUE;
}
