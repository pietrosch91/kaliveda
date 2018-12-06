// Author: Peter C. Wigg
// Created Wed 20 Jan 15:19:33  2016

////////////////////////////////////////////////////////////////////////////////
//
// ThreadedMassEstimator.cpp
//
// Description
//
// This class estimates the A value of a fragment detected in the
// Si:Isobutane:CsI detector stack, given the experimentally measured data (Z,
// Silicon Energy, CsI Light).
//
// It achieves this by minimising the absolute magnitude of the difference
// between the experimentally measured silicon energy and the silicon energy
// simulated in a set of test nuclei (with fixed Z) to determine which nucleus
// (A value) provides the closest match.
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Wed 20 Jan 15:19:33  2016
//
////////////////////////////////////////////////////////////////////////////////

#include "ThreadedMassEstimator.h"

ClassImp(ThreadedMassEstimator)

const Int_t me::ThreadStatus::kThreadStop(0);
const Int_t me::ThreadStatus::kThreadContinue(-1);

pthread_mutex_t ThreadedMassEstimator::io_mutex_;

//______________________________________________________________________________

ThreadedMassEstimator::ThreadedMassEstimator() :
   kInitialised_(kFALSE),
   load_balance_(2.25),
   max_iterations_(25),
#if __cplusplus < 201103L
   thread_data_(NULL),
   detector_stack_(NULL)
#else
   thread_data_(nullptr),
   detector_stack_(nullptr)
#endif

{

}

//______________________________________________________________________________

ThreadedMassEstimator::~ThreadedMassEstimator()
{
   pthread_mutex_destroy(&io_mutex_);
#if __cplusplus < 201103L
   if (detector_stack_) {
      delete detector_stack_;
      detector_stack_ = NULL;
   }


   if (thread_data_) {
      if (thread_data_[0].detector_stack) {
         delete thread_data_[0].detector_stack;
         thread_data_[0].detector_stack = NULL;
      }

      if (thread_data_[1].detector_stack) {
         delete thread_data_[1].detector_stack;
         thread_data_[1].detector_stack = NULL;
      }

      delete[] thread_data_;
      thread_data_ = NULL;
   }
#endif
}

//______________________________________________________________________________

Bool_t ThreadedMassEstimator::Init()
{
   if (kInitialised_) {
      return kTRUE;
   }

#if __cplusplus < 201103L
   pthread_mutex_init(&io_mutex_, NULL);

   // We build the detector stacks for each thread here to save the malloc
   // overheard from happening in each event.
   thread_data_ = new struct me::ThreadData[2];
   thread_data_[0].detector_stack = new MEDetectorStack();
   thread_data_[1].detector_stack = new MEDetectorStack();

   // This stack is used for the initial calculation only
   detector_stack_ = new MEDetectorStack();
#else
   pthread_mutex_init(&io_mutex_, nullptr);
   // We build the detector stacks for each thread here to save the malloc
   // overheard from happening in each event.
   thread_data_.reset(new struct me::ThreadData[2]);
   thread_data_[0].detector_stack.reset(new MEDetectorStack());
   thread_data_[1].detector_stack.reset(new MEDetectorStack());

   // This stack is used for the initial calculation only
   detector_stack_.reset(new MEDetectorStack());
#endif

   thread_data_[0].detector_stack->Init();
   thread_data_[1].detector_stack->Init();
   detector_stack_->Init();

   kInitialised_ = kTRUE;
   return kTRUE;
}

//______________________________________________________________________________

Bool_t ThreadedMassEstimator::IsInitialised() const
{
   return kInitialised_;
}

//______________________________________________________________________________

Bool_t ThreadedMassEstimator::SetIDTelescope(const TString& name)
{
   assert(kInitialised_);
   Bool_t status(kTRUE);

   status &= thread_data_[0].detector_stack->SetIDTelescope(name);
   status &= thread_data_[1].detector_stack->SetIDTelescope(name);
   status &= detector_stack_->SetIDTelescope(name);

   return status;
}

//______________________________________________________________________________

void ThreadedMassEstimator::GetPossibleAValues(
   const Int_t z_value,
   std::vector<Int_t>* results
)
{
   assert(results);

   // Destroy all the objects currently in result
   results->clear();

   for (Int_t a_value = 1; a_value < 240; ++a_value) {
      KVNucleus nucleus(z_value, a_value);
      if (nucleus.GetLifeTime() > 0.) {
         results->push_back(a_value);
      }
   }
}

//______________________________________________________________________________

void* ThreadedMassEstimator::ThreadExec(void* thread_data)
{
   struct me::ThreadData* data(
      static_cast<struct me::ThreadData*>(thread_data)
   );

   assert(data);

   Bool_t process_enable(kFALSE);

   struct me::SimulationParameters parameters;
   parameters.z = data->z_value_expt;
   parameters.si_energy = data->si_energy_expt;
   parameters.csi_light = data->csi_light_expt;

   Int_t status(0);
   data->counter = 0;

   if (data->kForwardIterator == kTRUE) {

      // Iterate forwards through the list of possible A values.

      std::vector<Int_t>::iterator itr;
      for (itr = data->possible_a_values.begin();
            itr != data->possible_a_values.end();
            ++itr
          ) {

         if (process_enable) {

            parameters.a = *itr;
            data->current_a = *itr;

            // Process this point
            status = ThreadProcess(data, &parameters);

            // If we are maximising or something went wrong we return
            if (status == me::ThreadStatus::kThreadStop) {
               break;
            }

         }

         // Do nothing until we reach the pre-defined starting value
         if (*itr == data->minimum_a) {
            // Remove the inhibitor switch for the next loop
            process_enable = kTRUE;
         }

      }

   }
   else {

      // Iterate backwards through the list of possible A values.

      std::vector<Int_t>::reverse_iterator ritr;
      for (ritr = data->possible_a_values.rbegin();
            ritr != data->possible_a_values.rend();
            ++ritr
          ) {

         if (process_enable) {

            parameters.a = *ritr;
            data->current_a = *ritr;

            // Process this point
            status = ThreadProcess(data, &parameters);

            if (status == me::ThreadStatus::kThreadStop) {
               break;
            }

         }

         // Do nothing until we reach the pre-defined starting value
         if (*ritr == data->minimum_a) {
            // Remove the inhibitor switch for the next loop
            process_enable = kTRUE;
         }


      }

   }

#if __cplusplus < 201103L
   pthread_exit(NULL);
#else
   pthread_exit(nullptr);
#endif

}

//______________________________________________________________________________

Int_t ThreadedMassEstimator::ThreadProcess(
   struct me::ThreadData* const data,
   const struct me::SimulationParameters* const parameters)
{

   ++data->counter;
   if (data->counter > data->max_iterations) {
      // We've exceeded the number of allowed iterations, stop processing and
      // signal an error.
      data->current_delta = -1.;
      return me::ThreadStatus::kThreadStop;
   }

   if (!data->detector_stack->Simulate(parameters, &(data->sim_result))) {
      data->current_delta = -1.;
      return me::ThreadStatus::kThreadStop;

   }

   data->current_delta = data->sim_result.delta;

   if (data->current_delta < 0.) {

      // This should be impossible but we might as well check

      pthread_mutex_lock(&io_mutex_);
      Error("ThreadedMassEstimator::ThreadProcess",
            "Negative value for current delta!");
      pthread_mutex_unlock(&io_mutex_);

      return me::ThreadStatus::kThreadStop;
   }

   if (data->minimum_delta < 0.) {

      // This should be impossible but we might as well check

      pthread_mutex_lock(&io_mutex_);
      Error("ThreadedMassEstimator::ThreadProcess",
            "Negative value for minimum delta!");
      pthread_mutex_unlock(&io_mutex_);

      return me::ThreadStatus::kThreadStop;
   }

   if (data->current_delta < data->minimum_delta) {
      // The current point (A value, Delta) yields a better result than the
      // stored point (a lower value for delta). We therefore overwrite the
      // stored data with the current values;

      data->minimum_delta = data->current_delta;
      data->minimum_a = data->current_a;

      return me::ThreadStatus::kThreadContinue;

   }
   else if (data->current_delta > data->minimum_delta) {
      // We are maximising delta which means we are moving away from the
      // minimum point we are searching for so we stop the processing.

      return me::ThreadStatus::kThreadStop;

   }
   else {
      // Deltas are equal, this means we cannot be sure we have the correct A
      // value (the current minimum value for A will be used)

      return me::ThreadStatus::kThreadStop;
   }

}

//______________________________________________________________________________

Bool_t ThreadedMassEstimator::EstimateA(
   const struct me::EstimatorInput* const input,
   const std::vector<Int_t>* const possible_a_values,
   struct me::EstimatorResult* const result
)
{
   assert(input);
   assert((input->si_energy > 0.) && (input->si_energy < 3000.));

   assert(possible_a_values);
   assert(result);

   if (!input) {
      Error("ThreadedMassEstimator::EstimateA", "Input data is nullptr");
      return kFALSE;
   }

   // You should never see this initialisation value in the output, it should
   // always be set to either a positive or a negative number below:
   result->status = 0;

   static Long64_t event_number(0);
   ++event_number;

   struct me::SimulationParameters parameters;
   parameters.z = input->z;

   // Initial simulation for the starting A value (common to both threads)
   //
   // IMPORTANT: The initial A value must be set at approximately the right
   // value (approximately 2*Z) in order to reduce the time taken for its
   // determination. However an additional linear scaling is needed to maintain
   // the balance of processing power between the "forward" thread and the
   // "backward" thread. If this balance is not maintained then all of the
   // processing is done in one thread and you lose the benefit of using two.
   // Empirically, the default scaling factor of 2.25 is sufficient over the
   // range of z values we expect in the E503 experiment.

   parameters.a = static_cast<Int_t>(load_balance_ * input->z);
   parameters.si_energy = input->si_energy;
   parameters.csi_light = input->csi_light;

   // Get the "delta" - the magnitude difference between the real silicon
   // energy and that simulated for our A = load_balance_*Z nucleus.
   // Single thread at this point, no need to lock with mutex
   Double_t delta_init(-1.);

   struct me::SimulationResult res;
   if (!detector_stack_->Simulate(&parameters, &res)) {
      return kFALSE;
   }

   delta_init = res.delta;

   // Pack the input data into structs

   for (Int_t i = 0; i < 2; ++i) {
      thread_data_[i].z_value_expt = input->z;
      thread_data_[i].si_energy_expt = input->si_energy;
      thread_data_[i].csi_light_expt = input->csi_light;

      // Give each thread a local copy of the possible A values
      thread_data_[i].possible_a_values = *possible_a_values;

      // Set the (A, delta) as the starting point for both threads
      thread_data_[i].minimum_a = parameters.a;
      thread_data_[i].minimum_delta = delta_init;
      thread_data_[i].max_iterations = max_iterations_;

   }

   // Iterating forwards (Increasing A from initial value)
   thread_data_[0].kForwardIterator = kTRUE;

   // Iterating backwards (Decreasing A from initial value)
   thread_data_[1].kForwardIterator = kFALSE;

   // Create the threads (one for each direction - increasing/decreasing A)

   pthread_t thr[2];
   Int_t status(0);

   //----------------
   // Create Thread 0
   //----------------

#if __cplusplus < 201103L
   status = pthread_create(
               &thr[0],
               NULL,
               ThreadExec,
               static_cast<void*>(&thread_data_[0])
            );
#else
   status = pthread_create(
               &thr[0],
               nullptr,
               ThreadExec,
               static_cast<void*>(&thread_data_[0])
            );
#endif

   if (status != 0) {
      Error("ThreadedMassEstimator::EstimateA", "Failed to create thread 0");
      return kFALSE;
   }

   //----------------
   // Create Thread 1
   //----------------

#if __cplusplus < 201103L
   status = pthread_create(
               &thr[1],
               NULL,
               ThreadExec,
               static_cast<void*>(&thread_data_[1])
            );
#else
   status = pthread_create(
               &thr[1],
               nullptr,
               ThreadExec,
               static_cast<void*>(&thread_data_[1])
            );
#endif

   if (status != 0) {
      Error("ThreadedMassEstimator::EstimateA", "Failed to create thread 1");
      return kFALSE;
   }

   //--------------
   // Join Thread 0
   //--------------

   Int_t thread_return(0);

   status = pthread_join(
               thr[0],
               reinterpret_cast<void**>(&thread_return)
            );

   if (status != 0) {
      Error(
         "ThreadedMassEstimator::EstimateA",
         "Failed to join Thread 0 (status = %d)", thread_return
      );
      return kFALSE;
   }

   //--------------
   // Join Thread 1
   //--------------

   status = pthread_join(
               thr[1],
               reinterpret_cast<void**>(&thread_return)
            );

   if (status != 0) {
      Error(
         "ThreadedMassEstimator::EstimateA",
         "Failed to join Thread 1 (status = %d)", thread_return
      );
      return kFALSE;
   }

   // We now compare the results of the two threads to see which one has the
   // best value for a_min

   Int_t a_min(-1);
   Double_t delta(-1.);
   Int_t result_status(0);

   Bool_t thread_ok[2];
   for (Int_t i = 0; i < 2; ++i) thread_ok[i] = kTRUE;

   if (thread_data_[0].minimum_delta < 0.) thread_ok[0] = kFALSE;
   if (thread_data_[1].minimum_delta < 0.) thread_ok[1] = kFALSE;

   if (thread_ok[0] && thread_ok[1]) {

      if (thread_data_[0].minimum_delta < thread_data_[1].minimum_delta) {
         a_min = thread_data_[0].minimum_a;
         delta = thread_data_[0].minimum_delta;
         result_status = kThreadZeroResult;

      }
      else if (thread_data_[1].minimum_delta < thread_data_[0].minimum_delta) {
         a_min = thread_data_[1].minimum_a;
         delta = thread_data_[1].minimum_delta;
         result_status = kThreadOneResult;

      }
      else {
         // Equal Deltas: This usually happens when the initial A value
         // provided the minimum delta. This is because both threads store
         // this point as their initial minimum reference)

         a_min = thread_data_[0].minimum_a;
         delta = thread_data_[0].minimum_delta;
         result_status = kEqualDeltas;
      }

   }
   else if (thread_ok[0] && !thread_ok[1]) {
      a_min = thread_data_[0].minimum_a;
      delta = thread_data_[0].minimum_delta;
      result_status = kThreadZeroResult;

   }
   else if (!thread_ok[0] && thread_ok[1]) {
      a_min = thread_data_[1].minimum_a;
      delta = thread_data_[1].minimum_delta;
      result_status = kThreadOneResult;

   }
   else {

      // IMPORTANT: Both threads failed to return a sensible result, you need
      // to handle this in your analysis class!

      result_status = kNoResultFound;

      result->z_value = input->z;
      result->a_value = -1;
      result->delta = -1.;
      result->forward_counter = thread_data_[0].counter;
      result->backward_counter = thread_data_[1].counter;
      result->status = result_status;

      return kFALSE;
   }

   result->z_value = input->z;
   result->a_value = a_min;
   result->delta = delta;
   result->forward_counter = thread_data_[0].counter;
   result->backward_counter = thread_data_[1].counter;
   result->status = result_status;

   return kTRUE;
}

//______________________________________________________________________________

void ThreadedMassEstimator::SetLoadBalance(Float_t load_balance)
{
   load_balance_ = load_balance;
}

//______________________________________________________________________________

void ThreadedMassEstimator::SetMaximumIterations(UInt_t iterations)
{
   max_iterations_ = iterations;
}

//______________________________________________________________________________

Float_t ThreadedMassEstimator::GetLoadBalance() const
{
   return load_balance_;
}

//______________________________________________________________________________

UInt_t ThreadedMassEstimator::GetMaximumIterations() const
{
   return max_iterations_;
}

