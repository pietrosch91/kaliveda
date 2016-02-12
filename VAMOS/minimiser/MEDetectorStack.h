// Author: Peter C. Wigg
// Created Wed 20 Jan 14:12:39  2016

////////////////////////////////////////////////////////////////////////////////
//
// MEDetectorStack.h
//
// Description
//
// A simple Silicon-Isobutane-CsI detector stack, used in the A value estimation
// algorithms - see SiliconEnergyMinimiser. Allows one to calculate the "delta"
// between the measured and simulated silicon detector energies in order to
// determine the most likely candidate for the A value (ideally "delta" would be
// zero so we look for the minimum value of "delta").
//
// Peter C. Wigg
// Wed 20 Jan 14:12:39  2016
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MEDETECTORSTACK_H__
#define __MEDETECTORSTACK_H__

#include <cassert>
#include <stdexcept>
#include <vector>

#include "KVCalibrator.h"
#include "KVDetector.h"
#include "KVIDTelescope.h"
#include "KVLightEnergyCsIVamos.h"
#include "KVList.h"
#include "KVNucleus.h"
#include "KVTelescope.h"
#include "KVUnits.h"
#include "KVVAMOS.h"
#include "Rtypes.h"
#include "TError.h"
#include "TMath.h"

// Mass estimator namespace (me) to prevent polluting the global namespace with
// our generic naming conventions.
namespace me {

// Struct containing all necessary variables for the mass estimator simulation.
   struct SimulationParameters {
      Int_t    z;         // Measured Z
      Int_t    a;         // Test value for A
      Double_t si_energy; // Measured Si Energy
      Double_t csi_light; // Measure CsI Light
   };

// Struct to be populated with the results of the mass estimator simulation.
   struct SimulationResult {
      Int_t    z;          // Measured Z
      Int_t    a;          // Test A value used
      Double_t si_energy;  // Measured Si Energy
      Double_t gap_energy; // Simulated Gap Energy
      Double_t csi_energy; // Simulated CsI Energy
      Double_t delta;      // Difference in Si Energy (Sim, Expt)
   };

} // namespace me

class MEDetectorStack {

public:

   // Default constructor
   MEDetectorStack();

   // Destructor
   virtual ~MEDetectorStack();

   // Initialise the detector stack
   // Note: To be called immediately post-construction.
   Bool_t Init();

   // Public accessor used to evaluate the initialisation status.
   Bool_t IsInitialised() const;

   // Simulate particle passage through the detector stack and determine delta.
   //
   // This function calculates the difference between the experimentally
   // measured silicon energy and that simulated for a given test nucleus. The
   // results are written into the address provided by 'result'.
   //
   // Note: Not thread-safe (If using threads you need to wrap each call with a
   // mutex lock)
   //
   // Parameter: parameters - Set the input values of the simulation
   // Parameter: result - Struct used to return the results of the simulation
   //
   // See me::SimulationParameters
   // See me::SimulationResult
   //
   // Return: Boolean status indicating success or failure of the simulation.
   Bool_t Simulate(
      const struct me::SimulationParameters* const parameters,
      struct me::SimulationResult* const result
   );

   // Set the identification telescope to imitate.
   //
   // This function is responsible for setting the thickness of the silicon
   // layer and setting the correct light -> energy calibrator for the caesium
   // iodide detector.
   //
   // Note: Not thread-safe (If using threads you need to wrap each call with
   // a mutex lock)
   //
   // Parameter: name - The ID telescope name as it is returned by
   // KVIDTelescope::GetName.
   //
   // Return: Boolean status indicating whether or not the telescope has been
   // set correctly.
   Bool_t SetIDTelescope(const TString& name);

   // Set the "signedness" of the delta calculation
   //
   // If set, the calculation will use an absolute value for delta (i.e. no
   // negative values). This is the default behaviour but can be changed using
   // this function.
   void SetAbsoluteDelta(Bool_t status = kTRUE);

private:

#if __cplusplus < 201103L
   KVMaterial* si_;          // Silicon detector material
   KVMaterial* isobutane_;   // Dead-layer material
   KVMaterial* csi_;         // CsI detector material
   KVNucleus*  sim_nucleus_; // Simulation nucleus
#else
   std::unique_ptr<KVMaterial> si_;         // Silicon detector material
   std::unique_ptr<KVMaterial> isobutane_;  // Dead-layer material
   std::unique_ptr<KVMaterial> csi_;        // CsI detector material
   std::unique_ptr<KVNucleus> sim_nucleus_; // Simulation nucleus
#endif

   KVLightEnergyCsIVamos* calibrator_; // CsI Light->Energy calibrator
   Bool_t kInitialised_; // Initialisation status

   // Flag used to determine if the calculation of delta is absolute or
   // relative.
   Bool_t absolute_delta_;

   // Copy constructor (Declared private to prevent copying as it is not worth
   // implementing yet).
   MEDetectorStack(const MEDetectorStack&);
   // Assignment operator (Declared private to prevent copying as it is not
   // worth implementing yet).
   MEDetectorStack& operator=(const MEDetectorStack&);

   ClassDef(MEDetectorStack, 1) // MEDetectorStack

};

#endif

