//Created by KVClassFactory on Thu Feb  3 10:04:41 2011
//Author: frankland,,,,

#ifndef __KVIONRANGETABLE_H
#define __KVIONRANGETABLE_H

#include "KVBase.h"

namespace Units {
   // UNITS
   // Standard units are:
   //   [L]   cm
   //   [M]  g
   //   [P]   torr
   // lengths
   Double_t cm = 1.0;
   Double_t um = 1.e-4;
   Double_t mm = 1.e-1;
   Double_t m = 1.e+2;
   //  weights
   Double_t g = 1.0;
   Double_t kg = 1.e+3;
   Double_t mg = 1.e-3;
   Double_t ug = 1.e-6;
   // pressures
   Double_t torr = 1.0;
   Double_t mbar = 0.75;
   Double_t atm = 760.;
};


class KVIonRangeTable : public KVBase {

public:
   KVIonRangeTable();
   virtual ~KVIonRangeTable();

   static KVIonRangeTable* GetRangeTable(const Char_t* name);

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
   
   // Returns range (in mg/cm**2) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   virtual Double_t GetRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.) = 0;

   // Returns linear range (in cm) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   virtual Double_t GetLinearRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.) = 0;

   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness r (in mg/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   virtual Double_t GetDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.) = 0;
      
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness d (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   virtual Double_t GetLinearDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat=0., Double_t T=-1., Double_t P=-1.) = 0;

   ClassDef(KVIonRangeTable, 1) //Abstract base class for calculation of range & energy loss of charged particles in matter
};

#endif
