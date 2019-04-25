//Created by KVClassFactory on Tue Jul 24 10:16:44 2018
//Author: eindra

#include "KVSilicon_GG_Energy.h"

ClassImp(KVSilicon_GG_Energy)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSilicon_GG_Energy</h2>
<h4>INDRA Silicon detector with GG-Energy calibration</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVSilicon_GG_Energy::SetCalibrators()
{
   //Set up calibrators for this detector. Call once name has been set.
   KVCalibrator* c = new KVChannelEnergyAlpha("GG", this);
   if (!AddCalibrator(c)) delete c;
   RefreshCalibratorPointers();
}

//__________________________________________________________________________________________
Double_t KVSilicon_GG_Energy::GetCalibratedEnergy()
{
   //Calculate energy in MeV from coder values.
   //Returns 0 if calibration not ready or no parameters fired

   if (IsCalibrated() && Fired("any"))
      return (fCalib->Compute(GetGG() - GetPedestal("GG")));
   return 0;
}

void KVSilicon_GG_Energy::RefreshCalibratorPointers()
{
   fCalib = (KVChannelEnergyAlpha*) GetCalibrator("Channel-Energy alpha GG");
}
