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
<h4>INDRA E503/E494s ChIo-Si identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDChIoSiCorr::KVIDChIoSiCorr() :

        fChIo(0),
        fSi(0),
        fChIoCorr(-5.),
        fChIoGG(-5.),
        fChIoPG(-5.),
        fChIoGGPedestal(-5.),
        fChIoPGPedestal(-5.),
        fSiCorr(-5.),
        fSiGG(-5.),
        fSiPG(-5.),
        fSiPGPedestal(-5.),
        fSiGGPedestal(-5.),
        chioSiGrid(0)
{

}

KVIDChIoSiCorr::~KVIDChIoSiCorr() 
{

}

//________________________________________________________________________________________//

void KVIDChIoSiCorr::Initialize() 
{
    fChIoGG = -5.;
    fChIoPG = -5.;
    fChIoGGPedestal = -5.;
    fChIoPGPedestal = -5.;
    fChIoCorr = -5.;

    fSiGG = -5.;
    fSiPG = -5.;
    fSiGGPedestal = -5.;
    fSiPGPedestal = -5.;
    fSiCorr = -5.;

    fChIo = 0;
    fChIo = (KVChIo*) GetDetector(1);

    fSi = 0;
    fSi = (KVSilicon*) GetDetector(2);

    chioSiGrid = 0;
    chioSiGrid = (KVIDGChIoSi*) GetIDGrid();

    if(chioSiGrid){
        chioSiGrid->Initialize();
        SetBit(kReadyForID);
    }else{
        ResetBit(kReadyForID);
    }

}

//________________________________________________________________________________________//

Double_t KVIDChIoSiCorr::GetIDMapX(Option_t *opt) 
{
    Option_t *tmp; tmp = opt; // not used (keeps the compiler quiet)

    fSiCorr = -5.;

    if(fSi){

        fSiPG = fSi->GetPG();
        fSiPGPedestal = fSi->GetPedestal("PG");

        fSiGG = fSi->GetGG();
        fSiGGPedestal = fSi->GetPedestal("GG");

        if(fSiGG < 3900.){

            fSiCorr = fSi->GetPGfromGG(fSiGG) - fSiPGPedestal;

        }else{

            fSiCorr = fSiPG - fSiPGPedestal;
        }
    
    }else{

        return 10000.;
    }

    return fSiCorr;
}

//________________________________________________________________________________________//

Double_t KVIDChIoSiCorr::GetIDMapY(Option_t *opt) 
{
    Option_t *tmp; tmp = opt; // not used (keeps the compiler quiet)

    fChIoCorr = -5.;

    if(fChIo){

        fChIoPG = fChIo->GetPG();
        fChIoPGPedestal = fChIo->GetPedestal("PG");

        fChIoGG = fChIo->GetGG();
        fChIoGGPedestal = fChIo->GetPedestal("GG");

        if(fChIoGG < 3900.){

            fChIo->SetPedestal("GG", 0.);
            fChIoGGPedestal = fChIo->GetPedestal("GG"); // Resets the stored value  
        
            fChIoCorr = fChIo->GetPGfromGG(fChIoGG) - fChIoPGPedestal;
        
        }else{
        
            fChIoCorr = fChIoPG - fChIoPGPedestal;
        }
    
    }else{

        return 10000.;
    }

    return fChIoCorr;

}

//________________________________________________________________________________________//

Bool_t KVIDChIoSiCorr::Identify(KVIdentificationResult * IDR, Double_t x, Double_t y) 
{
    if(!IDR){
        std::cerr << "Error: KVIDChIoSiCorr::Identify: IDR null pointer" << std::endl;
        return kFALSE;
    }

    IDR->SetIDType( GetType() );
    IDR->IDattempted = kTRUE;

    Double_t chio(GetIDMapY(""));
    Double_t si(GetIDMapX(""));

    // should be safe, as it will only be called when kReadyForID is set
    if (chioSiGrid->IsIdentifiable(si,chio)){
        chioSiGrid->Identify(si,chio,IDR);
    }

    Int_t quality = chioSiGrid->GetQualityCode();
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
    if(quality==KVIDGChIoSi::k_RightOfEmaxSi) 
        IDR->SetComment("point to identify has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable");

    return kTRUE;
}
