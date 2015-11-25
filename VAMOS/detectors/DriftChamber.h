// ----------------------------------
// DriftChamber.h
//
// Class used to model the drift chambers
// in VAMOS detection system (E503)
//
// Author: Peter C. Wigg
// Date: 14/09/10
// ----------------------------------

#ifndef DRIFT_CHAMBER_H
#define DRIFT_CHAMBER_H

#include "KVMaterial.h"
#include "KVDetector.h"
#include "Rtypes.h"

class DriftChamber {

   KVDetector* driftChamber;

   KVMaterial* entranceWindow;
   KVMaterial* exitWindow;
   KVMaterial* gas;

public:

   DriftChamber();
   ~DriftChamber();

   Float_t GetEntranceWindowThickness();
   Float_t GetExitWindowThickness();
   Float_t GetGasPressure();
   Float_t GetGasThickness();

   void SetEntranceWindowThickness(Float_t x)
   {
      entranceWindow->SetThickness(x);
   };
   void SetExitWindowThickness(Float_t x)
   {
      exitWindow->SetThickness(x);
   };
   void SetGasPressure(Float_t x)
   {
      gas->SetPressure(x);
   };
   void SetGasThickness(Float_t x)
   {
      gas->SetThickness(x);
   };

   Float_t GetEnergyLoss();
   Float_t GetEnergyLoss(Int_t n);
   Float_t GetEnergy();
   KVDetector* GetDetector();

};

#endif
