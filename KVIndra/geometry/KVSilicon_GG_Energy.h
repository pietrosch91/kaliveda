//Created by KVClassFactory on Tue Jul 24 10:16:44 2018
//Author: eindra

#ifndef __KVSILICON_GG_ENERGY_H
#define __KVSILICON_GG_ENERGY_H

#include "KVSilicon.h"
#include "KVChannelEnergyAlpha.h"

class KVSilicon_GG_Energy : public KVSilicon {
   KVChannelEnergyAlpha* fCalib;//!

public:
   KVSilicon_GG_Energy()
      : KVSilicon(), fCalib(nullptr) {}
   KVSilicon_GG_Energy(Float_t thick)
      : KVSilicon(thick), fCalib(nullptr) {}
   virtual ~KVSilicon_GG_Energy() {}

   void SetCalibrators();
   Bool_t IsCalibrated() const
   {
      return KVDetector::IsCalibrated();
   }
   Double_t GetCalibratedEnergy();

   ClassDef(KVSilicon_GG_Energy, 1) //INDRA Silicon detector with GG-Energy calibration
};

#endif
