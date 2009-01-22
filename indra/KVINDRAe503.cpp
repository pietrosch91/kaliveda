/*
$Id: KVINDRAe503.cpp,v 1.5 2008/12/17 13:01:26 franklan Exp $
$Revision: 1.5 $
$Date: 2008/12/17 13:01:26 $
*/

//Created by KVClassFactory on Wed May 17 12:26:57 2006
//Author: franklan

#include "KVINDRAe503.h"
#include "KVSilicon.h"
#include "KVCsI.h"

ClassImp(KVINDRAe503)
////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRAe503</h2>
<h4>INDRA multidetector array, configuration for experiments E503 & E494S</h4>

<img src="images/indra_e503.png"><br>

<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
    KVINDRAe503::KVINDRAe503()
{
   //Default constructor
}

KVINDRAe503::~KVINDRAe503()
{
   //Destructor
}

//____________________________________________________________________________________

void KVINDRAe503::MakeListOfDetectorTypes()
{
   //Add Si150um detectors to standard INDRA list

   KVINDRA4::MakeListOfDetectorTypes();

	KVSilicon *kvsi = (KVSilicon* )KVDetector::MakeDetector(Form("%s.SI",gDataSet->GetName()), 150.0);
	//kvsi->SetThickness(150.0);
	kvsi->SetLabel("SI150");
	fDetectorTypes->Add(kvsi);
}

//____________________________________________________________________________________

void KVINDRAe503::PrototypeTelescopes()
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

void KVINDRAe503::BuildGeometry()
{
   //Modify INDRA_camp4 multidetector so that it corresponds to E503 experiment:
   //      - no rings 1-3
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
   RemoveRing("SI-CSI", 2);
   RemoveRing("SI-CSI", 3);
   RemoveRing("CHIO", 2);
}

//_________________________________________________________________________________________

void KVINDRAe503::Build()
{
   //construct specific realisation of INDRA array for E503 experiment
   //      - the group corresponding to ChIo 4.17 is removed

   KVINDRA4::Build();
   
   GetRing("CHIO", 4)->RemoveTelescope("CI_0417");
   GetRing("SI-CSI", 4)->RemoveTelescope("SI_CSI_0417");
   GetRing("SI-CSI", 4)->RemoveTelescope("SI_CSI_0418");
   GetRing("SI-CSI", 5)->RemoveTelescope("SI_CSI_0517");
   GetRing("SI-CSI", 5)->RemoveTelescope("SI_CSI_0518");
   
   SetName("INDRA");
   SetTitle("INDRA detector, INDRA-VAMOS configuration");

}
