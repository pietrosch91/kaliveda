// --------------------------------------------
// IonisationChamber.h
// 
// Class used to model the Ionisation 
// Chamber in the VAMOS detection system (E503) 
//
// Author: Peter C. Wigg 
// Date: 14/09/10
// --------------------------------------------

#ifndef IONISATION_CHAMBER_H
#define IONISATION_CHAMBER_H

#include "KVMaterial.h"
#include "KVDetector.h"
#include "Rtypes.h"

class IonisationChamber{

        KVDetector *ioCh;

        KVMaterial *entranceWindow;
        KVMaterial *exitWindow;
        KVMaterial *deadGas;
        KVMaterial *activeGas;

    public:

        IonisationChamber();
        ~IonisationChamber();

        Float_t GetDeadGasThickness();
        Float_t GetDeadGasPressure();
        Float_t GetActiveGasThickness();
        Float_t GetActiveGasPressure();
        Float_t GetEntranceWindowThickness();
        Float_t GetExitWindowThickness();

        void SetDeadGasThickness(Float_t x){deadGas->SetThickness(x);};
        void SetDeadGasPressure(Float_t x){deadGas->SetPressure(x);};

        void SetActiveGasThickness(Float_t x){activeGas->SetThickness(x);};
        void SetActiveGasPressure(Float_t x){activeGas->SetPressure(x);};

        void SetEntranceWindowThickness(Float_t x){entranceWindow->SetThickness(x);};
        void SetExitWindowThickness(Float_t x){exitWindow->SetThickness(x);};

        KVDetector* GetDetector();
        Float_t GetEnergyLoss();
        Float_t GetEnergyLoss(Int_t);
        Float_t GetEnergy();


};
#endif
