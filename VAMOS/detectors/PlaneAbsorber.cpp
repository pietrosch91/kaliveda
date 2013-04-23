// --------------------------------------------
// PlaneAbsorber.cpp
// 
// Class used to model generic plane absorbers 
// in the VAMOS detection system (E503) - to 
// model the inactive sed for example.
//
// Author: Peter C. Wigg 
// Date: 14/09/10
// --------------------------------------------

#include "PlaneAbsorber.h"

PlaneAbsorber::PlaneAbsorber(){
    
    // Default Plane Absorber

    angle = 0.;
    thickness = 0.;

    realThickness = thickness/TMath::Cos(GetOrientation("RAD"));
    absorber = 0;
	
    absorber = new KVDetector();
    absorber->SetTitle("PLANE_ABS");
    absorberMaterial = new KVMaterial("Myl",realThickness);  // default Mylar - no thickness, no orientation
    absorber->AddAbsorber(absorberMaterial);
    absorber->SetActiveLayer(-1);                           // Inactive
}

PlaneAbsorber::~PlaneAbsorber(){
       if(absorber != 0){
        delete absorber;
        absorber=0;
    }
}

void PlaneAbsorber::SetMaterial(const Char_t *type){
    absorberMaterial->SetMaterial(type);
    absorberMaterial->SetThickness(realThickness);
}

const Char_t* PlaneAbsorber::GetMaterial(){
    return absorberMaterial->GetType();
}

void PlaneAbsorber::SetPressure(Float_t pressure){
    absorberMaterial->SetPressure(pressure);
}

Float_t PlaneAbsorber::GetPressure(){
    return (Float_t)absorberMaterial->GetPressure();
}

Float_t PlaneAbsorber::GetThickness(const Char_t* opt){

    if(!strcmp(opt,"TRUE")){
        return absorberMaterial->GetThickness();
    }else if(!strcmp(opt,"NORM")){
        return (absorberMaterial->GetThickness())*TMath::Cos(GetOrientation("RAD"));
    }else{
        // Return some recognisable number 
        printf("Error: Invalid Option in GetThickness\n");
        return -99.; 
    }
}

void PlaneAbsorber::SetThickness(Float_t thick, const Char_t *opt){
    
    if(!strcmp(opt,"TRUE")){ // As seen by a projectile       
        realThickness = thick;
        absorberMaterial->SetThickness(thick);
        thickness = (absorberMaterial->GetThickness())*TMath::Cos(GetOrientation("RAD"));
    }else if(!strcmp(opt,"NORM")){  // Perpendicular thickness of the plane
        thickness = thick;
        realThickness = thickness/TMath::Cos(GetOrientation("RAD"));
        absorberMaterial->SetThickness(realThickness);
    }else{
        // Do Nothing        
        printf("Error: Invalid Option in SetThickness\n");
    }

}

Float_t PlaneAbsorber::GetOrientation(const Char_t *opt){

    if(!strcmp(opt,"DEG")){
        return angle;
    }else if(!strcmp(opt,"RAD")){
        Float_t angleRad = angle*(TMath::Pi()/180);
        return angleRad;
    }else{
        // Return some recognisable number 
        printf("Error: Invalid Option in GetThickness\n");
        return -99.; 
    }
}

void PlaneAbsorber::SetOrientation(Float_t ang, const Char_t *opt){

    if(!strcmp(opt,"DEG")){
        angle = ang;
        realThickness = thickness/TMath::Cos(GetOrientation("RAD"));
        absorberMaterial->SetThickness(realThickness);
    }else if(!strcmp(opt,"RAD")){
        angle = ang*(180/TMath::Pi()); 
        realThickness = thickness/TMath::Cos(GetOrientation("RAD"));
        absorberMaterial->SetThickness(realThickness);
    }else{
        // Do Nothing
        printf("Error: Invalid Option in SetOrientation\n");
    }

}

KVDetector* PlaneAbsorber::GetDetector(){
    return (KVDetector*)absorber;
}

Float_t PlaneAbsorber::GetEnergy(){
    return absorber->GetEnergy();
}

Float_t PlaneAbsorber::GetEnergyLoss(){
    return absorberMaterial->GetEnergyLoss();
}

void PlaneAbsorber::SetActive(){
    absorber->SetActiveLayer(absorberMaterial);
}
		
void PlaneAbsorber::Clear(){
	absorber->Clear();
}
