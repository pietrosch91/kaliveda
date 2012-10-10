/***************************************************************************
                          KVIDSiCorrCsI_e503.h  -  description
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

#ifndef KVIDSICORRCSI_H
#define KVIDSICORRCSI_H

#include "KVIDSiCsI.h"
#include "KVTGIDManager.h"
#include "KVSilicon.h"
#include "KVTGID.h"
#include "KVCsI.h"
#include "KVChIo.h"
#include "KVINDRA.h"
#include "KVINDRAReconNuc.h"
#include "Riostream.h"
#include "KVDetector.h"
#include "KVMultiDetArray.h"
#include "KVDataSet.h"
#include "KVTGIDZA.h"
#include "TObjString.h"
#include "KVIdentificationResult.h"
#include "KVNumberList.h"

#include <sstream>

class KVMultiDetArray;

class KVIDSiCorrCsI_e503:public KVIDSiCsI, public KVTGIDManager{

        KVTGID *tgid;
        
        KVSilicon* fSi;
        KVCsI* fCsI;

        Double_t fCsILight;
        Double_t fCsIRPedestal;
        Double_t fCsILPedestal;

        Double_t fSiCorr;
        Double_t fSiPG;
        Double_t fSiGG;
        Double_t fSiPGPedestal;
        Double_t fSiGGPedestal;

    public:

        KVIDSiCorrCsI_e503(){};
        virtual ~KVIDSiCorrCsI_e503(){};

        virtual void Initialize();

        virtual Double_t GetIDMapX(Option_t *opt = ""); 
        virtual Double_t GetIDMapY(Option_t *opt = "");

        virtual Bool_t Identify(KVIdentificationResult *IDR, Double_t x=-1., Double_t y=-1.);

        virtual Bool_t SetIdentificationParameters(const KVMultiDetArray*);
        virtual void RemoveIdentificationParameters();

        void PrintFitParameters();

        // Methods to check the pedestals etc.

        //Double_t GetCsILight(){return fCsILight;};
        //Double_t GetCsIRPedestal(){return fCsIRPedestal;};
        //Double_t GetCsILPedestal(){return fCsILPedestal;};

        //Double_t GetSiCorr(){return fSiCorr;};
        //Double_t GetSiGG(){return fSiGG;};
        //Double_t GetSiPG(){return fSiPG;};
        //Double_t GetSiGGPedestal(){return fSiGGPedestal;};
        //Double_t GetSiPGPedestal(){return fSiPGPedestal;};

        ClassDef(KVIDSiCorrCsI_e503, 2)  
};

#endif
