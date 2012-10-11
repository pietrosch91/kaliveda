/***************************************************************************
                          KVIDSiCorrCsI.h  -  description
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

class KVIDSiCorrCsI:public KVIDSiCsI, public KVTGIDManager{

        KVTGID *tgid;
        
        KVSilicon* fSi;
        KVCsI* fCsI;

    public:

        KVIDSiCorrCsI(){};
        virtual ~KVIDSiCorrCsI(){};

        virtual void Initialize();

        virtual Double_t GetIDMapX(Option_t *opt = ""); 
        virtual Double_t GetIDMapY(Option_t *opt = "");

        virtual Bool_t Identify(KVIdentificationResult *IDR, Double_t x=-1., Double_t y=-1.);

        virtual Bool_t SetIdentificationParameters(const KVMultiDetArray*);
        virtual void RemoveIdentificationParameters();

        void PrintFitParameters();

        ClassDef(KVIDSiCorrCsI, 2)  
};

#endif
