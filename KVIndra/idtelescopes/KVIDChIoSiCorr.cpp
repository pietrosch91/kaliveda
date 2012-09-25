/*
$Id: KVIDChIoSiCorr.cpp,v 1.2 2009/04/06 15:20:28 franklan Exp $
$Revision: 1.2 $
$Date: 2009/04/06 15:20:28 $
*/

//Created by KVClassFactory on Mon Dec 10 12:58:26 2007
//Author: franklan

#include "KVIDChIoSiCorr.h"
#include "KVINDRACodes.h"

ClassImp(KVIDChIoSiCorr)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDChIoSiCorr</h2>
<h4>INDRA E503/E494s ChIo-Si identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//________________________________________________________________________________________//

Double_t KVIDChIoSiCorr::GetIDMapX(Option_t *opt) 
{
	return GetIDMapXY( (KVINDRADetector *)fsi, opt);
}
//________________________________________________________________________________________//

Double_t KVIDChIoSiCorr::GetIDMapY(Option_t *opt) 
{
	// We set the pedestal to zero in order to not cut the physics
	// as the pedestal GG seems to be too high.
	// PG-GG conversion has been calculated without ChIo GG pedestal.
	
	fchio->SetPedestal("GG",0.);
	return GetIDMapXY( (KVINDRADetector *)fchio, opt);
}
//________________________________________________________________________________________//

Double_t KVIDChIoSiCorr::GetIDMapXY(KVINDRADetector *det, Option_t *opt){
    opt = opt; // not used (keeps the compiler quiet)

    if(det){
        if(det->GetGG() < 3900.) return det->GetPGfromGG() - det->GetPedestal("PG");
        return det->GetPG() - det->GetPedestal("PG");
	}
    return 10000.;
}
//________________________________________________________________________________________//

Bool_t KVIDChIoSiCorr::Identify(KVIdentificationResult *IDR, Double_t x, Double_t y){

	Bool_t identOK = KVIDChIoSi::Identify(IDR,x,y);

	Int_t quality = ChIoSiGrid->GetQualityCode();
	if(quality==KVIDGChIoSi_e494s::k_BelowSeuilChIo)
		IDR->IDcode = kIDCode15;

	return identOK;
}
