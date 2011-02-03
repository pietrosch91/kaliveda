//Created by KVClassFactory on Thu Feb  3 10:04:41 2011
//Author: frankland,,,,

#ifndef __KVIONRANGETABLE_H
#define __KVIONRANGETABLE_H

#include "KVBase.h"

namespace Units
{
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
   

class KVIonRangeTable : public KVBase
{

   public:
   KVIonRangeTable();
   virtual ~KVIonRangeTable();

   static KVIonRangeTable* GetRangeTable(const Char_t* name);
   
   // Returns density (g/cm**3) of a material in the range tables
   virtual Double_t GetDensity(const Char_t* ) = 0;
   
   // Returns density (g/cm**3) of a gaseous material in the range tables
   // Temperature should be given in degrees celsius, and pressure in Torr
   virtual Double_t GetGasDensity(const Char_t*, Double_t temperature, Double_t pressure) = 0;
   
   // Returns atomic number of a material in the range tables
   virtual Double_t GetZ(const Char_t* ) = 0;
   
   // Returns atomic mass of a material in the range tables
   virtual Double_t GetAtomicMass(const Char_t* ) = 0;
   
   // Return kTRUE if material is in range tables
   virtual Bool_t IsMaterialKnown(const Char_t* ) = 0;
   
   // Return kTRUE if material is gaseous
   virtual Bool_t IsMaterialGas(const Char_t* ) = 0;
   
   // Return name of material of given type or name if it is in range tables
   virtual const Char_t* GetMaterialName(const Char_t* ) = 0;
   
   ClassDef(KVIonRangeTable,1)//Abstract base class for calculation of range & energy loss of charged particles in matter
};

#endif
