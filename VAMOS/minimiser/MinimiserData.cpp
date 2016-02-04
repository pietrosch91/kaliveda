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



