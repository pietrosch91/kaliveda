/*
$Id: KVIDChIoSiCorr.cpp,v 1.2 2009/04/06 15:20:28 franklan Exp $
$Revision: 1.2 $
$Date: 2009/04/06 15:20:28 $
*/

//Created by KVClassFactory on Mon Dec 10 12:58:26 2007
//Author: franklan

#include "KVIDChIoSiCorr.h"

ClassImp(KVIDChIoSiCorr)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDChIoSiCorr</h2>
<h4>INDRA 5th campaign ChIo-Si identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDChIoSiCorr::KVIDChIoSiCorr()
{
    // Default constructor
}

KVIDChIoSiCorr::~KVIDChIoSiCorr()
{
   // Destructor
}

//________________________________________________________________________________________//

void KVIDChIoSiCorr::Initialize(){

    fChIo = (KVChIo*)GetDetector(1);

    fChIoGG = -5.;
    fChIoPG = -5.;
    fChIoGGPedestal = -5.;
    fChIoPGPedestal = -5.;
    fChIoCorr = -5.;

    fSi = (KVSilicon*)GetDetector(2);
    fSiPGPedestal = fSi->GetPedestal("PG");
    fSiGGPedestal = fSi->GetPedestal("GG");

    ChIoSiGrid = 0;
    ChIoSiGrid = (KVIDGChIoSi *) GetIDGrid();

    if (ChIoSiGrid != 0) {
        ChIoSiGrid->Initialize();
        SetBit(kReadyForID);
    }else{
        ResetBit(kReadyForID);
    }

}

Double_t KVIDChIoSiCorr::GetIDMapX(Option_t *)
{
    Double_t si = -1.;
    Double_t siPG = -1.;
    Double_t siPG_ped = -1.;
    Double_t siGG = -1.;
    Double_t siGG_ped = -1.;

    Int_t ring = -1;
    Int_t module = -1;

    if(fSi != 0){

        string name = fSi->GetName();
        sscanf(name.c_str(), "SI_%02d%02d", &ring, &module);

        if(fSi->GetGG() < 3900.){

            siPG = (Double_t)fSi->GetPG();
            siPG_ped = fSiPGPedestal;

            siGG = (Double_t)fSi->GetGG();
            siGG_ped = fSiGGPedestal;

            si = fSi->GetPGfromGG(siGG) - siPG_ped;

        }else{

            siPG = (Double_t)fSi->GetPG();
            siPG_ped = fSiPGPedestal;

            si = siPG - siPG_ped;
        }

    }

    
   return si;
}

Double_t KVIDChIoSiCorr::GetIDMapY(Option_t *)
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

Bool_t KVIDChIoSiCorr::Identify(KVIdentificationResult * IDR, Double_t x, Double_t y)
{

    IDR->SetIDType( GetType() );
    IDR->IDattempted = kTRUE;

    Double_t chio = GetIDMapY("");
    Double_t si = GetIDMapX("");

    if (ChIoSiGrid->IsIdentifiable(si,chio)){
        ChIoSiGrid->Identify(si,chio,IDR);
    }

    Int_t quality = ChIoSiGrid->GetQualityCode();
    IDR->IDquality = quality;

    // set general ID code
    IDR->IDcode = kIDCode4;
    //if point lies above Zmax line, we give Zmax as Z of particle (real Z is >= Zmax)
    //general ID code = kIDCode5 (Zmin)
    if (quality==KVIDZAGrid::kICODE7)
    {
        IDR->IDcode = kIDCode5;
    }
    //Identified particles with subcode kID_LeftOfBragg are given
    //general ID code kIDCode5 (Zmin).
    if (quality==KVIDGChIoSi::k_LeftOfBragg)
    {
        IDR->IDcode = kIDCode5;
    }
    //unidentifiable particles with subcode kID_BelowSeuilSi are given
    //general ID code kIDCode5 (Zmin) and we estimate Zmin from energy
    //loss in ChIo
    if (quality==KVIDGChIoSi::k_BelowSeuilSi)
    {
        IDR->IDcode = kIDCode5;
        IDR->Z = fChIo->FindZmin();
        IDR->SetComment("point to identify left of Si threshold line (bruit/arret ChIo?)");
    }
    if(quality==KVIDGChIoSi::k_RightOfEmaxSi) IDR->SetComment("point to identify has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable");

    return kTRUE;
}
