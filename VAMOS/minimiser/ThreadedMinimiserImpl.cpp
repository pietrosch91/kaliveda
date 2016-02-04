// Author: Peter C. Wigg
// Created Sun 17 Jan 20:40:12  2016

///////////////////////////////////////////////////////////////////////////////
//
// ThreadedMinimiserImpl.cpp
//
// Description
//
// Threaded implementation of the silicon energy minimiser. This class
// implements the minimisation using the ThreadedMassEstimator.
//
// Warning: This implementation does not include a tolerance or a consistent
// maximisation check (maximisation is declared as soon as delta increases, i.e.
// we do not wait to see if delta is maximising consistently). It is recommended
// that you use the standard implementation (MonoMinimiserImpl) if you can as it
// does not require pthreads, is less complex and probably does the job well
// enough. If not then you are free to implement the aforementioned changes :)
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Sun 17 Jan 20:40:12  2016
//
////////////////////////////////////////////////////////////////////////////////

#include "ThreadedMinimiserImpl.h"

ClassImp(ThreadedMinimiserImpl)

ThreadedMinimiserImpl::ThreadedMinimiserImpl() :
   kInitialised_(kFALSE),
#if __cplusplus < 201103L
   estimator_input_(NULL),
   possible_a_values_(NULL),
   estimator_result_(NULL),
   mass_estimator_(NULL)
#else
   estimator_input_(nullptr),
   possible_a_values_(nullptr),
   estimator_result_(nullptr),
   mass_estimator_(nullptr)
#endif
{

}

ThreadedMinimiserImpl::~ThreadedMinimiserImpl()
{
#if __cplusplus < 201103L
   // Delete all threaded stuff
   if (mass_estimator_) {
      delete mass_estimator_;
      mass_estimator_ = NULL;
   }

   if (estimator_result_) {
      delete estimator_result_;
      estimator_result_ = NULL;
   }

   if (possible_a_values_) {
      delete possible_a_values_;
      possible_a_values_ = NULL;
   }

   if (estimator_input_) {
      delete estimator_input_;
      estimator_input_ = NULL;
   }
#endif
}

void ThreadedMinimiserImpl::Init()
{
   if (kInitialised_) return;

#if __cplusplus < 201103L
   estimator_input_ = new struct me::EstimatorInput();
   possible_a_values_ = new std::vector<Int_t>();
   estimator_result_ = new struct me::EstimatorResult();
   mass_estimator_ = new ThreadedMassEstimator();
#else
   estimator_input_.reset(new struct me::EstimatorInput());
   possible_a_values_.reset(new std::vector<Int_t>());
   estimator_result_.reset(new struct me::EstimatorResult());
   mass_estimator_.reset(new ThreadedMassEstimator());
#endif

   mass_estimator_->Init();
   kInitialised_ = kTRUE;
}

Bool_t ThreadedMinimiserImpl::SetIDTelescope(const TString& telescope_name)
{
   if (!kInitialised_) {
      Error("ThreadedMinimiserImpl::SetIDTelescope",
            "You need to call ThreadedMinimiserImpl::Init()");
      return kFALSE;
   }

   assert(mass_estimator_);
   return mass_estimator_->SetIDTelescope(telescope_name);
}

Int_t ThreadedMinimiserImpl::Minimise(
   UInt_t z_value, Double_t si_energy, Double_t csi_light,
   MinimiserData* const data
)
{
   if (!kInitialised_) {
      Error("ThreadedMinimiserImpl::ThreadedMinimiseImpl",
            "You need to call ThreadedMinimiserImpl::Init()");
      return kNotInitialised;
   }

#if __cplusplus < 201103L
   ThreadedMinimiserData* threaded_data(NULL);
#else
   ThreadedMinimiserData* threaded_data(nullptr);
#endif

   if (data) {
      // Optional data storage pointer provided, cast it to make sure we can
      // provide the extra data. It is assumed that the true nature of the
      // pointer being passed in is a ThreadedMinimiserData pointer.
      threaded_data = static_cast<ThreadedMinimiserData*>(data);
      assert(threaded_data);
      threaded_data->SetZ(z_value);
      threaded_data->SetStatusCode(0);
   }

   assert(estimator_input_);
   assert(possible_a_values_);
   assert(estimator_result_);
   assert(mass_estimator_);

#if __cplusplus < 201103L
   mass_estimator_->GetPossibleAValues(z_value, possible_a_values_);
#else
   mass_estimator_->GetPossibleAValues(z_value, possible_a_values_.get());
#endif

   // We should now also have a sorted vector of potential A values for the Z
   // value returned by the base class identification.

   // Pack the struct with the information needed by our mass estimator

   estimator_input_->z = z_value;
   estimator_input_->si_energy = si_energy;
   estimator_input_->csi_light = csi_light;

   // We now estimate the value of A by simulating the passage of a nucleus
   // (with various A values) through the Si:Isobutane:CsI detector stack and
   // look for the point which minimises the difference between simulated and
   // real silicon energy (This nucleus should have the correct A value).
   //
   // The mass estimator does over-estimate A by one in the low csi light
   // regime as the isotopic bands converge and enhance the effect of
   // simulation errors and approximations. This however should not be a
   // problem as this is the first stage estimate.

#if __cplusplus < 201103L
   if (!mass_estimator_->EstimateA(estimator_input_, possible_a_values_,
                                   estimator_result_)) {
      if (threaded_data) threaded_data->SetStatusCode(kEstimateAFailed);
      return kEstimateAFailed;
   }
#else
   if (!mass_estimator_->EstimateA(estimator_input_.get(),
                                   possible_a_values_.get(),
                                   estimator_result_.get())) {
      if (threaded_data) threaded_data->SetStatusCode(kEstimateAFailed);
      return kEstimateAFailed;
   }
#endif

   if (estimator_result_->status <= 0) {
      // Negative status values indicate problems in the estimator
      if (threaded_data) threaded_data->SetStatusCode(kEstimateAErrors);
      return kEstimateAErrors;
   }

   if (threaded_data) {
      threaded_data->SetA(estimator_result_->a_value);
      threaded_data->SetDelta(estimator_result_->delta);
      threaded_data->SetForwardCounter(estimator_result_->forward_counter);
      threaded_data->SetBackwardCounter(estimator_result_->backward_counter);
   }
   return estimator_result_->a_value;
}

void ThreadedMinimiserImpl::SetMaximumIterations(UInt_t max_iterations)
{
   mass_estimator_->set_max_iterations(max_iterations);
}

void ThreadedMinimiserImpl::SetTolerance(Double_t tolerance)
{
   UNUSED(tolerance);
   // Currently not implemented
}

void ThreadedMinimiserImpl::Print() const
{
   Info("ThreadedMinimiserImpl::Print", "Multi-threaded minimiser");
}

