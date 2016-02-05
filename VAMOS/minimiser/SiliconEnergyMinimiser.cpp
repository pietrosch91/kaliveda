// Author: Peter C. Wigg
// Created Sun 17 Jan 20:35:37  2016

////////////////////////////////////////////////////////////////////////////////
//
// SiliconEnergyMinimiser.cpp
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

#include "SiliconEnergyMinimiser.h"

ClassImp(SiliconEnergyMinimiser)

//______________________________________________________________________________

SiliconEnergyMinimiser::SiliconEnergyMinimiser() :
#if __cplusplus < 201103L
   impl_(NULL),
#else
   impl_(nullptr),
#endif
   kInitialised_(kFALSE)
{

}

//______________________________________________________________________________

SiliconEnergyMinimiser::~SiliconEnergyMinimiser()
{
#if __cplusplus < 201103L
   if (impl_) {
      delete impl_;
      impl_ = NULL;
   }
#endif
}

//______________________________________________________________________________

Bool_t SiliconEnergyMinimiser::Init()
{
   if (kInitialised_) return kTRUE;

#if __cplusplus < 201103L
   impl_ = new MonoMinimiserImpl();
#else
   impl_.reset(new MonoMinimiserImpl());
#endif

   if (!impl_->Init()) return kFALSE;

   kInitialised_ = kTRUE;
   return kTRUE;
}

//______________________________________________________________________________

Bool_t SiliconEnergyMinimiser::SetIDTelescope(const TString& telescope_name)
{
   if (!gMultiDetArray) {
      Error("SiliconEnergyMinimiser::SetIDTelescope",
            "You must build the multi-detector array before calling this "
            "function"
           );
      return kFALSE;
   }

   assert(gMultiDetArray);

   if (telescope_name.Length() == 0) {
      Error("SiliconEnergyMinimiser::SetIDTelescope",
            "Supplied telescope name has zero length");
      return kFALSE;
   }

   if (!kInitialised_) Init();
   assert(kInitialised_);

   KVSeqCollection* id_telescopes(gMultiDetArray->GetListOfIDTelescopes());

   assert(id_telescopes);
   if (!id_telescopes->FindObject(telescope_name.Data())) {
      Error("SiliconEnergyMinimiser::SetTolerance",
            "Supplied telescope name is not recognised");
      return kFALSE;
   }

   return impl_->SetIDTelescope(telescope_name);
}

//______________________________________________________________________________

Int_t SiliconEnergyMinimiser::Minimise(
   UInt_t z_value, Double_t si_energy, Double_t csi_light,
   MinimiserData* const data
)
{
   if (!kInitialised_) Init();
   assert(kInitialised_);

   // --------------------------------
   // --- Validate Z value input ---
   // --------------------------------
   if (z_value > 120) {
      Error("SiliconEnergyMinimiser::Minimise",
            "Requested Z value (%u) > Z range of known nuclei "
            "(Did you pass a negative integer?)", z_value);
      return -55;
   }

   if (z_value == 0) {
      Error("SiliconEnergyMinimiser::Minimise",
            "Requesting a z_value of 0 is not allowed (charged PID only)");
      return -55;
   }

   // --------------------------------
   // --- Validate Si Energy input ---
   // --------------------------------
   if (si_energy < 0.) {
      Error("SiliconEnergyMinimiser::Minimise",
            "Supplied silicon energy is negative");
      return -55;
   }

   if (si_energy == 0.) {
      Error("SiliconEnergyMinimiser::Minimise",
            "Supplied silicon energy == 0");
      return -55;
   }

   // --------------------------------
   // --- Validate CsI Light input ---
   // --------------------------------
   if (csi_light < 0.) {
      Error("SiliconEnergyMinimiser::Minimise",
            "Supplied CsI light is negative");
      return -55;
   }

   if (csi_light == 0.) {
      Error("SiliconEnergyMinimiser::Minimise",
            "Supplied CsI light == 0");
      return -55;
   }

   // We do not need to validate 'data' as it is allowed to be nullptr

   return impl_->Minimise(z_value, si_energy, csi_light, data);
}

//______________________________________________________________________________

void SiliconEnergyMinimiser::SetMaximumIterations(UInt_t max_iterations)
{
   if (!kInitialised_) Init();
   assert(kInitialised_);

   if (max_iterations > 300) {
      Error("SiliconEnergyMinimiser::SetMaximumIterations",
            "Requested number of iterations (%u) > "
            "A range of known nuclei (Did you pass a negative "
            "integer?)", max_iterations);
      return;
   }

   if (max_iterations == 0) {
      Error("SiliconEnergyMinimiser::SetMaximumIterations",
            "Requesting 0 as the maximum number of iterations does not "
            "make sense");
      return;
   }

   impl_->SetMaximumIterations(max_iterations);
}

//______________________________________________________________________________

void SiliconEnergyMinimiser::SetTolerance(Double_t tolerance)
{
   if (!kInitialised_) Init();
   assert(kInitialised_);

   if (tolerance <= 0.) {
      Error("SiliconEnergyMinimiser::SetTolerance",
            "We do not accept negative tolerance values, ignoring");
      return;
   }

   impl_->SetTolerance(tolerance);
}

//______________________________________________________________________________

void SiliconEnergyMinimiser::Print() const
{
   if (!kInitialised_) {
      Error("SiliconEnergyMinimiser::Print",
            "Init() has not been called, refusing to proceed");
      return;
   }
   impl_->Print();
}

//______________________________________________________________________________

UInt_t SiliconEnergyMinimiser::GetMaximumIterations() const
{
   if (!kInitialised_) {
      Error("SiliconEnergyMinimiser::GetMaximumIterations",
            "Init() has not been called, refusing to proceed");
      return 0;
   }
   return impl_->GetMaximumIterations();
}

//______________________________________________________________________________

Double_t SiliconEnergyMinimiser::GetTolerance() const
{
   if (!kInitialised_) {
      Error("SiliconEnergyMinimiser::GetTolerance",
            "Init() has not been called, refusing to proceed");
      return 0.;
   }
   return impl_->GetTolerance();
}

