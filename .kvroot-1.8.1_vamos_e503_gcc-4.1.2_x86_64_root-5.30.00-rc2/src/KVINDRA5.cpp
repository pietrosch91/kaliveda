/***************************************************************************
                          kvindra5.cpp  -  description
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
#include "TROOT.h"
#include "KVINDRA5.h"
#include "KVTelescope.h"
#include "KVSilicon.h"
#include "KVChIo.h"
#include "KVCsI.h"

ClassImp(KVINDRA5);
////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRA5</h2>
<h4>INDRA multidetector array, configuration for 5th campaign experiments</h4>

<img src="images/indra_camp5.png"><br>

<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRA5::KVINDRA5()
{
}

//____________________________________________________________________________________

KVINDRA5::~KVINDRA5()
{
}

//____________________________________________________________________________________

void KVINDRA5::MakeListOfDetectorTypes()
{
   //Add Si150um detectors to standard INDRA list

   KVINDRA4::MakeListOfDetectorTypes();

	KVSilicon *kvsi = (KVSilicon* )KVDetector::MakeDetector(Form("%s.SI",fDataSet.Data()), 150.0);
	//kvsi->SetThickness(150.0);
	kvsi->SetLabel("SI150");
	fDetectorTypes->Add(kvsi);
}

//________________________________________________________________________________________

void KVINDRA5::PrototypeTelescopes()
{
   //Make Si150-Csixxcm telescopes

   KVINDRA4::PrototypeTelescopes();
   // Si-CsI Ring 2
   KVINDRATelescope *kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si150-CsI Ring2");
   kvt->SetAzimuthalWidth(27.51);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 3
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si150-CsI Ring3");
   kvt->SetAzimuthalWidth(14.15);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 4
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si150-CsI Ring4");
   kvt->SetAzimuthalWidth(14.01);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 5
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si150-CsI Ring5");
   kvt->SetAzimuthalWidth(14.29);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 6
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI97"));
   kvt->SetType("Si150-CsI Ring6");
   kvt->SetAzimuthalWidth(14.29);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 7
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI97"));
   kvt->SetType("Si150-CsI Ring7");
   kvt->SetAzimuthalWidth(14.48);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 8
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI90"));
   kvt->SetType("Si150-CsI Ring8");
   kvt->SetAzimuthalWidth(14.18);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 9
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI90"));
   kvt->SetType("Si150-CsI Ring9");
   kvt->SetAzimuthalWidth(14.32);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
}

//_________________________________________________________________________________________

void KVINDRA5::Build()
{
   //construct specific realisation of INDRA array for 5th campaign
   //start from INDRA4 (with 1st ring Si-Csi) and replaces some Si300 with Si150
  
   KVINDRA4::Build();
   SetName("INDRA");
   SetTitle("5th campaign INDRA detector");
}

//_________________________________________________________________________________________

void KVINDRA5::BuildGeometry()
{
   //place 150um Si-CsI telescopes in INDRA4 array

   KVINDRA4::BuildGeometry();
   // 2-21
   ReplaceTelescope("SI_CSI_0221",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring2"));
   ReplaceTelescope("SI_CSI_0321",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring3"));
   ReplaceTelescope("SI_CSI_0322",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring3"));
   // 4-9
   ReplaceTelescope("SI_CSI_0409",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring4"));
   ReplaceTelescope("SI_CSI_0410",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring4"));
   ReplaceTelescope("SI_CSI_0509",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring5"));
   ReplaceTelescope("SI_CSI_0510",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring5"));
   // 6-3
   ReplaceTelescope("SI_CSI_0603",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring6"));
   ReplaceTelescope("SI_CSI_0604",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring6"));
   ReplaceTelescope("SI_CSI_0703",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring7"));
   ReplaceTelescope("SI_CSI_0704",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring7"));
   // 6-13
   ReplaceTelescope("SI_CSI_0613",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring6"));
   ReplaceTelescope("SI_CSI_0614",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring6"));
   ReplaceTelescope("SI_CSI_0713",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring7"));
   ReplaceTelescope("SI_CSI_0714",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring7"));
   // 8-7
   ReplaceTelescope("SI_CSI_0807",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0808",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0907",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   ReplaceTelescope("SI_CSI_0908",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   // 8-17
   ReplaceTelescope("SI_CSI_0817",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0818",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0917",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   ReplaceTelescope("SI_CSI_0918",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   // 8-23
   ReplaceTelescope("SI_CSI_0823",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0824",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0923",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   ReplaceTelescope("SI_CSI_0924",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
}


