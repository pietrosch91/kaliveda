/*
$Id: KVIDChIoCorrCsI_e503.cpp,v 1.2 2009/04/06 15:21:31 franklan Exp $
$Revision: 1.2 $
$Date: 2009/04/06 15:21:31 $
*/

//Created by KVClassFactory on Mon Mar 30 16:44:34 2009
//Author: John Frankland,,,

#include "KVIDChIoCorrCsI_e503.h"
#include "KVINDRA.h"
#include "KVINDRAReconNuc.h"
#include "KVIdentificationResult.h"

ClassImp(KVIDChIoCorrCsI_e503)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDChIoCorrCsI_e503</h2>
<h4>ChIo-CsI identification with grids for E503/E494s </h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDChIoCorrCsI_e503::KVIDChIoCorrCsI_e503() 
{

}

KVIDChIoCorrCsI_e503::~KVIDChIoCorrCsI_e503() 
{

}

//___________________________________________________________________________________________

void KVIDChIoCorrCsI_e503::Initialize()
{
    // Initialize telescope for current run.

    fChIoCorr = -5.;
    fChIoGG = -5.;
    fChIoPG = -5.;
    fChIoGGPedestal = -5.;
    fChIoPGPedestal = -5.;

    fCsILight = -5.;
    fCsIRPedestal = -5.;
    fCsILPedestal = -5.;

	fChIo = 0;
    fChIo = (KVChIo *) GetDetector(1);

	fCsI = 0;
    fCsI = (KVCsI *) GetDetector(2);

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

//___________________________________________________________________________________________

Double_t KVIDChIoCorrCsI_e503::GetIDMapX(Option_t * opt) 
{
    Option_t *tmp; tmp = opt; // not used (keeps the compiler quiet)

    fCsILight = fCsI->GetLumiereTotale();

    return fCsILight;
}

//__________________________________________________________________________//

Double_t KVIDChIoCorrCsI_e503::GetIDMapY(Option_t *opt) 
{
    Option_t *tmp; tmp = opt; // not used (keeps the compiler quiet)

    fChIoCorr = -5.;

    if(fChIo != 0){

        fChIoPG = fChIo->GetPG();
        fChIoPGPedestal = fChIo->GetPedestal("PG");

        fChIoGG = fChIo->GetGG();
        fChIoGGPedestal = fChIo->GetPedestal("GG");

        // Disabled for E503 - just returns fChIoPG - fChIoPGPedestal
        //if(fChIoGG < 3900.){
        //    fChIo->SetPedestal("GG", 0.);
        //    fChIoGGPedestal = fChIo->GetPedestal("GG"); // Resets the stored value  
        //
        //    fChIoCorr = fChIo->GetPGfromGG(fChIoGG) - fChIoPGPedestal;
        //
        //}else{
        //
            fChIoCorr = fChIoPG - fChIoPGPedestal;
        //}
    
    }else{

        return 10000.;
    }

    return fChIoCorr;
}

//________________________________________________________________________________________//

Bool_t KVIDChIoCorrCsI_e503::Identify(KVIdentificationResult* idr, Double_t x, Double_t y) 
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
