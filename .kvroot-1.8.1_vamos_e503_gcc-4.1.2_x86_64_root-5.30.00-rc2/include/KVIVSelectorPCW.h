//Created by KVClassFactory on Wed Nov  9 10:01:49 2011
//Author: Peter Wigg

#ifndef __KVIVSELECTORPCW_H
#define __KVIVSELECTORPCW_H

#include "KVIVSelector.h"
#include "LogFile.h"
#include "Sive503.h"
#include "Reconstructionv.h"
#include "DriftChamberv.h"
#include "IonisationChamberv.h"
#include "CsIv.h"
#include "EnergyTree.h"
#include "Identificationv.h"

class KVIVSelectorPCW : public KVIVSelector {

        Long64_t entries;
        Long64_t evNum;

        TFile *f;

        TTree *inTree;
        TTree *outTree;

        LogFile *l;
    
        Sive503 *si;
        DriftChamberv *dr;
        Reconstructionv *rcn;

        IonisationChamberv *ic;
        CsIv *csi;
        EnergyTree *et;
        Identificationv *id;

        Int_t runNumber;
        Bool_t workOnSps;
        string dir;

        KVSiliconVamos **siVamos;
        KVMaterial *isoButane;
        KVDetector *deadLayer;
        KVCsIVamos *csiVamos;
        KVTelescope **fpTelescope;

    public:

        KVIVSelectorPCW() {};
        virtual ~KVIVSelectorPCW() {};
   
        virtual void InitRun();
        virtual void EndRun();
        virtual void InitAnalysis();
        virtual Bool_t Analysis();
        virtual void EndAnalysis();

        Bool_t LoadGrids();
        void BuildFocalPlane();
        void DeleteFocalPlane();

        KVTelescope* GetTelescope(Int_t siNumber);
        void PassTelescope(Int_t siNumber);

        ClassDef(KVIVSelectorPCW,1)//User analysis class
};


#endif
