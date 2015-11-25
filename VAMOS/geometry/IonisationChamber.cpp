// --------------------------------------------
// IonisationChamber.cpp
//
// Class used to model the Ionisation
// Chamber in the VAMOS detection system (E503)
//
// Author: Peter C. Wigg
// Date: 14/09/10
// --------------------------------------------

#include "IonisationChamber.h"

IonisationChamber::IonisationChamber()
{

   // Default Ionisation Chamber
   ioCh = 0;
   entranceWindow = 0;
   exitWindow = 0;
   deadGas = 0;
   activeGas = 0;

   ioCh = new KVDetector();
   ioCh->SetTitle("IOCH");

   entranceWindow = new KVMaterial("Myl", 1.5);    // 1.5um mylar
   exitWindow = new KVMaterial("Myl", 1.5);

   deadGas = new KVMaterial("C4H10", 20.68);       // 20.68 mm Dead Layer
   deadGas->SetPressure(40.);                      // 40 mBar

   activeGas = new KVMaterial("C4H10", 104.57);    // 104.57 mm Active Zone
   activeGas->SetPressure(40.);

   ioCh->AddAbsorber(entranceWindow);              // Entrance Window
   ioCh->AddAbsorber(deadGas);                     // Gas Layer (Dead)
   ioCh->AddAbsorber(activeGas);                   // Gas Layer (Active)
   ioCh->SetActiveLayer(activeGas);
   ioCh->AddAbsorber(exitWindow);                  // Exit Window

}

IonisationChamber::~IonisationChamber()
{
   if (ioCh != 0) {
      delete ioCh;
      ioCh = 0;
   }
   if (entranceWindow != 0) {
      delete entranceWindow;
      entranceWindow = 0;
   }
   if (exitWindow != 0) {
      delete exitWindow;
      exitWindow = 0;
   }
   if (deadGas != 0) {
      delete deadGas;
      deadGas = 0;
   }
   if (activeGas != 0) {
      delete activeGas;
      activeGas = 0;
   }
}

Float_t IonisationChamber::GetDeadGasThickness()
{
   return (Float_t)deadGas->GetThickness();
}

Float_t IonisationChamber::GetDeadGasPressure()
{
   return (Float_t)deadGas->GetPressure();
}

Float_t IonisationChamber::GetActiveGasThickness()
{
   return (Float_t)activeGas->GetThickness();
}

Float_t IonisationChamber::GetActiveGasPressure()
{
   return (Float_t)activeGas->GetPressure();
}

Float_t IonisationChamber::GetEntranceWindowThickness()
{
   return (Float_t)entranceWindow->GetThickness();
}

Float_t IonisationChamber::GetExitWindowThickness()
{
   return (Float_t)exitWindow->GetThickness();
}

KVDetector* IonisationChamber::GetDetector()
{
   return (KVDetector*)ioCh;
}

Float_t IonisationChamber::GetEnergyLoss()
{
   return (Float_t)(entranceWindow->GetEnergyLoss() +
                    deadGas->GetEnergyLoss() + activeGas->GetEnergyLoss() +
                    exitWindow->GetEnergyLoss());
}

Float_t IonisationChamber::GetEnergyLoss(Int_t n)
{

   switch (n) {
      case 1:
         return (Float_t)entranceWindow->GetEnergyLoss();
      case 2:
         return (Float_t)deadGas->GetEnergyLoss();
      case 3:
         return (Float_t)activeGas->GetEnergyLoss();
      case 4:
         return (Float_t)exitWindow->GetEnergyLoss();
      default:
         // No Material: 0.0 will not affect subsequent calculations
         // unless used in a division/multiplication
         // but this will be easier to notice as an error

         return 0.;
   }
}

Float_t IonisationChamber::GetEnergy()
{
   return (Float_t)ioCh->GetEnergy();
}
