// Author: Peter C. Wigg
// Created Thu  4 Feb 15:43:34  2016

////////////////////////////////////////////////////////////////////////////////
//
// MinimiserData.cpp
//
// Description
//
// Base class for handling data produced by the SiliconEnergyMinimiser. The
// minimiser function usually just returns the A value it has determined to be
// the most likely for a given Z (i.e. It returns an integer). Therefore to
// obtain more interesting information about how well the algorithm is
// performing, we need to pass in an (optional) pointer to a data class such as
// this one for the minimiser implementation to populate.
//
// This class handles only the essential data produced by any hypothetical
// minimiser implementation. For data specific to the implementation you must
// create an inheriting class (See MonoMinimiserData and ThreadedMinimiserData).
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Thu  4 Feb 15:43:34  2016
//
////////////////////////////////////////////////////////////////////////////////

#include "MinimiserData.h"

ClassImp(MinimiserData)

MinimiserData::MinimiserData() :
   z_value_(0),
   a_value_(0),
   delta_(0.),
   status_code_(0)
{

}

MinimiserData::~MinimiserData()
{

}

// -----------------------------------------------------------------------------
// ------------------------------ Accessors ------------------------------------
// -----------------------------------------------------------------------------

Int_t MinimiserData::GetZ() const
{
   return z_value_;
}

Int_t MinimiserData::GetA() const
{
   return a_value_;
}

Double_t MinimiserData::GetDelta() const
{
   return delta_;
}

Int_t MinimiserData::GetStatusCode() const
{
   return status_code_;
}

// -----------------------------------------------------------------------------
// ------------------------------ Mutators -------------------------------------
// -----------------------------------------------------------------------------

void MinimiserData::SetZ(Int_t value)
{
   z_value_ = value;
}

void MinimiserData::SetA(Int_t value)
{
   a_value_ = value;
}

void MinimiserData::SetDelta(Double_t value)
{
   delta_ = value;
}

void MinimiserData::SetStatusCode(Int_t value)
{
   status_code_ = value;
}



