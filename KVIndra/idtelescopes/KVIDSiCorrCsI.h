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

        Double_t param0;
        Double_t param1;
        Double_t param2;
    
        KVTGID *tgid;
        
        KVSilicon* fSi;//!the silicon
        KVCsI* fCsI;//!the csi

        Double_t fCsIRPedestal;//!CsI Rapide pedestal for current run
        Double_t fCsILPedestal;//!CsI Lente pedestal for current run

        Double_t fSiPGPedestal;//!Silicon PG pedestal for current run
        Double_t fSiGGPedestal;//!Silicon GG pedestal for current run

        

    public:

        enum {                       //identification status subcodes
            kZOK_AnoTGID,             //Z id OK, mass ID attempted but no KVTGIDZA found
            kZOK_AOutOfIDRange,       //Z id OK, mass ID attempted but point to identify outside of identification range of KVTGIDZA
            kZOK_A_ZtooSmall,         //Z id ok, mass ID attempted but Z passed to IdentA too small (<1)
            kZOK_A_ZtooLarge,         //Z id ok, mass ID attempted but Z passed to IdentA larger than max Z defined by KVTGIDZA
        };

        KVIDSiCorrCsI() {

            fSi = 0;
            fCsI = 0;
            SetLabel("KVIDSiCorrCsI");
            //SetType("KVIDSiCorrCsI");

        };
        
        virtual ~ KVIDSiCorrCsI() {
        };

        virtual Bool_t Identify(KVIdentificationResult *IDR, Double_t x=-1., Double_t y=-1.);

        void Print(Option_t *opt = "") const;

        virtual Double_t GetIDMapX(Option_t *opt = "");
        virtual Double_t GetIDMapY(Option_t *opt = "");

        const Char_t *GetIDSubCodeString(KVIDSubCode &concat) const;

        virtual Bool_t SetIdentificationParameters(const KVMultiDetArray*);
        virtual Bool_t SetSiCorrelationParameters(const KVMultiDetArray*);
        virtual void RemoveIdentificationParameters();

        //void PrintFitParameters(const KVMultiDetArray *MDA);
        void PrintFitParameters();
        void PrintSiCorrelationParameters();

        void AddParamsToTelescope(Double_t par0, Double_t par1, Double_t par2){
            param0 = par0;
            param1 = par1;
            param2 = par2;
        }

        Double_t GetSiCorrParam(Int_t npar);

        virtual void Initialize();

    ClassDef(KVIDSiCorrCsI, 2)  
};

#endif
