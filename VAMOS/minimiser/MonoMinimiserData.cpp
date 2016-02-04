#include "MonoMinimiserData.h"

ClassImp(MonoMinimiserData)

MonoMinimiserData::MonoMinimiserData() :
   forward_counter_(0),
   backward_counter_(0)
{

}

MonoMinimiserData::~MonoMinimiserData()
{

}

Int_t MonoMinimiserData::GetForwardCounter() const
{
   return forward_counter_;
}

Int_t MonoMinimiserData::GetBackwardCounter() const
{
   return backward_counter_;
}

void MonoMinimiserData::SetForwardCounter(Int_t value)
{
   forward_counter_ = value;
}

void MonoMinimiserData::SetBackwardCounter(Int_t value)
{
   backward_counter_ = value;
}

