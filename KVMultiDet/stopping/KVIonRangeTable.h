//Created by KVClassFactory on Thu Feb  3 10:04:41 2011
//Author: frankland,,,,

#ifndef KVIONRANGETABLE_H
#define KVIONRANGETABLE_H

#include "KVBase.h"
#include "KVUnits.h"

class KVIonRangeTable : public KVBase {

public:
   enum SolType {
      kEmax,
      kEmin
   };
   
   KVIonRangeTable();
   virtual ~KVIonRangeTable();

   static KVIonRangeTable* GetRangeTable(const Char_t* name);
   
   // Create and fill a list of all materials for which range tables exist. 
   // Each entry is a TNamed with the name and type (title) of the material.
   virtual TObjArray* GetListOfMaterials() = 0;

   // Return maximum incident energy (in MeV) for which range table is valid for given
   // material and (Z,A) of incident ion
   virtual Double_t GetEmaxValid(const Char_t* material, Int_t Z, Int_t A) = 0;
   
   // Returns density (g/cm**3) of a material in the range tables
   virtual Double_t GetDensity(const Char_t*) = 0;

   // Set temperature (in degrees celsius) and pressure (in torr) for a given material.
   // This usually has no effect except for gaseous materials, for which T & P determine the density (in g/cm**3).
   virtual void SetTemperatureAndPressure(const Char_t*, Double_t temperature, Double_t pressure) = 0;

   // Returns atomic number of a material in the range tables
   virtual Double_t GetZ(const Char_t*) = 0;

   // Returns atomic mass of a material in the range tables
   virtual Double_t GetAtomicMass(const Char_t*) = 0;

   // Return kTRUE if material is in range tables
   virtual Bool_t IsMaterialKnown(const Char_t*) = 0;

   // Return kTRUE if material is gaseous
   virtual Bool_t IsMaterialGas(const Char_t*) = 0;

   // Return name of material of given type or name if it is in range tables
   virtual const Char_t* GetMaterialName(const Char_t*) = 0;
   
   // Returns range (in g/cm**2) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   virtual Double_t GetRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.) = 0;

   // Returns linear range (in cm) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   virtual Double_t GetLinearRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.) = 0;

   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness r (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   virtual Double_t GetDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.) = 0;
      
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness d (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   virtual Double_t GetLinearDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.) = 0;

   virtual Double_t GetEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.)=0;
   virtual Double_t GetLinearEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.)=0;
      
   virtual Double_t GetEIncFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.)=0;
   virtual Double_t GetLinearEIncFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.)=0;

   virtual Double_t GetEIncFromDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t DeltaE, Double_t e, enum SolType type = kEmax, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.)=0;
   virtual Double_t GetLinearEIncFromDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t DeltaE, Double_t e, enum SolType type = kEmax, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.)=0;

   virtual Double_t GetDeltaEFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t ERes, Double_t e, Double_t isoAmat = 0., Double_t T=-1., Double_t P=-1.)=0;
   virtual Double_t GetLinearDeltaEFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t ERes, Double_t e, Double_t isoAmat = 0., Double_t T=-1., Double_t P=-1.)=0;

   virtual Double_t GetPunchThroughEnergy(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) =0;
   virtual Double_t GetLinearPunchThroughEnergy(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.)=0 ;
   
   virtual Double_t GetMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) =0;
   virtual Double_t GetEIncOfMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) =0;
   virtual Double_t GetLinearMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) =0;
   virtual Double_t GetLinearEIncOfMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) =0;
   
   ClassDef(KVIonRangeTable, 1) //Abstract base class for calculation of range & energy loss of charged particles in matter
};

#endif
