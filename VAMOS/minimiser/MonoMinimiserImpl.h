// Author: Peter C. Wigg
// Created Sun 17 Jan 20:43:44  2016

////////////////////////////////////////////////////////////////////////////////
//
// MonoMinimiserImpl.h
//
// Description
//
// Standard implementation of the silicon energy minimiser. This class
// implements the minimisation by using two while loops operating in different
// directions (in terms of A value), both starting from the same initial ansatz
// that A is most likely to be around to 2Z. It is essentially the single
// threaded version of the ThreadedMassEstimator, operating to reduce the
// difference between the observed and simulated silicon detector energies in
// order to determine the most likely value for A.
//
// The bisection method previously used has been replaced by this "brute force"
// method. The reasoning behind this decision is that the bisection method
// relies upon the function being well behaved and passing through delta = 0
// with positive values on one side of the minimum and negative ones on the
// other. However due to the large A range being used, this does not seem to be
// guaranteed.  For example if one takes a value in A which is too large, the
// energy calculation can result in something quite extreme and using the
// bisection method you will lose 1/2 of the available solution space from this
// single error.
//
// On my desktop this algorithm processes at about 85 events/s compared with 220
// events/s for the threaded version (ThreadedMassEstimator).
//
// Note: It is important that you keep the public interface (public member
// functions) of the implementation classes in agreement with those in this
// class, as all functions are simple passed off to the implementation class.
// For example SiliconEnergyMinimiser::SetIDTelescope() simply calls
// MonoMinimiserImpl::SetIDTelescope().
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Sun 17 Jan 20:43:44  2016
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MONO_MINIMISER_IMPL_H__
#define __MONO_MINIMISER_IMPL_H__

#include <cassert>
#include <memory>

#include "Rtypes.h"
#include "TString.h"
#include "TError.h"
#include "TMath.h"

#include "MEDetectorStack.h"
#include "MonoMinimiserData.h"
#include "KVMacros.h"

class MonoMinimiserImpl {

public:

   // Default constructor
   MonoMinimiserImpl();
   // Destructor
   virtual ~MonoMinimiserImpl();

   // Initialisation function (to be called post-construction)
   Bool_t Init();

   // Standard minimisation implementation
   //
   // We use two while loops operating from the same starting point (A=2Z) but
   // working in opposite directions. This is essentially the single threaded
   // version of the ThreadedMinimiserImpl minimiser implementation.
   //
   // Parameter: z_value - Measured Z value
   // Parameter: si_energy - Measured silicon energy (MeV)
   // Parameter: csi_light - Measured raw caesium iodide light (Channel)
   // Parameter: data - Optional pointer for storing extra data
   //
   // See SiliconEnergyMinimiser::Minimise
   // See MonoMinimiserData
#if __cplusplus < 201103L
   Int_t Minimise(UInt_t z_value, Double_t si_energy, Double_t csi_light,
                  MinimiserData* const data = NULL);
#else
   Int_t Minimise(UInt_t z_value, Double_t si_energy, Double_t csi_light,
                  MinimiserData* const data = nullptr);
#endif

   // Set the identification telescope
   // Parameter: telescope_name - As returned by KVIDTelescope::GetName()
   Bool_t SetIDTelescope(const TString& telescope_name);

   // Set the maximum number of iterations allowed
   // See SiliconEnergyMinimiser::SetMaximumIterations
   void SetMaximumIterations(UInt_t max_iterations);

   // Set the tolerance of the algorithm
   // See SiliconEnergyMinimiser::SetAbsoluteTolerance
   void SetTolerance(Double_t tolerance);

   // Print minimiser implementation information
   void Print() const;

   UInt_t GetMaximumIterations() const;
   Double_t GetTolerance() const;

private:

   // Copy constructor (Declared private to prevent copying as it is not worth
   // implementing yet).
   MonoMinimiserImpl(const MonoMinimiserImpl& rhs);
   // Assignment operator (Declared private to prevent copying as it is not
   // worth implementing yet).
   MonoMinimiserImpl& operator=(const MonoMinimiserImpl& rhs);

   // Minimisation routine error codes
   enum MinimiserErrorCodes {
      kTelescopeNotSet = -1,
      kForwardExceeded = -2,
      kBackwardExceeded = -3,
      kNoValidResult = -4
   };

   Bool_t kInitialised_;  // Initialisation status
   Bool_t kTelescopeSet_; // Telescope setting status

   UInt_t max_iterations_; // Maximum number of iterations

   // The tolerance allowed between delta values before declaring a maximisation
   // event.
   //
   // By default this is set to 5% of the last delta value and two successive
   // events above this tolerance results in the iterative loop being terminated
   // - "consistent maximisation" implies that one is moving away from the
   // minimum so further processing is a waste of time.
   Double_t tolerance_;

#if __cplusplus < 201103L
   // Struct containing simulation input parameters
   struct me::SimulationParameters* sim_parameters_;
   // Struct containing simulation output parameters
   struct me::SimulationResult* sim_results_;

   // Si-Isobutane-CsI detector stack
   //
   // Used to determine the value of delta (absolute value) between the
   // experimental and simulated silicon detector energies for a give nucleus.
   //
   // See MEDetectorStack
   MEDetectorStack* stack_;
#else
   // Struct containing simulation input parameters
   std::unique_ptr<struct me::SimulationParameters> sim_parameters_;
   // Struct containing simulation output parameters
   std::unique_ptr<struct me::SimulationResult> sim_results_;

   // Si-Isobutane-CsI detector stack
   //
   // Used to determine the value of delta (absolute value) between the
   // experimental and simulated silicon detector energies for a give nucleus.
   //
   // See MEDetectorStack
   std::unique_ptr<MEDetectorStack> stack_;
#endif

   ClassDef(MonoMinimiserImpl, 1)
};

#endif
