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

SiliconEnergyMinimiser::SiliconEnergyMinimiser() :
#if __cplusplus < 201103L
   impl_(NULL),
#else
   impl_(nullptr),
#endif
   kInitialised_(kFALSE)
{

}

SiliconEnergyMinimiser::~SiliconEnergyMinimiser()
{
#if __cplusplus < 201103L
   if (impl_) {
      delete impl_;
      impl_ = NULL;
   }
#endif
}

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

Int_t SiliconEnergyMinimiser::Minimise(
   UInt_t z_value, Double_t si_energy, Double_t csi_light,
   MinimiserData* const data
)
{
   return impl_->Minimise(z_value, si_energy, csi_light, data);
}

void SiliconEnergyMinimiser::SetMaximumIterations(UInt_t max_iterations)
{
   impl_->SetMaximumIterations(max_iterations);
}

void SiliconEnergyMinimiser::SetTolerance(Double_t tolerance)
{
   impl_->SetTolerance(tolerance);
}

void SiliconEnergyMinimiser::Print() const
{
   impl_->Print();
}

