#ifndef KVUNITS__H
#define KVUNITS__H

#include <Rtypes.h>

namespace KVUnits {
   // UNITS
   // Standard units are:
   //   [L]   cm
   //   [M]  g
   //   [P]   Torr
   // lengths
const Double_t cm = 1.0;
const Double_t um = 1.e-4;
const Double_t mm = 1.e-1;
const Double_t m = 1.e+2;
   //  masses
const Double_t g = 1.0;
const Double_t kg = 1.e+3;
const Double_t mg = 1.e-3;
const Double_t ug = 1.e-6;
   // pressures
const Double_t torr = 1.0;
const Double_t atm = 760.;
const Double_t Pa = atm/101325.;
const Double_t mbar = 100.*Pa;
};

#endif
