/***************************************************************************
$Id: KVINDRA.cpp,v 1.68 2009/01/21 10:05:51 franklan Exp $
                          kvindra.cpp  -  description
                             -------------------
    begin                : Mon May 20 2002
    copyright            : (C) 2002 by J.D. Frankland
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

#include "Riostream.h"
#include "KVINDRA.h"
#include "KVMaterial.h"
#include "KVDetector.h"
#include "KVTelescope.h"
#include "KVIDTelescope.h"
#include "KVIDSiCsI.h"
#include "KVIDCsI.h"
#include "KVIDChIoSi.h"
#include "KVIDChIoCsI.h"
#include "KVIDChIoSi75.h"
#include "KVIDSi75SiLi.h"
#include "KVIDSiLiCsI.h"
#include "KVIDPhoswich.h"
#include "KVLayer.h"
#include "KVRing.h"
#include "KVGroup.h"
#include "KVSilicon.h"
#include "KVChIo.h"
#include "KVCsI.h"
#include "KVPhoswich.h"
#include "KVMultiDetBrowser.h"
#include "KVNucleus.h"
#include "KVDetectorEvent.h"
#include "KVINDRAReconEvent.h"
#include "KVINDRAUpDater.h"
#include "TEnv.h"
#include "KVFileReader.h"
#include "INDRAGeometryBuilder.h"

using namespace std;

ClassImp(KVINDRA)
///////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRA</h2>
<h4>INDRA multidetector array, 1st campaign configuration</h4>

<img src="images/indra_camp1.png"><br>

<!-- */
// --> END_HTML
///////////////////////////////////////////////////////////////////////////////////////

//Use this static array to translate EBaseIndra_type
//signal type to a string giving the signal type
Char_t KVINDRA::SignalTypes[][3] = {
   "",                          //dummy for index=0
   "GG", "PG", "T",
   "GG", "PG", "T",
   "R", "L", "T",
   "GG", "PG", "T",
   "GG", "PG", "T"
};


KVINDRA *gIndra;
//_________________________________________________________________________________
KVINDRA::KVINDRA()
{
   //Default constructor
   //Set up lists of ChIo, Si, CsI, Phoswich
   fChIo = new KVHashList();
   fChIo->SetCleanup(kTRUE);
   fSi = new KVHashList();
   fSi->SetCleanup(kTRUE);
   fCsI = new KVHashList();
   fCsI->SetCleanup(kTRUE);
   fChIoLayer = 0;
   fPhoswich = new KVHashList();
   fPhoswich->SetCleanup(kTRUE);
   fTrigger = 0;
   gIndra = this;
   fPHDSet=kFALSE;
   fSelecteur=0;
}

//_________________________________________________________________________________
KVINDRA::~KVINDRA()
{
   //Delets lists of ChIo, Si, etc.
   //Resets global gIndra pointer.

   if (fChIo && fChIo->TestBit(kNotDeleted)) {
      fChIo->Clear();
      delete fChIo;
   }
   fChIo = 0;
   if (fSi && fSi->TestBit(kNotDeleted)) {
      fSi->Clear();
      delete fSi;
   }
   fSi = 0;
   if (fCsI && fCsI->TestBit(kNotDeleted)) {
      fCsI->Clear();
      delete fCsI;
   }
   fCsI = 0;
   if (fPhoswich && fPhoswich->TestBit(kNotDeleted)) {
      fPhoswich->Clear();
      delete fPhoswich;
   }
   fPhoswich = 0;
   if(fSelecteur) delete fSelecteur;
   gIndra = 0;
}

//_________________________________________________________________________________

void KVINDRA::MakeListOfDetectorTypes()
{
//Make list of all detector types needed to build INDRA array

   // Silicon detectors
	KVSilicon *kvsi = (KVSilicon* )KVDetector::MakeDetector(Form("%s.SI",fDataSet.Data()), 300.0);
	//kvsi->SetThickness(300.0);
	kvsi->SetLabel("SI300");
	fDetectorTypes->Add(kvsi);

	// Etalons - Si 80um
  	kvsi = new KVSi75(80);
   fDetectorTypes->Add(kvsi);

	// Etalons - Si(Li) 2mm
   kvsi = new KVSiLi(2000);
   fDetectorTypes->Add(kvsi);

	// Phoswich detectors
   KVPhoswich *kvph = new KVPhoswich(0.05, 25.0);
   kvph->SetLabel("PHOS");
   fDetectorTypes->Add(kvph);

   // Ionisation chambers
	Float_t press_chio[3]={50.0,30.0,20.0};
	KVChIo *kvch = NULL;
	for (Int_t ii=0;ii<3;ii+=1){
		kvch = (KVChIo* )KVDetector::MakeDetector(Form("%s.CI",fDataSet.Data()), press_chio[ii]);
		//kvch->SetPressure(press_chio[ii]);
		kvch->SetLabel(Form("CHIO%1.0f",press_chio[ii]));
		fDetectorTypes->Add(kvch);
	}
   // CsI scintillators
	Float_t thick_csi[7]={13.80,9.70,9.0,7.60,6.0,5.0,4.80};
	KVCsI *kvcsi = NULL;
	for (Int_t ii=0;ii<7;ii+=1){
		kvcsi = (KVCsI* )KVDetector::MakeDetector(Form("%s.CSI",fDataSet.Data()), thick_csi[ii]);
		//kvcsi->SetThickness(thick_csi[ii]);
		kvcsi->SetLabel(Form("CSI%1.0f",thick_csi[ii]*10));
		fDetectorTypes->Add(kvcsi);
	}
}

//_________________________________________________________________________________
void KVINDRA::PrototypeTelescopes()
{
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Begin");
   GetDetectorTypes()->Print();
#endif

// create prototypes for all telescopes needed to build INDRA

   // Phoswich
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Phoswich - begin");
#endif
   KVINDRATelescope *kvt = new KVINDRATelescope;
   kvt->AddDetector((KVPhoswich *) GetDetectorTypes()->
                    FindObjectByLabel("PHOS"));
   kvt->SetType("Phoswich");
   kvt->SetAzimuthalWidth(30.0);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Phoswich - success");
#endif

   // Si-CsI Ring 2
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI300"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si-CsI Ring2");
   kvt->SetAzimuthalWidth(27.51);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Si300-CsI Ring 2 - success");
#endif

   // Si-CsI Ring 3
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI300"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si-CsI Ring3");
   kvt->SetAzimuthalWidth(14.15);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Si300-CsI Ring 3 - success");
#endif

   // Si-CsI Ring 4
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI300"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si-CsI Ring4");
   kvt->SetAzimuthalWidth(14.01);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Si300-CsI Ring 4 - success");
#endif

   // Si-CsI Ring 5
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI300"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si-CsI Ring5");
   kvt->SetAzimuthalWidth(14.29);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Si300-CsI Ring 5 - success");
#endif

   // Si-CsI Ring 6
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI300"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI97"));
   kvt->SetType("Si-CsI Ring6");
   kvt->SetAzimuthalWidth(14.29);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Si300-CsI Ring 6 - success");
#endif

   // Si-CsI Ring 7
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI300"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI97"));
   kvt->SetType("Si-CsI Ring7");
   kvt->SetAzimuthalWidth(14.48);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Si300-CsI Ring 7 - success");
#endif

   // Si-CsI Ring 8
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI300"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI90"));
   kvt->SetType("Si-CsI Ring8");
   kvt->SetAzimuthalWidth(14.18);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Si300-CsI Ring 8 - success");
#endif

   // Si-CsI Ring 9
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI300"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI90"));
   kvt->SetType("Si-CsI Ring9");
   kvt->SetAzimuthalWidth(14.32);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Si300-CsI Ring 9 - success");
#endif

   // CsI Ring 10
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI76"));
   kvt->SetType("CsI Ring10");
   kvt->SetAzimuthalWidth(14.44);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "CsI Ring 10 - success");
#endif

   //Ring 10 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSi75 *) fDetectorTypes->FindObjectByLabel("SI75"));
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring10");
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 1.0);
   kvt->SetAzimuthalWidth(14.44 / 2.);
   fTelescopes->Add(kvt);

   // CsI Ring 11
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI76"));
   kvt->SetType("CsI Ring11");
   kvt->SetAzimuthalWidth(14.52);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "CsI Ring 11 - success");
#endif

   //Ring 11 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSi75 *) fDetectorTypes->FindObjectByLabel("SI75"));
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 1.0);
   kvt->SetType("Etalons Ring11");
   kvt->SetAzimuthalWidth(14.52 / 2.);
   fTelescopes->Add(kvt);

   // CsI Ring 12
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI48"));
   kvt->SetType("CsI Ring12");
   kvt->SetAzimuthalWidth(14.56);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "CsI Ring 12 - success");
#endif

   //Ring 12 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSi75 *) fDetectorTypes->FindObjectByLabel("SI75"));
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring12");
   kvt->SetAzimuthalWidth(14.56 / 2.);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 1.0);
   fTelescopes->Add(kvt);

   // CsI Ring 13
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI60"));
   kvt->SetType("CsI Ring13");
   kvt->SetAzimuthalWidth(14.64);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "CsI Ring 13 - success");
#endif

   //Ring 13 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSi75 *) fDetectorTypes->FindObjectByLabel("SI75"));
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring13");
   kvt->SetAzimuthalWidth(14.64 / 2.);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 1.0);
   fTelescopes->Add(kvt);

   // CsI Ring 14
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI50"));
   kvt->SetType("CsI Ring14");
   kvt->SetAzimuthalWidth(22.08);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "CsI Ring 14 - success");
#endif

   //Ring 14 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSi75 *) fDetectorTypes->FindObjectByLabel("SI75"));
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring14");
   kvt->SetAzimuthalWidth(22.08 / 2.);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 1.0);
   fTelescopes->Add(kvt);

   // CsI Ring 15
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI50"));
   kvt->SetType("CsI Ring15");
   kvt->SetAzimuthalWidth(21.98);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "CsI Ring 15 - success");
#endif

   //Ring 15 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSi75 *) fDetectorTypes->FindObjectByLabel("SI75"));
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring15");
   kvt->SetAzimuthalWidth(21.98 / 2.);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 1.0);
   fTelescopes->Add(kvt);

   // CsI Ring 16
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI50"));
   kvt->SetType("CsI Ring16");
   kvt->SetAzimuthalWidth(43.65);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "CsI Ring 16 - success");
#endif

   //Ring 16 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSi75 *) fDetectorTypes->FindObjectByLabel("SI75"));
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring16");
   kvt->SetAzimuthalWidth(43.65 / 2.);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 1.0);
   fTelescopes->Add(kvt);

   // CsI Ring 17
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI50"));
   kvt->SetType("CsI Ring17");
   kvt->SetAzimuthalWidth(39.99);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "CsI Ring 17 - success");
#endif

   //Ring 17 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSi75 *) fDetectorTypes->FindObjectByLabel("SI75"));
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring17");
   kvt->SetAzimuthalWidth(39.99 / 2.);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 1.0);
   fTelescopes->Add(kvt);


   // ChIo 2-3
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO30"));
   kvt->SetType("ChIo Ring2");
   kvt->SetAzimuthalWidth(28.84);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 2 - success");
#endif

   // ChIo 4-5
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO30"));
   kvt->SetType("ChIo Ring4");
   kvt->SetAzimuthalWidth(29.10);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 4 - success");
#endif

   // ChIo 6-7
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO30"));
   kvt->SetType("ChIo Ring6");
   kvt->SetAzimuthalWidth(29.28);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 6 - success");
#endif

   // ChIo 8-9
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO20"));
   kvt->SetType("ChIo Ring8");
   kvt->SetAzimuthalWidth(29.03);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 8 - success");
#endif

   // ChIo 10-11
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO20"));
   kvt->SetType("ChIo Ring10");
   kvt->SetAzimuthalWidth(29.33);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 10 - success");
#endif

   // ChIo 12
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO20"));
   kvt->SetType("ChIo Ring12");
   kvt->SetAzimuthalWidth(29.44);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 12 - success");
#endif

   // ChIo 13
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO20"));
   kvt->SetType("ChIo Ring13");
   kvt->SetAzimuthalWidth(44.44);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 13 - success");
#endif

   // ChIo 14-15
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO20"));
   kvt->SetType("ChIo Ring14");
   kvt->SetAzimuthalWidth(44.26);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 14 - success");
#endif

   // ChIo 16-17
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVChIo *) fDetectorTypes->
                    FindObjectByLabel("CHIO20"));
   kvt->SetType("ChIo Ring16");
   kvt->SetAzimuthalWidth(41.54);
	kvt->SetDepth(1, 0.0);
   fTelescopes->Add(kvt);
#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "ChIo Ring 16 - success");
#endif


#ifdef KV_DEBUG
   Info("PrototypeTelescopes", "Success");
#endif

}

//_________________________________________________________________________________
void KVINDRA::BuildGeometry()
{
   // Construction of 1st campaign INDRA detector array. All
   // subsequent realisations derive from this class and make
   // modifications to this basic structure
   
   KVLayer *kvl = new KVLayer;    //Build ionisation chamber layer

   //Pressures correspond to Xe+Sn (should be replaced when SetParameters()
   //is called with values read from database corresponding to dataset)
   //1st Layer - Ionisation chamber

   kvl->
       AddRing(new
               KVRing(2, 3.13, 6.98, 30., 12, 655.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring2"), 2)
       );

   kvl->
       AddRing(new
               KVRing(4, 7.21, 13.83, 30., 12, 385.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring4"))

       );

   kvl->
       AddRing(new
               KVRing(6, 14.21, 26.74, 30., 12, 250.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring6"))

       );

   kvl->
       AddRing(new
               KVRing(8, 27.84, 44.67, 30., 12, 120.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring8"))

       );

   kvl->
       AddRing(new
               KVRing(10, 45.23, 69.69, 30., 12, 120.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring10"))

       );

   kvl->
       AddRing(new
               KVRing(12, 70.25, 87.60, 30., 12, 120.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring12"))

       );

   kvl->
       AddRing(new
               KVRing(13, 92.40, 109.76, 45., 8, 120.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring13"))

       );

   kvl->
       AddRing(new
               KVRing(14, 110.31, 141.80, 45., 8, 120.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring14"))

       );

   kvl->
       AddRing(new
               KVRing(16, 142.38, 174.77, 45., 8, 120.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("ChIo Ring16"))

       );

   kvl->SetName("CHIO");

   AddLayer(kvl);

   fChIoLayer = kvl;



#ifdef KV_DEBUG

   Info("Build", "Made ChIo Layer");

#endif



   Float_t chio_depth = 50.005 ; // include mylar in total depth



   //Build Si75-SiLi etalons layer

   //As a first guess, these have 1/2 the phi-width of the corresponding CsI and

   //are placed at the centre of the CsI with 1/2 the polar width
	//etalons are placed 4 mm behind ChIo back window

   kvl = new KVLayer;

   Float_t th_width = 56.90 - 45.25;

   kvl->
       AddRing(new
               KVRing(10, 45.25 + .25 * th_width, 56.90 - .25 * th_width,
                      37.5, 1, 120. + chio_depth + 4.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Etalons Ring10"), 1, 2));

   th_width = 69.69 - 56.99;

   kvl->
       AddRing(new
               KVRing(11, 56.99 + .25 * th_width, 69.69 - .25 * th_width,
                      37.5, 1, 120. + chio_depth + 4.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Etalons Ring11"), 1, 2));

   th_width = 88.59 - 70.28;

   kvl->
       AddRing(new
               KVRing(12, 70.28 + .25 * th_width, 88.59 - .25 * th_width,
                      37.5, 1, 120. + chio_depth + 4.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Etalons Ring12"), 1, 2));

   th_width = 109.65 - 91.72;

   kvl->
       AddRing(new
               KVRing(13, 91.72 + .25 * th_width, 109.65 - .25 * th_width,
                      75.0, 1, 120. + chio_depth + 4.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Etalons Ring13"), 1, 4));

   th_width = 126.03 - 110.44;

   kvl->
       AddRing(new
               KVRing(14, 110.44 + .25 * th_width, 126.03 - .25 * th_width,
                      78.75, 1, 120. + chio_depth + 4.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Etalons Ring14"), 1, 3));

   th_width = -(126.12 - 141.69);

   kvl->
       AddRing(new
               KVRing(15, 126.12 + .25 * th_width, 141.69 - .25 * th_width,
                      78.75, 1, 120. + chio_depth + 4.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Etalons Ring15"), 1, 3));

   th_width = -(142.50 - 157.08);

   kvl->
       AddRing(new
               KVRing(16, 142.50 + .25 * th_width, 157.08 - .25 * th_width,
                      90.0, 1, 120. + chio_depth + 4.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Etalons Ring16"), 1, 2));

   th_width = -(157.17 - 175.70);

   kvl->
       AddRing(new
               KVRing(17, 157.17 + .25 * th_width, 175.70 - .25 * th_width,
                      90.0, 1, 120. + chio_depth + 4.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Etalons Ring17"), 1, 2));

   kvl->SetName("Etalons");
   AddLayer(kvl);



   //Build Phoswich/Si-CsI/CsI layer



   kvl = new KVLayer;
   kvl->AddRing(new KVRing(1, 2., 3., 30., 12, 1300.,
                           (KVTelescope *) fTelescopes->
                           FindObjectByType("Phoswich")));
   kvl->
       AddRing(new
               KVRing(2, 3.06, 4.46, 30.0, 12, 655. + chio_depth +5.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Si-CsI Ring2"), 2));
   // note step=2 for si-csi modules on ring 2 ^^^^^^^
   kvl->
       AddRing(new
               KVRing(3, 4.48, 6.96, 22.50, 24, 655. + chio_depth +5.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Si-CsI Ring3"))
       );
   kvl->
       AddRing(new
               KVRing(4, 7.16, 9.95, 22.50, 24, 385. + chio_depth +5.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Si-CsI Ring4"))
       );
   kvl->
       AddRing(new
               KVRing(5, 9.99, 13.72, 22.50, 24, 385. + chio_depth +5.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Si-CsI Ring5"))
       );
   kvl->
       AddRing(new
               KVRing(6, 14.23, 19.93, 22.50, 24, 250. + chio_depth +5.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Si-CsI Ring6"))
       );
   kvl->
       AddRing(new
               KVRing(7, 19.98, 26.61, 22.50, 24, 250. + chio_depth +5.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Si-CsI Ring7"))
       );
   kvl->
       AddRing(new
               KVRing(8, 27.62, 34.87, 22.50, 24, 120. + chio_depth +5.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Si-CsI Ring8"))
       );
   kvl->
       AddRing(new
               KVRing(9, 34.96, 44.61, 22.50, 24, 120. + chio_depth +5.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("Si-CsI Ring9"))
       );
   kvl->
       AddRing(new
               KVRing(10, 45.25, 56.90, 22.50, 24, 120. + chio_depth + 10.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("CsI Ring10"))
       );
   kvl->
       AddRing(new
               KVRing(11, 56.99, 69.69, 22.50, 24, 120. + chio_depth + 10.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("CsI Ring11"))
       );
   kvl->
       AddRing(new
               KVRing(12, 70.28, 88.59, 22.50, 24, 120. + chio_depth + 10.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("CsI Ring12"))
       );
   kvl->
       AddRing(new
               KVRing(13, 91.72, 109.65, 30.0, 24, 120. + chio_depth + 10.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("CsI Ring13"))
       );
   kvl->
       AddRing(new
               KVRing(14, 110.44, 126.03, 33.75, 16, 120. + chio_depth + 10.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("CsI Ring14"))
       );
   kvl->
       AddRing(new
               KVRing(15, 126.12, 141.69, 33.75, 16, 120. + chio_depth + 10.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("CsI Ring15"))
       );
   kvl->
       AddRing(new
               KVRing(16, 142.50, 157.08, 45.0, 8, 120. + chio_depth + 10.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("CsI Ring16"))
       );
   kvl->
       AddRing(new
               KVRing(17, 157.17, 175.70, 45.0, 8, 120. + chio_depth + 10.,
                      (KVTelescope *) fTelescopes->
                      FindObjectByType("CsI Ring17"))
       );
   kvl->SetName("SI-CSI");
   AddLayer(kvl);

#ifdef KV_DEBUG
   Info("Build", "Made Phoswich/Si-CsI Layer");
#endif

}

//_________________________________________________________________________________________

void KVINDRA::Build()
{
   // Overrides KVMultiDetArray::Build in order to set the name of the detector.
   // Correspondance between CsI detectors and pin lasers is set up if known.
   // GG to PG conversion factors for Si and ChIo are set if known.
	//Correspondance between Si and ChIo detectors and nunmber of the QDC is made

   KVMultiDetArray::Build();

   SetName("INDRA");
   SetTitle("1st & 2nd campaign INDRA multidetector");
   
   SetPinLasersForCsI();
	SetGGtoPGConversionFactors();
	LinkToCodeurs();
}

void KVINDRA::SetArrayACQParams()
{
   // Overrides KVMultiDetArray::SetArrayACQParams() in order to
   // add the following acquisition parameters which are not associated to a detector:
   //
   // STAT_EVE
   // R_DEC
   // CONFIG
   // PILA_01_PG
   // PILA_01_GG
   // PILA_02_PG
   // PILA_02_GG
   // PILA_03_PG
   // PILA_03_GG
   // PILA_04_PG
   // PILA_04_GG
   // PILA_05_PG
   // PILA_05_GG
   // PILA_06_PG
   // PILA_06_GG
   // PILA_07_PG
   // PILA_07_GG
   // PILA_08_PG
   // PILA_08_GG
   // SI_PIN1_PG
   // SI_PIN1_GG
   // SI_PIN2_PG
   // SI_PIN2_GG
   //
   // We also create and initialize the KVINDRATriggerInfo object (fSelecteur) used to read
   // the status of the DAQ trigger event by event (access through GetTriggerInfo()).
   
   KVACQParam* ste = new KVACQParam("STAT_EVE");
   AddACQParam(ste);
   KVACQParam* dec = new KVACQParam("R_DEC");
   AddACQParam(dec);
   KVACQParam* conf = new KVACQParam("CONFIG");
   AddACQParam(conf);
   fSelecteur = new KVINDRATriggerInfo;
   fSelecteur->SetSTAT_EVE_PAR(ste);
   fSelecteur->SetR_DEC_PAR(dec);
   fSelecteur->SetVXCONFIG_PAR(conf);
   
   AddACQParam(new KVACQParam("PILA_01_PG"));
   AddACQParam(new KVACQParam("PILA_01_GG"));
   AddACQParam(new KVACQParam("PILA_02_PG"));
   AddACQParam(new KVACQParam("PILA_02_GG"));
   AddACQParam(new KVACQParam("PILA_03_PG"));
   AddACQParam(new KVACQParam("PILA_03_GG"));
   AddACQParam(new KVACQParam("PILA_04_PG"));
   AddACQParam(new KVACQParam("PILA_04_GG"));
   AddACQParam(new KVACQParam("PILA_05_PG"));
   AddACQParam(new KVACQParam("PILA_05_GG"));
   AddACQParam(new KVACQParam("PILA_06_PG"));
   AddACQParam(new KVACQParam("PILA_06_GG"));
   AddACQParam(new KVACQParam("PILA_07_PG"));
   AddACQParam(new KVACQParam("PILA_07_GG"));
   AddACQParam(new KVACQParam("PILA_08_PG"));
   AddACQParam(new KVACQParam("PILA_08_GG"));
   AddACQParam(new KVACQParam("SI_PIN1_PG"));
   AddACQParam(new KVACQParam("SI_PIN1_GG"));
   AddACQParam(new KVACQParam("SI_PIN2_PG"));
   AddACQParam(new KVACQParam("SI_PIN2_GG"));
}

//_________________________________________________________________________________________

void KVINDRA::MakeListOfDetectors()
{
   //Overrides KVMultiDetArray method to add FillListsOfDetectorsByType()

   KVMultiDetArray::MakeListOfDetectors();
   FillListsOfDetectorsByType();
}

//_________________________________________________________________________________________
void KVINDRA::FillListsOfDetectorsByType()
{
   //Fill lists of ChIo, Si, CsI and phoswich

   fChIo->Clear();
   fSi->Clear();
   fCsI->Clear();
   fPhoswich->Clear();
   TIter next_det(fDetectors);
   KVDetector *kvd;
   while ((kvd = (KVDetector *) next_det())) {
      if (kvd->InheritsFrom("KVChIo")) {
         fChIo->Add(kvd);
      }
      if (kvd->InheritsFrom("KVSilicon")) {
         fSi->Add(kvd);
      }
      if (kvd->InheritsFrom("KVCsI")) {
         fCsI->Add(kvd);
      }
      if (kvd->InheritsFrom("KVPhoswich")) {
         fPhoswich->Add(kvd);
      }
   }
}

//_______________________________________________________________________________________
void KVINDRA::UpdateArray()
{
    // Updates pointer to chio layer if necessary (it may have been removed),
    // then performs normal update KVMultiDetArray::UpdateArray()

	fChIoLayer = GetLayer("CHIO");
	KVMultiDetArray::UpdateArray();
}

//_________________________________________________________________________________________
void KVINDRA::SetNamesChIo()
{
   // INDRA-specific numbering of ChIo according to smallest ring,module of csi behind
   // the cell

	if(!GetChIoLayer()) return; // chios have been removed ?
	
   TIter next_ring(GetChIoLayer()->GetRings());
   KVRing *robj;
   while ((robj = (KVRing *) next_ring())) {
      TIter next_tele(robj->GetTelescopes());
      KVTelescope *chio;
      while ((chio = (KVTelescope *) next_tele())) {    // loop over chio telescopes

         TIter next_in_group((chio->GetGroup())->GetTelescopes());
         KVTelescope *ttest;
         UInt_t rmin = 1000, tmin = 1000;
         while ((ttest = (KVTelescope *) next_in_group())) {    //loop over group members

            if (ttest->ContainsType("CSI")) {   // only look at si/csi
               if ((ttest->GetRingNumber()) < rmin)
                  rmin = (ttest->GetRingNumber());
               if ((ttest->GetNumber()) < tmin)
                  tmin = (ttest->GetNumber());
            }
         }

         chio->SetNumber(tmin); // set module number
         robj->SetNumber(rmin); // set ring number
         chio->GetDetector(1)->SetName( chio->GetDetector(1)->GetArrayName() );
      }
   }
   // need to rehash detector lists - names of chios have changed
   ((KVHashList*)fDetectors)->Rehash();
   ((KVHashList*)fChIo)->Rehash();
}

//_________________________________________________________________________________________

void KVINDRA::SetGroupsAndIDTelescopes()
{
   //Overrides KVMDA method to add INDRA-specific numbering of ChIo according to
   //smallest ring,module of csi behind the cell

   KVMultiDetArray::SetGroupsAndIDTelescopes();
   SetNamesChIo();

}

KVChIo *KVINDRA::GetChIoOf(const Char_t * detname)
{

   //Returns a pointer to the Ionisation Chamber placed directly in front of the
   //detector "detname". If no ChIo is present, a null pointer is returned.

   KVINDRADetector *kvd;
   kvd = dynamic_cast<KVINDRADetector*>(GetDetector(detname));
   return (kvd ? (KVChIo*)kvd->GetChIo() : NULL);
}

//________________________________________________________________________________________

KVLayer *KVINDRA::GetChIoLayer(void)
{
   //Return pointer to layer in INDRA structure corresponding to ionisation
   //chambers.
   //
   return fChIoLayer;
}

//_________________________________________________________________________________

void KVINDRA::SetTrigger(UChar_t trig)
{
   //
   //Define multiplicity trigger used for acquisition and filter.
   //Events with multipicity >= trig are OK.
   //
   fTrigger = trig;
}

KVINDRADetector *KVINDRA::GetDetectorByType(UInt_t cou, UInt_t mod, UInt_t type) const
{
   //Find a detector based on the old BaseIndra type definitions:
   //
//enum EBaseIndra_type {
//      ChIo_GG=1,
//      ChIo_PG,//=2
//      ChIo_T,//=3
//      Si_GG,//=4
//      Si_PG,//=5
//      Si_T,//=6
//      CsI_R,//=7
//      CsI_L,//=8
//      CsI_T,//=9
//      Si75_GG,//=10
//      Si75_PG,//=11
//      Si75_T,//=12
//      SiLi_GG,//=13
//      SiLi_PG,//=14
//      SiLi_T//=15
//};
//enum EBaseIndra_typePhos {
//      Phos_R=1,
//      Phos_L,//=2
//      Phos_T,//=3
//};

   KVINDRADetector *det = 0;

   char nom_det[20];
   if (cou == 1) {
      if (type >= Phos_R && type <= Phos_T) {
         sprintf(nom_det, "PHOS_%02d", mod);
         det =
             (KVINDRADetector *) GetListOfPhoswich()->FindObjectByName(nom_det);
      }
   }
   if (det)
      return det;
   else if (type >= ChIo_GG && type <= ChIo_T) {
      sprintf(nom_det, "CI_%02d%02d", cou, mod);
      det = (KVINDRADetector *) GetListOfChIo()->FindObject(nom_det);
      return det;
   } else if (type >= Si_GG && type <= Si_T) {
      sprintf(nom_det, "SI_%02d%02d", cou, mod);
      det = (KVINDRADetector *) GetListOfSi()->FindObject(nom_det);
      return det;
   } else if (type >= SiLi_GG && type <= SiLi_T) {
      sprintf(nom_det, "SILI_%02d", cou);
      det = (KVINDRADetector *) GetListOfSi()->FindObject(nom_det);
      return det;
   } else if (type >= Si75_GG && type <= Si75_T) {
      sprintf(nom_det, "SI75_%02d", cou);
      det = (KVINDRADetector *) GetListOfSi()->FindObject(nom_det);
      return det;
   } else if (type >= CsI_R && type <= CsI_T) {
      sprintf(nom_det, "CSI_%02d%02d", cou, mod);
      det = (KVINDRADetector *) GetListOfCsI()->FindObject(nom_det);
      return det;
   }
   return 0;
}

//_______________________________________________________________________________________

void KVINDRA::GetIDTelescopes(KVDetector * de, KVDetector * e,
                              TCollection * idtels)
{
   //Override KVMultiDetArray method for special case of "etalon" modules:
   //we need to add ChIo-CsI identification telescope by hand

   KVMultiDetArray::GetIDTelescopes(de, e, idtels);

   if (de->InheritsFrom("KVSiLi") && e->InheritsFrom("KVCsI")) {
      KVChIo *chio = (KVChIo*)((KVINDRADetector*)e)->GetChIo();
      if (chio) {
         /*printf("chio=%p\n",(TObject*)chio);
         printf("chio-class=%s\n",((TObject*)chio)->ClassName());
         chio->Print("all");*/
         KVMultiDetArray::GetIDTelescopes(chio, e, idtels);
      }
   }

}

//_______________________________________________________________________________________

void KVINDRA::SetPinLasersForCsI()
{
	// Sets the KVCsI::fPinLaser member of each CsI detector with the number of the
	// pin laser associated for the stability control of these detectors.
	//
	// We look for a file with the following format:
	//
	// CSI_0101     1
	// CSI_0102     1
	// CSI_0103     1
	// CSI_0104     1
	// etc.
	//
	// i.e. 'name of CsI detector'   'number of pin laser (1-8)'
	// Comment lines must begin with '#'
	//
	// The default name of this file is defined in .kvrootrc by
	//
	// INDRADB.CsIPinCorr:          CsI_PILA.dat
	//
	// Dataset-specific version can be specified:
	//
	// INDRA_e999.INDRADB.CsIPinCorr:    CorrCsIPin_2054.dat
	//
	// This file should be in the directory corresponding to the current dataset,
	// i.e. in $KVROOT/KVFiles/name_of_dataset

	ifstream pila_file;
	if( gDataSet->OpenDataSetFile( gDataSet->GetDataSetEnv("INDRADB.CsIPinCorr",""), pila_file ) ){

		Info("SetPinLasersForCsI", "Setting correspondance CsI-PinLaser using file %s.",
				gDataSet->GetDataSetEnv("INDRADB.CsIPinCorr",""));
		// read file, set correspondance
		KVString line;
		line.ReadLine(pila_file);
		while( pila_file.good() ){
			if( !line.BeginsWith("#") ){
				line.Begin(" ");
				KVString detname = line.Next(kTRUE);
				KVCsI* det = (KVCsI*)GetDetector(detname.Data());
				Int_t pila = line.Next(kTRUE).Atoi();
				if(det){
					det->SetPinLaser(pila);
				}
			}
			line.ReadLine(pila_file);
		}
		pila_file.close();
	}
	else
	{
		Info("SetPinLasersForCsI", "File %s not found. Correspondance Csi-PinLaser is unknown.",
				gDataSet->GetDataSetEnv("CsIPinCorr",""));
	}
}

//_______________________________________________________________________________________

void KVINDRA::LinkToCodeurs()
{
   
	// Link detectors with electronic modules
	// for the moment only QDC for Si and ChIo are implemented
	// This information is accessible via KVINDRADetector::GetNumeroCodeur()
	// To be active one has to put in the dataset directory
	// a file name Codeurs.dat containing the name of the file for the concerned type
	// of electronic module 
	// for example see INDRA_e613 dataset
	// [dataset name].INDRADB.Codeurs:    ...
	
	
	KVFileReader flist;
	TString fp;
	if (!KVBase::SearchKVFile(gDataSet->GetDataSetEnv("INDRADB.Codeurs",""), fp, gDataSet->GetName())){
		Error("LinkToCodeurs","Fichier %s, inconnu au bataillon",gDataSet->GetDataSetEnv("INDRADB.Codeurs",""));
		return;
	}
	
	if (!flist.OpenFileToRead(fp.Data())){
		//Error("ReadGainList","Error opening file named %s",fp.Data());
		return;
	}
	Info("LinkToCodeurs()", "Reading correspondance Codeur-Detecteur ...");
	
	TEnv* env = 0;
	KVINDRADetector* idet = 0;
	while (flist.IsOK()){
		flist.ReadLine(NULL);
		KVString file = flist.GetCurrentLine();
		if (file!=""){
		if ( KVBase::SearchKVFile(file.Data(), fp, gDataSet->GetName()) ){
			env = new TEnv();
			env->ReadFile(fp.Data(),kEnvAll);
			TEnvRec* rec = 0;
			TObjArray* toks = 0;
			TIter it(env->GetTable());
			while ( (rec = (TEnvRec* )it.Next()) ){
				if (!strcmp(rec->GetName(),"type")){
					Info("LinkToCodeurs","Module type %s",rec->GetValue());	
				}
				else {
					toks = TString(rec->GetValue()).Tokenize(",");
					for (Int_t ii=0;ii<toks->GetEntries();ii+=1){
						idet = (KVINDRADetector* )gIndra->GetDetector( ((TObjString* )toks->At(ii))->GetString().Data() );
						if (idet)
							idet->SetNumeroCodeur(TString(rec->GetName()).Atoi());
					}
					delete toks;
				}
			}
			delete env;
		}
		}
	}
	
	flist.CloseFile();
	
	
}


//________________________________________________________________________________________
TGraph *KVINDRA::GetPedestals(const Char_t * det_signal,const Char_t * det_type, Int_t ring_number,Int_t
run_number)
{

	//Renvoie sous forme de TGraph (en fonction du numero de module)
	//les piedestaux du signal (det_signal) asssocies aux detecteurs de type (det_type)
	//qui sont presents dans la couronne ring_number pour un numero de run donne (si run_number==-1)
	//on suppose que gIndra->SetParameters(xxx) a ete fait en amont
	//L'utilisateur doit effacer ce TGraph tout seul comme un grand apres usage
	//Une recherche sur l existence ou non du graph permet d eviter des boucles inutiles
	//Si l appel est reitere

	if (run_number!=-1 || run_number!=Int_t(GetCurrentRunNumber()))
		SetParameters(run_number);

	KVSeqCollection* sltype = 0;
	KVSeqCollection* slring = 0;
	TGraph* gr_ped=0;

	KVString sgraph; sgraph.Form("KVPed_%s_%s_%d_%d",det_signal,det_type,ring_number,GetCurrentRunNumber());
	if ( ( gr_ped=(TGraph* )gROOT->FindObject(sgraph.Data()) ) ) return gr_ped;

	if ( (sltype = GetListOfDetectors()->GetSubListWithMethod(det_type,"GetType")) ){
		KVString sring; sring.Form("%d",ring_number);
		if ( (slring = sltype->GetSubListWithMethod(sring,"GetRingNumber")) ){
				gr_ped = new TGraph(); gr_ped->SetName(sgraph.Data());
				for (Int_t mm=0;mm<slring->GetEntries();mm+=1){
					gr_ped->SetPoint(gr_ped->GetN(),
						((KVINDRADetector*)slring->At(mm))->GetModuleNumber(),
						((KVDetector*)slring->At(mm))->GetPedestal(det_signal));

				}
			delete slring;
			return gr_ped;
		}
		delete sltype;
	}
	return 0;

}

//_____________________________________________________________________________

void KVINDRA::GetDetectorEvent(KVDetectorEvent* detev, KVSeqCollection* fired_params )
{
   // Overrides KVMultiDetArray::GetDetectorEvent.
   // If the list of fired acquisition parameters is given (meaning we are reading raw data)
   // then we check that what we have read is in fact an INDRA event
   // (see KVINDRATriggerInfo::IsINDRAEvent()) : if not, we do not try to find the hit groups.
   
   if( fired_params && !GetTriggerInfo()->IsINDRAEvent() ) return;
   KVMultiDetArray::GetDetectorEvent(detev,fired_params);
}

//_______________________________________________________________________________

void KVINDRA::SetGGtoPGConversionFactors()
{
   // Sets the parameters for linear conversion of silicon & ChIo coder values
   // between GG and PG, using the following formula:
   //
   //   PG  = alpha + beta*(GG - GG_0) + PG_0
   //
   // where GG_0 and PG_0 are respectively GG and PG pedestals
   //
   // We look for the file whose name is given by the .kvrootrc variable
   //    [dataset].INDRADB.GGtoPGFactors:
   // or by default
   //    INDRADB.GGtoPGFactors:
   // and expect to find in it a line for each detector of the form:
   //    Det_Name   alpha   beta
   // Comments in the file can be written on lines beginning with the character '#'
   
	ifstream datfile;
	if( !gDataSet->OpenDataSetFile( gDataSet->GetDataSetEnv("INDRADB.GGtoPGFactors",""), datfile ) ){

		Info("SetGGtoPGConversionFactors", "Cannot open file with parameters for conversion (%s).",
				gDataSet->GetDataSetEnv("INDRADB.GGtoPGFactors",""));
	   return;
	}
	else
	{
		Info("SetGGtoPGConversionFactors", "Reading parameters from file %s",
				gDataSet->GetDataSetEnv("INDRADB.GGtoPGFactors",""));

   	Char_t detname[30];
   	Double_t a, b;
   	TString aline;
      aline.ReadLine(datfile);
   	while (datfile.good()) {
	
     		if (aline[0]!='#') {     //skip comments

				sscanf(aline.Data(), "%s %lf %lf", detname, &a, &b);
         	KVINDRADetector *det = (KVINDRADetector *)GetDetector(detname);
         	if (!det) {
            	//no detector found with cou, mod and type
            	Error("SetGGtoPGConversionFactors", "Unknown detector : %s", detname);
         	}
         	else{
         		det->SetGGtoPGConversionFactors(a,b);
         		//Info("SetGGtoPGConversionFactors", "%s : PG = %f + %f * GG", detname, a, b);
      		}
      	}
      	aline.ReadLine(datfile);
   	}                            //while( datfile.good()
   	datfile.close();
   }
}

//_________________________________________________________________________________

TGeoManager* KVINDRA::CreateGeoManager(Double_t dx, Double_t dy, Double_t dz, Bool_t closegeo)
{   
   // Overrides KVMultiDetArray::CreateGeoManager in order to use INDRAGeometryBuilder
   // which builds the TGeo representation of INDRA using the Y. Huguet CAO data.
   //
   // The optional arguments (dx,dy,dz) are the half-lengths in centimetres of the "world"/"top" volume
   // into which all the detectors of the array are placed. This should be big enough so that all detectors
   // fit in. The default values of 500 give a "world" which is a cube 1000cmx1000cmx1000cm (with sides
   // going from -500cm to +500cm on each axis).

   if (!IsBuilt()) return NULL;
   INDRAGeometryBuilder igb;
   // build multidetector, but not the target. energy losses in target are handled
   // by KVMultiDetArray::DetectEvent
   return igb.Build(kFALSE);
}


