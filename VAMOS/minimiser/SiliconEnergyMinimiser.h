// Author: Peter C. Wigg
// Created Sun 17 Jan 20:35:37  2016

////////////////////////////////////////////////////////////////////////////////
//
// SiliconEnergyMinimiser.h
//
// Description
//
// Wrapper class for the silicon energy minimisation routines. This class allows
// the underlying implementation to be changed easily and without affecting any
// existing code.
//
// To change the implementation, for example to use the multi-threaded minimiser
// instead of the single threaded one, simply ensure that the appropriate
// implementation class header is included and then change any occurrences of
// MonoMinimiserImpl in the code to use your chosen implementation (e.g.
// ThreadedMinimiserImpl). Then when you recompile KaliVeda the new
// implementation will be used instead.
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Sun 17 Jan 20:35:37  2016
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __SILICON_ENERGY_MINIMISER_H__
#define __SILICON_ENERGY_MINIMISER_H__

#include <cassert>
#include <vector>

#include "Rtypes.h"
#include "TString.h"

//#include "ThreadedMinimiserImpl.h"
#include "MonoMinimiserImpl.h"
#include "MinimiserData.h"

class SiliconEnergyMinimiser {

   // Minimiser implementation
#if __cplusplus < 201103L
   MonoMinimiserImpl* impl_;
#else
   std::unique_ptr<MonoMinimiserImpl> impl_;
#endif

public:

   // Default constructor
   SiliconEnergyMinimiser();
   // Destructor
   virtual ~SiliconEnergyMinimiser();

   // Initialisation function (to be called post-construction)
   Bool_t Init();

   // Set the identification telescope
   // Parameter: telescope_name - Name as returned by KVIDTelescope::GetName()
   Bool_t SetIDTelescope(const TString& telescope_name);

   // Estimate the A value of a nucleus by minimising the difference between
   // measured and simulated silicon energies in VAMOS. Runs the underlying
   // implementation (either MonoMinimiserImpl::Minimise or
   // ThreadedMinimiserImpl::Minimise) which is compile-time dependent.
   // MonoMinimiserImpl is the default.
   //
   // Parameter: z_value - Measured Z value
   // Parameter: si_energy - Measured silicon energy (MeV)
   // Parameter: csi_light - Measured raw caesium iodide light (Channel)
   // Parameter: data - Optional pointer for storing extra data
   //
   // See MonoMinimiserImpl::Minimise
   // See ThreadedMinimiserImpl::Minimise
   // See MinimiserData
#if __cplusplus < 201103L
   Int_t Minimise(UInt_t z_value, Double_t si_energy, Double_t csi_light,
                  MinimiserData* const data = NULL);
#else
   Int_t Minimise(UInt_t z_value, Double_t si_energy, Double_t csi_light,
                  MinimiserData* const data = nullptr);
#endif

   // Set the maximum number of iterations allowed
   void SetMaximumIterations(UInt_t max_iterations);

   // Set the algorithm tolerance (if possible)
   void SetTolerance(Double_t tolerance = 0.05);

   // Print minimiser information
   void Print() const;

   UInt_t GetMaximumIterations() const;
   Double_t GetTolerance() const;

private:

   // Copy constructor (Declared private to prevent copying as it is not worth
   // implementing yet).
   SiliconEnergyMinimiser(const SiliconEnergyMinimiser&);
   // Assignment operator (Declared private to prevent copying as it is not
   // worth implementing yet).
   SiliconEnergyMinimiser& operator=(const SiliconEnergyMinimiser&);

   // Initialisation status
   Bool_t kInitialised_;

   ClassDef(SiliconEnergyMinimiser, 1)
};

#endif
