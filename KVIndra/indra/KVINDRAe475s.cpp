/*
$Id: KVINDRAe475s.cpp,v 1.7 2008/12/17 13:01:26 franklan Exp $
$Revision: 1.7 $
$Date: 2008/12/17 13:01:26 $
*/

//Created by KVClassFactory on Wed May 17 12:26:57 2006
//Author: franklan

#include "KVINDRAe475s.h"
#include "KVSilicon.h"
#include "KVCsI.h"
#include "KVIDGridManager.h"

ClassImp(KVINDRAe475s)
////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRAe475s</h2>
<h4>INDRA multidetector array, configuration for experiment E475S</h4>

<img src="images/indra_e475s.png"><br>

<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
    KVINDRAe475s::KVINDRAe475s()
{
   //Default constructor
}

KVINDRAe475s::~KVINDRAe475s()
{
   //Destructor
}

//____________________________________________________________________________________

void KVINDRAe475s::MakeListOfDetectorTypes()
{
   //Add Si150um detectors to standard INDRA list

   KVINDRA4::MakeListOfDetectorTypes();

	KVSilicon *kvsi = (KVSilicon* )KVDetector::MakeDetector(Form("%s.SI",gDataSet->GetName()), 150.0);
	//kvsi->SetThickness(150.0);
	kvsi->SetLabel("SI150");
	fDetectorTypes->Add(kvsi);
}

//____________________________________________________________________________________

void KVINDRAe475s::PrototypeTelescopes()
{
   //Make Si150-Csixxcm telescope prototypes for rings 6 & 7

   KVINDRA4::PrototypeTelescopes();

   // Si-CsI Ring 6
   KVTelescope *kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI97"));
   kvt->SetType("Si150-CsI Ring6");
   kvt->SetAzimuthalWidth(14.29);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 7
   kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI97"));
   kvt->SetType("Si150-CsI Ring7");
   kvt->SetAzimuthalWidth(14.48);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
}

//____________________________________________________________________________________

void KVINDRAe475s::BuildGeometry()
{
   //Modify INDRA_camp4 multidetector so that it corresponds to E475S experiment:
   //      - no 1st ring
   //      - no ChIo after ring 12
   //      - all silicons on rings 6&7 are 150um thick

   KVINDRA4::BuildGeometry();
   // ring 6
   for (int i = 1; i <= 24; i++)
      ReplaceTelescope(Form("SI_CSI_06%02d", i),
                       (KVTelescope *) fTelescopes->
                       FindObjectByType("Si150-CsI Ring6"));
   // ring 7
   for (int i = 1; i <= 24; i++)
      ReplaceTelescope(Form("SI_CSI_07%02d", i),
                       (KVTelescope *) fTelescopes->
                       FindObjectByType("Si150-CsI Ring7"));

   RemoveRing("SI-CSI", 1);
   RemoveRing("CHIO", 13);
   RemoveRing("CHIO", 14);
   RemoveRing("CHIO", 16);
}

//_________________________________________________________________________________________

void KVINDRAe475s::Build()
{
   //construct specific realisation of INDRA array for E475S experiment
   //set pedestals for all detectors

   KVINDRA4::Build();
   SetName("INDRA");
   SetTitle("INDRA detector, E475S configuration");

}
