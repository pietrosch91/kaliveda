// Author: Peter C. Wigg
// Created Thu  4 Feb 16:04:22  2016

////////////////////////////////////////////////////////////////////////////////
//
// ThreadedMinimiserData.cpp
//
// Description
//
// Class for handling data produced by the SiliconEnergyMinimiser. The minimiser
// function usually just returns the A value it has determined to be the most
// likely for a given Z (i.e. It returns an integer). Therefore to obtain more
// interesting information about how well the algorithm is performing, we need
// to pass in an (optional) pointer to a data class such as this one for the
// minimiser implementation to populate.
//
// This class handles the extra data produced by the ThreadedMinimiserImpl
// implementation, in addition to the basic minimiser data (See MinimiserData).
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Thu  4 Feb 16:04:22  2016
//
////////////////////////////////////////////////////////////////////////////////

#include "ThreadedMinimiserData.h"

ClassImp(ThreadedMinimiserData)

ThreadedMinimiserData::ThreadedMinimiserData() :
   forward_counter_(0),
   backward_counter_(0)
{

}

ThreadedMinimiserData::~ThreadedMinimiserData()
{

}

// -----------------------------------------------------------------------------
// ------------------------------ Accessors ------------------------------------
// -----------------------------------------------------------------------------

Int_t ThreadedMinimiserData::GetForwardCounter() const
{
   return forward_counter_;
}

Int_t ThreadedMinimiserData::GetBackwardCounter() const
{
   return backward_counter_;
}

// -----------------------------------------------------------------------------
// ------------------------------ Mutators -------------------------------------
// -----------------------------------------------------------------------------

void ThreadedMinimiserData::SetForwardCounter(Int_t value)
{
   forward_counter_ = value;
}

void ThreadedMinimiserData::SetBackwardCounter(Int_t value)
{
   backward_counter_ = value;
}



