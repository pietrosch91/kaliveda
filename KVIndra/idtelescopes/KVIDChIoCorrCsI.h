/*
$Id: KVIDChIoCorrCsI.h,v 1.2 2009/04/06 15:21:31 franklan Exp $
$Revision: 1.2 $
$Date: 2009/04/06 15:21:31 $
*/

//Created by KVClassFactory on Mon Mar 30 16:44:34 2009
//Author: John Frankland,,,

#ifndef __KVIDCHIOCORRCSI_H
#define __KVIDCHIOCORRCSI_H

#include "KVIDZAGrid.h"
#include "KVINDRAIDTelescope.h"
#include "KVChIo.h"
#include "KVIDChIoCsI.h"
#include "KVCsI.h"

class KVIDChIoCorrCsI : public KVIDChIoCsI
{

        KVIDZAGrid* fGrid;

	    KVChIo* fChIo;
	    KVCsI* fCsI;

	    Double_t fCsIRPedestal;
	    Double_t fCsILPedestal;

        Double_t fChIoCorr;
        Double_t fChIoGGPedestal;
        Double_t fChIoPGPedestal;
        Double_t fChIoGG;
        Double_t fChIoPG;

    public:

        KVIDChIoCorrCsI();
        virtual ~KVIDChIoCorrCsI();

        virtual void Initialize(void);

        virtual Double_t GetIDMapX(Option_t * opt = "");
        virtual Double_t GetIDMapY(Option_t * opt = "");

        Bool_t Identify(KVIdentificationResult*, Double_t x=-1., Double_t y=-1.);

        Double_t GetChIoCorr(){return fChIoCorr;};
        Double_t GetChIoGG(){return fChIoGG;};
        Double_t GetChIoPG(){return fChIoPG;};
        Double_t GetChIoGGPedestal(){return fChIoGGPedestal;};
        Double_t GetChIoPGPedestal(){return fChIoPGPedestal;};

        

   ClassDef(KVIDChIoCorrCsI,1)
};

#endif
