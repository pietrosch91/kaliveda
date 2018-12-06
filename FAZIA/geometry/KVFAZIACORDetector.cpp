//Created by KVClassFactory on Tue Aug 29 16:12:18 2017
//Author: gruyer diego

#include "KVFAZIACORDetector.h"
#include "KVFAZIACalibrator.h"
#include "KVIDTelescope.h"
#include "KVFAZIA.h"
#include "KVSignal.h"
#include "KVPSAResult.h"
#include "TClass.h"
#include "KVLightEnergyCsI.h"

ClassImp(KVFAZIACORDetector)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIACORDetector</h2>
<h4>re-implementation due to special CSI calibration</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIACORDetector::KVFAZIACORDetector()
   : KVFAZIADetector()
{
   // Default constructor
}

//____________________________________________________________________________//

KVFAZIACORDetector::KVFAZIACORDetector(const Char_t* type, const Float_t thick)
   : KVFAZIADetector(type, thick)
{
   // Constructor inherited from KVFAZIADetector
}

//____________________________________________________________________________//

KVFAZIACORDetector::~KVFAZIACORDetector()
{
   // Destructor
}

//________________________________________________________________
void KVFAZIACORDetector::SetCalibrators()
{
   //Set up calibrators for this detector. Call once name has been set.
   //test to check that there is not already defined calibrators
   //
   if (GetListOfCalibrators())
      return;

   TString sf = "";

   if (fIdentifier == kCSI) {
      fChannelToEnergy = new KVLightEnergyCsI(this);
      fChannelToEnergy->SetName(GetName());
      fChannelToEnergy->SetType("Channel-Energy");

   }
   else {
      fChannelToEnergy = new KVFAZIACalibrator(GetName(), "Channel-Energy");
      fChannelToEnergy->SetDetector(this);
      sf = gEnv->GetValue("FAZIADetector.Calib.Channel-Energy", "");
      if (sf == "") {
         Warning("SetCalibrators", "No formula defined for Calibration Channel-Energy");
      }
      else {
         ((KVFAZIACalibrator*)fChannelToEnergy)->SetFunction(sf.Data());
      }
   }

   fChannelToVolt = new KVFAZIACalibrator(GetName(), "Channel-Volt");
   fChannelToVolt->SetDetector(this);
   sf = gEnv->GetValue("FAZIADetector.Calib.Channel-Volt", "");
   if (sf == "") {
      Warning("SetCalibrators", "No formula defined for Calibration Channel-Volt");
   }
   else {
      ((KVFAZIACalibrator*)fChannelToVolt)->SetFunction(sf.Data());
   }

   fVoltToEnergy = new KVFAZIACalibrator(GetName(), "Volt-Energy");
   fVoltToEnergy->SetDetector(this);
   sf = gEnv->GetValue("FAZIADetector.Calib.Volt-Energy", "");
   if (sf == "") {
      Warning("SetCalibrators", "No formula defined for Calibration Volt-Energy");
   }
   else {
      ((KVFAZIACalibrator*)fVoltToEnergy)->SetFunction(sf.Data());
   }

   AddCalibrator(fChannelToEnergy);
   AddCalibrator(fChannelToVolt);
   AddCalibrator(fVoltToEnergy);

}
