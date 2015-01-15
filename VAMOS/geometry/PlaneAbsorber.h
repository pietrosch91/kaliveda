// --------------------------------------------
// PlaneAbsorber.h
// 
// Class used to model generic plane absorbers 
// in the VAMOS detection system (E503) - to 
// model the inactive sed for example.
//
// Author: Peter C. Wigg 
// Date: 14/09/10
// --------------------------------------------

#ifndef PLANE_ABSORBER_H
#define PLANE_ABSORBER_H 

#include "KVMaterial.h"
#include "KVDetector.h"
#include "TMath.h"
#include "Rtypes.h"

class PlaneAbsorber{

        Float_t angle;
        Float_t thickness;
        Float_t realThickness;

        KVDetector *absorber;

        KVMaterial *absorberMaterial;

    public:

        PlaneAbsorber();
        ~PlaneAbsorber();

        void SetMaterial(const Char_t *type);
        void SetThickness(Float_t, const Char_t *);
        void SetPressure(Float_t);
        void SetOrientation(Float_t, const Char_t *);

        const Char_t* GetMaterial();

        Float_t GetOrientation(const Char_t *);
        Float_t GetThickness(const Char_t *);
        Float_t GetPressure();
        Float_t GetEnergy();
        Float_t GetEnergyLoss();
    
        void SetActive();

        void Clear();
	
        KVDetector* GetDetector();

};

#endif
