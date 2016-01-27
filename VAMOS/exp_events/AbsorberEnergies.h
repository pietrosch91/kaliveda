// Author: Peter C. Wigg
// Created Wed 20 Jan 14:07:48  2016

////////////////////////////////////////////////////////////////////////////////
//
// AbsorberEnergies.h
//
// Description
//
// Basic class acting as a container for the energy loss information of a
// particle passing through the VAMOS absorber materials.
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Wed 20 Jan 14:07:48  2016
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __STAGE_ENERGIES_H__
#define __STAGE_ENERGIES_H__

#include "Rtypes.h"
#include <cassert>

// Namespace for the identification correction code, to prevent polluting the
// global namespace.
namespace idc {

   class AbsorberEnergies {

      // Copy function
      // Parameter: rhs - Target to copy
      void Copy(const AbsorberEnergies& rhs);

   public:

      // Default constructor
      AbsorberEnergies();
      // Destructor
      virtual ~AbsorberEnergies();

      // Copy constructor
      // Parameter: rhs - Target to copy
      AbsorberEnergies(const AbsorberEnergies& rhs);
      // Assignment operator
      // Parameter: rhs - Target to copy
      AbsorberEnergies& operator=(const AbsorberEnergies& rhs);

      // Reset the stored data to their default values
      void Reset();

      // Energy loss in the target
      Double_t target;
      // Energy loss in the strip foil
      Double_t strip_foil;
      // Energy loss in drift chamber 1
      Double_t dc1;
      // Energy loss in the ACTIVE region of drift chamber 1
      Double_t dc1_active;
      // Energy loss in the SED
      Double_t sed;
      // Energy loss in drift chamber 2
      Double_t dc2;
      // Energy loss in the ACTIVE region of drift chamber 2
      Double_t dc2_active;
      // Energy loss in the ionisation chamber
      Double_t ic;
      // Energy loss in the ACTIVE region of the ionisation chamber
      Double_t ic_active;
      // Energy loss in the silicon detector
      Double_t si;
      // Energy loss in the isobutane dead-layer
      Double_t iso_sicsi;
      // Energy loss in the caesium iodide detector
      Double_t csi;

      ClassDef(AbsorberEnergies, 1);
   };

} // namespace idc

#endif
