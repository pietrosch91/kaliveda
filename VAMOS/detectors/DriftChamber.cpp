// ----------------------------------
// DriftChamber.cpp 
// 
// Class used to model the drift chambers 
// in VAMOS detection system (E503) 
//
// Author: Peter C. Wigg 
// Date: 14/09/10
// ----------------------------------

#include "DriftChamber.h"

DriftChamber::DriftChamber(){

    // Default drift chamber 
    entranceWindow = 0;
    exitWindow = 0;
    gas = 0;
    driftChamber = 0;
        
    entranceWindow = new KVMaterial("Myl", 1.5);    // 1.5um Mylar Window
    exitWindow = new KVMaterial("Myl", 1.5);        // 1.5um Mylar Window

    gas = new KVMaterial("C4H10", 130.);            // 130mm

    driftChamber = new KVDetector();             

    driftChamber->AddAbsorber(entranceWindow);      // Entrance Window 
    driftChamber->AddAbsorber(gas);                 // Gas Layer 
    driftChamber->SetActiveLayer(gas);
    gas->SetPressure(18.);                          // 18mBar
    driftChamber->AddAbsorber(exitWindow);          // Exit Window 

}

DriftChamber::~DriftChamber(){
       if(entranceWindow != 0){
        delete entranceWindow;
        entranceWindow=0;
    }
    if(exitWindow != 0){
        delete exitWindow;
        exitWindow=0;
    }
    if(gas != 0){
        delete gas;  
        gas=0;
    }
    if(driftChamber != 0){
        delete driftChamber;
        driftChamber=0;
    }
}

Float_t DriftChamber::GetGasPressure(){
    return (Float_t)(gas->GetPressure());
}

Float_t DriftChamber::GetGasThickness(){
    return (Float_t)gas->GetThickness();
}

Float_t DriftChamber::GetEntranceWindowThickness(){
    return (Float_t)entranceWindow->GetThickness();
}

Float_t DriftChamber::GetExitWindowThickness(){
    return (Float_t)exitWindow->GetThickness();
}

Float_t DriftChamber::GetEnergyLoss(){
    return (Float_t)(entranceWindow->GetEnergyLoss() +
                    gas->GetEnergyLoss() +
                    exitWindow->GetEnergyLoss());
}

Float_t DriftChamber::GetEnergyLoss(Int_t n){

    switch(n){
        case 1:
                return (Float_t)entranceWindow->GetEnergyLoss();
        case 2:
                return (Float_t)gas->GetEnergyLoss();
        case 3:
                return (Float_t)exitWindow->GetEnergyLoss();
        default:
                return 0.;
    }
}

Float_t DriftChamber::GetEnergy(){
    return (Float_t)driftChamber->GetEnergy();
}

KVDetector* DriftChamber::GetDetector(){
    return (KVDetector*)driftChamber;
}
