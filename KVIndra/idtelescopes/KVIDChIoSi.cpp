/***************************************************************************
                          KVIDChIoSi.cpp  -  description
                             -------------------
    begin                : Fri Feb 20 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVIDChIoSi.h"
#include "KVIDGChIoSi.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRACodes.h"
#include "KVINDRA.h"

ClassImp(KVIDChIoSi)
/////////////////////////////////////////////////////////////////////////////
//KVIDChIoSi
//
//Identification in ChIo-Si matrices of INDRA
//
//Identification subcodes are written in bits 12-15 of KVIDSubCodeManager
//(see KVINDRACodes). They correspond to the subcodes of the KVIDGChIoSi
//identification grid class (see corresponding class description).
/////////////////////////////////////////////////////////////////////////////

KVIDChIoSi::KVIDChIoSi()
{
    fIDCode = kIDCode_ChIoSi;
    fZminCode = kIDCode_ArretChIo;
    fECode = kECode1;
    SetSubCodeManager(4, 15);
    ChIoSiGrid = 0;
    fchio=fsi=0;
}

KVIDChIoSi::~KVIDChIoSi()
{
}

//________________________________________________________________________________________//

Bool_t KVIDChIoSi::Identify(KVReconstructedNucleus * nuc)
{
    //Particle identification and code setting using identification grid KVIDGChIoSi

    //identification
    Double_t chio = GetIDMapY();
    Double_t si = GetIDMapX();

    if (ChIoSiGrid->IsIdentifiable(si,chio))
        ChIoSiGrid->Identify(si,chio,nuc);
    Int_t quality = ChIoSiGrid->GetQualityCode();

    //set subcode from grid status
    KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;
    SetIDSubCode(irnuc->GetCodes().GetSubCodes(),quality);

    // set general ID code
    irnuc->SetIDCode( kIDCode4 );
    //if point lies above Zmax line, we give Zmax as Z of particle (real Z is >= Zmax)
    //general ID code = kIDCode5 (Zmin)
    if (quality==KVIDZAGrid::kICODE7)
    {
        irnuc->SetIDCode( kIDCode5 );
    }
    //Identified particles with subcode kID_LeftOfBragg are given
    //general ID code kIDCode5 (Zmin).
    if (quality==KVIDGChIoSi::k_LeftOfBragg)
    {
        irnuc->SetIDCode( kIDCode5 );
    }
    //unidentifiable particles with subcode kID_BelowSeuilSi are given
    //general ID code kIDCode5 (Zmin) and we estimate Zmin from energy
    //loss in ChIo
    if (quality==KVIDGChIoSi::k_BelowSeuilSi)
    {
        irnuc->SetIDCode( kIDCode5 );
        irnuc->SetZ( fchio->FindZmin() );
    }

    return kTRUE;
}

//________________________________________________________________________________________//

Double_t KVIDChIoSi::GetIDMapX(Option_t *)
{
    //Default X coordinate for ChIo-Si identification is KVSilicon::GetEnergy
    //unless silicon is not calibrated; then we use silicon pedestal-corrected 'PG' channel
    return (fsi->IsCalibrated()?
            fsi->GetEnergy() : (fsi->GetACQData("PG") - fsipgped));
}

Double_t KVIDChIoSi::GetIDMapY(Option_t *)
{
    //Default Y coordinate for ChIo-Si identification is KVChIo::GetEnergy
    //unless chio is not calibrated; then we use chio pedestal-corrected 'PG' channel
    return (fchio->IsCalibrated()?
            fchio->GetEnergy() : (fchio->GetACQData("PG") - fchiopgped));
}

//____________________________________________________________________________________

const Char_t *KVIDChIoSi::GetIDSubCodeString(KVIDSubCode & concat) const
{
    //Returns string explaining subcodes for this telescope

    static TString messages[] =
    {
        "OK",
        "slight ambiguity of Z, which could be larger",
        "slight ambiguity of Z, which could be smaller",
        "slight ambiguity of Z, which could be larger or smaller",
        "point is in between two lines of different Z, too far from either to be considered well-identified",
        "point is in between two lines of different Z, too far from either to be considered well-identified",
        "(x,y) is below first line in grid",
        "(x,y) is above last line in grid",
        " no identification: (x,y) out of range covered by grid",
        "warning: point to identify below punch-through line",
        "point to identify left of Si threshold line (bruit/arret ChIo?)",
        "point to identify below Bragg curve. Z given is a Zmin",
        "point to identify has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable"
    };
    Int_t code = GetIDSubCode(concat);
    if (code == -1)
        return SubCodeNotSet();
    return messages[code];
}

//____________________________________________________________________________________

void KVIDChIoSi::Initialize()
{
    // Initialisation of telescope before identification.
    // This method MUST be called once before any identification is attempted.
    // Initialisation of grid is performed here.
    // IsReadyForID() will return kTRUE if a grid is associated to this telescope for the current run.

    fchio = GetDetector(1);
    fsi = GetDetector(2);
    fsipgped = fsi->GetPedestal("PG");
    fchiopgped = fchio->GetPedestal("PG");
    ChIoSiGrid = (KVIDGChIoSi *) GetIDGrid();
    if ( ChIoSiGrid )
    {
        ChIoSiGrid->Initialize();
        SetBit(kReadyForID);
    }
    else
        ResetBit(kReadyForID);
}
