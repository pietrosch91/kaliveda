//Created by KVClassFactory on Thu Aug 20 16:03:37 2009
//Author: marini

#ifndef __ENERGYTREE_H
#define __ENERGYTREE_H

#include "TTree.h"
#include "TFile.h"
#include "TEventList.h"
#include "KVLightEnergyCsI.h"
#include "KVLightEnergyCsIVamos.h"
#include "KVTelescope.h"
#include "KVDetector.h"
#include "KVTelescope.h"
#include "KVIDTelescope.h"
#include "KVList.h"
#include "TCut.h"
#include "Sive503.h"
#include "CsIv.h"
#include "LogFile.h"

#include "PlaneAbsorber.h"
#include "IonisationChamber.h"
#include "KVSiliconVamos.h"
#include "KVCsIVamos.h"
#include "KVIDSiCsIVamos.h"
#include "KVIDGraph.h"
#include "KVIDZAGrid.h"

#include "KVClassMonitor.h"
#include <string>
#include <string.h>

#include "KVReconstructedNucleus.h"

class EnergyTree
{

    public:
   	
        Int_t eN;
        Int_t status;

        UShort_t eLightSi;
        UShort_t eLightCsI;
        Double_t LightCsI;
        Double_t eEnergySi;
        Double_t eEnergyCsI;
        
        Double_t Einc;
        Double_t EEsi;
        Double_t Echio;
	
        Double_t sEnergySi;
        Double_t sEnergyCsI;
        Double_t sRefECsI;
        
        Double_t esi1;
        Double_t esi2;
        Double_t ecsi1;
        Double_t ecsi2;
        Double_t diff1; 
        Double_t diff2;
          
        Double_t diffsi;
        Double_t diffcsi;
        Double_t diffetot;
        Double_t difflum;
          
        Double_t CanalCsI;
         
        Int_t right;
        Int_t left;
        /*   Double_t right; */
        /*   Double_t left; */
        
        Int_t eZ;
        Int_t eA;
        Int_t sA;
        Double_t iA;
        //  Double_t iA_new;
          
        
        Double_t ePied;
        Double_t a;
        Double_t b;
        Double_t c;
        Double_t alpha;
        
        Double_t thick;
	
	KVReconstructedNucleus *frag;	
        KVLightEnergyCsIVamos* lum;
        
        KVTelescope *kvt_icsi;
        KVTelescope *kvt_sicsi;
        //KVDetector *kvd_si;
	KVSiliconVamos *kvd_si;
	//KVDetector *kvd_csi;
	KVCsIVamos *kvd_csi;
		
	KVDetector *kvd_gap;	
	KVList *list;
        KVIDGraph *kvid;
	//KVIDTelescope *kvid;
	
        IonisationChamber *ioCh;
        KVSiliconVamos *si;
        PlaneAbsorber *gap;
        KVCsIVamos *csi;   
            
        KVNucleus part;
        KVNucleus part2;
   	    
        Sive503 *Si;
        CsIv *CsI;
        
        EnergyTree(LogFile *Log, Sive503 *Si);
        LogFile *L;
        virtual ~EnergyTree();
        
        Int_t ClearEvent(Int_t);	
	string name;
  	string module_map[18][80];
          
        //necessary methods to Init 
        Double_t GetSiliconThickness(Int_t);
        void SetSiliconThickness(Int_t);
	void SetModuleMap(string id_modulemap[18][80]);  
	string GetModuleName(Int_t, Int_t);
	void InitTelescope(Int_t, Int_t);
        //void InitDetector(Int_t);
            
        void InitIcSi(Int_t);
        Double_t GetResidualEnergyIc(Int_t, Int_t, Double_t);
        
        void InitSiCsI(Int_t);  
        void InitSiCalib(Int_t);
        void SetCalSi(Float_t,Float_t,Float_t);
        void InitCsIPed(Int_t);
        void SetCsIPed(Float_t);
        void InitCsICalib(Int_t,Int_t);
        void SetCalCsI(Float_t,Float_t,Float_t);
	
        void DoIt(UShort_t, UShort_t, Int_t);
	
        void CalculateCanalCsI();
        
        void SetCalibration(Sive503*,CsIv*,Int_t,Int_t);
        void SetFragmentZ(Int_t);
        
        //necessary methods to GetResidualEnergyCsI: best estimation of ECsI and A
        Double_t GetResidualEnergyCsI(UShort_t,UShort_t);	/* Original :  Double_t GetResidualEnergyCsI(UShort_t,UShort_t) */
        Double_t GetResidualEnergyCsI2(UShort_t,UShort_t,Int_t, Int_t);
        void CalculateESi(UShort_t);
        void ECsIch(UShort_t);
        void Bisection(Int_t,UShort_t);
        void SetFragmentA(Int_t);
        void  CompleteSimulation(UShort_t);
        void CalculateECsI();
        void SimulateEvent();
        void ClearTelescope();
        void GetECsI();
        void GetESi();
        void Interpolate();
        Double_t GetInterpolationD(Double_t, Double_t, Double_t, Double_t, Double_t);
        
        Double_t RetrieveEnergySi();
        Double_t RetrieveA();
        Double_t RetrieveLight();
        Double_t RetrieveEnergyCsI();
        
        ClassDef(EnergyTree,1)//EnergyTree
};
     
#endif
