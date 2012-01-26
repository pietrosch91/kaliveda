/*
$Id: KVIDChIoSiCorr.h,v 1.1 2007/12/10 13:02:13 franklan Exp $
$Revision: 1.1 $
$Date: 2007/12/10 13:02:13 $
*/

//Created by KVClassFactory on Mon Dec 10 12:58:26 2007
//Author: franklan

#ifndef __KVIDCHIOSICORR_H
#define __KVIDCHIOSICORR_H

#include "KVChIo.h"
#include "KVIDChIoSi.h"
#include "KVSilicon.h"
#include "KVINDRAIDTelescope.h"
#include "KVIDGChIoSi.h"

#include "KVINDRAReconNuc.h"
#include "KVINDRACodes.h"
#include "KVINDRA.h"
#include "KVIdentificationResult.h"

class KVIDChIoSiCorr : public KVIDChIoSi
{

        KVChIo *fChIo;
        KVSilicon *fSi;

        Double_t fChIoCorr;
        Double_t fChIoGGPedestal;
        Double_t fChIoPGPedestal;
        Double_t fChIoGG;
        Double_t fChIoPG;

        Double_t fSiPGPedestal;
        Double_t fSiGGPedestal;

        KVIDGChIoSi* ChIoSiGrid;

    public:

        KVIDChIoSiCorr();
        virtual ~KVIDChIoSiCorr();

        void Initialize();

        Double_t GetIDMapX(Option_t * opt = "");
        Double_t GetIDMapY(Option_t * opt = "");

        Bool_t Identify(KVIdentificationResult * IDR, Double_t x, Double_t y);

        Double_t GetChIoCorr(){return fChIoCorr;};
        Double_t GetChIoGG(){return fChIoGG;};
        Double_t GetChIoPG(){return fChIoPG;};
        Double_t GetChIoGGPedestal(){return fChIoGGPedestal;};
        Double_t GetChIoPGPedestal(){return fChIoPGPedestal;};

        ClassDef(KVIDChIoSiCorr,2)
};

#endif
